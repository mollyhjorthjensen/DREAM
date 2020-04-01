#!/bin/bash

export LCGENV_PATH=/cvmfs/sft.cern.ch/lcg/releases
export PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/latest/x86_64-centos7/bin/:$PATH
source /cvmfs/sft.cern.ch/lcg/views/LCG_96b/x86_64-centos7-gcc8-opt/setup.sh
eval "`/cvmfs/sft.cern.ch/lcg/releases/lcgenv/latest/lcgenv -p LCG_96b x86_64-centos7-gcc8-opt HepMC`"

# http://ep-dep-sft.web.cern.ch/geant4-usage-cern (accessed 1 October 2019)
source /cvmfs/sft.cern.ch/lcg/contrib/gcc/8/x86_64-centos7-gcc8-opt/setup.sh            #set up compiler
source /cvmfs/geant4.cern.ch/geant4/10.5.p01/x86_64-centos7-gcc8-opt-MT/CMake-setup.sh  #set up environment for Geant4
export CXX=`which g++`                                                                  #tell CMake about compiler used 
export CC=`which gcc`
