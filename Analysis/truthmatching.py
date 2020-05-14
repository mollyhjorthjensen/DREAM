import ROOT
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

pdf_true = pd.read_csv('truth.csv')
pdf_cluster = pd.read_csv('clustering.csv')

# remove showers not entering calorimeter
pdf_true = pdf_true[pdf_true.IsShower == 1]

# convert index to column
pdf_cluster['clusterId'] = pdf_cluster.index.astype(int)

pdf = pdf_true.merge(pdf_cluster, how='outer', on=['eventId'])

# remember that the signals are pooled before clustering (hence the factor 2)
euclidean_distance = lambda x: np.sqrt((x.true_comi/2-x.cluster_comi)**2+(x.true_comj/2-x.cluster_comj)**2)
pdf['dist'] = pdf.apply(euclidean_distance, axis=1)

def matching(x):
	lst = []
	while not x.empty:
		df = x.loc[x.dist.idxmin()]
		lst += [(df.clusterId.astype(int), df.showerId.astype(int), df.dist)]
		x = x[(x.showerId != df.showerId) & (x.clusterId != df.clusterId)]
	return lst

grouped_matching = pdf[(~pdf.showerId.isna()) & (~pdf.clusterId.isna())].groupby('eventId').apply(matching)

flat_list = [item for sublist in grouped_matching for item in sublist]
df_new = pd.DataFrame(flat_list, columns=['clusterId', 'showerId', 'dist'])
pdf_cluster_new = pdf_cluster.merge(df_new, how='left', on=['clusterId'])

pdf_final = pdf_true.merge(pdf_cluster_new, how='outer', on=['eventId', 'showerId'])
pdf_final['rad_mean'] = pdf_final.apply(lambda x: (x.S_sum*x.S_rad_mean+x.C_sum*x.C_rad_mean)/(x.S_sum+x.C_sum), axis=1)
pdf_final['dist_thresh'] = pdf_final.apply(lambda x: np.sqrt(x.rad_mean**2 + 1**2), axis=1)
pdf_final['showerIdBool'] = ~pdf_final.showerId.isna()
pdf_final['clusterIdBool'] = ~pdf_final.clusterId.isna() & ((pdf_final.dist < pdf_final.dist_thresh) | pdf_final.dist.isna())

def distance2charged(x):
	df = x.loc[x.IsCharged]
	x['dist2charge'] = x.apply(lambda x: (568/2)*np.sqrt(2) if df.empty else np.sqrt((df.cluster_comi-x.cluster_comi)**2+(df.cluster_comj-x.cluster_comj)**2), axis=1)
	return x

grouped_charged = pdf_final[~pdf_final.clusterId.isna()].groupby('eventId').apply(distance2charged)
grouped_charged = grouped_charged.filter(items=['eventId', 'clusterId', 'dist2charge'])
pdf_final = pdf_final.merge(grouped_charged, how='left', on=['eventId', 'clusterId'])

print(pdf_final.head())

confusion_matrix = pd.crosstab(pdf_final.showerIdBool, pdf_final.clusterIdBool, rownames=['Actual'], colnames=['Predicted'])
print(confusion_matrix)

# get dataframe when there is both a shower and cluster
pdf_ml = pdf_final[pdf_final.showerIdBool & pdf_final.clusterIdBool]
print(pdf_ml.PrimaryDecayMode.value_counts())
print(pdf_ml.VecShowerPDG.value_counts())

pdf_ml['label'] = pdf_ml.VecShowerPDG.map({11: 0, 13: 1, 22: 2, -211: 3})
pdf_ml['CoverS'] = pdf_ml.apply(lambda x: 0. if x.S_sum == 0. else x.C_sum / x.S_sum, axis=1)

print(pdf_ml.label.value_counts())

pdf_ml = pdf_ml.filter(items=['eventId', 'clusterId', 'PrimaryDecayMode', 'VecShowerEnergy', 'S_sum', 'S_rad_mean', 'S_hot', 'C_sum', 'C_rad_mean', 'C_hot', 'dist2charge', 'label'])
pdf_ml.clusterId = pdf_ml.clusterId.astype(int)
pdf_ml.PrimaryDecayMode = pdf_ml.PrimaryDecayMode.astype(int)

print(pdf_ml)
pdf_ml.to_csv("data.csv", index=False)
