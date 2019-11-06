#include "QFramework/TQCounterGrid.h"
#include "TMath.h"
#include <iostream>
#include <map>

#include <stdint.h>
#include <sstream>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQCounterGrid
//
// The TQCounterGrid class provides the functionality of a multi-dimensional
// histogram. The dimensionality is dynamic and can be chosen at creation time.
// While it is in principle possible to fill a TQCounterGrid manually,
// it is mainly aimed at being filled automatically while looping over a TTree.
// Data can be retrieved in the form of individual counter numbers
// or, more generally, partially integrated projections on any of the axis.
//
////////////////////////////////////////////////////////////////////////////////////////////////


ClassImp(TQCounterGrid)

void TQCounterGrid::print(unsigned int indent){
  // print the contents of the grid in a human-readable form
  // WARNING: for large grids, this can take considerable time 
  // and flood the entire output with numbers!
  for(size_t i=0; i<indent; i++){
    std::cout << "\t";
  }
  std::cout << this->variable.name << ": " << this->variable.nSteps << " bins from " << this->variable.min << " to " << this->variable.max << std::endl;
  for(size_t i=0; i<this->nodes.size(); i++){
    if(nodes[i]) this->nodes[i]->print(indent+1);
  }
}

TQCounterGrid::TQCounterGrid(TQCounterGrid* other) : 
  TQCounterGridNode(other ? other->scaleFactor : 1),
  TNamed(other ? other->GetName() : "",other ? other->GetName() : ""),
  variable(other ? other->variable : NULL)
{
  // copy constructor, pointer variant
  this->copyNodes(other);
} 

TQCounterGrid::TQCounterGrid(TQCounterGrid& other) : 
  TQCounterGridNode(other.scaleFactor),
  TNamed(other.GetName(),other.GetName()),
  variable(other.variable)
{
  // copy constructor, reference variant
  this->copyNodes(&other);
} 

void TQCounterGrid::copyNodes(TQCounterGrid* other){
  // copy all the nodes from the given grid to this one
  for(size_t i=0; i<other->nodes.size(); i++){
    TQCounterGridNode* newNode = (other->nodes[i] ? other->nodes[i]->copy() : NULL);
    this->nodes.push_back(newNode);
  }
  this->init();
}

TQCounterGrid* TQCounterGrid::copy(){
  // return a copy of this grid
  // internally calls the copy constructor
  return new TQCounterGrid(this);
}

TQCounterGrid::TQCounterGrid(double scale) :
  TQCounterGridNode(scale)
{
  // Initialize the TQCounterGrid with a scale parameter.
  this->init();
}

TQCounterGrid::TQCounterGrid() :
  TQCounterGridNode(1)
{
  // Initialize the TQCountergrid without a scale parameter (scale=1).
  this->init();
}


size_t TQCounterGrid::getMemSize(){
  // Obtain the total amount of space occupied by the actual data members
  // of the TQCounterGrid. This estimate is somehow optimistic, as it does 
  // not take into account additional class members used to store temporary
  // information, e.g. integration ranges and the TQCounterVariable.
  size_t n=0;
  for(size_t i=0; i<nodes.size(); i++){
    if(nodes[i]) n += nodes[i]->getMemSize();
  }
  return n;
}

void TQCounterGrid::init(){
  // Common initialization function, used by several constructors.
  this->minBin = 0;
  if(this->nodes.size() < 1)
    this->maxBin = 0;
  else
    this->maxBin = this->nodes.size()-1;
}

void TQCounterGrid::setVariableTitle(const TString& varname, const TString& vartitle){
  // set/change the title of the given variable
  // loops through all nodes recursively 
  // and applys the change everywhere
  if(this->variable.name == varname){
    this->variable.title = vartitle;
    return;
  }
  for(size_t i=0; i<nodes.size(); i++){
    TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(this->nodes[i]);
    if(!g) continue;
    g->setVariableTitle(varname,vartitle);
  }
}

TString TQCounterGrid::getVariableTitle(const TString& varname){
  // get/obtain the variable title of the given variable
  if(this->variable.name == varname){
    return this->variable.title;
  }
  TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(this->nodes[0]);
  if(g) 
    return g->getVariableTitle(varname);
  return "";
}

