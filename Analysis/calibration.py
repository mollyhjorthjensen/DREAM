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

assert len(sys.argv) == 3
treeName = "B4"

# get calibration constants
fileName = sys.argv[1]

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
stats1 = [None, None]
legend = [None, None]
mean = [None, None, None]
model = [("", "", 100, 0.17, 0.21), ("", "", 100, 8.2, 10.2)]
xlabel = ["Beam energy / scintillating p.e.", "Beam energy / Cerenkov p.e."]
for i in range(len(col)):
    c.cd(i+1)
    ROOT.gStyle.SetOptStat("e")  # "nemr ou"
    ROOT.gStyle.SetOptFit(1)
    ROOT.gPad.SetLeftMargin(0.15)
    ROOT.gPad.SetBottomMargin(0.15)
    ROOT.gPad.SetRightMargin(0.05)
    ROOT.gPad.SetTopMargin(0.05)
    h[i] = d[i].Histo1D(model[i], col[i])
    h[i].GetXaxis().SetTitle(xlabel[i])
    h[i].GetXaxis().SetTickLength(.02)
    h[i].GetXaxis().SetTitleSize(0.044)
    h[i].GetXaxis().SetLabelSize(0.035)
    h[i].GetXaxis().SetLabelOffset(0.012)
    h[i].GetXaxis().CenterTitle()
    h[i].GetXaxis().SetTitleOffset(1.15)

    #h[i].GetXaxis().SetNdivisions(1010)
    binwidth = h[i].GetBinWidth(1)
    h[i].SetAxisRange(0., 100., "Y")
    h[i].GetYaxis().SetTitle(f"Events / {binwidth:.4g}")
    h[i].GetYaxis().SetTickLength(.02)
    h[i].GetYaxis().SetTitleSize(0.044)
    h[i].GetYaxis().SetLabelSize(0.035)
    h[i].GetYaxis().SetLabelOffset(0.012)
    h[i].GetYaxis().CenterTitle()
    h[i].GetYaxis().SetTitleOffset(1.35)
    h[i].DrawCopy("E1")
    r = h[i].Fit("gaus", "S")
    r.Print()
    nbins = h[i].GetNbinsX()
    print(f"Underflow : {h[i].GetBinContent(0)}\t\tOverflow : {h[i].GetBinContent(nbins+1)}")
    h[i].SetLineColor(palette['blue'].GetNumber())
    h[i].SetLineWidth(2)
    gaus = h[i].GetFunction("gaus")
    gaus.SetLineColor(palette['red'].GetNumber())
    gaus.SetLineWidth(2)
    mean[i] = r.Parameter(1)
    rms = r.Parameter(2)
    # stat box
    c.Update()
    #ROOT.gStyle.SetOptStat("e")
#    stats1[i] = ROOT.gPad.GetPrimitive("stats")
    stats1[i] = h[i].GetListOfFunctions().FindObject("stats")
    h[i].GetListOfFunctions().Remove(stats1[i])
    h[i].SetStats(0)
    stats1[i].GetLineWith("Entries").SetTextColor(palette['blue'].GetNumber())
    stats1[i].SetTextColor(palette['red'].GetNumber())
    stats1[i].SetX2NDC(0.89+0.05)
    stats1[i].SetY2NDC(0.885+0.05)
    stats1[i].SetX1NDC(stats1[i].GetX2NDC()-0.42)
    stats1[i].SetY1NDC(stats1[i].GetY2NDC()-0.24)
    stats1[i].SetTextSize(0.035)
    stats1[i].SetBorderSize(0)
    stats1[i].Draw()
    # add legend
    legend[i] = ROOT.TLegend(stats1[i].GetX1NDC()+0.13, 0.21, stats1[i].GetX2NDC(), 0.31)
    legend[i].SetFillColor(0)
    legend[i].SetBorderSize(0)
    legend[i].SetTextSize(0.035)
    legend[i].AddEntry(h[i].GetValue(), "simulated data", "l")
    legend[i].AddEntry(gaus, "gaussian fit", "l")
    legend[i].Draw()
    
ROOT.gPad.Modified()
ROOT.gPad.Update()
c.Print("calibration.png")

cal = {"Scnt": mean[0], "Ckov": mean[1]}
print(cal)

# get chi constant
fileName2 = sys.argv[2]

# create rdataframe
rdf2 = ROOT.ROOT.RDataFrame(treeName, fileName2)

