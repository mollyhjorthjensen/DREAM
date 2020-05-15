import sys
import os
import numpy as np
import ROOT
import pandas as pd

energy_thresh = 200.

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)
 
d = ROOT.ROOT.RDataFrame(treeName, fileName)

cal = np.load("calibration.pkl.npy", allow_pickle=True).item()
print(cal)

# filter
is_neutrino_code = '''
using namespace ROOT::VecOps;
RVec<int> is_neutrino(RVec<int> &VecShowerPDG) {
  RVec<int> neutrino = {12, 14, 16};
  auto any = Map(VecShowerPDG, [&](int i){return Any(neutrino == abs(i));});
  return any;
}
'''
ROOT.gInterpreter.Declare(is_neutrino_code)

decay_mode_code = '''
std::map<int,int> mode2num = {{0,1}, {1,1}, {2,1}, {3,3}, {4,5}};
using namespace ROOT::VecOps;
bool check_decay_mode(int &PrimaryDecayMode, RVec<int> &VecShowerPDG) {
	return mode2num.at(PrimaryDecayMode) == VecShowerPDG.size();
}
'''
ROOT.gInterpreter.Declare(decay_mode_code)

d = d.Define("IsNeutrino", "is_neutrino(VecShowerPDG)")
d = d.Define("HasEntered", "(VecShowerScntCoMi != -1) || (VecShowerScntCoMj != -1)")
d = d.Define("IsShower", "(HasEntered == 1) && (IsNeutrino == 0)")
d = d.Define("IsCharged", "abs(VecShowerCharge) > 0.")
d = d.Define("VecShowerPDG_filtered", "Take(VecShowerPDG, Nonzero(IsShower))")
d = d.Filter("check_decay_mode(PrimaryDecayMode, VecShowerPDG_filtered)", "check decay mode")
d = d.Filter(f"All(Take(VecShowerEnergy, Nonzero(IsShower)) > {energy_thresh})", f"energy > {int(energy_thresh)} MeV")
d = d.Filter("(Sum(VecSignalScnt) > 0) || (Sum(VecSignalCkov) > 0)", "at least one signal")

# define new columns
d = d.Define("eventId", "rdfentry_")
d = d.Define("VecSignalScnt_corr", "VecSignalScnt*PrimaryEnergy/(PrimaryEnergy-LateralLeakage)")
d = d.Define("VecSignalCkov_corr", "VecSignalCkov*PrimaryEnergy/(PrimaryEnergy-LateralLeakage)")
d = d.Define("VecSignalScnt_cal", f"VecSignalScnt_corr*{cal['Scnt']}")
d = d.Define("VecSignalCkov_cal", f"VecSignalCkov_corr*{cal['Ckov']}")
d = d.Define("Ssum", "Sum(VecSignalScnt_cal)")
d = d.Define("Csum", "Sum(VecSignalCkov_cal)")
d = d.Define("true_comi", "(Ssum*VecShowerScntCoMi+Csum*VecShowerCkovCoMi)/(Ssum+Csum)")
d = d.Define("true_comj", "(Ssum*VecShowerScntCoMj+Csum*VecShowerCkovCoMj)/(Ssum+Csum)")

print('All stats:')
allCutsReport = d.Report()
allCutsReport.Print()

# save root file to be converted to tfrecord
base, ext = os.path.splitext(fileName)
d.Snapshot(treeName, base + "_filtered" + ext)

# save flattened ntuple to pandas dataframe
eventCols = ['eventId', 'PrimaryDecayMode']
showerCols = ['VecShowerPDG', 'IsCharged', 'VecShowerEnergy',
	      'IsShower', 'true_comi', 'true_comj']
cols = eventCols + showerCols
npy = d.AsNumpy(columns=cols)
cols = eventCols + ['showerId'] + showerCols
pdf = pd.DataFrame(columns=cols)
eventVars = zip(npy['eventId'], npy['PrimaryDecayMode'])
showerId = 0
for i, event in enumerate(eventVars):
	showerVars = zip(npy['VecShowerPDG'][i], npy['IsCharged'][i], npy['VecShowerEnergy'][i],
			 npy['IsShower'][i], npy['true_comi'][i], npy['true_comj'][i]) 
	for j, shower in enumerate(showerVars):
		new = list(event) + [showerId] + list(shower)
		pdfj = pd.DataFrame([new], columns=cols)
		pdf = pdf.append(pdfj)
		showerId += 1
pdf.reset_index(drop=True, inplace=True)
pdf.eventId = pdf.eventId.astype(int)
pdf = pdf[pdf.IsShower == 1]
pdf.drop(['IsShower'], inplace=True, axis=1)
print(pdf.head(20))
print(pdf.shape)

pdf.to_csv('truth.csv', index=False)

