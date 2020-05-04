# Event generation and decay with Pythia/Tauola

## Setup Tauola environment
```
source setup_tauola.sh
```

## Compile taumain_pythia_example.cxx
```
make
```

## Execute taumain_pythia_example.exe
- Configure Pythia with taumain_pythia_example.conf.
- Redirect stdout to one file (.out) and stderror to another (.err).
- Outputs (i) tau- event subtree in HepMC event format to hepmc.dat, (ii)
(angle,helicity) to htree.root, and (iii) results from MC-TESTER to mc-tester.root.
```
./taumain_pythia_example.exe > taumain_pythia_example.out 2>taumain_pythia_example.err
```

## Generate TauPol.png
```
source ../analysis/setup_python.sh
python drawTauPol.py
```
