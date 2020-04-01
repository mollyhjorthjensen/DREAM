#!/bin/bash

# http://ep-dep-sft.web.cern.ch/geant4-usage-cern (accessed 1 October 2019)
rm -r build
mkdir build
cd build
cmake -DGeant4_DIR=/cvmfs/geant4.cern.ch/geant4/10.5.p01/x86_64-centos7-gcc8-opt-MT ../B4
make
cd ..
