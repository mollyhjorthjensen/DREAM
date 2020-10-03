# import libraries
import os
import ROOT
import numpy as np
from scipy import stats
from tableauColors import palette
from array import array
import copy
import sys

# leakage correction
leakage = "{}*1./(1.-LateralLeakage)"

# get chi constant
cal = np.load("calibration.pkl.npy", allow_pickle=True).item()
print(cal)

# get root files
assert len(sys.argv) == 3
treeName = "B4"
fileName1 = sys.argv[1]
fileName2 = sys.argv[2]

# create rdataframe
rdf_e = ROOT.ROOT.RDataFrame(treeName, fileName1)
rdf_pi = ROOT.ROOT.RDataFrame(treeName, fileName2)

c4 = ROOT.TCanvas("c4", "c4", 600, 600)
c4.SetLeftMargin(0.15)
c4.SetRightMargin(0.05)
c4.SetBottomMargin(0.15)
c4.SetTopMargin(0.05)
mg = ROOT.TMultiGraph()

ROOT.gStyle.SetOptFit(1111)

rdf = {'e-': rdf_e, 'pi-': rdf_pi}
gr = [None, None]
for i,pdg,s in zip([0, 1], [11, -211], ['e-', 'pi-']):
    rdf[s] = rdf[s].Define("VecSignalScnt_corr", leakage.format("VecSignalScnt"))
    rdf[s] = rdf[s].Define("VecSignalCkov_corr", leakage.format("VecSignalCkov"))
    rdf[s] = rdf[s].Define("VecSignalScnt_cal", f"VecSignalScnt_corr/{cal['Scnt']}")
    rdf[s] = rdf[s].Define("VecSignalCkov_cal", f"VecSignalCkov_corr/{cal['Ckov']}")
    rdf[s] = rdf[s].Define("Ssum", "Sum(VecSignalScnt_cal)")
    rdf[s] = rdf[s].Define("Csum", "Sum(VecSignalCkov_cal)")
    rdf[s] = rdf[s].Define("Snorm", f"Ssum / PrimaryEnergy")
    rdf[s] = rdf[s].Define("Cnorm", f"Csum / PrimaryEnergy")
    rdf[s] = rdf[s].Filter(f"PrimaryPDG == {pdg}", f"PDG = {pdg}")
    rdf[s] = rdf[s].Define("chi", "(Ssum-PrimaryEnergy)/(Csum-PrimaryEnergy)")

    # print cuts report
    cutsReport = rdf[s].Report()
    cutsReport.Print()

    gr[i] = rdf[s].Graph("Snorm", "Cnorm")
    mg.Add(gr[i].GetPtr())

    gr[i].SetMarkerStyle(6)
    gr[i].SetMarkerColor(palette[s].GetNumber())
    if s == 'pi-':
        # linear fit
        f = ROOT.TF1("f1","x/[0]+1-1/[0]", 0.85, 1.0)
        f.SetParameter(0, 0.15)
        f.SetParName(0, "#chi-value")
        r = gr[i].Fit("f1", "SR", "", 0.85, 1.0)
        # r2 = gr[i].Fit("pol1", "SR", "", 0.85, 1.0)
        # r = gr[i].Fit("pol1", "SR", "", 0.8, 1.0)
        r.Print()


mg.Draw("ap")

ratio = 1.2
ratio2 = 1.2*ratio
linewidth = 2
x2ndc = 0.89+0.05
x1ndc = x2ndc-0.5
y2ndc = 0.88+0.05
linespacing = 0.05

# labelsize = 0.03 * ratio
# titlesize = 0.04 * ratio
# titleoffset = 1.2 * ratio
# labeloffset = 0.005 * ratio * 1.5
# linewidth = 3
# ticklength = 0.03

mg.GetXaxis().SetTitle("S / E_{beam}")
mg.GetYaxis().SetTitle("C / E_{beam}")
mg.GetXaxis().CenterTitle()
mg.GetYaxis().CenterTitle()
mg.SetMinimum(0)
mg.SetMaximum(1.2)
mg.GetXaxis().SetLimits(0, 1.2)

