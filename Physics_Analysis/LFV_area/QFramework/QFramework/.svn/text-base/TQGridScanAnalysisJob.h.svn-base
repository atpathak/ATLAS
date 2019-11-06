//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQGridScanAnalysisJob__
#define __TQGridScanAnalysisJob__

#include "QFramework/TQAnalysisJob.h"
#include "TObject.h"
#include "QFramework/TQCounterGrid.h"

class TQSample;

class TQGridScanAnalysisJob : public TQAnalysisJob {

protected:

  std::vector<TQCounterGridVariable> variables;
  std::vector<TTreeFormula*> formulas;
  TQCounterGrid* grid;
  TQSampleFolder* poolAt;

  void initializeObservables();
  void finalizeObservables();
  void initializeGrid();
  bool finalizeGrid();

public:

  TQGridScanAnalysisJob();
  TQGridScanAnalysisJob(TString name_);

  std::vector<TQCounterGridVariable> getBookedVariables();

  bool bookVariable(TQCounterGridVariable v);
  bool bookVariable(TString name, TString title, int nSteps, double min, double max, TString formula="");

  virtual bool initializeSampleFolder(TQSampleFolder* sf) override;
  virtual bool finalizeSampleFolder(TQSampleFolder* sf) override;
  virtual bool initializeSelf() override;
  virtual bool finalizeSelf() override;
  virtual bool execute(double weight) override;
 
  virtual ~TQGridScanAnalysisJob();
  virtual TObjArray * getBranchNames() override;
 
  ClassDefOverride(TQGridScanAnalysisJob, 1);

};

#endif