TQCounterGridVariable TQCounterGrid::getVariable(const TString& varname){
  // get/obtain the variable of the given variable
  if(this->variable.name == varname){
    return this->variable;
  }
  TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(this->nodes[0]);
  if(g) 
    return g->getVariable(varname);
  return TQCounterGridVariable();
}

void TQCounterGrid::resetProjectionRanges(){
  // Reset all integration ranges to the full available range.
  // For safety reasons, this function should ALWAYS be called
  // before any data is retrieved from the TQCounterGrid.
  this->init();
  for(size_t i=0; i<nodes.size(); i++){
    if(nodes[i]) nodes[i]->resetProjectionRanges();
  }
}

void TQCounterGrid::resetProjectionRange(const TString& varname){
  // Reset integration ranges for given variable to the full available range.
  if(this->variable.name == varname){
    this->init();
    return;
  }
  for(size_t i=0; i<nodes.size(); i++){
    if(nodes[i]) nodes[i]->resetProjectionRange(varname);
  }
}

void TQCounterGrid::ensureNode(size_t i){
  if(this->nodes[i]) return;
  this->nodes[i] = this->nodes[0]->copy();
  this->nodes[i]->clear();
}

void TQCounterGrid::compactify(){
  // remove all empty nodes 
  // WARNING: HIGHLY EXPERIMENTAL
  for(size_t i=nodes.size()-1; i>0; i--){
    if(nodes[i] && nodes[i]->valueRaw() == 0){
      delete nodes[i];
      nodes[i] = NULL;
    } else {
      TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(nodes[i]);
      if(g) g->compactify();
    }
  }
} 


TQCounterGrid::TQCounterGrid(std::vector<TQCounterGridVariable*> vars, double s):
  TQCounterGridNode(s),
  variable(*vars.back())
{
  // This constructor takes a std::vector* container of TQCounterGridVariables
  // and builds the recursive tree structure back-to-front from the contents
  // of the given vector.
  vars.pop_back();
  if(vars.size() > 0){
    for(size_t i=0; i<variable.nSteps+2; i++){
      this->nodes.push_back(new TQCounterGrid(vars));
    }

  } else {
    for(size_t i=0; i<variable.nSteps+2; i++){
      this->nodes.push_back(new TQCounterGridPoint());
    } 
  }
  this->init();
}

TQCounterGrid::TQCounterGrid(std::vector<TQCounterGridVariable> vars, double s):
  TQCounterGridNode(s),
  variable(vars.back())
{
  // This constructor takes a std::vector container of TQCounterGridVariables
  // and builds the recursive tree structure back-to-front from the contents
  // of the given vector.
  vars.pop_back();
  if(vars.size() > 0){
    for(size_t i=0; i<variable.nSteps+2; i++){
      this->nodes.push_back(new TQCounterGrid(vars));
    }

  } else {
    for(size_t i=0; i<variable.nSteps+2; i++){
      this->nodes.push_back(new TQCounterGridPoint());
    } 
  }
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable* v, double s):
  TQCounterGridNode(s),
  variable(*v)
{
  // This constructor takes a single variable
  // and constructs a one-dimensional TQCounterGrid
  // which is equivalent to a TH1F
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGridPoint());
  } 
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable v, double s):
  // This constructor takes a single variable
  // and constructs a one-dimensional TQCounterGrid
  // which is equivalent to a TH1F
  TQCounterGridNode(s),
  variable(v)
{
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGridPoint());
  } 
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable* v1, TQCounterGridVariable* v2, double s):
  TQCounterGridNode(s),
  variable(*v1)
{
  // This constructor takes two variables
  // and constructs a two-dimensional TQCounterGrid
  // which is equivalent to a TH2F
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGrid(v2));
  } 
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable v1, TQCounterGridVariable v2, double s):
  TQCounterGridNode(s),
  variable(v1)
{
  // This constructor takes two variables
  // and constructs a two-dimensional TQCounterGrid
  // which is equivalent to a TH2F
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGrid(v2));
  } 
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable* v1, TQCounterGridVariable* v2, TQCounterGridVariable* v3, double s):
  TQCounterGridNode(s),
  variable(*v1)
{
  // This constructor takes three variables
  // and constructs a three-dimensional TQCounterGrid
  // which is equivalent to a TH3F
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGrid(v2,v3));
  } 
  this->init();
}

