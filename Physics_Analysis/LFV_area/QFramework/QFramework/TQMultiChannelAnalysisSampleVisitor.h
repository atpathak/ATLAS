//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQMultiChannelAnalysisSampleVisitor__
#define __TQMultiChannelAnalysisSampleVisitor__

#include <map>

#include "TTree.h"

#include "QFramework/TQCut.h"
#include "QFramework/TQAnalysisSampleVisitorBase.h"

class TQMultiChannelAnalysisSampleVisitor : public TQAnalysisSampleVisitorBase {
 
protected:

  bool fUseObservableSets = true;
  std::map<TString,TQCut*> fChannels;

  bool checkCut(TQCut * baseCut);
  bool checkChannel(const TString& pathPattern);
  
  int analyseTree(TQSample * sample, TString& message);

  virtual int visitFolder(TQSampleFolder * sampleFolder, TString& message) override;
  virtual int revisitFolder(TQSampleFolder * sampleFolder, TString& message) override;
  virtual int visitSample(TQSample * sample, TString& message) override;
  virtual int revisitSample(TQSample * sample, TString& message) override;
  
  void updateFriends(TQSampleFolder* sf);
  
public: 
 
  TQMultiChannelAnalysisSampleVisitor();

  void printChannels();
  void useObservableSets(bool useSets);
  void addChannel(const TString& pathPattern, TQCut * baseCut);
  TQCut * getBaseCut(const TString& path);

  virtual ~TQMultiChannelAnalysisSampleVisitor();
 
  ClassDefOverride(TQMultiChannelAnalysisSampleVisitor, 0);
 
};

#endif


