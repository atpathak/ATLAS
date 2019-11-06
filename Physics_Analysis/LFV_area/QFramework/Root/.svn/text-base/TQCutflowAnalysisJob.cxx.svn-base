#include "QFramework/TQCutflowAnalysisJob.h"
#include "QFramework/TQCompiledCut.h"
#include "QFramework/TQAnalysisJob.h"
#include "QFramework/TQCounter.h"
#include "QFramework/TQSample.h"

// #define _DEBUG_
#include "QFramework/TQLibrary.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

#include <stdexcept>

using std::cout;
using std::endl;

ClassImp(TQCutflowAnalysisJob)


//______________________________________________________________________________________________

TQCutflowAnalysisJob::TQCutflowAnalysisJob() : 
TQAnalysisJob("TQCutflowAnalysisJob"),
  poolAt(NULL)
{
  init();
}


//______________________________________________________________________________________________

TQCutflowAnalysisJob::TQCutflowAnalysisJob(TString name_) : 
  TQAnalysisJob(name_),
  poolAt(NULL)
{
  init();
}


//______________________________________________________________________________________________

bool TQCutflowAnalysisJob::initializeSelf() {
  /* initialize this analysis job */
  DEBUGclass("initializing analysis job '%s'",this->GetName());

  /* we need a parent cut defining the name of the counter */
  if (!this->getCut()) {
    throw std::runtime_error("this analysis job has no cut assigned");
  }

  if ( !this->fCounter ) {
    this->poolAt = this->fSample;
    
    /* create a new counter */
    TQCut* c = this->getCut();
    fCounter = new TQCounter(c->GetName(),c->GetTitle());
  }
  DEBUGclass("finished initializing cutflow analysis job");
  return true;
}


//______________________________________________________________________________________________

bool TQCutflowAnalysisJob::finalizeSelf() {
  /* get the cutflow folder */
  DEBUGclass("attempting to create .cutflow folder in sample '%s'",this->fSample->getPath().Data());

  if ( this->poolAt == this->fSample ) {
    TQFolder * cfFolder = this->poolAt->getFolder(".cutflow+");
    
    /* stop if we failed to get the folder */
    if (!cfFolder) { return false; }
    
    /* normalize the counter */
    // this->fCounter->scale(fSample->getNormalisation()); // this was moved to execute()

    /* remove existing counter */
    cfFolder->removeObject(fCounter->GetName());
    /* add the counter */
    cfFolder->addObject(fCounter);
    
    /* remove the pointer to the counter */
    this->fCounter = 0;
    // this->poolAt = NULL;
  }

  /* finalize this analysis job */
  return true;

}


//__________________________________________________________________________________|___________

bool TQCutflowAnalysisJob::initializeSampleFolder(TQSampleFolder* sf){
  // initialize this job on a sample folder (taking care of pooling)
  bool pool = false;
  sf->getTagBool(".aj.pool.counters",pool);
  // std::cout << std::endl << "initialize samplefolder called on " << sf->GetName() << " pool=" << pool << ", fHistograms=" << fHistograms << std::endl << std::endl;
  if(pool && !this->fCounter){
    /* create a new counter */
    TQCut* c = this->getCut();
    fCounter = new TQCounter(c->GetName(),c->GetTitle());
    this->poolAt = sf;
  }

  return true;
}

//______________________________________________________________________________________________

bool TQCutflowAnalysisJob::execute(double weight) {
 
  /* count this event: add its weight to the counter */
  fCounter->add(weight*fSample->getNormalisation());

  return true;

}

//__________________________________________________________________________________|___________

bool TQCutflowAnalysisJob::finalizeSampleFolder(TQSampleFolder* sf){

  if (!sf) { return false; }

  // finalize this job on a sample folder (taking care of pooling)
  if(sf == this->poolAt) {
    TQFolder * cfFolder = this->poolAt->getFolder(".cutflow+");

    /* stop if we failed to get the folder */
    if (!cfFolder) { return false; }

    /* normalize the counter */
    // this->fCounter->scale(fSample->getNormalisation()); // COUNTERS ARE NOW SCALED IN TQCutflowAnalysisJob::execute
    /* remove existing counter */
    cfFolder->removeObject(fCounter->GetName());
    /* add the counter */
    cfFolder->addObject(fCounter);
    
    /* remove the pointer to the counter */
    this->fCounter = 0;
    // this->poolAt = NULL;

  }

  return true;
}

//______________________________________________________________________________________________

void TQCutflowAnalysisJob::init() {
  fCounter = 0;
}


//______________________________________________________________________________________________

TQCutflowAnalysisJob::~TQCutflowAnalysisJob() {
  if (fCounter) { delete fCounter; }
}


