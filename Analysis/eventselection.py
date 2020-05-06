import sys
import os
import numpy as np
import ROOT

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)
 
d = ROOT.ROOT.RDataFrame(treeName, fileName)

cal = np.load("calibration.pkl.npy", allow_pickle=True).item()
print(cal)

d = d.Filter("(Sum(VecSignalScnt) > 0) || (Sum(VecSignalCkov) > 0)", "at least one signal")
d = d.Filter("Min(VecShowerEnergy) > 350.", "minimum shower energy 350 MeV")

d = d.Define("VecSignalScnt_cal", f"VecSignalScnt*{cal['Scnt']}")
d = d.Define("VecSignalCkov_cal", f"VecSignalCkov*{cal['Ckov']}")

base, ext = os.path.splitext(fileName)
d.Snapshot(treeName, base + "_filtered" + ext)

print('All stats:')
allCutsReport = d.Report()
allCutsReport.Print()