#ifndef RUNCHAIN
#define RUNCHAIN
#endif

// Author: Aaron Armbruster
// Date:   2011-11-16
//
// Description:
//
// Run over all functions necessary to create a workspace at one mass point, assuming no interpolation.
//
// 1) setup.C       : Initialize global variables
// 2) writeHistos.C : Write out all nominal and systematic histograms from input trees
// 3) makeNorms.C   : Normalize systematic histograms and write normalization systematics to txt file
// 4) writeXML.C    : Write out XML files, run histfactory, and import asimov data to the workspaces

#include "macros/writeHistos.C"
#include "macros/makeNorms.C"
#include "macros/writeXML.C"
#include "macros/setup.C"
#include "macros/setFlags.C"

#include <sstream>
#include <stdlib.h>

using namespace std;

// dummy function for compiling without running
void runChain() {

}

// Dumps the list of systs to run over into a text file
void writeSystsListFile(std::string version, vector<Sys> & fileNames);

//Dumps in a file which remapping (channels, region, nBins) should be done
void writeRebinningConfigFile(std::string version);

//If offsetSys==-1, hists corresponding to all systematic variations will be done (if doHists==1 of course), otherwise only hists corresponding to the syst variation #offsetSys will be done
//If doOnlySignal==false, adding only signal samples to samplesNames so that hists are built only for signal (interesting to get the mass points made in jobs different from the -common- background hists)
void runChain(double mass, string version = "test", bool alt = false, bool doHists = 1, bool settingsOnly = 0, int offsetSys=-1, bool doOnlySignal=false) {
  // DEBUG OUTPUT
  // - ERROR
  // - WARNING
  // - INFO
  // - DEBUG
  LOG::ReportingLevel() = LOG::FromString("WARNING");

  // Use external config file to set all flags
  // Settings will be printed to the logfile
  LOG(logWARNING) << "Initial setup";
  LOG(logWARNING) << "=============";
  setFlags("config/config_2012.cfg");
  // setFlags("config/config_2011.cfg");
  // setFlags("config/config_2012_vbf_profggf_new.cfg");
  // setFlags("config/config_2012_vbf_profggf.cfg");
  // setFlags("config/config_2012_vbf.cfg");
  // setFlags("config/config_2012_vbf_newbaseline_pacman.cfg");
  // setFlags("config/config_2012_vbf_newbaseline.cfg");
  // setFlags("config/config_2012_vbf_moriond_SF.cfg");
  // setFlags("config/config_2012_vbf.cfg");
  // setFlags("config/config_2011_vbf.cfg");
  // setFlags("config/config_2012_spin_mva.cfg");
  // setFlags("config/config_2012_spin_cb.cfg");
  LOG(logWARNING) << "=============";

  if (do2011 == do2012) {
    LOG(logERROR) << "Need either 2011 or 2012, but not both!";
    exit(1);
  }

  if (doABCD2j && doPacman2j) {
    LOG(logERROR) << "Please select either Pacman or ABCD for VBF workspace";
    exit(1);
  } 

  if ((doPreHCP == 1 && doPostHCP == 1) || ((doPreHCP || doPostHCP) && !do2012)) {
    LOG(logERROR) << "Only one can be true for this check!";
    exit(1);
  }



  LOG(logWARNING) << "Computing flags depending on used configuration";

  if (mode == 3 || mode == 4 || mode == 5) {
    CutDPhi = 10e9;
    CutDPhi2j = 10e9;
    
    if (mode == 3) {
      CutMT = 190;
    }

    if (mode == 4 || mode == 5) {
      CutMT = mass;
    }
    
    if (mode == 5) {
      CutMTUp = 0.75*mass;
    }
  }

  useHighMass2   = (mass > massBoundary2) || (mass == massBoundary2 && alt);
  useHighMass    = ((mass > massBoundary) || (mass == massBoundary && alt)) && !useHighMass2;
  useLowPt       = 0 && !useHighMass && !useHighMass2;
  useTheoryWW    = 0 || useHighMass || useHighMass2;
  useShape       = 1 && useDetSys && mode != 0;
  
  if (doCRVRcheck) {
    mode         = 1;
    doee         = 0;
    domm         = 0;
    splitem      = 0;
    combineCRs   = 0;
    combineSFCRs = 0;
    do0j         = 1;
    do1j         = 0;
    do2j         = 0;
    nrBins_0j    = 1;
  }

  // Use the WW control region
  if(useHighMass || useHighMass2) {
    doWWCR_MVA = 0;
  }
  else {
    doWWCR_MVA = 1;
  }

  // Use the Z+jets subtraining rather then ptll cut
  doZjetsSubtraining  = 0 && doMVA;

  if (!doSingleBin2j && mode != 0) {
    CutMT2j          = 10e9;
  }
  else {
    CutMT2j          = 150; // 1.2*mass; cut at 150 for 2011 for the moment to avoid mH depended background in 2011 2j
  }

  // Initialize smart rebinning and other stuff
  if (doMVA && !doSpin) {
    // Stuff from VBF
    doSmartRemapBDT2j       = 1;
    fraction1stBin2j        = 0.95;
    doRemoveEmptyBins       = 0;
    mappingMode             = 2;
    if (dowzzz == 0 || doWjets == 0) conditionalAsimov = 0;
    if(useHighMass || useHighMass2) {
      mappingMode           = 0;
      doSmartRemapBDT0j     = 1; // only for high mass
      doSmartRemapBDT1j     = 1; // only for high mass
      doSmartRemapBDT2j     = 1;
      nrBins0j_MVA          = 10;
      nrBins1j_MVA          = 5;
      nrBins2j_MVA          = 5;
      doRemoveEmptyBins     = 0;
      fraction1stBin0j      = 0.5;
      fraction1stBin1j      = 0.5;
      fraction1stBin2j      = 0.9;
      doCombineRightmostBin = 0;
    }
    useTheoryWW  = 0;
  }

  // set the right flags to do spin analysis
  if (doMVA && doSpin) { // added the doMVA here because these settings apply only to spin + MVA
    // binning
    nrBins0j_MVA_X = 10;
    nrBins0j_MVA_Y = 5;
    doSmartRemapBDT0j = 0;
    doSmartRemapBDT1j = 0;
    doSmartRemapBDT2j = 0;
    doRemoveEmptyBins = 0;
    doRemoveFirstBins = 0;
    doDynamicBinning = 0;
    splitww = 0;
    doPreHCP = 0;
    decoFakes = 1;
    if( useDetSys )   applySlopeSpin = true;

    if(doSpin2plus){ //TODO: move this into config file
      doSpin2plus_100qq=1;
      doSpin2plus_75qq=0;
      doSpin2plus_50qq=0;
      doSpin2plus_25qq=0;
      doSpin2plus_100gg=0;
    }

    if(!doSpin1D) {
      if(doDynamicBinning) {
        nrBins0j_MVA = 13; //TODO: why this value?
      } else {
        nrBins0j_MVA = nrBins0j_MVA_X * nrBins0j_MVA_Y;
      }
      //if(doCutBasedSpin) nrBins0j_MVA = 100;
    }
    else {
      nrBins0j_MVA = 50;
    }
    
    // control regions
    if (!doWWCR_MVA) {
      useTheoryWW = 1;
    }
    doZCR_MVA           = 1;
    
    // samples
    dovbf               = 0;
    dowh                = 0;
    dozh                = 0;
    doVH                = 0;
    dowzzzew            = 0; 
    dowwew              = 0; 

    // channels
    do1j                = 0;
    do2j                = 0;
    doee                = 0;
    domm                = 0;
    splitem             = 0;
    combineCRs          = 0;
    
    // various settings
    splitzjets          = 0;    
    skiptrackMET        = 1;
    useDYmtshape        = 0;
  }

  // CUTS, SETTINGS, ...
  
  // if (!doWjets) conditionalAsimov = 0;
  if (useLumiAsPOI) conditionalAsimov = 0;

  if (!splitem) dome = 0;

  if (useLowPt && !useHighPt && (useHighMass || useHighMass2)) return;

  if (mode == 0 || CutMTandRebin) {
    nrBins_0j       = 1;
    nrBins_1j       = 1;
    doSingleBinSF2j = 1;
    doSingleBin2j   = 1;
    variableBin2j   = 0;
    nrBins_2j       = 1;
  }

  // RUN THE WHOLE CHAIN

  setup(mass, alt);

  // Dumps the list of systs to run over into a text file
  writeSystsListFile(version, *fileNames);

  //Dumps in a file which remapping (channels, region, nBins) should be done
  writeRebinningConfigFile(version);

  overriden = 1;
  if (settingsOnly) return;

  stringstream massStr;
  massStr << mass;
  string smass = massStr.str();

  if (doHists)
  {
    writeHistos(mass, version, alt, offsetSys, doOnlySignal);
  }
  if (offsetSys==-1) {
    makeNorms(mass, version, alt);

    system(("mkdir -vp rev/" + version + "/tag").c_str());

    writeXML(mass, version, alt);

    tag(mass, version, alt, doHists);
  }
}