labelsize = ratio * mg.GetXaxis().GetLabelSize()
labeloffset = (ratio + 1) * mg.GetXaxis().GetLabelOffset()

mg.GetXaxis().SetTitleSize(ratio2 * mg.GetXaxis().GetTitleSize())
mg.GetXaxis().SetLabelSize(labelsize)
mg.GetXaxis().SetLabelOffset(labeloffset)
mg.GetXaxis().SetTitleOffset(ratio2 * mg.GetXaxis().GetTitleOffset())

mg.GetYaxis().SetTitleSize(ratio2 * mg.GetYaxis().GetTitleSize())
mg.GetYaxis().SetLabelSize(labelsize)
mg.GetYaxis().SetLabelOffset(labeloffset)
mg.GetYaxis().SetTitleOffset(mg.GetXaxis().GetTitleOffset()+0.1)

mg.GetXaxis().SetTickLength(0.7 * ratio * mg.GetXaxis().GetTickLength())
mg.GetYaxis().SetTickLength(0.7 * ratio * mg.GetYaxis().GetTickLength())

# mg.GetXaxis().SetTickLength(ticklength)
# mg.GetYaxis().SetTickLength(ticklength)
# mg.GetXaxis().SetLabelSize(labelsize)
# mg.GetYaxis().SetLabelSize(labelsize)
# mg.GetXaxis().SetTitleSize(titlesize)
# mg.GetYaxis().SetTitleSize(titlesize)
# mg.GetXaxis().SetTitleOffset(titleoffset)
# mg.GetYaxis().SetTitleOffset(titleoffset)
# mg.GetXaxis().SetLabelOffset(labeloffset)
# mg.GetYaxis().SetLabelOffset(labeloffset)

x1ndc = 0.21-0.05+mg.GetYaxis().GetTickLength()

# stat box
c4.Update()
stats4 = gr[1].GetListOfFunctions().FindObject("stats")
gr[1].GetListOfFunctions().Remove(stats4)
ROOT.gStyle.SetOptFit(0)

stats4.GetListOfLines().Remove(stats4.GetLineWith("#chi-value"))
stats4.AddText(f"#chi-value = {r.Parameter(0):.4f} #pm {r.ParError(0):.4f}")

stats4.SetTextColor(palette['pi-'].GetNumber())
stats4.SetY2NDC(y2ndc)
stats4.SetX1NDC(x1ndc)
stats4.SetX2NDC(stats4.GetX1NDC()+0.47)
stats4.SetY1NDC(stats4.GetY2NDC()-3 * ratio * linespacing)
stats4.SetTextSize(labelsize)
stats4.SetBorderSize(0)
stats4.Draw()

# # add legend
# x2ndc = 0.89+0.05
# y1ndc = 0.21-0.05+mg.GetXaxis().GetTickLength()
# legend = ROOT.TLegend(x2ndc-0.25, y1ndc, x2ndc, y1ndc+2 * ratio * linespacing)
# legend.SetFillColor(0)
# legend.SetBorderSize(0)
# legend.SetTextSize(labelsize)
# legend.AddEntry(gr[0].GetValue(), f"Electron", "p")
# legend.AddEntry(gr[1].GetValue(), f"Pion", "p")
# legend.Draw()

# add legend
legend = ROOT.TLegend(stats4.GetX1NDC(), stats4.GetY1NDC()-2 * ratio * linespacing -0.03, 
                      stats4.GetX2NDC(), stats4.GetY1NDC() -0.03)
legend.SetFillColor(0)
legend.SetBorderSize(0)
legend.SetTextSize(labelsize)
legend.AddEntry(gr[0].GetValue(), f"Electron", "p")
legend.AddEntry(gr[1].GetValue(), f"Pion", "p")
legend.Draw()

c4.Draw()
c4.Print("hSC_100GeV.png")

c4 = ROOT.TCanvas("c4", "c4", 600, 600)
model = ("", "", 200, -2., 2.)
h4 = rdf['pi-'].Histo1D(model, "chi")
h4.Draw()
c4.Print("chi_test3.png")