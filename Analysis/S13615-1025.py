import numpy as np
import ROOT
from array import array
from scipy import interpolate
from tableauColors import palette

x_nm, y = np.loadtxt('S13615-1025.txt', unpack=True)

# E=hc/lambda
h_Planck = 6.62606896e-34  # joule*s
c_light = 2.99792458e+8  # m/s
hc = h_Planck * c_light  # joule*m
e_SI  = 1.602176487e-19  # joule/eV
x_eV = hc/(x_nm * 1e-9 * e_SI)

# interpolate to get pde for given photon energies
xvals = [2.034, 2.068, 2.103, 2.139,
	 2.177, 2.216, 2.256, 2.298,
	 2.341, 2.386, 2.433, 2.481,
	 2.532, 2.585, 2.640, 2.697,
	 2.757, 2.820, 2.885, 2.954,
	 3.026, 3.102, 3.181, 3.265,
	 3.353, 3.446, 3.545, 3.649,
	 3.760, 3.877, 4.002, 4.136]

f = interpolate.interp1d(x_eV, y, kind='linear', fill_value='extrapolate')
np.set_printoptions(precision=4, floatmode='maxprec_equal')
fxvals = f(xvals)
print(fxvals / 100)

ratio = 1.3
ratio2 = 1.2*ratio

# plot PDE vs photon energy
c = ROOT.TCanvas('c', 'c', 1000, 1000)
c.SetGrid()
ROOT.gPad.SetTicks(1, 1)

ROOT.gPad.SetLeftMargin(0.15)
ROOT.gPad.SetRightMargin(0.15)
ROOT.gPad.SetBottomMargin(0.15)
ROOT.gPad.SetTopMargin(0.15)
#gr = ROOT.TGraph(len(x_eV), array('d', x_eV), array('d', y))
gr = ROOT.TGraph(len(x_nm), array('d', x_nm), array('d', y))
gr1 = ROOT.TGraph(len(xvals), array('d', xvals), array('d', fxvals))
gr.SetTitle("")
#gr.GetXaxis().SetTitle('Photon energy (eV)')
gr.GetXaxis().SetTitle('Wavelength [nm]')
gr.GetYaxis().SetTitle('Photon detection efficiency [%]')
gr.SetLineWidth(5)
gr.SetLineColor(palette['neutral'].GetNumber())
gr.GetXaxis().SetLimits(300., 900.)
gr.GetHistogram().SetMinimum(0.)
gr.GetHistogram().SetMaximum(30.)
gr.GetXaxis().SetTickLength(ratio * gr.GetXaxis().GetTickLength())
gr.GetYaxis().SetTickLength(ratio * gr.GetYaxis().GetTickLength())
gr.GetXaxis().CenterTitle()
gr.GetYaxis().CenterTitle()

# c.SetFrameFillColor(palette['neutral'].GetNumber())
# c.SetFrameFillColorAlpha(0.2)

# alpha = 0.1
# adapt = ROOT.gROOT.GetColor(palette['neutral'].GetNumber())
# new_idx = ROOT.gROOT.GetListOfColors().GetLast() + 1
# trans = ROOT.TColor(new_idx, adapt.GetRed(), adapt.GetGreen(), adapt.GetBlue(), '', alpha)
c.SetFrameFillColor(19)

gr.GetXaxis().SetLabelOffset(3.5 * gr.GetXaxis().GetLabelOffset())
gr.GetYaxis().SetLabelOffset(3.5 * gr.GetYaxis().GetLabelOffset())
gr.GetXaxis().SetTitleOffset(ratio * gr.GetXaxis().GetTitleOffset())
gr.GetYaxis().SetTitleOffset(gr.GetXaxis().GetTitleOffset())
gr.GetXaxis().SetLabelSize(ratio * gr.GetXaxis().GetLabelSize())
gr.GetYaxis().SetLabelSize(ratio * gr.GetYaxis().GetLabelSize())
gr.GetXaxis().SetTitleSize(ratio2 * gr.GetXaxis().GetTitleSize())
gr.GetYaxis().SetTitleSize(ratio2 * gr.GetYaxis().GetTitleSize())
gr.Draw('al')
gr1.SetMarkerStyle(ROOT.kMultiply)
gr1.Draw('same p')
c.Update()
c.Print('S13615-1025.png')
