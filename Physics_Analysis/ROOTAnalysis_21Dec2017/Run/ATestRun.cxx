#include "PathResolver/PathResolver.h"

void ATestRun (const std::string& submitDir)
{
  //===========================================
  // FOR ROOT6 WE DO NOT PUT THIS LINE 
  // (ROOT6 uses Cling instead of CINT)
  // Load the libraries for all packages
  // gROOT->Macro("$ROOTCOREDIR/scripts/load_packages.C");
  // Instead on command line do:
  // > root -l '$ROOTCOREDIR/scripts/load_packages.C' 'ATestRun.cxx ("submitDir")'
  // The above works for ROOT6 and ROOT5
  //==========================================


  // Set up the job for xAOD access:
  xAOD::Init().ignore();

  // create a new sample handler to describe the data files we use
  SH::SampleHandler sh;
  
  // scan for datasets in the given directory
  // this works if you are on lxplus, otherwise you'd want to copy over files
  // to your local machine and use a local path.  if you do so, make sure
  // that you copy all subdirectories and point this to the directory
  // containing all the files, not the subdirectories.

  // use SampleHandler to scan all of the subdirectories of a directory for particular MC single file:
  std::string inputFilePath = PathResolverFindCalibDirectory("/eos/user/n/ndang/xTau/DAOD/mc15_13TeV.344084.PowhegPythia8EvtGen_CT10_AZNLOCTEQ6L1_ggH125_taumu.merge.DAOD_HIGG4D2.e5003_s2726_r7772_r7676_p2949/");
  SH::ScanDir().filePattern("DAOD_HIGG4D2.10353992*").scan(sh,inputFilePath);


  // set the name of the tree in our files
  // in the xAOD the TTree containing the EDM containers is "CollectionTree"
  sh.setMetaString ("nc_tree", "CollectionTree");

  // further sample handler configuration may go here

  // print out the samples we found
  sh.print ();

  // this is the basic description of our job
  EL::Job job;
  job.sampleHandler (sh); // use SampleHandler in this job
  //job.options()->setDouble (EL::Job::optMaxEvents, 2000); // for testing purposes, limit to run over the first 500 events only!

  // add our algorithm to the job
  MyxAODAnalysis *alg = new MyxAODAnalysis;

  // later on we'll add some configuration options for our algorithm that go here

  job.algsAdd (alg);

  // make the driver we want to use:
  // this one works by running the algorithm directly:
  EL::DirectDriver driver;
  // we can use other drivers to run things on the Grid, with PROOF, etc.

  // process the job using the driver
  driver.submit (job, submitDir);
}
