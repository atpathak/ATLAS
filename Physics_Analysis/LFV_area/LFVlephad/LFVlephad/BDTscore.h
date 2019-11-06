//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __BDTSCORE__
#define __BDTSCORE__
#include "LFVlephad/LepHadObservable.h"

class BDTscore : public LepHadObservable {
protected:
  // put here data members you wish to retrieve
  // be careful to declare them as follows:
  // mutable ClassName const * varname = 0
  // the "mutable" keyword ensures that this member can be changed also by const functions
  // the "const" keyword ensures that const containers can be retrieved
  // for example, use
  // mutable CompositeParticleContainer const * mCand = 0;

public:
  virtual double getValue() const override;
protected:
  TString fExpression = "";

public:
  virtual bool hasExpression() const override;
  virtual const TString& getExpression() const override;
  virtual void setExpression(const TString& expr) override;

  BDTscore();
  BDTscore(const TString& expression);
  virtual ~BDTscore();
  ClassDefOverride(BDTscore, 1);


};
#endif