// Dumps the list of systs to run over into a text file
void writeSystsListFile(std::string version, vector<Sys> & systs) {
  std::string systsListFile=std::string("rev/"+version+"/systsList.txt");
  ifstream iSystsFile;
  iSystsFile.open (systsListFile.c_str(), ifstream::in);
  if (iSystsFile.good()) {
    iSystsFile.close();
    std::cout << "WARNING: not writing again the existing file containing the list of syst variations to run for hists production !" << std::endl;
  }
  else {
    ofstream oSystsFile(systsListFile.c_str());
    for (unsigned int iSys=0; iSys<systs.size(); iSys++) {
      std::string sampleNames="";
      for (std::set<std::string>::iterator it=systs[iSys].sampleNames.begin(); it!=systs[iSys].sampleNames.end(); ++it)
	sampleNames += *it +",";
      oSystsFile << systs[iSys].folder << "\t" << systs[iSys].fileUp << "\t" << sampleNames.c_str() << "\n";
      if (systs[iSys].fileDown != systs[iSys].fileUp)
	oSystsFile << systs[iSys].folder << "\t" << systs[iSys].fileDown << "\t" << sampleNames.c_str() << "\n";
    }
    oSystsFile.close();
    std::cout << "List of syst variations to run for hists production dumped into " << systsListFile.c_str() << std::endl;
  }
}


