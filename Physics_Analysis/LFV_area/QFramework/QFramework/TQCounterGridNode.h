//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_COUNTERGRIDNODE__
#define __TQ_COUNTERGRIDNODE__

#include "TObject.h"
#include "TString.h"
#include "TTree.h"
#include "TTreeFormula.h"
#include "TH1.h"
#include "TH2.h"
#include <vector>

class TQCounterGridNode;
class TQCounterGridPoint;
class TQCounterGrid;

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-parameter"

class TQCounterGridNode {
protected:
  double scaleFactor;

  virtual void fillTH1F(const TString& varname, TH1F* hist, bool raw = false){};
  virtual TH1F* constructTH1F(const TString& varname){return NULL;};

  virtual void init(){}
  virtual TQCounterGridNode* copy() = 0;

  friend class TQCounterGridPoint;
  friend class TQCounterGrid;

public:

  virtual size_t getMemSize() = 0;

  virtual bool add(TQCounterGridNode* addition, double factor=1, double thisFactor=1, bool statError = true) = 0;

  TQCounterGridNode(double scale=1);
  virtual ~TQCounterGridNode();

  virtual void scale(double s);
  virtual void setScale(double s);
  virtual double getScale();

  virtual TQCounterGridNode* getNode(size_t i=0);

  virtual bool isTrivial() = 0;

  virtual void cleanup(){};
  virtual void clear() = 0;
  virtual void print(unsigned int indent = 0) = 0;

  virtual void fill(double weight = 1) = 0;
  virtual void fillFromTree(double weight = 1) = 0;
  virtual double value() = 0;
  virtual int valueRaw() = 0;
  virtual double uncertainty() = 0;
  virtual double uncertaintyRel() = 0;
  virtual double relUncertainty() = 0;
  virtual double variance() = 0;

  virtual size_t nBins (const TString & varname);
  virtual size_t nBinsTotal(const TString & varname);
  virtual double minimum(const TString & varname);
  virtual double maximum(const TString & varname);
  virtual double stepWidth(const TString & varname);
  virtual TString title(const TString & varname);

  virtual void setMinCut (const TString& varname, double varmin);
  virtual void setMaxCut (const TString& varname, double varmin);
  virtual void setRangeCuts(const TString& varname, double varmin, double varmax);
  virtual void setMinBin (const TString& varname, int binmin);
  virtual void setMaxBin (const TString& varname, int binmax);
  virtual void setRangeBins(const TString& varname, int binmin, int binmax);
  virtual void resetProjectionRanges();
  virtual void resetProjectionRange(const TString& varname);
  virtual void printProjectionRanges();

  virtual size_t getMinBin (const TString& varname);
  virtual size_t getMaxBin (const TString& varname);
  virtual double getMinCut (const TString& varname);
  virtual double getMaxCut (const TString& varname);

  virtual double getBinMin (const TString& varname, size_t bin);
  virtual double getBinMax (const TString& varname, size_t bin);

  virtual bool isEqual(const TQCounterGridNode* g1) const = 0;
 
  // ClassDefOverride(TQCounterGridNode, 2);

};

#pragma GCC diagnostic pop

#endif 
