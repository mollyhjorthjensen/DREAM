import ROOT
from tableauColors import palette

# d1 = {'S_{r}': 1202, 'C/S': 1090, 'S_{hot}': 981, 'C_{hot}': 949, 'C_{r}': 938}
label = {'C_{r}': "#it{C}_{#it{r}}", 'C/S': "#it{C/S}", 'S_{r}': "#it{S}_{#it{r}}",
         'S_{hot}': "#it{S}_{hot}", 'C_{hot}': "#it{C}_{hot}"}
# d1 = {'C_{r}': 330893.461, 'C/S': 47891.316, 'S_{r}': 26016.191, 'S_{hot}': 15927.954, 'C_{hot}': 15646.102}
# d2 = {'S_{r}': 0.019, 'C/S': 0.130, 'S_{hot}': 0.018, 'C_{hot}': 0.011, 'C_{r}': 0.211}
d1 = {'C_{r}': 88416.272, 'S_{r}': 11732.464, 'C/S': 2770.881, 'S_{hot}': 860.015, 'C_{hot}': 939.699}
d2 = {'S_{r}': 0.175, 'C_{r}': 0.175, 'C/S': 0.060, 'S_{hot}': 0.004, 'C_{hot}': 0.001}
d1 = {k: v/sum(d1.values()) for k,v in d1.items()}
d2 = {k: v/sum(d2.values()) for k,v in d2.items()}
print(d1)
print(d2)
print(sum(d1.values()), sum(d2.values()))

cb = ROOT.TCanvas("cb","cb",600,400)

ROOT.gPad.SetLeftMargin(0.15)
ROOT.gPad.SetRightMargin(0.05)
ROOT.gPad.SetBottomMargin(0.12)
ROOT.gPad.SetTopMargin(0.08)

cb.SetTicks(1,1)
# cb.SetGrid(0,1)

ROOT.gStyle.SetHistMinimumZero()

nx = len(d1)
h1b = ROOT.TH1F("LightGBM gain","",nx,0,nx)
h1b.SetFillColor(palette['blue'].GetNumber())
h1b.SetLineWidth(0)
h1b.SetBarWidth(0.3)
h1b.SetBarOffset(0.2)
h1b.SetStats(0)
h1b.SetMinimum(0.)
h1b.SetMaximum(1.0)

h2b = ROOT.TH1F("Sklearn permutation","",nx,0,nx)
h2b.SetFillColor(palette['cyan'].GetNumber())
h2b.SetLineWidth(0)
h2b.SetBarWidth(0.3)
h2b.SetBarOffset(0.5)
h2b.SetStats(0)

for i,(k,v) in enumerate(d1.items()):
    h1b.SetBinContent(i+1, v)
    h2b.SetBinContent(i+1, d2[k])
    h1b.GetXaxis().SetBinLabel(i+1, label[k])

h1b.Draw("bar2")
h2b.Draw("bar2 same")

ratio = 1.5
ratio2 = 1.2*ratio
linespacing = 0.05
linewidth = 2

h1b.GetYaxis().SetTitle('Normalised feature importance')

h1b.GetXaxis().SetTitleSize(ratio2 * h1b.GetXaxis().GetTitleSize())
h1b.GetYaxis().SetTitleSize(ratio2 * h1b.GetYaxis().GetTitleSize())

labelsize = ratio * h1b.GetXaxis().GetLabelSize()
h1b.GetXaxis().SetLabelSize(1.5*labelsize)
h1b.GetYaxis().SetLabelSize(labelsize)

labeloffset = (ratio) * h1b.GetXaxis().GetLabelOffset()
# labeloffset2 = (ratio + 1.5) * h1b.GetXaxis().GetLabelOffset()
h1b.GetXaxis().SetLabelOffset(labeloffset)
h1b.GetYaxis().SetLabelOffset(labeloffset)

# h1b.GetXaxis().SetTitleOffset(1.3 * h1b.GetXaxis().GetTitleOffset())
h1b.GetYaxis().SetTitleOffset(1. * h1b.GetXaxis().GetTitleOffset())
    
# hs[i].GetXaxis().SetTickLength(0.7 * ratio * hs[i].GetXaxis().GetTickLength())
# h1b.GetYaxis().SetTickLength(0.7 * ratio * h1b.GetYaxis().GetTickLength())

h1b.GetXaxis().SetTickLength(0.)
h1b.GetYaxis().SetTickLength(0.)

h1b.GetXaxis().CenterTitle()
h1b.GetYaxis().CenterTitle()

# x1 = ROOT.gPad.GetLeftMargin()+h1b.GetYaxis().GetTickLength()+0.005
# x2 = x1 + 0.3
x2 = 1-ROOT.gPad.GetRightMargin()
x1 = x2-0.38
y2 = 1-ROOT.gPad.GetTopMargin()-h1b.GetYaxis().GetTickLength()-0.015
y1 = y2-2*ratio*linespacing
legend = ROOT.gPad.BuildLegend(x1, y1, x2, y2, "", "f")
legend.SetBorderSize(0)
legend.SetMargin(0.25)
legend.SetTextSize(labelsize)
ROOT.gPad.Modified()
ROOT.gPad.Update()


cb.Print('feature_importance.png')
