//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_NFMANUALSETTER__
#define __TQ_NFMANUALSETTER__

#include "QFramework/TQNFBase.h"
#include <iostream>
#ifdef __CINT__ 
#define override
#endif
class TQNFManualSetter : public TQNFBase {
public:
  /*
  enum Method {
    Single,
    FractionFitter,
    MatrixInversion,
    Unity,
    FAIL,
    UNDEFINED
  };
  */
protected:
  int status;
  //Why is this even here?
  /*inline Method getMethod(TString name){
    name.ToLower();
    if(name == "single" || name == "simple"){
      return Single;
    }
    if(name == "fractionfitter" || name == "tfractionfitter"){
      return FractionFitter;
    }
    if(name == "matrixinversion" || name == "matrix"){
      return MatrixInversion;
    }
    if(name == "unity" || name == "1" || name == "allone" || name == "noop"){
      return Unity;
    }
    if(name == "fail"){
      return FAIL;
    }
    if(this->verbosity > 0){
      this->messages.sendMessage(TQMessageStream::ERROR,"method '%s' not implemented!",name.Data());
    }
    this->status = -50;
    return UNDEFINED;
  }
  */

  TList* configs;

  //std::vector<Method> methods;

  bool initializeSelf() override;//
  bool finalizeSelf() override;//
  
  int deployNF(const TString& name, const std::vector<TString>& startAtCutNames, const std::vector<TString>& stopAtCut, TQFolder* config);//
  int deployNF(const TString& name, const TString& cutName, TQFolder* config);//

public:
  
  int deployResult(const std::vector<TString>& startCutNames, const std::vector<TString>& stopAtCut, int overwrite) override;
  
  bool success() override;//
  int execute(int itrNumber = -1) override;//
  void clear();//
  
  TString getStatusMessage() override; //
  int getStatus() override; //

  void printStatus();//

  bool readConfiguration(TQFolder* f) override;//

  TQNFManualSetter(TQSampleFolder* f = NULL);//
  TQNFManualSetter(TQSampleDataReader* rd);//
  virtual ~TQNFManualSetter();//

  ClassDefOverride(TQNFManualSetter,1)

};

#endif
