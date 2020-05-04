SRCS=taumain_pythia_example.cxx
OBJS=$(subst .cxx,.o,$(SRCS))
EXE=$(subst .cxx,.exe,$(SRCS))

SHELL = /bin/sh
top_builddir=/afs/cern.ch/user/m/molly/private/sw/TAUOLA
LIBTOOL = $(SHELL) $(top_builddir)/libtool
ROOTINC = /cvmfs/sft.cern.ch/lcg/releases/ROOT/5.34.24-64287/x86_64-slc6-gcc49-opt/include
ROOTLIBS = -L/cvmfs/sft.cern.ch/lcg/releases/ROOT/5.34.24-64287/x86_64-slc6-gcc49-opt/lib -lGui -lCore -lCint -lRIO -lNet -lHist -lGraf -lGraf3d -lGpad -lTree -lRint -lPostscript -lMatrix -lPhysics -lMathCore -lThread -pthread -lm -ldl -rdynamic

### Tauola
prefix=$(TAUOLALOCATION)
INCLUDES = -I$(prefix)/include
AM_LDFLAGS = -R $(prefix)/lib
FLIBS = -lgfortran
LDADD = $(FLIBS) $(prefix)/lib/libTauolaCxxInterface.so $(prefix)/lib/libTauolaFortran.so

### HepMC
HEPMC_DIR=$(HEPMCLOCATION)
AM_LDFLAGS += -R $(HEPMC_DIR)/lib
INCLUDES += -I$(HEPMC_DIR)/include
LDADD += -L$(HEPMC_DIR)/lib -lHepMC

### Pythia8
PYTHIA8_DIR=$(PYTHIALOCATION)
AM_LDFLAGS  += -R $(PYTHIA8_DIR)/lib/archive
INCLUDES += -I$(PYTHIA8_DIR)/include
LDADD += -L$(PYTHIA8_DIR)/lib -lpythia8 -ldl

### MC-TESTER
MCTESTER_DIR=$(MCTESTERLOCATION)
AM_LDFLAGS  += -R $(MCTESTER_DIR)/lib
INCLUDES += -I$(PYTHIA8_DIR)/include -I$(MCTESTER_DIR)/include -I$(ROOTINC)
LDADD += -L$(MCTESTER_DIR)/lib -lHEPEvent -lHepMCEvent -lMCTester $(ROOTLIBS)

### All other examples require HepMC ###
am__append_1 = -R $(HEPMC_DIR)/lib
am__append_2 = -I$(HEPMC_DIR)/include
am__append_3 = -L$(HEPMC_DIR)/lib -lHepMC

### Taugun & taummk examples (require Pythia8) ###
am__append_5 = -R $(PYTHIA8_DIR)/lib/archive
am__append_6 = -I$(PYTHIA8_DIR)/include
am__append_7 = -L$(PYTHIA8_DIR)/lib -lpythia8 -ldl

### Main example (requires Pythia8 and MC-Tester) ###
am__append_9 = -R $(MCTESTER_DIR)/lib
am__append_10 = -I$(PYTHIA8_DIR)/include -I$(MCTESTER_DIR)/include -I$(ROOTINC)
am__append_11 = -L$(MCTESTER_DIR)/lib -lHEPEvent -lHepMCEvent -lMCTester $(ROOTLIBS)

###
DEPDIR = .deps

INCLUDES = -I$(prefix)/include $(am__append_2) $(am__append_6) $(am__append_10)
AM_LDFLAGS = -R $(prefix)/lib $(am__append_1) $(am__append_5) $(am__append_9)
LDADD = $(FLIBS) $(prefix)/lib/libTauolaCxxInterface.so $(prefix)/lib/libTauolaFortran.so $(am__append_3) $(am__append_7) $(am__append_11)

DEPENDENCIES = $(prefix)/lib/libTauolaCxxInterface.so $(prefix)/lib/libTauolaFortran.so

.SUFFIXES: .cxx .o

CFLAGS = -g -O2
CPPFLAGS = -I/afs/cern.ch/user/m/molly/private/sw/Pythia8/include -I/include -I/afs/cern.ch/user/m/molly/private/sw/HepMC/include 
CXX = /cvmfs/sft.cern.ch/lcg/contrib/gcc/4.9/x86_64-slc6/bin/g++
CXXFLAGS = -O2
DEFS = -DHAVE_CONFIG_H
DEFAULT_INCLUDES = -I. -I$(top_builddir)/config
am__mv = mv -f
CXXCOMPILE = $(CXX) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) $(CXXFLAGS)
CXXLINK = $(LIBTOOL) --tag=CXX --mode=link $(CXX) $(CXXFLAGS) $(AM_LDFLAGS) -o $@

$(EXE): $(OBJS) $(DEPENDENCIES)
	@rm -f $(EXE)
	$(CXXLINK) $(OBJS) $(LDADD)

include ./$(DEPDIR)/taumain_pythia_example.Po
.cxx.o:
	$(CXXCOMPILE) -MT $@ -MD -MP -MF $(DEPDIR)/$*.Tpo -c -o $@ $<
	$(am__mv) $(DEPDIR)/$*.Tpo $(DEPDIR)/$*.Po

clean:

.MAKE: $(EXE)

.PHONY: clean
