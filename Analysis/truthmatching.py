import ROOT
import pandas as pd
import numpy as np
import matplotlib.pyplot as plt

energy_thresh = 350

pdf_true = pd.read_csv('truth.csv')
pdf_cluster = pd.read_csv('clustering.csv')

# remove showers not entering calorimeter
pdf_true = pdf_true[pdf_true.IsShower == 1]

# convert index to column
pdf_cluster['clusterId'] = pdf_cluster.index

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

grouped_matching = pdf[~pdf.clusterId.isna()].groupby('eventId').apply(matching)

flat_list = [item for sublist in grouped_matching for item in sublist]
df_new = pd.DataFrame(flat_list, columns=['clusterId', 'showerId', 'dist'])
pdf_cluster_new = pdf_cluster.merge(df_new, how='left', on=['clusterId'])

pdf_final = pdf_true.merge(pdf_cluster_new, how='outer', on=['eventId', 'showerId'])
pdf_final['rad_mean'] = pdf_final.apply(lambda x: (x.S_sum*x.S_rad_mean+x.C_sum*x.C_rad_mean)/(x.S_sum+x.C_sum), axis=1)
pdf_final['dist_thresh'] = pdf_final.apply(lambda x: np.sqrt(x.rad_mean**2 + 1**2), axis=1)
pdf_final['showerIdBool'] = ~pdf_final.showerId.isna()
pdf_final['clusterIdBool'] = ~pdf_final.clusterId.isna() & ((pdf_final.dist < pdf_final.dist_thresh) | pdf_final.dist.isna())
pdf_final['IsCharged'] = pdf_final.VecShowerCharge.abs() > 0.

def distance2charged(x):
	df = x.loc[x.IsCharged]
	if df.empty:
		print("dataframe empty")
	x['dist2charge'] = x.apply(lambda x: np.nan if df.empty else np.sqrt((df.cluster_comi-x.cluster_comi)**2+(df.cluster_comj-x.cluster_comj)**2), axis=1)
	return x

grouped_charged = pdf_final[~pdf_final.clusterId.isna()].groupby('eventId').apply(distance2charged)
grouped_charged = grouped_charged.filter(items=['eventId', 'clusterId', 'dist2charge'])
pdf_final = pdf_final.merge(grouped_charged, how='left', on=['eventId', 'clusterId'])

# filtering particles by energy threshold
pdf_final = pdf_final.loc[(pdf_final.PrimaryEnergy > energy_thresh) | pdf_final.PrimaryEnergy.isna()]
# filtering off neutrinos
pdf_final = pdf_final.loc[(pdf_final.VecShowerPDG.abs() != 12) & (pdf_final.VecShowerPDG.abs() != 14) & (pdf_final.VecShowerPDG.abs() != 16)]

print(pdf_final.head())

confusion_matrix = pd.crosstab(pdf_final.showerIdBool, pdf_final.clusterIdBool, rownames=['Actual'], colnames=['Predicted'])
print(confusion_matrix)

counts = pdf_final[pdf_final.showerIdBool & pdf_final.clusterIdBool].PrimaryDecayMode.value_counts()
print("Decay mode counts:")
print(counts)

print(pdf_final.VecShowerPDG.value_counts())

pdf_final['label'] = pdf_final.VecShowerPDG.map({np.nan: 0, 11: 1, 13: 2, 22: 3, -211: 4})

print(pdf_final.label.value_counts())

pdf_ml = pdf_final[~pdf_final.clusterId.isna()].filter(items=['eventId', 'clusterId', 'S_sum', 'S_rad_mean', 'S_hot', 'C_sum', 'C_rad_mean', 'C_hot', 'dist2charge', 'label'])
pdf_ml['CoverS'] = pdf_ml.apply(lambda x: x.C_sum / x.S_sum, axis=1)

print(pdf_ml)
