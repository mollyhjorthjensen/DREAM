import ROOT
import pandas as pd
import numpy as np

pdf_true = pd.read_csv('truth.csv')
# remove showers not entering calorimeter
pdf_true = pdf_true[pdf_true.IsShower == 1]

pdf_cluster = pd.read_csv('clustering.csv')

pdf = pdf_true.merge(pdf_cluster, how='outer', on=['eventId'])

# remember that the signals are pooled before clustering (hence the factor 2)
pdf['dist'] = pdf.apply(lambda x: np.sqrt((x.true_comi/2-x.cluster_comi)**2+(x.true_comj/2-x.cluster_comj)**2) , axis=1)

print(pdf_true.shape, pdf_cluster.shape, pdf.shape)
print(pdf.head(20))
