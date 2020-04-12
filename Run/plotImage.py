import ROOT

rdf = ROOT.ROOT.RDataFrame("B4", "0_t0.root")

c = ROOT.TCanvas("", "", 1000, 500)
c.Divide(2)

rdf = rdf.Define("Ci", "VectorIndexCerenkov / 568")
rdf = rdf.Define("Cj", "VectorIndexCerenkov % 568")
rdf = rdf.Define("Si", "VectorIndexScintillation / 568")
rdf = rdf.Define("Sj", "VectorIndexScintillation % 568")

model = ("", "", 568, 0., 568., 568, 0., 568.)
c.cd(1)
hC = rdf.Histo2D(model, "Ci", "Cj", "VectorSignalCerenkov")
hC.Draw("COLZ")
c.cd(2)
hS = rdf.Histo2D(model, "Si", "Sj", "VectorSignalScintillation")
hS.Draw("COLZ")
c.Print("test.png")
