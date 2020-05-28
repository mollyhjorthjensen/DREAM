import ROOT
from ROOT import TColor

palette = {
	'blue':	TColor(TColor.GetFreeColorIndex(),	31./256.,	119./256.,	180./256.	),
	'red':	TColor(TColor.GetFreeColorIndex(),      214./256.,	39./256., 	40./256.	),
}

d = ROOT.ROOT.RDataFrame("T", "htree.root")
d = d.Define("cosTheta", "cos(event.theta)")
d = d.Define("negHelMinus", "-event.helMinus")

c = ROOT.TCanvas("c", "c", 600, 600)
c.SetLeftMargin(.15)
c.SetBottomMargin(.15)
c.SetRightMargin(.05)
c.SetTopMargin(.05)
ROOT.gStyle.SetOptFit(1)
ROOT.gStyle.SetOptStat("e")
model = ("", "", 40, -1., 1.)
p1 = d.Profile1D(model, "cosTheta", "negHelMinus")
p2 = p1.Clone("p2")
p3 = p1.Clone("p3")

print(f"Underflow : {p1.GetBinContent(0)}\t\tOverflow : {p1.GetBinContent(p1.GetNbinsX()+1)}")
p1.Draw()

p1.SetLineColor(ROOT.kBlack)
p1.SetMarkerStyle(ROOT.kFullDotLarge)

# X axis
p1.GetXaxis().SetTitle("cos#kern[0.3]{#theta}_{#tau}")
p1.GetXaxis().SetTickLength(.02)
p1.GetXaxis().SetLabelSize(0.03)
p1.GetXaxis().CenterTitle()
p1.GetXaxis().SetTitleOffset(1.2)

# Y axis
p1.GetYaxis().SetTitle("P_{#tau} / " + str(p1.GetXaxis().GetBinWidth(0)))
p1.GetYaxis().SetRangeUser(-.5, .25)
p1.GetYaxis().SetTickLength(.02)
p1.GetYaxis().SetLabelSize(0.03)
p1.GetYaxis().CenterTitle()
p1.GetYaxis().SetTitleOffset(1.6)

#ROOT.gPad.SetTicks(1, 1)

p1.StatOverflows(ROOT.kTRUE)

v1 = ROOT.TF1("Ptau", "-([0]*(1+x**2)+2*[1]*x)/((1+x**2)+2*[0]*[1]*x)", -1, 1);
v1.SetParNames("#it{A}_{#tau}", "#it{A}_{e}")
v1.SetParameter(0, 0.1439)
v1.SetParError(0, 0.0043)
v1.SetParameter(1, 0.1498)
v1.SetParError(1, 0.0049)
v1.SetLineStyle(ROOT.kSolid)
v1.SetLineColor(palette['blue'].GetNumber())
v1.SetLineWidth(3)

v2 = ROOT.TF1("Ptau2", "-([0]*(1+x**2)+2*[0]*x)/((1+x**2)+2*[0]**2*x)", -1, 1);
v2.SetParNames("#it{A}_{#it{l}}")
v2.SetParameter(0, 0.1465)
v2.SetParError(0, 0.0033)
v2.SetLineStyle(7)
v2.SetLineColor(palette['red'].GetNumber())
v2.SetLineWidth(3)

p2.Draw("sames")
p2.StatOverflows(ROOT.kTRUE)
p2.SetLineColor(ROOT.kBlack)
p2.SetMarkerStyle(ROOT.kFullDotLarge)

r1 = p1.Fit(v1, "SEM+")
r1.Print()
r2 = p2.Fit(v2, "SEM+")
r2.Print()

p3.Draw("sames")
p3.StatOverflows(ROOT.kTRUE)
p3.SetLineColor(ROOT.kBlack)
p3.SetMarkerStyle(ROOT.kFullDotLarge)

c.Update()

stats1 = c.GetPrimitive("stats")
stats1.SetName("stats1")
listOfLines1 = stats1.GetListOfLines()
#myt1 = ROOT.TLatex(0, 0, "#sqrt{s} = 91.2 GeV")
#myt1.SetTextSize(0.03)
#myt1.SetTextFont(42)
#myt1.SetTextColor(ROOT.kBlack)
listOfLines1.First().SetTextColor(ROOT.kBlack)
#listOfLines1.AddFirst(myt1)
stats1.SetTextColor(palette['blue'].GetNumber())
stats1.SetX2NDC(0.89+0.05)
stats1.SetY2NDC(0.885+0.05)
stats1.SetX1NDC(stats1.GetX2NDC()-0.3)
stats1.SetY1NDC(stats1.GetY2NDC()-0.16)
stats1.SetTextSize(0.03)
stats1.SetBorderSize(0)
p1.SetStats(0)


ROOT.gStyle.SetOptStat("")
stats2 = c.GetPrimitive("stats")
stats2.SetName("stats2")
stats2.SetTextColor(palette['red'].GetNumber())
stats2.SetX2NDC(stats1.GetX2NDC())
stats2.SetY2NDC(stats1.GetY1NDC())
stats2.SetX1NDC(stats1.GetX1NDC())
stats2.SetY1NDC(stats1.GetY1NDC()-0.08)
stats2.SetTextSize(0.03)
stats2.SetBorderSize(0)
p2.SetStats(0);

## Add header
#header = ROOT.TLatex()
#header.SetTextSize(0.025)
#header.SetTextFont(42)
#header.SetTextColor(ROOT.kGray)
#x = c.GetLeftMargin()
#y = 1.-c.GetTopMargin()+0.01
#header.DrawLatexNDC(x, y, "FCC-ee Simulation (Pythia/Tauola)")

# Add legend
#x1 = c.GetLeftMargin()+0.1*(1-c.GetLeftMargin()-c.GetRightMargin())
#x2 = x1 + 0.5
#y2 = c.GetBottomMargin()+(1-c.GetBottomMargin()-c.GetTopMargin())/5.5
#y1 = y2 - 0.2
x1 = c.GetLeftMargin() + 0.03
x2 = x1 + 0.25
y1 = c.GetBottomMargin() + 0.03
y2 = y1 + 0.12
legend = ROOT.TLegend(x1, y1, x2, y2)
legend.SetFillColor(0)
#legend.SetTextAlign(ROOT.kHAlignLeft+ROOT.kVAlignTop)
legend.SetBorderSize(0)
legend.SetTextSize(0.03)
legend.AddEntry(p1.GetValue(), "simulated data")
legend.AddEntry(v1, "no universality", "l")
legend.AddEntry(v2, "universality", "l")
legend.Draw()

c.Modified();

c.SaveAs("TauPol.png")
c.Close()