TQCounterGrid::TQCounterGrid(TQCounterGridVariable v1, TQCounterGridVariable v2, TQCounterGridVariable v3, double s):
  TQCounterGridNode(s),
  variable(v1)
{
  // This constructor takes three variables
  // and constructs a three-dimensional TQCounterGrid
  // which is equivalent to a TH3F
  for(size_t i=0; i<variable.nSteps+2; i++){
    this->nodes.push_back(new TQCounterGrid(v2,v3));
  } 
  this->init();
}


size_t TQCounterGrid::getInterval(double val){
  // retrieve the bin (or interval) index for a given value
  if(val > this->variable.max) return (this->nodes.size() -1);
  if(val < this->variable.min) return 0;
  return ceil((val - this->variable.min) / this->variable.step);
}

size_t TQCounterGrid::getInterval(const TString& varname, double val){
  if(this->variable.name == varname)
    return this->getInterval(val);
  TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(this->nodes[0]);
  if(!g) return -1;
  return g->getInterval(varname, val);
}

double TQCounterGrid::getCenter(size_t i){
  // retrieve the center value of a given interval (or bin)
  i = std::min((int)i,(int)this->variable.nSteps+1);
  return this->variable.min + ((-0.5 + i)*(this->variable.step));
}

void TQCounterGrid::clear(){
  // delete ALL the data from the TQCounterGrid
  // e.g. set all the counters / bin values
  // back to zero
  for(size_t i=0; i<nodes.size(); i++){
    if(nodes[i]) nodes[i]->clear();
  }
}

void TQCounterGrid::fill(double weight){
  // fill the TQCounterGrid with one entry of given weight
  // (default 1)
  // This will increase ALL subcounters of the grid by their respective
  // hypervolume times given weight, as no localisation of the event
  // within the volume of the gridspace is given.
  for(size_t i=0; i<nodes.size(); i++){
    this->ensureNode(i);
    this->nodes[i]->fill(weight / nodes.size() );
  }
}

double TQCounterGrid::value(){
  // retrieve the total weighted number of entries in this grid
  double s = 0;
  for(size_t i=this->minBin; i<=this->maxBin; i++){
    if(nodes[i]) s += nodes[i]->value();
  }
  return (this->scaleFactor)*s;
}
int TQCounterGrid::valueRaw(){
  // retrieve the total unweighted number of entries from this grid
  double s = 0;
  for(size_t i=this->minBin; i<=this->maxBin; i++)
    if(nodes[i]) s += nodes[i]->valueRaw();
  return s;
}
double TQCounterGrid::uncertainty(){
  // retrieve the total uncertainty on the weighted number of entries in this grid
  return TMath::Sqrt(this->variance());
}

double TQCounterGrid::variance(){
  // retrieve the total variance (square uncertainty) 
  // of the weighted number of entries in this grid
  double s2 = 0;
  for(size_t i=this->minBin; i<=this->maxBin; i++)
    if(nodes[i]) s2 += pow(nodes[i]->uncertainty(),2);
  return pow(this->scaleFactor,2)*s2;
}

double TQCounterGrid::uncertaintyRel(){
  // return the relative uncertainty on the weighted value stored in this bin
  return this->uncertainty()/this->value();
}

double TQCounterGrid::relUncertainty(){
  // return the relative uncertainty on the weighted value stored in this bin
  return this->uncertaintyRel();
}

void TQCounterGrid::fillFromTree(double weight){
  // fill the grid with the current entry from the tree
  double val = this->variable.evaluateFromTree();
  size_t i = this->getInterval(val);
  this->ensureNode(i); 
  if(nodes[i]->isTrivial())
    this->nodes[i]->fill(weight);
  else 
    this->nodes[i]->fillFromTree(weight);
}

bool TQCounterGrid::isTrivial(){
  // return true if this is a trivial node (leaf)
  // and false otherwise
  // will return false for TQCounterGrids in all cases
  return false;
}

