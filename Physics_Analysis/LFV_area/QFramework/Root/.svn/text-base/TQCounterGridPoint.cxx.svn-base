#include "QFramework/TQCounterGrid.h"
#include "TMath.h"
#include <iostream>
#include <map>

#include <stdint.h>
#include <sstream>
#include <limits>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQCounterGridPoint
//
// The TQCounterGridPoint is a leaf node of a TQCounterGrid, both inheriting from
// the abstract TQCounterGridNode base class
// One TQCounterGridPoint will store exactly one counter and correspond to one bin
// in the multi-dimensional histgram structure spanned by a TQCounterGrid
//
////////////////////////////////////////////////////////////////////////////////////////////////

// ClassImp(TQCounterGridPoint)

TQCounterGridPoint* TQCounterGridPoint::copy(){
  return new TQCounterGridPoint(this);
}

void TQCounterGridPoint::print(unsigned int indent){
  // print the contents of the gridpoint in a human-readable form
  for(size_t i=0; i<indent; i++){
    std::cout << "\t";
  }
  std::cout << this->sumW << " +/- " << sqrt(this->sumW2) << " (#" << this->sum << ")" << std::endl;
}

size_t TQCounterGridPoint::getMemSize(){
  // Obtain the total amount of space occupied by the actual data members
  // of the TQCounterGridPoint (two double values and one integer value). 
  return 2*sizeof(double) + sizeof(int);
}

TQCounterGridPoint::TQCounterGridPoint():
  TQCounterGridNode(1),
  sum(0),
  sumW(0),
  sumW2(0)
{
  // default constructor, initializing the point
  // without any data and a scale-factor of s=1
}

TQCounterGridPoint::TQCounterGridPoint(const TQCounterGridPoint* other):
  TQCounterGridNode(other ? other->scaleFactor : 1),
  sum(other ? other->sum : 0),
  sumW(other ? other->sumW : 0),
  sumW2(other ? other->sumW2 : 0)
{
  // copy constructor, pointer variant
}

TQCounterGridPoint::TQCounterGridPoint(const TQCounterGridPoint& other):
  TQCounterGridNode(other.scaleFactor),
  sum(other.sum),
  sumW(other.sumW),
  sumW2(other.sumW2)
{
  // copy constructor, pointer variant
}


TQCounterGridPoint::TQCounterGridPoint(double s) :
  TQCounterGridNode(s),
  sum(0),
  sumW(0),
  sumW2(0)
{
  // constructor variant, initializing the point
  // without any data and a given scale-factor
}

bool TQCounterGridPoint::isTrivial(){
  // return true if this is a trivial node (leaf)
  // and false otherwise
  // will return false for TQCounterGridPoints in all cases
  return true;
}

void TQCounterGridPoint::fill(double weight){
  // fill the TQCounterGrid with one entry of given weight
  // (default 1)
  this->sum++;
  this->sumW += weight;
  this->sumW2 += (weight*weight);
}

void TQCounterGridPoint::fillFromTree(double weight){
  // fill the TQCounterGrid with one entry of given weight
  // (default 1)
  this->fill(weight);
}

double TQCounterGridPoint::value(){
  // return the weighted value stored in this bin
  return (this->scaleFactor)*(this->sumW);
}
int TQCounterGridPoint::valueRaw(){
  // return the unweighted (raw) value stored in this bin
  return sum;
}
double TQCounterGridPoint::uncertainty(){
  // return the total uncertainty on the weighted value stored in this bin
  return (this->scaleFactor)*TMath::Sqrt(this->sumW2);
}

double TQCounterGridPoint::uncertaintyRel(){
  // return the relative uncertainty on the weighted value stored in this bin
  return TMath::Sqrt(this->sumW2)/this->sumW;
}

double TQCounterGridPoint::relUncertainty(){
  // return the relative uncertainty on the weighted value stored in this bin
  return TMath::Sqrt(this->sumW2)/this->sumW;
}

double TQCounterGridPoint::variance(){
  // return the total variance 
  // (square uncertainty)
  // on the weighted value stored in this bin
  return pow(this->scaleFactor,2)*(this->sumW2);
}

void TQCounterGridPoint::clear(){
  // erase all data stored in this point/bin
  // will set all counters to zero
  this->sum = 0;
  this->sumW = 0;
  this->sumW2 = 0;
}

bool TQCounterGridPoint::add(TQCounterGridNode* addition, double factor, double thisFactor, bool statError){
  // add another grid node to this one
  // this will only work if both nodes are compatible (same dimensionality)
  // internally, the addition will be cast to a (TQCounterGridPoint*)
  // and the corresponding variant of the add function will be used
  if(!addition){
    std::cout << "ERROR in TQCounterGridPoint: cannot add NULL pointer!" << std::endl;
    return false;
  }
  if(!addition->isTrivial()){
    std::cout << "ERROR: cannot add grids of different dimensionalities!" << std::endl;
    return false;
  }
  return this->add(dynamic_cast<TQCounterGridPoint*>(addition),factor,thisFactor,statError);
}

bool TQCounterGridPoint::add(TQCounterGridPoint* addition, double factor, double thisFactor, bool statError){
  // add another grid node to this one
  // this will only work if both nodes are compatible (same dimensionality)
  // if the contribution of this gridpoint will be multiplied by thisFactor
  // the contribution of the newly added gridpoint will be multiplied by factor
  // if the statError flag is true (default), errors will be added in quadrature
  if(!addition){
    std::cout << "ERROR in TQCounterGridPoint: cannot add NULL pointer!" << std::endl;
    return false;
  }
  this->sum += addition->sum;
  this->sumW = (this->sumW * thisFactor * this->scaleFactor) + (addition->sumW * factor * addition->scaleFactor);
  if(statError)
    this->sumW2 = (this->sumW2 * pow(thisFactor * this->scaleFactor,2)) + (addition->sumW2 * pow(factor * addition->scaleFactor,2));
  else
    this->sumW2 = pow((sqrt(this->sumW2) * thisFactor * this->scaleFactor) + (sqrt(addition->sumW2) * factor * addition->scaleFactor),2);
  this->scaleFactor = 1;
  return true;
}

bool TQCounterGridPoint::isEqual(const TQCounterGridPoint& p) const {
  if(p.sum != this->sum)
    return false;
  if(p.sumW != this->sumW)
    return false;
  if(p.sumW2 != this->sumW2)
    return false;
  return true;
}

bool TQCounterGridPoint::isEqual(const TQCounterGridNode* g) const {
  const TQCounterGridPoint* gr =dynamic_cast<const TQCounterGridPoint*>(g);
  if(!gr) return false;
  return this->isEqual(*gr);
}

bool operator==(const TQCounterGridPoint& g1, const TQCounterGridPoint& g2){
  return g1.isEqual(g2);
}

