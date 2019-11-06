//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_COUNTERGRID__
#define __TQ_COUNTERGRID__

#include "QFramework/TQObservable.h"
#include "QFramework/TQCounterGridNode.h"
#include "QFramework/TQCounterGridPoint.h"
#include "QFramework/TQCounterGridVariable.h"

class TQCounterGrid : public TQCounterGridNode, public TNamed{
protected:
  TQCounterGridVariable variable;
  std::vector<TQCounterGridNode*> nodes;
 
  size_t minBin; //!
  size_t maxBin; //!

  void setMinBin(int minbin);
  void setMaxBin(int maxbin);
  double getBinMin(size_t bin);
  double getBinMax(size_t bin);
  size_t getInterval(double val);
  double getCenter(size_t i);

  virtual void fillTH1F(const TString& varname, TH1F* hist, bool raw = false) override;

  virtual TQCounterGrid* copy() override;
  void copyNodes(TQCounterGrid* other);
  virtual void init() override;

public:
  TQCounterGrid(std::vector<TQCounterGridVariable> vars, double scale = 1);
  TQCounterGrid(std::vector<TQCounterGridVariable*> vars, double scale = 1);
  TQCounterGrid(TQCounterGridVariable*v, double scale = 1);
  TQCounterGrid(TQCounterGridVariable v, double scale = 1);
  TQCounterGrid(TQCounterGridVariable*v1, TQCounterGridVariable*v2, double scale = 1);
  TQCounterGrid(TQCounterGridVariable v1, TQCounterGridVariable v2, double scale = 1);
  TQCounterGrid(TQCounterGridVariable*v1, TQCounterGridVariable*v2, TQCounterGridVariable*v3, double scale = 1);
  TQCounterGrid(TQCounterGridVariable v1, TQCounterGridVariable v2, TQCounterGridVariable v3, double scale = 1);

  TQCounterGrid(double scale);
  TQCounterGrid(TQCounterGrid* other);
  TQCounterGrid(TQCounterGrid& other);
  TQCounterGrid();

  bool hasVariable(const TString& varname);
  void setVariableTitle(const TString& varname, const TString& vartitle);
  TString getVariableTitle(const TString& varname);
  TQCounterGridVariable getVariable(const TString& varname);

  ~TQCounterGrid();
  virtual size_t getMemSize() override;
  virtual bool isTrivial() override;
  virtual void resetProjectionRanges() override;
  virtual void resetProjectionRange(const TString& varname) override;
  virtual void printProjectionRanges() override;

  TString getDetailsAsString();

  void compactify();
  void ensureNode(size_t i);

  void setObservable(const TString& variable, TQObservable* f);
  virtual void clear() override;
  virtual void print(unsigned int indent = 0) override;
  virtual bool add(TQCounterGridNode* addition, double factor=1, double thisFactor = 1, bool statError=true) override;
  bool add(TQCounterGrid* addition, double factor=1, double thisFactor = 1, bool statError=true);

  virtual void setMinCut (const TString& varname, double varmin) override;
  virtual void setMaxCut (const TString& varname, double varmin) override;
  virtual void setRangeCuts (const TString& varname, double varmin, double varmax) override;
  virtual void setMinBin (const TString& varname, int binmin) override;
  virtual void setMaxBin (const TString& varname, int binmax) override;
  virtual size_t getMinBin (const TString& varname) override;
  virtual size_t getMaxBin (const TString& varname) override;
  virtual double getMinCut (const TString& varname) override;
  virtual double getMaxCut (const TString& varname) override;
  virtual void setRangeBins (const TString& varname, int binmin, int binmax) override;
  TH1F* getTH1F (const TString& varname, bool raw = false);
  TH2F* getTH2F (const TString& xvarname, const TString& yvarname, bool raw = false);

  virtual double getBinMin (const TString& varname, size_t bin) override;
  virtual double getBinMax (const TString& varname, size_t bin) override;
  size_t getInterval(const TString& varname, double val);

  virtual TQCounterGridNode* getNode(size_t i=0) override;

  virtual size_t nBins(const TString & varname) override;
  virtual size_t nBinsTotal(const TString & varname) override;
  virtual double minimum(const TString & varname) override;
  virtual double maximum(const TString & varname) override;
  virtual double stepWidth(const TString & varname) override;
  virtual TString title(const TString & varname) override;

  virtual void fill(double weight = 1) override;
  virtual void fillFromTree(double weight = 1) override;
  virtual double value() override;
  virtual int valueRaw() override;
  virtual double uncertainty() override;
  virtual double uncertaintyRel() override;
  virtual double relUncertainty() override;
  virtual double variance() override;

  bool isEqual(const TQCounterGrid& g1) const;
  virtual bool isEqual(const TQCounterGridNode* g1) const override;
  friend bool operator==(const TQCounterGrid& g1, const TQCounterGrid& g2);

  ClassDefOverride(TQCounterGrid, 2);
};

#endif
