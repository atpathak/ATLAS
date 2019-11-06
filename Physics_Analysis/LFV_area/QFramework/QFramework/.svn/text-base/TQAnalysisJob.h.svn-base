//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQAnalysisJob__
#define __TQAnalysisJob__

#include "TObject.h"
#include "TTree.h"
#include "QFramework/TQToken.h"
#include "QFramework/TQSampleFolder.h"
#include "QFramework/TQTreeObservable.h"


class TQSample;
class TQCut;

class TQAnalysisJob : public TNamed {
 
protected:
 
  TQCut * fCut; //!
 
  TQSample * fSample; //!
 
  void copyTransientMembersFrom(TQAnalysisJob* other);

public:

  TQAnalysisJob();
  TQAnalysisJob(TString name_);
 
  virtual void reset();

  virtual void print(const TString& options = "");
  
  virtual bool initializeSampleFolder(TQSampleFolder* sf);
  virtual bool finalizeSampleFolder (TQSampleFolder* sf);
 
  void setTreeObservableTemplates(TList * treeObservableTemplates);
 
  virtual TQAnalysisJob * getClone();

  virtual TString getDescription();

  void setCut(TQCut * cut_);
  TQCut * getCut();
  int addToCuts(TList* cuts, const TString& cutname = "*");
 
  virtual TObjArray * getBranchNames();
 
  bool initialize(TQSample * sample);
  bool finalize();

  virtual bool initializeSelf() = 0;
  virtual bool finalizeSelf() = 0;
 
  virtual bool execute(double weight);
 
  virtual ~TQAnalysisJob();
 
  ClassDefOverride(TQAnalysisJob, 1);

};

#endif