TQCounterGrid::~TQCounterGrid(){
  // destructor will erase all data in the grid
  for(size_t i=0; i<this->nodes.size(); i++){
    if(nodes[i]) delete nodes[i];
  }
  nodes.clear();
}

void TQCounterGrid::printProjectionRanges(){
  // print all cuts and restrictions currently applied
  std::cout << this->getBinMin(this->minBin) << "<" << this->variable.name << "<" << this->getBinMax(this->maxBin) << "\t" << "(bins " << this->minBin << "-" << this->maxBin << ")" << std::endl;
  this->nodes[0]->printProjectionRanges();
}

void TQCounterGrid::setMinCut(const TString& varname, double mincut){
  // set the minimum value for a given variable
  // all data retrieved from the grid will have this cut applied
  // the cut value will be rounded to the next bin boundary
  if(this->variable.name == varname)
    this->minBin = this->getInterval(mincut)+1;
  else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setMinCut(varname, mincut);
}

void TQCounterGrid::setMaxCut(const TString& varname, double maxcut){
  // set the maximum value for a given variable
  // all data retrieved from the grid will have this cut applied
  // the cut value will be rounded to the next bin boundary
  if(this->variable.name == varname)
    this->maxBin = this->getInterval(maxcut);
  else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setMaxCut(varname, maxcut);
}
void TQCounterGrid::setRangeCuts(const TString& varname, double mincut, double maxcut){
  // set the minimum and maximum value for a given variable
  // all data retrieved from the grid will have these cuts applied
  // the cut values will be rounded to the next bin boundary
  if(this->variable.name == varname){
    this->minBin = this->getInterval(mincut)+1;
    this->maxBin = this->getInterval(maxcut);
  } else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setRangeCuts(varname, mincut, maxcut);
}

void TQCounterGrid::setMinBin(int minbin){
  // set the minimum bin for THIS GRID ONLY
  // all data retrieved from the grid will have this restriction applied
  if(minbin < 0)
    this->minBin = 0;
  else if((size_t)(minbin) >= this->nodes.size())
    this->minBin = this->nodes.size() -1;
  else
    this->minBin = minbin;
}

void TQCounterGrid::setMaxBin(int maxbin){
  // set the maximum bin for THIS GRID ONLY
  // all data retrieved from the grid will have this restriction applied
  if(maxbin < 0)
    this->maxBin = 0;
  else if((size_t)(maxbin) >= this->nodes.size())
    this->maxBin = this->nodes.size() -1;
  else
    this->maxBin = maxbin;
}

void TQCounterGrid::setMinBin(const TString& varname, int minbin){
  // set the minimum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
  if(this->variable.name == varname)
    this->setMinBin(minbin);
  else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setMinBin(varname, minbin);
}

void TQCounterGrid::setMaxBin(const TString& varname, int maxbin){
  // set the maximum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
  if(this->variable.name == varname)
    this->setMaxBin(maxbin);
  else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setMaxBin(varname, maxbin);
}

void TQCounterGrid::setRangeBins(const TString& varname, int minbin, int maxbin){
  // set the minimum and maximum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
  if(this->variable.name == varname){
    this->setMinBin(minbin);
    this->setMaxBin(maxbin);
  } else
    for(size_t i=0; i<this->nodes.size(); i++)
      if(nodes[i]) this->nodes[i]->setRangeBins(varname, minbin, maxbin);
}

size_t TQCounterGrid::getMinBin (const TString& varname){
  // get current lower projection range boundary bin number for the given dimension
  if(this->variable.name == varname)
    return this->minBin;
  else 
    return this->nodes[0]->getMinBin(varname);
}

size_t TQCounterGrid::getMaxBin (const TString& varname){
  // get current upper projection range boundary bin number for the given dimension
  if(this->variable.name == varname)
    return this->maxBin;
  else 
    return this->nodes[0]->getMaxBin(varname);
}

double TQCounterGrid::getMinCut (const TString& varname){
  // get current lower projection range boundary for the given dimension
  if(this->variable.name == varname)
    return this->getBinMin(this->minBin);
  else 
    return this->nodes[0]->getMinCut(varname);
}

