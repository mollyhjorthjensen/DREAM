import sys
import os
import ROOT
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

path = sys.argv[1]
assert len(sys.argv) == 2

# remember that the signals are pooled before clustering (hence the factor 2)
def euclidean_distance(x, signal):
	assert signal in ['S', 'C']
	if signal == 'S':
		return np.sqrt((x.VecShowerScntCoMi/2-x.S_comi)**2+(x.VecShowerScntCoMj/2-x.S_comj)**2)
	elif signal == 'C':
		return np.sqrt((x.VecShowerCkovCoMi/2-x.C_comi)**2+(x.VecShowerCkovCoMj/2-x.C_comj)**2)

def matching(x):
	lst = []
	while not x.empty:
		df = x.loc[x.dist.idxmin()]
		lst += [(df.clusterId.astype(int), df.showerId.astype(int), df.dist)]
		x = x[(x.showerId != df.showerId) & (x.clusterId != df.clusterId)]
	return lst

# import dataframes
pdf_true = pd.read_csv(os.path.join(path, 'truth.csv'))
pdf_cluster = {}
pdf_final = pdf_true.copy()
for signal in ['S', 'C']:
	pdf_cluster[signal] = pd.read_csv(os.path.join(path, signal+'_cluster.csv'))
	pdf_cluster[signal]['clusterId'] = pdf_cluster[signal].index.astype(int)
	pdf_merged = pdf_true.merge(pdf_cluster[signal], how='outer', on=['eventId'])
	pdf_merged = pdf_merged[(~pdf_merged.showerId.isna()) & (~pdf_merged.clusterId.isna())]
	pdf_merged['dist'] = pdf_merged.apply(lambda x: euclidean_distance(x, signal), axis=1)
	grouped_matched = pdf_merged.groupby('eventId').apply(matching)
	flatlist = [item for sublist in grouped_matched for item in sublist]
	pdf_new = pd.DataFrame(flatlist, columns=['clusterId', 'showerId', signal+'_dist'])
	pdf_cluster[signal] = pdf_cluster[signal].merge(pdf_new, how='left', on=['clusterId'])
	pdf_cluster[signal].rename(columns={'clusterId': signal+'_clusterId'}, inplace=True)
	pdf_final = pdf_final.merge(pdf_cluster[signal], how='outer', on=['eventId', 'showerId'])

pdf_final['showerIdBool'] = ~pdf_final.showerId.isna()
pdf_final['S_clusterIdBool'] = ~pdf_final.S_clusterId.isna() & ((pdf_final.S_dist < pdf_final.S_rad_mean) | pdf_final.S_dist.isna())
pdf_final['C_clusterIdBool'] = ~pdf_final.C_clusterId.isna() & ((pdf_final.C_dist < pdf_final.C_rad_mean) | pdf_final.C_dist.isna())
pdf_final['clusterIdBool'] = pdf_final.S_clusterIdBool | pdf_final.C_clusterIdBool
pdf_final.reset_index(drop=True, inplace=True)
pdf_final['clusterId'] = pdf_final.apply(lambda x: x.name if x.clusterIdBool else np.nan, axis=1)

confusion_matrix = pd.crosstab(pdf_final.showerIdBool, pdf_final.clusterIdBool, rownames=['Actual'], colnames=['Predicted'])
print(confusion_matrix)

def weighted_comi(x):
	if np.isnan(x.S_comi):
		return x.C_comi
	elif np.isnan(x.C_comi):
		return x.S_comi
	else:
		return (x.S_sum*x.S_comi+x.C_sum*x.C_comi)/(x.S_sum+x.C_sum)

def weighted_comj(x):
	if np.isnan(x.S_comj):
		return x.C_comj
	elif np.isnan(x.C_comj):
		return x.S_comj
	else:
		return (x.S_sum*x.S_comj+x.C_sum*x.C_comj)/(x.S_sum+x.C_sum)

# remember there is only one charged track in any event
def distance2charged(x):
	df = x.loc[x.IsCharged]
	a = (568/2)*np.sqrt(2)
	x['dist2charge'] = x.apply(lambda x: a if df.empty else np.sqrt((df.comi-x.comi)**2+(df.comj-x.comj)**2), axis=1)
	return x

pdf_final['comi'] = pdf_final.apply(weighted_comi, axis=1)
pdf_final['comj'] = pdf_final.apply(weighted_comj, axis=1)
pdf_final.IsCharged = pdf_final.IsCharged == 1.0
grouped_charged = pdf_final[pdf_final.clusterIdBool].groupby('eventId').apply(distance2charged)
grouped_charged = grouped_charged.filter(items=['eventId', 'clusterId', 'dist2charge'])
pdf_final = pdf_final.merge(grouped_charged, how='left', on=['eventId', 'clusterId'])
#
print(pdf_final.head())

# get dataframe when there is both a shower and cluster
pdf_ml = pdf_final[pdf_final.showerIdBool & pdf_final.clusterIdBool]
print(pdf_ml.PrimaryDecayMode.value_counts())
print(pdf_ml.VecShowerPDG.value_counts())

pdf_ml['label'] = pdf_ml.VecShowerPDG.map({11: 0, 13: 1, 22: 2, -211: 3})
pdf_ml['CoverS'] = pdf_ml.apply(lambda x: 0. if x.S_sum == 0. else x.C_sum / x.S_sum, axis=1)

print(pdf_ml.label.value_counts())

pdf_ml = pdf_ml.filter(items=['eventId', 'clusterId', 'PrimaryDecayMode', 'VecShowerEnergy', 'S_sum', 'C_sum', 'S_rad_mean', 'C_rad_mean', 'S_hot', 'C_hot', 'CoverS', 'dist2charge', 'label'])
pdf_ml.clusterId = pdf_ml.clusterId.astype(int)
pdf_ml.PrimaryDecayMode = pdf_ml.PrimaryDecayMode.astype(int)

print(pdf_ml)
pdf_ml.to_csv(os.path.join(path, "data.csv"), index=False)
