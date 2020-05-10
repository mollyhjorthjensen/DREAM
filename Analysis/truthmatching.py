import ROOT
import pandas as pd
import numpy as np

pdf_true = pd.read_csv('truth.csv')
# remove showers not entering calorimeter
pdf_true = pdf_true[pdf_true.IsShower == 1]

pdf_cluster = pd.read_csv('clustering.csv')

# convert index to column
pdf_cluster['clusterId'] = pdf_cluster.index

pdf = pdf_true.merge(pdf_cluster, how='outer', on=['eventId'])

# remember that the signals are pooled before clustering (hence the factor 2)
pdf['dist'] = pdf.apply(lambda x: np.sqrt((x.true_comi/2-x.cluster_comi)**2+(x.true_comj/2-x.cluster_comj)**2) , axis=1)

print(pdf_true.shape, pdf_cluster.shape, pdf.shape)
print(pdf.head(20))

def func(x):
	lst = []
	while not x.empty:
		df = x.loc[x.dist.idxmin()]
		lst += [(df.clusterId.astype(int), df.showerId.astype(int))]
		x = x[(x.showerId != df.showerId) & (x.clusterId != df.clusterId)]
	return lst

grouped = pdf[~pdf.clusterId.isna()].groupby('eventId').apply(func)
print(grouped)

flat_list = [item for sublist in grouped for item in sublist]
df_new = pd.DataFrame(flat_list, columns=['clusterId', 'showerId'])
pdf_cluster_new = pdf_cluster.merge(df_new, how='left', on=['clusterId'])
print(pdf_cluster_new.head())


pdf_final = pdf_true.merge(pdf_cluster_new, how='outer', on=['eventId', 'showerId'])

print(pdf_final.head())