import sys
import ROOT
import numpy as np
from tableauColors import palette
from scipy import stats

assert len(sys.argv) == 2
treeName = "B4"

# leakage correction
leakage = "{}*1./(1.-LateralLeakage)"

# get calibration constants
fileName = sys.argv[1]

# create rdataframe
rdf = ROOT.ROOT.RDataFrame(treeName, fileName)

# define new columns and filter
rdf = rdf.Define("VecSignalScnt_corr", leakage.format("VecSignalScnt"))
rdf = rdf.Define("VecSignalCkov_corr", leakage.format("VecSignalCkov"))
rdf = rdf.Define("Ssum", "Sum(VecSignalScnt_corr)")
rdf = rdf.Define("Csum", "Sum(VecSignalCkov_corr)")
rdf = rdf.Filter("PrimaryPDG == 11", "only electrons")
rdf = rdf.Define("Snorm", "Ssum/PrimaryEnergy")
rdf = rdf.Define("Cnorm", "Csum/PrimaryEnergy")

# print cuts report
cutsReport = rdf.Report()
cutsReport.Print()

c = ROOT.TCanvas("c", "c", 1200, 600)
c.Divide(2)
col = ["Snorm", "Cnorm"]
d = [rdf, rdf]
h = [None, None]
stats1 = [None, None]
legend = [None, None]
mean = [None, None]
model = [("", "", 125, 5, 7.5), ("", "", 100, 0.10, 0.15)]
xlabel = ["Scintillation signal / E_{beam} [p.e. / MeV]", 
          "#check{C}erenkov signal / E_{beam}  [p.e. / MeV]"]
# title = ['Scintillator', '#check{C}erenkov']

ratio = 1.2
ratio2 = 1.2*ratio
linewidth = 2
x2ndc = 0.89+0.05
x1ndc = x2ndc-0.5
y2ndc = 0.88+0.05
linespacing = 0.05

for i in range(len(col)):
    c.cd(i+1)
    ROOT.gStyle.SetOptStat("e")
    ROOT.gStyle.SetOptFit(1)

    ROOT.gPad.SetLeftMargin(0.15)
    ROOT.gPad.SetBottomMargin(0.15)
    ROOT.gPad.SetRightMargin(0.05)
    ROOT.gPad.SetTopMargin(0.05)
    h[i] = d[i].Histo1D(model[i], col[i])
    # h[i] = d[i].Histo1D(col[i])

    # h[i].SetTitle(title[i])
    # h[i].SetTitleSize(ratio2 * h[i].GetTitleSize())

    h[i].GetXaxis().SetNdivisions(505)
    # h[i].SetAxisRange(0., 140., "Y")

    h[i].GetXaxis().SetTitle(xlabel[i])
    h[i].GetYaxis().SetTitle(f"Events / {h[i].GetBinWidth(1):.4g}")

    h[i].GetXaxis().SetTitleSize(ratio2 * h[i].GetXaxis().GetTitleSize())
    h[i].GetYaxis().SetTitleSize(ratio2 * h[i].GetYaxis().GetTitleSize())

    labelsize = ratio * h[i].GetXaxis().GetLabelSize()
    h[i].GetXaxis().SetLabelSize(labelsize)
    h[i].GetYaxis().SetLabelSize(labelsize)

    labeloffset = (ratio + 1) * h[i].GetXaxis().GetLabelOffset()
    h[i].GetXaxis().SetLabelOffset(labeloffset)
    h[i].GetYaxis().SetLabelOffset(labeloffset)

    if i == 0:
        h[i].GetXaxis().SetTitleOffset(ratio2 * h[i].GetXaxis().GetTitleOffset())
    else:
        h[i].GetXaxis().SetTitleOffset((ratio2-0.1) * h[i].GetXaxis().GetTitleOffset())

    h[i].GetYaxis().SetTitleOffset(1.08 * h[0].GetXaxis().GetTitleOffset())

    h[i].GetXaxis().SetTickLength(0.7 * ratio * h[i].GetXaxis().GetTickLength())
    h[i].GetYaxis().SetTickLength(0.7 * ratio * h[i].GetYaxis().GetTickLength())

    h[i].GetXaxis().CenterTitle()
    h[i].GetYaxis().CenterTitle()
    
    h[i].DrawCopy("E1")
    r = h[i].Fit("gaus", "S")
    r.Print()

    assert h[i].GetBinContent(0) == 0 and h[i].GetBinContent(h[i].GetNbinsX()+1) == 0
    
    h[i].SetLineColor(ROOT.kBlack)
    h[i].SetLineWidth(linewidth)
    gaus = h[i].GetFunction("gaus")
    gaus.SetLineColor(palette['red'].GetNumber())
    gaus.SetLineWidth(linewidth)
    mean[i] = r.Parameter(1)

    # stat box
    c.Update()
    stats1[i] = h[i].GetListOfFunctions().FindObject("stats")
    h[i].GetListOfFunctions().Remove(stats1[i])
    h[i].SetStats(0)
    stats1[i].GetLineWith("Entries").SetTextColor(ROOT.kBlack)
    stats1[i].SetTextColor(palette['red'].GetNumber())

    Res = r.Parameter(2) / r.Parameter(1)
    ResErr = Res * (r.ParError(1) / r.Parameter(1) + r.ParError(2) / r.Parameter(2))

    if i == 1:
        stats1[i].GetListOfLines().Remove(stats1[i].GetLineWith("Sigma"))
        stats1[i].AddText(f"Sigma = {r.Parameter(2):.5f} #pm {r.ParError(2):.5f}")

    stats1[i].AddText(f"#sigma/#mu = {Res:.4f} #pm {ResErr:.4f}")
    stats1[i].SetX1NDC(x1ndc)
    stats1[i].SetY2NDC(y2ndc)
    stats1[i].SetX2NDC(x2ndc)
    stats1[i].SetY1NDC(stats1[i].GetY2NDC()-6 * ratio * linespacing)
    stats1[i].SetTextSize(labelsize)
    stats1[i].SetBorderSize(0)
    stats1[i].Draw()
    # add legend
    legend[i] = ROOT.TLegend(stats1[i].GetX2NDC()-0.4, 0.2, stats1[i].GetX2NDC(), 0.2+2 * ratio * linespacing)
    legend[i].SetFillColor(0)
    legend[i].SetBorderSize(0)
    legend[i].SetTextSize(labelsize)
    legend[i].AddEntry(h[i].GetValue(), "Simulated data", "l")
    legend[i].AddEntry(gaus, "Gaussian fit", "l")
    legend[i].Draw()
    
ROOT.gPad.Modified()
ROOT.gPad.Update()
c.Print("calibration.png")

cal = {"Scnt": mean[0], "Ckov": mean[1]}
print(cal)
np.save("calibration.pkl", cal)


# test
c2 = ROOT.TCanvas("c2", "c2", 600, 600)
h2 = rdf.Graph("Snorm", "Cnorm")
h2.Draw('ap')
c2.Print("test_cal.png")

# correlation coefficients
npy = rdf.AsNumpy(columns=["Snorm", "Cnorm"])
print(f"Correlation coefficient : {stats.pearsonr(x=npy['Snorm'], y=npy['Cnorm'])}")