double TQCounterGrid::getMaxCut (const TString& varname){
  // get current upper projection range boundary for the given dimension
  if(this->variable.name == varname)
    return this->getBinMax(this->maxBin);
  else 
    return this->nodes[0]->getMaxCut(varname);
}

double TQCounterGrid::getBinMin(size_t bin){
  // return the lower boundary of the bin of the given number
  // for THIS GRID
  if(bin == 0)
    return -std::numeric_limits<double>::infinity();
  return this->variable.min + (this->variable.step * (bin-1));
}

double TQCounterGrid::getBinMin(const TString& varname, size_t bin){
  // return the upper boundary of the bin of the given number and dimension
  if(this->variable.name == varname)
    return this->getBinMin(bin);
  return this->nodes[0]->getBinMin(varname,bin);
}

double TQCounterGrid::getBinMax(size_t bin){
  // return the upper boundary of the bin of the given number
  // for THIS GRID
  if(bin > this->variable.nSteps)
    return std::numeric_limits<double>::infinity();
  return this->variable.min + (this->variable.step * bin);
}

double TQCounterGrid::getBinMax(const TString& varname, size_t bin){
  // return the upper boundary of the bin of the given number and dimension
  if(this->variable.name == varname)
    return this->getBinMax(bin);
  return this->nodes[0]->getBinMax(varname,bin);
}

bool TQCounterGrid::hasVariable(const TString& varname){
  // return true if varname is the name of a dimension of this grid
  // fals otherwise
  if(varname == this->variable.name)
    return true;
  TQCounterGrid* g = dynamic_cast<TQCounterGrid*>(this->nodes[0]);
  if(!g)
    return false;
  return g->hasVariable(varname);
}

TH1F* TQCounterGrid::getTH1F(const TString& varname, bool raw){
  // obtain a TH1F histogram, filled with data from the grid
  // the binning and range will be chosen appropriately
  // according the the binning of the grid
  // and the currently set projection ranges
  // if the RAW flag is used, the histogram will be filled
  // with the unweighted ("raw") numbers instead
  // the number of entries will correspond the total number
  // of entries in the grid that contributed to the histogram
  if(!this->hasVariable(varname))
    return NULL;
  TH1F* hist = new TH1F(varname, this->title(varname),
                        this->nBins(varname),
                        this->minimum(varname),
                        this->maximum(varname));
  hist->SetDirectory(NULL);
  // if that succeeded, we fill it
  this->fillTH1F(varname, hist, raw);
  // after having filled it successfully,
  // we can proceed by "finalizing" the histogram
  // setting the number of entries
  hist->SetEntries(this->valueRaw());
  if(raw){
    // setting the bin errors (raw mode)
    for(int i=0; i<hist->GetNbinsX()+2; i++){
      hist->SetBinError(i,TMath::Sqrt(hist->GetBinContent(i)));
    }
  } else {
    // post-processing the bin errors (weighted mode)
    for(int i=0; i<hist->GetNbinsX()+2; i++){
      hist->SetBinError(i,TMath::Sqrt(hist->GetBinError(i)));
    }
    // and scaling the histogram
    hist->Scale(this->scaleFactor);
  }
  return hist;
}

TQCounterGridNode* TQCounterGrid::getNode(size_t i){
  // obtain the pointer to a given sub-node
  // will return NULL when bin index is invalid (out of range)
  if(i < this->nodes.size())
    return this->nodes[i];
  return NULL;
}

