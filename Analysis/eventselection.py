import sys
import os
import numpy as np
import ROOT
import pandas as pd

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)
 
d = ROOT.ROOT.RDataFrame(treeName, fileName)

cal = np.load("calibration.pkl.npy", allow_pickle=True).item()
print(cal)

# filter
d = d.Filter("(Sum(VecSignalScnt) > 0) || (Sum(VecSignalCkov) > 0)", "at least one signal")

# define new columns
d = d.Define("eventId", "rdfentry_")
d = d.Define("VecSignalScnt_corr", "VecSignalScnt/(VecSignalScnt-LateralLeakage)")
d = d.Define("VecSignalCkov_corr", "VecSignalCkov/(VecSignalCkov-LateralLeakage)")
d = d.Define("VecSignalScnt_cal", f"VecSignalScnt_corr*{cal['Scnt']}")
d = d.Define("VecSignalCkov_cal", f"VecSignalCkov_corr*{cal['Ckov']}")

print('All stats:')
allCutsReport = d.Report()
allCutsReport.Print()

# save root file to be converted to tfrecord
base, ext = os.path.splitext(fileName)
d.Snapshot(treeName, base + "_filtered" + ext)

# save flattened ntuple to pandas dataframe
eventCols = ['eventId', 'PrimaryPDG', 'PrimaryEnergy', 'PrimaryDecayMode']
showerCols = ['VecShowerPDG', 'VecShowerCharge', 'VecShowerEnergy', 'VecShowerScntCoMi',
	      'VecShowerScntCoMj', 'VecShowerCkovCoMi', 'VecShowerCkovCoMj']
cols = eventCols + showerCols
npy = d.AsNumpy(columns=cols)
cols = eventCols + ['showerId'] + showerCols
pdf = pd.DataFrame(columns=cols)
eventVars = zip(npy['eventId'], npy['PrimaryPDG'], npy['PrimaryEnergy'], npy['PrimaryDecayMode'])
showerId = 0
for i, event in enumerate(eventVars):
	showerVars = zip(npy['VecShowerPDG'][i], npy['VecShowerCharge'][i], npy['VecShowerEnergy'][i],
			 npy['VecShowerScntCoMi'][i], npy['VecShowerScntCoMj'][i],
			 npy['VecShowerCkovCoMi'][i], npy['VecShowerCkovCoMj'][i]) 
	for j, shower in enumerate(showerVars):
		new = list(event) + [showerId] + list(shower)
		pdfj = pd.DataFrame([new], columns=cols)
		pdf = pdf.append(pdfj)
		showerId += 1
pdf.reset_index(drop=True, inplace=True)
		
print(pdf.head())
print(pdf.shape)

pdf.to_csv('truth.csv', index=False)