#include "QFramework/TQGridScanAnalysisJob.h"
#include "QFramework/TQCompiledCut.h"
#include "QFramework/TQAnalysisJob.h"
#include "QFramework/TQCounter.h"
#include "QFramework/TQSample.h"
#include "QFramework/TQUtils.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

using std::cout;
using std::endl;

ClassImp(TQGridScanAnalysisJob)


////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQGridScanAnalysisJob
//
// The TQGridScanAnalysisJob allow to schedule the creation of TQCounterGrids
// for each and every sample visitied during the analysis
// This information can later be retrieved from a .gridscan subfolder 
// of the analysis root file folder structure
// to perform a scan over different cut configurations (grid scan cut optimization)
//
////////////////////////////////////////////////////////////////////////////////////////////////


//______________________________________________________________________________________________

TQGridScanAnalysisJob::TQGridScanAnalysisJob() : 
TQAnalysisJob("TQGridScanAnalysisJob"), 
  grid(NULL),
  poolAt(NULL)
{
  // default constructor
}


//______________________________________________________________________________________________

TQGridScanAnalysisJob::TQGridScanAnalysisJob(TString name_) : 
  TQAnalysisJob(name_),
  grid(NULL),
  poolAt(NULL)
{
  // create a new TQGridScanAnalysisJob
  // take care of choosing an appropriate name (default "gridscan")
  // as this will determine the name under which the resulting grids
  // will be saved in the folder structure

}

//______________________________________________________________________________________________

void TQGridScanAnalysisJob::initializeObservables(){
  // initialize all the observables properly
  for(size_t i=0; i<variables.size(); i++){
    this->variables[i].initialize(this->fSample);
  }
}

//______________________________________________________________________________________________

void TQGridScanAnalysisJob::initializeGrid(){
  // initialize the TQCounterGrid object
  this->grid = new TQCounterGrid(this->variables);
}

//______________________________________________________________________________________________

void TQGridScanAnalysisJob::finalizeObservables(){
  // finalize all the observables properly
  for(size_t i=0; i<variables.size(); i++){
    this->variables[i].finalize();
  }
}

//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::finalizeGrid(){
  // initialize the TQCounterGrid object
  // and write it to the pool folder
  if(!this->poolAt) return false;
 
  /* get the cutflow folder */
  TQFolder * cfFolder = this->poolAt->getFolder(TString::Format(".gridscan/%s+",this->getCut()->GetName()));
 
  /* stop if we failed to get the folder */
  if (!cfFolder) { return false; }
 
  /* remove existing grid */ 
  TObject* o = cfFolder->FindObject(this->GetName());
  if(o){
    cfFolder->Remove(o);
  } 
  // clean the grid of unwanted (temporary)
  // data members before saving
  this->grid->cleanup();
  /* add the grid */
  cfFolder->addObject(this->grid);
  this->grid = NULL;

  return true;
}

//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::initializeSelf() {
  // initialize the job on the given sample

  TString name = this->GetName();
  if(name.IsNull())
    this->SetName("counterGrid");

  if(!this->grid)
    this->initializeGrid();

  this->initializeObservables();

  this->grid->SetName(this->GetName());
 
  return true;
}



//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::finalizeSelf() {
  // finalize the job, writing all results to the pool folder

  if(this->fSample == this->poolAt)
    this->finalizeGrid();

  this->finalizeObservables();

  return true;
 
}

//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::initializeSampleFolder(TQSampleFolder * sf) {
  // initalize the job on a sample folder
  // check for pool tag

  bool pool = false;
  sf->getTagBool(".aj.pool.countergrid",pool);

  if(pool && !this->grid){
    TString name = this->GetName();
    if(name.IsNull())
      this->SetName("counterGrid");
 
    this->poolAt = sf;
  }

  return true;
 
}




//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::finalizeSampleFolder(TQSampleFolder* sf) {
  // finalize the job on a sample folder
  // check for pool tag 

  bool pool =(sf == this->poolAt);

  /* stop if we no sample is defined */
  if (!sf) { return false; }
 
  if(pool)
    return this->finalizeGrid();

  return true;
}


//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::execute(double weight) {
  // execute the job on the current event
  // adding it to the countergrid it with the given weight

  // count this event: add its weight to the counter 
  this->grid->fillFromTree(weight*(this->fSample->getNormalisation()));

  return true;
 
}



//______________________________________________________________________________________________

TQGridScanAnalysisJob::~TQGridScanAnalysisJob() {
  // destructor
  if(this->grid) delete this->grid;
}

//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::bookVariable(TString name, TString title, int nSteps, double min, double max,TString formula){
  // book a variable, that is, add a new dimension to the grids produced by this job
  // the syntax is that of TH1* constructors, that is name,title,nBins,min,max
  // just like TH1*, the grids will contain an additional over- and underflow bin
  if(formula.IsNull())
    formula = name;
  return this->bookVariable(TQCounterGridVariable(name,title,nSteps,min,max,formula));
}

//______________________________________________________________________________________________

bool TQGridScanAnalysisJob::bookVariable(TQCounterGridVariable v){
  // book a variable, that is, add a new dimension to the grids produced by this job
  this->variables.push_back(v);
  return true;
}

//______________________________________________________________________________________________

std::vector<TQCounterGridVariable> TQGridScanAnalysisJob::getBookedVariables(){
  // return a std::vector of TQCounterGridVariables that have been booked for this job
  return this->variables;
}

//______________________________________________________________________________________________

TObjArray * TQGridScanAnalysisJob::getBranchNames() {
  // Return all used TTree branches in a TObjArray* 
  // The strings should include all used branch names,
  // but can also include formulas, number, etc.

  TObjArray * bNames = new TObjArray();

  for(size_t i=0; i<this->variables.size(); i++){
    TCollection* c = this->variables[i].getObservable()->getBranchNames();
    bNames->AddAll(c);
    delete c;
  }
 
  return bNames;
}