TH2F* TQCounterGrid::getTH2F(const TString& xvarname, const TString& yvarname, bool raw){
  // obtain a TH2F histogram, filled with data from the grid
  // the binning and range will be chosen appropriately
  // according the the binning of the grid
  // and the currently set projection ranges
  // if the RAW flag is used, the histogram will be filled
  // with the unweighted ("raw") numbers instead
  // the number of entries will correspond the total number
  // of entries in the grid that contributed to the histogram
  if(!this->hasVariable(xvarname) || !this->hasVariable(yvarname))
    return NULL;
  TH2F* hist = new TH2F(xvarname+"_"+yvarname, 
                        this->title(xvarname) + " vs. " + this->title(yvarname)+";"+this->title(yvarname)+";"+this->title(xvarname),
                        this->nBins (yvarname),
                        this->minimum(yvarname),
                        this->maximum(yvarname),
                        this->nBins (xvarname),
                        this->minimum(xvarname),
                        this->maximum(xvarname));
  hist->SetDirectory(NULL);
  // if that succeeded, we fill it
  double xmintotal = this->minimum(yvarname);
  size_t xmin = this->getMinBin(xvarname);
  size_t xmax = this->getMaxBin(xvarname);
  double xstep = this->stepWidth(xvarname);
  double ymintotal = this->minimum(yvarname);
  size_t ymin = this->getMinBin(yvarname);
  size_t ymax = this->getMaxBin(yvarname);
  double ystep = this->stepWidth(xvarname);
  for(size_t x=xmin; x<=xmax; x++){
    this->setRangeBins(xvarname,x,x);
    for(size_t y=ymin; y<=ymax; y++){
      this->setRangeBins(yvarname,y,y);
      int bin = hist->FindBin(xmintotal + xstep*x,ymintotal+ystep*y);
      if(raw){
        double val = this->valueRaw();
        hist->SetBinContent(bin,val);
        hist->SetBinError(bin,sqrt(val));
      } else {
        double val = this->valueRaw();
        hist->SetBinContent(bin,val);
        hist->SetBinError(bin,sqrt(val));
      }
    }
  }
  this->setRangeBins(xvarname,xmin,xmax);
  this->setRangeBins(yvarname,ymin,ymax);
  hist->SetEntries(this->valueRaw());
  return hist;
}

void TQCounterGrid::fillTH1F(const TString& varname, TH1F* hist, bool raw){
  // fill a TH1F histogram with data from the grid
  // if the RAW flag is used, the histogram will be filled
  // with the unweighted ("raw") numbers instead
  if(!hist) return;
  if(varname == this->variable.name){
    if(raw){
      for(size_t i=this->minBin; i<=this->maxBin; i++){
        if(!this->nodes[i]) continue;
        int val = nodes[i]->valueRaw();
        // the errors will be taken care of 
        // when the histogram gets "finalized" 
        // in the original function
        // (i.e. getTH1F)
        hist->AddBinContent(i,val);
      }
    } else {
      for(size_t i=this->minBin; i<=this->maxBin; i++){
        if(!this->nodes[i]) continue;
        double val = nodes[i]->value();
        // important remark:
        // this line needs to go BEFORE the next one
        hist->SetBinError(i,hist->GetBinError(i)+nodes[i]->variance());
        // otherwise, the error will be messed up
        // since ROOT otherwise sets the error value to sqrt(n) 
        // on the first call of SetBinContent
        // in addition to that, we set the errors to the VARIANCE
        // for the moment. we will take the sqrt of these values
        // when finalizing the histogram
        // that way, we save a bunch of sqrt and pow operations
        // and make the code more understandable
        hist->AddBinContent(i,val);
      }
    }
  } else {
    for(size_t i=this->minBin; i<=this->maxBin; i++){
      if(!this->nodes[i]) continue;
      this->nodes[i]->fillTH1F(varname,hist,raw);
    }
  }
}

bool TQCounterGrid::add(TQCounterGridNode* addition, double factor, double thisFactor, bool statError){
  // add another grid node to this one
  // this will only work if both nodes are compatible (same dimensionality)
  // internally, the addition will be cast to a (TQCounterGrid*)
  // and the corresponding variant of the add function will be used
  if(!addition){
    std::cout << "ERROR in TQCounterGrid: cannot add NULL pointer!" << std::endl;
    return false;
  }
  if(addition->isTrivial()){
    std::cout << "ERROR: cannot add grids of different dimensionalities!" << std::endl;
    return false;
  }
  return this->add(dynamic_cast<TQCounterGrid*>(addition),factor,thisFactor,statError);
}

