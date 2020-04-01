#!/bin/bash

export LCGENV_PATH=/cvmfs/sft.cern.ch/lcg/releases
export PATH=/cvmfs/sft.cern.ch/lcg/contrib/git/latest/x86_64-centos7/bin/:$PATH
source /cvmfs/sft.cern.ch/lcg/views/LCG_96python3/x86_64-centos7-gcc8-opt/setup.sh
eval "`/cvmfs/sft.cern.ch/lcg/releases/lcgenv/latest/lcgenv -p LCG_96python3 x86_64-centos7-gcc8-opt Python`"
eval "`/cvmfs/sft.cern.ch/lcg/releases/lcgenv/latest/lcgenv -p LCG_96python3 x86_64-centos7-gcc8-opt ROOT`"

