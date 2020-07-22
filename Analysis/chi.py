import sys
import ROOT
import numpy as np
from tableauColors import palette

# leakage correction
leakage = "{}*1./(1.-LateralLeakage)"

# get chi constant
cal = np.load("calibration.pkl.npy", allow_pickle=True).item()
print(cal)

# get root file
assert len(sys.argv) == 2
treeName = "B4"
fileName2 = sys.argv[1]

# create rdataframe
rdf2 = ROOT.ROOT.RDataFrame(treeName, fileName2)

# define new columns and filter
rdf2 = rdf2.Define("VecSignalScnt_corr", leakage.format("VecSignalScnt"))
rdf2 = rdf2.Define("VecSignalCkov_corr", leakage.format("VecSignalCkov"))
rdf2 = rdf2.Define("VecSignalScnt_cal", f"VecSignalScnt_corr/{cal['Scnt']}")
rdf2 = rdf2.Define("VecSignalCkov_cal", f"VecSignalCkov_corr/{cal['Ckov']}")
rdf2 = rdf2.Define("Ssum", "Sum(VecSignalScnt_cal)")
rdf2 = rdf2.Define("Csum", "Sum(VecSignalCkov_cal)")
rdf2 = rdf2.Filter("PrimaryPDG == -211", "only pions")
rdf2 = rdf2.Define("chi", "(Ssum-PrimaryEnergy)/(Csum-PrimaryEnergy)")

# print cuts report
cutsReport = rdf2.Report()
cutsReport.Print()

ratio = 1.2
ratio2 = 1.2*ratio
linewidth = 2
x2ndc = 0.89+0.05
x1ndc = x2ndc-0.5
y2ndc = 0.88+0.05
linespacing = 0.05

c2 = ROOT.TCanvas("c", "c", 600, 600)
ROOT.gStyle.SetOptStat("e ou")  # "nemr ou"
ROOT.gStyle.SetOptFit(1)
ROOT.gPad.SetLeftMargin(0.15)
ROOT.gPad.SetBottomMargin(0.15)
ROOT.gPad.SetRightMargin(0.05)
ROOT.gPad.SetTopMargin(0.05)
model2 = ("", "", 90, -0.4, 1.4)
h2 = rdf2.Histo1D(model2, "chi")

# assert h2.GetBinContent(0) == 0 
# assert h2.GetBinContent(h2.GetNbinsX()+1) == 0


# labelsize = 1 * h2.GetYaxis().GetLabelSize()
# titlesize = 1.2 * h2.GetYaxis().GetTitleSize()
# xtitleoffset = 1.3 * h2.GetXaxis().GetTitleOffset()
# ytitleoffset = 1.1 * xtitleoffset
# xlabeloffset = 2.05 * h2.GetXaxis().GetLabelOffset()
# ylabeloffset = 2.05 * h2.GetYaxis().GetLabelOffset()
# linewidth = 3
# x2ndc = 0.89+0.05
# y2ndc = 0.88+0.05

labelsize = ratio * h2.GetXaxis().GetLabelSize()
labeloffset = (ratio + 1) * h2.GetXaxis().GetLabelOffset()

h2.GetXaxis().SetTitle("#chi")
h2.GetXaxis().SetTitleSize(ratio2 * h2.GetXaxis().GetTitleSize())
h2.GetXaxis().SetLabelSize(labelsize)
h2.GetXaxis().SetLabelOffset(labeloffset)
h2.GetXaxis().CenterTitle()
h2.GetXaxis().SetTitleOffset(ratio2 * h2.GetXaxis().GetTitleOffset())

h2.SetAxisRange(0., 160., "Y")
h2.GetYaxis().SetTitle(f"Events / {h2.GetBinWidth(1):.4g}")
h2.GetYaxis().SetTitleSize(ratio2 * h2.GetYaxis().GetTitleSize())
h2.GetYaxis().SetLabelSize(labelsize)
h2.GetYaxis().SetLabelOffset(labeloffset)
h2.GetYaxis().CenterTitle()
h2.GetYaxis().SetTitleOffset(h2.GetXaxis().GetTitleOffset())

h2.GetXaxis().SetTickLength(0.7 * ratio * h2.GetXaxis().GetTickLength())
h2.GetYaxis().SetTickLength(0.7 * ratio * h2.GetYaxis().GetTickLength())

h2.DrawCopy("E1")
r2 = h2.Fit("gaus", "S")
r2.Print()
nbins = h2.GetNbinsX()
print(f"Underflow : {h2.GetBinContent(0)}\t\tOverflow : {h2.GetBinContent(nbins+1)}")
h2.SetLineColor(ROOT.kBlack)
h2.SetLineWidth(linewidth)
gaus2 = h2.GetFunction("gaus")
gaus2.SetLineColor(palette['red'].GetNumber())
gaus2.SetLineWidth(linewidth)
cal['Chi'] = r2.Parameter(1)

# stat box
Res = r2.Parameter(2) / r2.Parameter(1)
ResErr = Res * (r2.ParError(1) / r2.Parameter(1) + r2.ParError(2) / r2.Parameter(2))

c2.Update()
ROOT.gStyle.SetOptStat("e ou")
stats2 = ROOT.gPad.GetPrimitive("stats")
stats2 = h2.GetListOfFunctions().FindObject("stats")
h2.GetListOfFunctions().Remove(stats2)
stats2.AddText(f"#sigma/#mu = {Res:.4f} #pm {ResErr:.4f}")

h2.SetStats(0)
stats2.GetLineWith("Entries").SetTextColor(ROOT.kBlack)
stats2.GetLineWith("Overflow").SetTextColor(ROOT.kBlack)
stats2.GetLineWith("Underflow").SetTextColor(ROOT.kBlack)
stats2.SetTextColor(palette['red'].GetNumber())
stats2.SetX2NDC(x2ndc)
stats2.SetY2NDC(y2ndc)
stats2.SetX1NDC(stats2.GetX2NDC()-0.42)
stats2.SetY1NDC(stats2.GetY2NDC()-8 * ratio * linespacing)
stats2.SetTextSize(labelsize)
stats2.SetBorderSize(0)
stats2.Draw()
# add legend
# legend2 = ROOT.TLegend(stats2.GetX1NDC()+2 * ratio * linespacing, 0.2, stats2.GetX2NDC(), 0.3)
legend2 = ROOT.TLegend(stats2.GetX2NDC()-0.4, 0.2, stats2.GetX2NDC(), 0.2+2 * ratio * linespacing)
legend2.SetFillColor(0)
legend2.SetBorderSize(0)
legend2.SetTextSize(labelsize)
legend2.AddEntry(h2.GetValue(), "Simulated data", "l")
legend2.AddEntry(gaus2, "Gaussian fit", "l")
legend2.Draw()

ROOT.gPad.Modified()
ROOT.gPad.Update()
c2.Print("chi.png")

print(cal)
np.save("calibration_chi.pkl", cal)

ROOT.gStyle.SetOptStat("enmr")
c3 = ROOT.TCanvas("c3", "c3", 1200, 600)
c3.Divide(2)
c3.cd(1)
h3 = rdf2.Histo1D("Ssum")
h3.Draw()
c3.cd(2)
h4 = rdf2.Histo1D("Csum")
h4.Draw()
c3.Print("chi_test.png")

c4 = ROOT.TCanvas("c4", "c4", 600, 600)
h4 = rdf2.Graph("Ssum", "Csum")
h4.Draw()
c4.Print("chi_test2.png")