bool TQCounterGrid::add(TQCounterGrid* addition, double factor, double thisFactor, bool statError){
  // add another counter grid to this one 
  // this will only work if both nodes are compatible (same dimensionality)
  // additionally, the variables need to be compatible, 
  // that is, one of the following must be true
  // - the entire addition must fit into the under- or overflow bins of this instance
  // - the addition must have the same bin width as this instance 
  // and the minima must only differ by integer multiples of this width
  // during the addition, the scale factors of both instances will be applied to the data
  // if an inconsistency occurs, the function will issue an error message 
  // and exit gracefully returning false without currupting your data
  // if the contribution of this grid will be multiplied by thisFactor
  // the contribution of the newly added grid will be multiplied by factor
  // if the statError flag is true (default), errors will be added in quadrature
  thisFactor *= this->scaleFactor;
  factor *= addition->scaleFactor;
  if(!addition){
    std::cout << "ERROR in TQCounterGrid: cannot add NULL pointer!" << std::endl;
    return false;
  }
  if(addition->variable <= this->variable || addition->variable >= this->variable){
    for(size_t theirBin = 0; theirBin < addition->variable.nSteps+2; theirBin++){
      size_t myBin = this->getInterval(addition->getCenter(theirBin));
      if(!this->nodes[myBin]->add(addition->nodes[theirBin],factor,thisFactor,statError))
        return false;
    }
    this->scaleFactor = 1;
    return true;
  }
  std::cout << "Inconsistency detected: Trying to add grids with different variables!" << std::endl;
  return false;
}

size_t TQCounterGrid::nBins(const TString & varname){
  // obtain the number of bins (sub-nodes)
  // excluding over- and underflow bins
  if(varname == this->variable.name){
    return this->variable.nSteps;
  }
  if(this->nodes.size() == 0)
    return 0;
  return this->nodes[0]->nBins(varname);
}


size_t TQCounterGrid::nBinsTotal(const TString & varname){
  // obtain the total number of bins (sub-nodes)
  // including over- and underflow bins
  if(varname == this->variable.name){
    return this->nodes.size();
  }
  if(this->nodes.size() == 0)
    return 0;
  return this->nodes[0]->nBinsTotal(varname);
}

double TQCounterGrid::minimum(const TString & varname){
  // return the minimum of the lowest bin for the given variable
  if(varname == this->variable.name){
    return this->variable.min;
  }
  if(this->nodes.size() == 0)
    return 0;
  return this->nodes[0]->minimum(varname);
}

double TQCounterGrid::maximum(const TString & varname){
  // return the maximum of the highest bin for the given variable
  if(varname == this->variable.name){
    return this->variable.max;
  }
  if(this->nodes.size() == 0)
    return 0;
  return this->nodes[0]->maximum(varname);
}

double TQCounterGrid::stepWidth(const TString & varname){
  // return the step (or bin) width for the given variable
  // return the maximum of the highest bin for the given variable
  if(varname == this->variable.name){
    return this->variable.step;
  }
  if(this->nodes.size() == 0)
    return 0;
  return this->nodes[0]->stepWidth(varname);
}


TString TQCounterGrid::title(const TString & varname){
  // return the title of the given variable
  if(varname == this->variable.name){
    return this->variable.title;
  }
  if(this->nodes.size() == 0)
    return "";
  return this->nodes[0]->title(varname);
}

bool TQCounterGrid::isEqual(const TQCounterGrid& g) const {
  if(this->nodes.size() != g.nodes.size())
    return false;
  for(size_t i=0; i<this->nodes.size(); i++){
    if( ( (!this->nodes[i]) || (this->nodes[i]->valueRaw() == 0)) && ( (!g.nodes[i]) || (g.nodes[i]->valueRaw() == 0)))
      return true;
    if(! this->nodes[i]->isEqual(g.nodes[i]))
      return false;
  }
  return true;
}


bool TQCounterGrid::isEqual(const TQCounterGridNode* g) const {
  const TQCounterGrid* gr=dynamic_cast<const TQCounterGrid*>(g);
  if(!gr) return false;
  return this->isEqual(*gr);
}


bool operator==(const TQCounterGrid& g1, const TQCounterGrid& g2){
  return g1.isEqual(g2);
}

TString TQCounterGrid::getDetailsAsString(){
  return TString::Format("S(w) = %g +/- %g, n=%d",this->value(),this->uncertainty(),this->valueRaw());
}
