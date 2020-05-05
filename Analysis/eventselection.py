import sys
import ROOT

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)
 
d = ROOT.ROOT.RDataFrame(treeName, fileName)

d = d.Filter("(Sum(VecSignalScnt) > 0) || (Sum(VecSignalCkov) > 0)", "At least one signal")

print('All stats:')
allCutsReport = d.Report()
allCutsReport.Print()