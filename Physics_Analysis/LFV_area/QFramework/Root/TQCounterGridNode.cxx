#include "QFramework/TQCounterGridNode.h"
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQCounterGridNode
//
// TQCounterGridNode provides a common base class for the classes involved in the
// construction of the multi-dimensional histogram-like TQCounterGrid class. 
//
////////////////////////////////////////////////////////////////////////////////////////////////


#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

TQCounterGridNode::TQCounterGridNode(double scale):
  scaleFactor(scale)
{
  // The standard constructor with a name and a scale
  // of the CounterGrid (scale=1).
}

TQCounterGridNode::~TQCounterGridNode()
{
  // The standard destructor
}

void TQCounterGridNode::scale(double s){
  // The scale parameter can be adjusted.
  // All data retrieved from this CounterGrid
  // will be scaled by the given number.
  this->scaleFactor *= s;
}

void TQCounterGridNode::setScale(double s){
  // The scale parameter can be set.
  // All data retrieved from this CounterGrid
  // will be scaled by the given number.
  this->scaleFactor = s;
}

double TQCounterGridNode::getScale(){
  // retrieve the scale parameter
  return this->scaleFactor;
}

TQCounterGridNode* TQCounterGridNode::getNode(size_t i){
  // obtain the pointer to a given sub-node
  // will return NULL for trivial nodes (leaves)
  return NULL;
}

size_t TQCounterGridNode::nBins(const TString & varname){
  // obtain the number of bins (sub-nodes)
  // will return 0 for trivial nodes (leaves)
  return 0;
}

size_t TQCounterGridNode::nBinsTotal(const TString & varname){
  // obtain the total number of bins (sub-nodes)
  // will return 0 for trivial nodes (leaves)
  return 0;
}

void TQCounterGridNode::setMinCut (const TString& varname, double varmin){
  // set the minimum value for a given variable
  // all data retrieved from the grid will have this cut applied
  // the cut value will be rounded to the next bin boundary
}

void TQCounterGridNode::setMaxCut (const TString& varname, double varmin){
  // set the maximum value for a given variable
  // all data retrieved from the grid will have this cut applied
  // the cut value will be rounded to the next bin boundary
}

void TQCounterGridNode::setRangeCuts(const TString& varname, double varmin, double varmax){
  // set the minimum and maximum value for a given variable
  // all data retrieved from the grid will have these cuts applied
  // the cut values will be rounded to the next bin boundary
}

void TQCounterGridNode::setMinBin (const TString& varname, int binmin){
  // set the minimum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
}

void TQCounterGridNode::setMaxBin (const TString& varname, int binmax){
  // set the maximum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
}

void TQCounterGridNode::setRangeBins(const TString& varname, int binmin, int binmax){
  // set the minimum and maximum bin for a given variable
  // all data retrieved from the grid will have this restriction applied
}

void TQCounterGridNode::resetProjectionRanges(){
  // reset all cuts and restrictions
  // all data retrieved from the grid will (again) contain all bins in all dimensions
}

void TQCounterGridNode::resetProjectionRange(const TString& varname){
  // reset cuts and restrictions for the given variable
  // all data retrieved from the grid will (again) contain all bins in the given dimension
}

void TQCounterGridNode::printProjectionRanges(){
  // print all cuts and restrictions currently applied
}

double TQCounterGridNode::getBinMin (const TString& varname, size_t bin){
  // return the lower boundary of the bin of the given number and dimension
  return 0;
}

double TQCounterGridNode::getBinMax (const TString& varname, size_t bin){
  // return the upper boundary of the bin of the given number and dimension
  return 0;
}

double TQCounterGridNode::minimum(const TString & varname){
  // return the minimum of the lowest bin for the given variable
  return 0;
}
double TQCounterGridNode::maximum(const TString & varname){
  // return the maximum of the highest bin for the given variable
  return 0;
}
double TQCounterGridNode::stepWidth(const TString & varname){
  // return the step (or bin) width for the given variable
  return 0;
}

TString TQCounterGridNode::title(const TString & varname){
  // return the title of the given variable
  return "";
}


size_t TQCounterGridNode::getMinBin (const TString& varname){ return std::numeric_limits<size_t>::quiet_NaN(); }
size_t TQCounterGridNode::getMaxBin (const TString& varname){ return std::numeric_limits<size_t>::quiet_NaN(); }
double TQCounterGridNode::getMinCut (const TString& varname){ return std::numeric_limits<double>::quiet_NaN(); }
double TQCounterGridNode::getMaxCut (const TString& varname){ return std::numeric_limits<double>::quiet_NaN(); }

#pragma GCC diagnostic pop;

