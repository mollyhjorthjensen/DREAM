import pandas as pd
import os
import sys
import matplotlib.pyplot as plt
import ROOT

path = sys.argv[1]
assert len(sys.argv) == 2

c = ROOT.TCanvas("c", "c", 1200, 600)
c.Divide(2)

f1 = ROOT.TF1("f1","x", 0., 100.)

tdf = {}

ratio = 1.2
ratio2 = 1.2*ratio

title = ['Scintillator', '#check{C}erenkov']

for i,s in enumerate(['S', 'C']):
    c.cd(i+1)

    ROOT.gPad.SetLeftMargin(0.15)
    ROOT.gPad.SetBottomMargin(0.15)
    ROOT.gPad.SetRightMargin(0.05)
    ROOT.gPad.SetTopMargin(0.05)

    filename = os.path.join(path, s+'_dist.csv')
    tdf[s] = ROOT.RDF.MakeCsvDataFrame(filename)
    
    model = ("", title[i], 100, 0., 100., 100, 0., 100.)
    ROOT.gPad.SetLogz()
    h = tdf[s].Histo2D(model, "threshold", s+"_dist")
    

    h.GetYaxis().SetTitle("Distance, d")
    h.GetXaxis().SetTitle("Threshold")

    h.GetXaxis().SetTitleSize(ratio2 * h.GetXaxis().GetTitleSize())
    h.GetYaxis().SetTitleSize(ratio2 * h.GetYaxis().GetTitleSize())

    labelsize = ratio * h.GetXaxis().GetLabelSize()
    h.GetXaxis().SetLabelSize(labelsize)
    h.GetYaxis().SetLabelSize(labelsize)

    labeloffset = (ratio + 1) * h.GetXaxis().GetLabelOffset()
    h.GetXaxis().SetLabelOffset(labeloffset)
    h.GetYaxis().SetLabelOffset(labeloffset)

    h.DrawCopy('COLZ')
    f1.Draw("l same")

c.Print("S_dist2.png")

# a = plt.hist2d(Sdf.S_dist.values, Sdf.threshold.values)
# plt.savefig("S_dist.png")