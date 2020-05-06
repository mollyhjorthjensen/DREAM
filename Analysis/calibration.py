import sys
import ROOT
import numpy as np
from tableauColors import palette

def leakageCorrection(col):
    return f"{col}*PrimaryEnergy/(PrimaryEnergy-LateralLeakage)"

def modifiedZscore(rdf, col, D=3.5):
    x = rdf.AsNumpy(columns=[col])[col]
    xtilde = np.median(x)
    MAD = np.median(np.abs(x-xtilde))
    return f"abs(0.6745*({col}-({xtilde}))/{MAD}) <= {D}"

fileName = sys.argv[1]
treeName = "B4"
assert(len(sys.argv) == 2)

# create rdataframe
rdf = ROOT.ROOT.RDataFrame(treeName, fileName)

# define new columns and filter
rdf = rdf.Define("VecSignalScnt_corr", leakageCorrection("VecSignalScnt"))
rdf = rdf.Define("VecSignalCkov_corr", leakageCorrection("VecSignalCkov" ))
rdf = rdf.Define("Ssum", "Sum(VecSignalScnt_corr)")
rdf = rdf.Define("Csum", "Sum(VecSignalCkov_corr)")
rdf = rdf.Filter("PrimaryPDG == 11", "only electrons")
rdf_scnt = rdf.Filter("Ssum > 0.", "any scintillating signal")
rdf_scnt = rdf_scnt.Define("Snorm", "PrimaryEnergy/Ssum")
rdf_ckov = rdf.Filter("Csum > 0.", "any Cerenkov signal")
rdf_ckov = rdf_ckov.Define("Cnorm", "PrimaryEnergy/Csum")

# modified Z score
rdf_scnt = rdf_scnt.Filter(modifiedZscore(rdf_scnt, "Snorm"), "outliers scintillating")
rdf_ckov = rdf_ckov.Filter(modifiedZscore(rdf_ckov, "Cnorm"), "outliers Cerenkov")

# print cuts report
cutsReport_scnt = rdf_scnt.Report()
cutsReport_scnt.Print()
cutsReport_ckov = rdf_ckov.Report()
cutsReport_ckov.Print()

c = ROOT.TCanvas("c", "c", 800, 400)
c.Divide(2)
col = ["Snorm", "Cnorm"]
d = [rdf_scnt, rdf_ckov]
h = [None, None]
mean = [None, None]
for i in range(len(col)):
    c.cd(i+1)
    ROOT.gStyle.SetOptStat("nemr ou")
    ROOT.gStyle.SetOptFit(1111)
    ROOT.gPad.SetLeftMargin(0.12)
    ROOT.gPad.SetBottomMargin(0.12)
    ROOT.gPad.SetRightMargin(0.08)
    ROOT.gPad.SetTopMargin(0.08)
    h[i] = d[i].Histo1D(col[i])
    h[i].GetXaxis().SetTitle(col[i])
    h[i].GetXaxis().SetTitleSize(0.04)
    h[i].GetYaxis().SetTitle("Events")
    h[i].GetYaxis().SetTitleSize(0.04)
    h[i].DrawCopy("E1")
    r = h[i].Fit("gaus", "S")
    h[i].SetLineColor(palette['blue'].GetNumber())
    h[i].SetLineWidth(2)
    gaus = h[i].GetFunction("gaus")
    gaus.SetLineColor(palette['red'].GetNumber())
    gaus.SetLineWidth(2)
    mean[i] = r.Parameter(1)
    rms = r.Parameter(2)
    # add legend
    legend = ROOT.TLegend(0.62, 0.2, 0.82, 0.3)
    legend.SetFillColor(0)
    legend.SetBorderSize(0)
    legend.SetTextSize(0.03)
    legend.AddEntry(h[i].GetValue(), "Simulated data", "l")
    legend.AddEntry(gaus, "Gaussian fit", "l")
    legend.Draw()
    
ROOT.gPad.Modified()
ROOT.gPad.Update()
c.Print("calibration.png")

cal = {"Scnt": mean[0], "Ckov": mean[1]}
print(cal)
np.save("calibration.pkl", cal)