//Dumps in a file which remapping (channels, region, nBins) should be done
void writeRebinningConfigFile(std::string version) {

  std::string rebinningListFile=std::string("rev/"+version+"/rebinningList.txt");
  ifstream iRebinningFile;
  iRebinningFile.open (rebinningListFile.c_str(), ifstream::in);
  ofstream oRebinningFile(rebinningListFile.c_str());

  //If some rebinning is needed (otherwise just dump an empty file)
  if (mappingMode == 2)  {
    for (unsigned int ireg = 0; ireg < regions->size(); ireg++)    {
      Region* r = &(*regions)[ireg];
      unsigned int nrChannels = r->channels.size();

      for (unsigned int ichan = 0; ichan < nrChannels; ichan++)      {
        Channel* c = &r->channels[ichan];

        bool skipLoop = skipRegion(r, c->name, c->jetName);
        if (skipLoop) continue;

        if ( c->jetName == "0j" && doMVA && !doSmartRemapBDT0j) continue;
        if ( c->jetName == "1j" && doMVA && !doSmartRemapBDT1j) continue;
        if ( c->jetName == "2j" && doMVA && !doSmartRemapBDT2j) continue;

	//dump info about rebinning
	oRebinningFile << c->name << "\t" << r->name << "_" << c->jetName << "\t" << getNbinsForRegion(r, c) << "\n";
      }
    }
    std::cout << "List of rebinning to do for hists production dumped into " << rebinningListFile.c_str() << std::endl;
  }
  else { }//dumping an empty file
  oRebinningFile.close();
}


// #endif
