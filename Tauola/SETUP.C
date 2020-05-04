{
//look at tau decays
Setup::decay_particle=15;
Setup::mass_scale_on=true;
Setup::debug_mode=false;

// Setup histograms
int n_bins=60;
double default_min_bin=0.0;
double default_max_bin=1.1; //Max in GeV

Setup::SetHistogramDefaults(n_bins,default_min_bin,default_max_bin);


Setup::gen1_desc_1="Pythia 8.1 demo; e-e at 92GeV, $Z^0$ single production";
Setup::gen1_desc_2="$Z^0$ decay to $\\tau^\\pm$ exclusively. No $\\pi$ decays.";
Setup::gen1_desc_3="";

if (Setup::stage==0)
    printf("Setup loaded from SETUP.C, ANALYSIS stage.\n");
else 
    printf("Setup loaded from SETUP.C, GENERATION stage %i.\n",Setup::stage);

 Setup::SuppressDecay(111); // suppress pi0 decays

};

