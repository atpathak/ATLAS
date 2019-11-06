//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_COUNTERGRIDVARIABLE__
#define __TQ_COUNTERGRIDVARIABLE__

#include "TString.h"
#include "QFramework/TQObservable.h"

class TQCounterGridVariable : public TNamed {
protected:
  TQObservable* fObservable;

public:
  TString name;
  TString title;
  TString expression;
  double min;
  double max;
  unsigned int nSteps;
  double step;

  friend bool operator== (TQCounterGridVariable &v1, TQCounterGridVariable &v2);
  friend bool operator!= (TQCounterGridVariable &v1, TQCounterGridVariable &v2);
  friend bool operator< (TQCounterGridVariable &v1, TQCounterGridVariable &v2);
  friend bool operator> (TQCounterGridVariable &v1, TQCounterGridVariable &v2);
  friend bool operator<=(TQCounterGridVariable &v1, TQCounterGridVariable &v2);
  friend bool operator>=(TQCounterGridVariable &v1, TQCounterGridVariable &v2);

  bool initialize(TQSample* s);
  bool finalize();
  TQObservable* getObservable();

  TQCounterGridVariable();
  TQCounterGridVariable(const TQCounterGridVariable* other);
  TQCounterGridVariable(const TQCounterGridVariable& other);
  ~TQCounterGridVariable();
  TQCounterGridVariable(const TString& name);
  TQCounterGridVariable(const TString& name, const TString& title, int nSteps, double min, double max, const TString& exp = "");
  double evaluateFromTree() const;

  ClassDefOverride(TQCounterGridVariable, 3);
};

#endif
