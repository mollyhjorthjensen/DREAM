/**
 * Example of use of tauola C++ interface. Pythia events are
 * generated with and without tau decays. Events with taus decayed
 * by pythia are compared against events with taus decayed via tauola.
 *
 * Fraction of energies carried by principal charged decay products to
 * tau energy is printed; respectively for pythia and tauola tau decays.
 *
 * @author Nadia Davidson  Mikhail Kirsanov Tomasz Przedzinski
 * @date 29 July 2010
 */

#include "Tauola/Log.h"
#include "Tauola/Plots.h"
#include "Tauola/Tauola.h"
#include "Tauola/TauolaHepMCEvent.h"
#include "tauola_print_parameters.h"

// Pythia8 header files
#include "Pythia8/Pythia.h"
#include "Pythia8Plugins/HepMC2.h"

// HepMC header files
#include "HepMC/IO_AsciiParticles.h"
#include "HepMC/IO_GenEvent.h"
#include "HepMC/GenRanges.h"

// MC-TESTER header files
#include "Generate.h"
#include "HepMCEvent.H"
#include "Setup.H"

// ROOT
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "TRandom.h"
#include "TMath.h"
#include <limits>
#include <algorithm>

using namespace std;
using namespace Pythia8;
using namespace Tauolapp;

int NumberOfEvents = 1000;

int main(int argc,char **argv)
{
  TFile hfile("htree.root","RECREATE","Demo ROOT file with histograms & trees");
  TTree tree("T","An example of ROOT tree with a few branches");
  typedef struct {Int_t helPlus, helMinus; Double_t EWwt, EWwt0, theta;} EVENT;
  EVENT root_event;
  tree.Branch("event", &root_event, "helPlus/I:helMinus/I:EWwt/D:EWwt0/D:theta/D");

  Log::SummaryAtExit();
  Log::SetWarningLimit(10);

  Pythia pythia;
  pythia.readFile("taumain_pythia_example.conf");
  Event& event = pythia.event; 
  HepMC::Pythia8ToHepMC ToHepMC;
  HepMC::IO_AsciiParticles ascii_io1("cout", std::ios::out);
  HepMC::IO_GenEvent ascii("hepmc.dat", ios::out);
  pythia.init();
  
  // Set up TAUOLA

  // set seed for tauola-fortran random number generator RANMAR
  Tauola::setSeed(1,2,3);

  // Radiative corrections
  Tauola::setRadiation(false);

  Tauola::initialize();
 
  // Set BR
  int modes[] = {Tauola::ElectronMode, Tauola::MuonMode, Tauola::PionMode, Tauola::RhoMode, Tauola::A1Mode};
  int num_modes = sizeof(modes)/sizeof(modes[0]);
  double p = 1./double(num_modes); 
  cout << "Number of selected decay modes : " << num_modes << endl;
  for (int i=1; i<23; ++i) {
    if (std::find(modes, modes+num_modes, i) == modes+num_modes) { 
      cout << i << endl;
      Tauola::setTauBr(i, 0.);  // AFTER Tauola::initialize(), see Appendix C.2
    } else  {
      Tauola::setTauBr(i, p);  // AFTER Tauola::initialize(), see Appendix C.2
    }
  }

  // Change the relative branching ratio pi- pi- pi+ / pi- pi0 pi0 (Tauola::A1Mode)
  Tauola::setTaukle(0., 0.5, 0.5, 0.6667);  // AFTER Tauola::initialize(), see Appendix C.2
 
//  Tauola::setEtaK0sPi(0, 0, 0);  // switches to decay eta K0_S and pi0 1/0 on/off, see Appendix C.5

  tauola_print_parameters();  // Prints TAUOLA  parameters (residing inside its library): e.g. to test user interface
  Tauola::setUnits(Tauola::GEV,Tauola::MM);
 
// --- Event loop with pythia and tauola --------------------------------------
 
  MC_Initialize();

  for (int iEvent = 0; iEvent < NumberOfEvents; ++iEvent) {
    if (!pythia.next()) continue;
   
    // rotate entire event such that tau minus is directed along z-axis 
    for (int i=0; i<event.size(); ++i) {
      if (event[i].id() == 15) {
	root_event.theta = event[i].theta();
	event.rot(0., -event[i].phi());
	event.rot(-event[i].theta(), 0.);
        Log::Assert(std::abs(event[i].pz()) == event[i].pAbs(), const_cast<char*>("tau- not directed along z-axis"));
	//event.list();
	break;
      }
    }

    // convert event record to HepMC
    HepMC::GenEvent* HepMCEvt = new HepMC::GenEvent();
    HepMCEvt->use_units(HepMC::Units::GEV, HepMC::Units::MM);
    ToHepMC.fill_next_event(event, HepMCEvt);
    event.clear();

    // decay taus with tauola
    TauolaHepMCEvent* t_event = new TauolaHepMCEvent(HepMCEvt);
    t_event->undecayTaus();  // troubleshooting
    t_event->decayTaus();
    delete t_event;
    
    root_event.helPlus = Tauola::getHelPlus();
    root_event.helMinus = Tauola::getHelMinus();
    root_event.EWwt = Tauola::getEWwt();
    root_event.EWwt0 = Tauola::getEWwt0();
    tree.Fill();

    // subtree of tau minus and descendants
    HepMC::GenEvent* descendants = new HepMC::GenEvent();
    HepMCEvt->use_units(HepMC::Units::GEV, HepMC::Units::MM);

    // find tau+ and tau-
    HepMC::GenParticle *tm, *tp;
    HepMC::GenEventParticleRange ep(*HepMCEvt);
    for (HepMC::GenEvent::particle_iterator p=ep.begin(); p!=ep.end(); ++p) {
      int pdg = (*p)->pdg_id();
      if (pdg == 15) tm = (*p);
      else if (pdg == -15) tp = (*p);
    }
    Log::Assert(tm, const_cast<char*>("missing tau- HepMC particle"));
    Log::Assert(tp, const_cast<char*>("missing tau+ HepMC particle"));
    
    // remove tau+ from tau production vertex
    Log::Assert(tm->production_vertex(), const_cast<char*>("missing tau- production vertex"));
    tm->production_vertex()->remove_particle(tp);

    // add production vertices of all tau- descendants
    HepMC::GenVertexParticleRange vp(*(tm->production_vertex()), HepMC::descendants);
    for (HepMC::GenVertex::particle_iterator p=vp.begin(); p!=vp.end(); ++p) {
      descendants->add_vertex((*p)->production_vertex());
    }

  // Run MC-TESTER on the event
    HepMCEvent temp_event(*descendants);
    MC_Analyze(&temp_event); 

    // print to HepMCEvt format
    ascii << descendants;

    // clean up HepMC event
    HepMCEvt->clear();
    delete HepMCEvt;
    delete tm;
  }
  tree.Print();
  hfile.Write();
  hfile.Close();
 
  pythia.stat();
  MC_Finalize();
  Tauola::summary();
}
