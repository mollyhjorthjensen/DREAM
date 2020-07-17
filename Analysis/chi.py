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

# print cuts report
cutsReport = rdf2.Report()
cutsReport.Print()

c2 = ROOT.TCanvas("c", "c", 600, 600)
ROOT.gStyle.SetOptStat("e")  # "nemr ou"
ROOT.gStyle.SetOptFit(1)
ROOT.gPad.SetLeftMargin(0.15)
ROOT.gPad.SetBottomMargin(0.15)
ROOT.gPad.SetRightMargin(0.05)
ROOT.gPad.SetTopMargin(0.05)
model2 = ("", "", 100, -0.1, 0.9)
h2 = rdf2.Histo1D(model2, "chi")

labelsize = 1 * h2.GetYaxis().GetLabelSize()
titlesize = 1.2 * h2.GetYaxis().GetTitleSize()
xtitleoffset = 1.3 * h2.GetXaxis().GetTitleOffset()
ytitleoffset = 1.1 * xtitleoffset
xlabeloffset = 2.05 * h2.GetXaxis().GetLabelOffset()
ylabeloffset = 2.05 * h2.GetYaxis().GetLabelOffset()
linewidth = 3
x2ndc = 0.89+0.05
y2ndc = 0.88+0.05

h2.GetXaxis().SetTitle("#chi")
h2.GetXaxis().SetTitleSize(titlesize)
h2.GetXaxis().SetLabelSize(labelsize)
h2.GetXaxis().SetLabelOffset(xlabeloffset)
h2.GetXaxis().CenterTitle()
h2.GetXaxis().SetTitleOffset(xtitleoffset)
binwidth = h2.GetBinWidth(1)
h2.SetAxisRange(0., 100., "Y")
h2.GetYaxis().SetTitle(f"Events / {binwidth:.4g}")
h2.GetYaxis().SetTitleSize(titlesize)
h2.GetYaxis().SetLabelSize(labelsize)
h2.GetYaxis().SetLabelOffset(ylabeloffset)
h2.GetYaxis().CenterTitle()
h2.GetYaxis().SetTitleOffset(ytitleoffset)
h2.DrawCopy("E1")
r2 = h2.Fit("gaus", "S")
r2.Print()
nbins = h2.GetNbinsX()
print(f"Underflow : {h2.GetBinContent(0)}\t\tOverflow : {h2.GetBinContent(nbins+1)}")
h2.SetLineColor(palette['blue'].GetNumber())
h2.SetLineWidth(linewidth)
gaus2 = h2.GetFunction("gaus")
gaus2.SetLineColor(palette['red'].GetNumber())
gaus2.SetLineWidth(linewidth)
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
stats2.SetX2NDC(x2ndc)
stats2.SetY2NDC(y2ndc)
stats2.SetX1NDC(stats2.GetX2NDC()-0.42)
stats2.SetY1NDC(stats2.GetY2NDC()-0.25)
stats2.SetTextSize(labelsize)
stats2.SetBorderSize(0)
stats2.Draw()
# add legend
legend2 = ROOT.TLegend(stats2.GetX1NDC()+0.12, 0.2, stats2.GetX2NDC(), 0.3)
legend2.SetFillColor(0)
legend2.SetBorderSize(0)
legend2.SetTextSize(labelsize)
legend2.AddEntry(h2.GetValue(), "simulated data", "l")
legend2.AddEntry(gaus2, "gaussian fit", "l")
legend2.Draw()

ROOT.gPad.Modified()
ROOT.gPad.Update()
c2.Print("chi.png")


cal['chi'] = mean[2]
np.save("calibration.pkl", cal)

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