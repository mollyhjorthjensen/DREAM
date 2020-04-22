import ROOT
import numpy as np

rdf = ROOT.ROOT.RDataFrame("B4", "hepmc_validation/0_t0.root")

c = ROOT.TCanvas("", "", 1000, 500)
c.Divide(2)

rdf = rdf.Define("Ci", "VecIndexCkov / VoxelsAlongY")
rdf = rdf.Define("Cj", "VecIndexCkov % VoxelsAlongY")
rdf = rdf.Define("Si", "VecIndexScnt / VoxelsAlongY")
rdf = rdf.Define("Sj", "VecIndexScnt % VoxelsAlongY")

model = ("", "", 568, 0., 568., 568, 0., 568.)
c.cd(1)
hC = rdf.Range(1).Histo2D(model, "Cj", "Ci", "VecSignalCkov")
hC.Draw("COLZ")
c.cd(2)
hS = rdf.Range(1).Histo2D(model, "Sj", "Si", "VecSignalScnt")
hS.Draw("COLZ")
c.Print("test.png")

npy = rdf.AsNumpy(columns=["VecShowerPosition", "VecShowerCkovCoMi", 
	"VecShowerCkovCoMj", "VecShowerScntCoMi", "VecShowerScntCoMj"])

print([np.array(v[0]) for k,v in npy.items()])
