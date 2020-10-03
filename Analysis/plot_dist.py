import pandas as pd
import os
import sys
import matplotlib.pyplot as plt
import ROOT

path = sys.argv[1]
assert len(sys.argv) == 2

c = ROOT.TCanvas("c", "c", 1300, 600)
c.Divide(2)

f1 = ROOT.TF1("f1", "x", 0., 200.)

tdf = {}

ratio = 1.2
ratio2 = 1.2*ratio
linespacing = 0.05

title = ['Scintillator', '#check{C}erenkov']

h = [None, None]

titlesize = [1.1, 0.9]
offset = [0., 0.005]

for i,s in enumerate(['S', 'C']):
    c.cd(i+1)

    ROOT.gPad.SetLeftMargin(0.12*75/60)
    ROOT.gPad.SetRightMargin(0.18*75/60)
    if i == 1:
        ROOT.gPad.SetLeftMargin(0.15*75/60)
        ROOT.gPad.SetRightMargin(0.15*75/60)
    ROOT.gPad.SetBottomMargin(0.15)
    ROOT.gPad.SetTopMargin(0.15)

    filename = os.path.join(path, s+'_dist.csv')
    tdf[s] = ROOT.RDF.MakeCsvDataFrame(filename)
    tdf[s] = tdf[s].Define("threshold_mm", "threshold*3.")
    tdf[s] = tdf[s].Define(s+"_dist_mm", s+"_dist*3.")
    entries = tdf[s].Count().GetValue()
    print(f"Total entries : {entries}")
    entries_below = tdf[s].Filter(s+"_dist_mm<threshold_mm").Count().GetValue()
    print(f"Entries below line : {entries_below} ({entries_below/entries*100:.1f} %)")

    model = ("", title[i], 100, 0., 200., 100, 0., 200.)
    ROOT.gPad.SetLogz()
    h[i] = tdf[s].Histo2D(model, "threshold_mm", s+"_dist_mm")    
    
    h[i].SetAxisRange(0., 1e4, "Z")
    
    h[i].GetXaxis().SetTitle("Threshold, #it{t}_{#lower[-0.15]{dist}} [mm]")
    h[i].GetYaxis().SetTitle("Distance, #it{d} [mm]")
    h[i].GetZaxis().SetTitle(f"Events / {h[i].GetXaxis().GetBinWidth(1):.1g} mm #times {h[i].GetYaxis().GetBinWidth(1):.1g} mm")

    h[i].GetXaxis().SetTitleSize(ratio2 * h[i].GetXaxis().GetTitleSize())
    h[i].GetYaxis().SetTitleSize(ratio2 * h[i].GetYaxis().GetTitleSize())
    h[i].GetZaxis().SetTitleSize(ratio2 * h[i].GetZaxis().GetTitleSize())

    labelsize = ratio * h[i].GetXaxis().GetLabelSize()
    h[i].GetXaxis().SetLabelSize(labelsize)
    h[i].GetYaxis().SetLabelSize(labelsize)
    h[i].GetZaxis().SetLabelSize(labelsize)

    labeloffset = (ratio + 1) * h[i].GetXaxis().GetLabelOffset()
    h[i].GetXaxis().SetLabelOffset(labeloffset)
    h[i].GetYaxis().SetLabelOffset(labeloffset)
    # h[i].GetZaxis().SetLabelOffset(labeloffset)

    h[i].GetXaxis().SetTitleOffset(ratio2 * h[i].GetXaxis().GetTitleOffset())
    h[i].GetYaxis().SetTitleOffset(1.12 * h[i].GetXaxis().GetTitleOffset())
    h[i].GetZaxis().SetTitleOffset(1. * h[i].GetXaxis().GetTitleOffset())

    h[i].GetXaxis().SetTickLength(0.7 * ratio * h[i].GetXaxis().GetTickLength())
    h[i].GetYaxis().SetTickLength(0.7 * ratio * h[i].GetYaxis().GetTickLength())
    h[i].GetZaxis().SetTickLength(0.7 * ratio * h[i].GetZaxis().GetTickLength())

    h[i].GetXaxis().CenterTitle()
    h[i].GetYaxis().CenterTitle()
    h[i].GetZaxis().CenterTitle()

    h[i].Draw('COLZ')
    f1.Draw("l same")


    # pt = c.GetPrimitive("title")
    # pt.SetTextSize(0.5)
    # c.Modified()
    # h[i].SetTitleSize(2., 't')
    ROOT.gStyle.SetTitleFont(62, 't')
    ROOT.gStyle.SetTitleFontSize(titlesize[i] * ratio * h[i].GetXaxis().GetTitleSize())
    ROOT.gStyle.SetTitleX(ROOT.gPad.GetLeftMargin()+(1-ROOT.gPad.GetLeftMargin()-ROOT.gPad.GetRightMargin())/2)
    ROOT.gStyle.SetTitleY(1-ROOT.gPad.GetTopMargin()+h[i].GetXaxis().GetTickLength()+offset[i])
    ROOT.gStyle.SetTitleAlign(ROOT.kHAlignCenter+ROOT.kVAlignBottom)

    # stat box
    # c.Update()
    ROOT.gStyle.SetOptStat(0)
    # stats = h[i].GetListOfFunctions().FindObject("stats")
    # h[i].GetListOfFunctions().Remove(stats)
    # h[i].SetStats(0)
    # stats.GetLineWith("Entries").SetTextColor(ROOT.kRed)

    # stats.SetX1NDC(ROOT.gPad.GetLeftMargin()+h[i].GetYaxis().GetTickLength())
    # stats.SetX2NDC(stats.GetX1NDC()+0.3)
    # stats.SetY1NDC(1-ROOT.gPad.GetTopMargin())
    # stats.SetY2NDC(stats.GetY1NDC()+1 * ratio * linespacing)

    # stats.SetTextSize(labelsize)
    # stats.SetBorderSize(0)

    # stats.Draw()

    


c.Print("S_dist.png")