# define new columns and filter
rdf2 = rdf2.Define("VecSignalScnt_corr", leakageCorrection("VecSignalScnt"))
rdf2 = rdf2.Define("VecSignalCkov_corr", leakageCorrection("VecSignalCkov" ))
rdf2 = rdf2.Define("VecSignalScnt_cal", f"VecSignalScnt_corr*{cal['Scnt']}")
rdf2 = rdf2.Define("VecSignalCkov_cal", f"VecSignalCkov_corr*{cal['Ckov']}")
rdf2 = rdf2.Define("Ssum", "Sum(VecSignalScnt_cal)")
rdf2 = rdf2.Define("Csum", "Sum(VecSignalCkov_cal)")
rdf2 = rdf2.Filter("PrimaryPDG == -211", "only pions")
rdf2 = rdf2.Define("chi", "(Ssum-PrimaryEnergy)/(Csum-PrimaryEnergy)")

# modified Z score
rdf2 = rdf2.Filter(modifiedZscore(rdf2, "chi"), "outliers")

# print cuts report
cutsReport = rdf2.Report()
cutsReport.Print()

c2 = ROOT.TCanvas("c", "c", 400, 400)
ROOT.gStyle.SetOptStat("e")  # "nemr ou"
ROOT.gStyle.SetOptFit(1)
ROOT.gPad.SetLeftMargin(0.15)
ROOT.gPad.SetBottomMargin(0.15)
ROOT.gPad.SetRightMargin(0.05)
ROOT.gPad.SetTopMargin(0.05)
model2 = ("", "", 100, -0.1, 0.9)
h2 = rdf2.Histo1D(model2, "chi")
h2.GetXaxis().SetTitle("#chi")
h2.GetXaxis().SetTickLength(.02)
h2.GetXaxis().SetTitleSize(0.044)
h2.GetXaxis().SetLabelSize(0.035)
h2.GetXaxis().SetLabelOffset(0.012)
h2.GetXaxis().CenterTitle()
h2.GetXaxis().SetTitleOffset(1.15)
binwidth = h2.GetBinWidth(1)
h2.SetAxisRange(0., 100., "Y")
h2.GetYaxis().SetTitle(f"Events / {binwidth:.4g}")
h2.GetYaxis().SetTickLength(.02)
h2.GetYaxis().SetTitleSize(0.044)
h2.GetYaxis().SetLabelSize(0.035)
h2.GetYaxis().SetLabelOffset(0.012)
h2.GetYaxis().CenterTitle()
h2.GetYaxis().SetTitleOffset(1.35)
h2.DrawCopy("E1")
r2 = h2.Fit("gaus", "S")
r2.Print()
nbins = h2.GetNbinsX()
print(f"Underflow : {h2.GetBinContent(0)}\t\tOverflow : {h2.GetBinContent(nbins+1)}")
h2.SetLineColor(palette['blue'].GetNumber())
h2.SetLineWidth(2)
gaus2 = h2.GetFunction("gaus")
gaus2.SetLineColor(palette['red'].GetNumber())
gaus2.SetLineWidth(2)
mean[2] = r2.Parameter(1)
rms = r2.Parameter(2)
# stat box
c2.Update()
ROOT.gStyle.SetOptStat("e")
stats2 = ROOT.gPad.GetPrimitive("stats")
stats2 = h2.GetListOfFunctions().FindObject("stats")
h2.GetListOfFunctions().Remove(stats2)
h2.SetStats(0)
stats2.GetLineWith("Entries").SetTextColor(palette['blue'].GetNumber())
stats2.SetTextColor(palette['red'].GetNumber())
stats2.SetX2NDC(0.89+0.05)
stats2.SetY2NDC(0.885+0.05)
stats2.SetX1NDC(stats2.GetX2NDC()-0.42)
stats2.SetY1NDC(stats2.GetY2NDC()-0.24)
stats2.SetTextSize(0.035)
stats2.SetBorderSize(0)
stats2.Draw()
# add legend
legend2 = ROOT.TLegend(stats2.GetX1NDC()+0.13, 0.21, stats2.GetX2NDC(), 0.31)
legend2.SetFillColor(0)
legend2.SetBorderSize(0)
legend2.SetTextSize(0.035)
legend2.AddEntry(h2.GetValue(), "simulated data", "l")
legend2.AddEntry(gaus2, "gaussian fit", "l")
legend2.Draw()

ROOT.gPad.Modified()
ROOT.gPad.Update()
c2.Print("chi.png")


cal['chi'] = mean[2]
np.save("calibration.pkl", cal)


