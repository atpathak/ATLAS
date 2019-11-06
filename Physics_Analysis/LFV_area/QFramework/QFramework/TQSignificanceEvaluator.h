//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_SIGNIFICANCEEVALUTATOR__
#define __TQ_SIGNIFICANCEEVALUTATOR__

#include "TString.h"
#include "TNamed.h"
#include "QFramework/TQTaggable.h"
#include "TH1.h"
#include "THnBase.h"
#include "QFramework/TQCounterGrid.h"
#include "QFramework/TQSampleDataReader.h"

class TQSampleDataReader;
class TQGridScanner;
class TQSampleFolder;

class TQSignificanceEvaluator : public TQTaggable, public TNamed {
protected:
  TClass* initialization; //!
  TQSampleDataReader* reader; //!
  TQGridScanner* scanner; //!
  double luminosity; 
  double luminosityScale;
  TString fileIdentifier; //!
  bool verbose; //!
  std::vector<TString> regions; //!
  std::vector<TString> autoNFs; //!
public:
  TString info;
  virtual double getLuminosity(TString folderName="info",TString tagName="luminosity");
  virtual bool scaleLuminosity(double lumi = 1000);
  virtual void setFileIdentifier(TString s);
  virtual void setVerbose(bool v = true);
	virtual bool setRangeAxis(int axis, double low, double up);
	virtual bool updateHists(std::vector<int> axisToScan, TQGridScanner* scanner, int axisToEvaluate);
  TQSampleDataReader* getReader();

  virtual void bookNF(TString path);
  virtual void addRegion(TString cutname);

  virtual bool hasNativeRegionSetHandling();
  virtual bool prepareNextRegionSet(const TString& suffix = "");
  virtual bool isPrepared();

  virtual double evaluate() = 0;
  TQSignificanceEvaluator(const TString& name = "");
  TQSignificanceEvaluator(const TString& name, TQSampleFolder* sf);
  virtual bool initialize(TQGridScanner* scanner) = 0;

  virtual double getSignificance(size_t i) = 0;
  virtual double getSignificance2(size_t i) = 0;

  ClassDefOverride(TQSignificanceEvaluator,1)

};

class TQSimpleSignificanceEvaluator : public TQSignificanceEvaluator {
protected:
  TString signalPath; //! 
  TString backgroundPath; //!

	TH1F* bkgproj;
	TH1F* sigproj;
  std::vector<THnBase*> backgroundHists; //!
  std::vector<THnBase*> signalHists; //!
  virtual double getSignificance(size_t i) override;
  virtual double getSignificance2(size_t i) override;

public:
  virtual double evaluate() override;
  virtual bool initialize(TQGridScanner* scanner) override;
  virtual void printHistogramAxis();
	virtual bool setRangeAxis(int axis, double low, double up) override;
	virtual bool updateHists(std::vector<int> axisToScan, TQGridScanner* scanner, int axisToEvaluate) override;
  TQSimpleSignificanceEvaluator(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="s/sqrt(b)");

  ClassDefOverride(TQSimpleSignificanceEvaluator,1)
};

class TQSimpleSignificanceEvaluator2 : public TQSimpleSignificanceEvaluator {
protected:
  virtual double getSignificance2(size_t i) override;
public:
  TQSimpleSignificanceEvaluator2(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="s/sqrt(s+b)");

  ClassDefOverride(TQSimpleSignificanceEvaluator2,1)
};

class TQSimpleSignificanceEvaluator3 : public TQSimpleSignificanceEvaluator {
protected:
  virtual double getSignificance2(size_t i) override;
  TString _name;
public:
  TQSimpleSignificanceEvaluator3(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="s/b");

  ClassDefOverride(TQSimpleSignificanceEvaluator3,1)
};

class TQPoissonSignificanceEvaluator : public TQSimpleSignificanceEvaluator {
protected:
  virtual double getSignificance2(size_t i) override;
public:
  TQPoissonSignificanceEvaluator(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="poisson");

  ClassDefOverride(TQPoissonSignificanceEvaluator,1)
};

// class TQSeparationSignificanceEvaluator : public TQSimpleSignificanceEvaluator {
// protected:
//   virtual double getSignificance2(size_t i) override;
//   TString _name;
//   TString _varName;
// public:
//   TQSeparationSignificanceEvaluator(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString varName="", TString name="sep");

//   ClassDefOverride(TQSeparationSignificanceEvaluator,1)
// };

// class TQWeightedSignificanceEvaluator : public TQSimpleSignificanceEvaluator {
// protected:
//   virtual double getSignificance2(size_t i) override;
// public:
//   TQWeightedSignificanceEvaluator(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="s/db");

//   ClassDefOverride(TQWeightedSignificanceEvaluator,1)
// };

// class TQWeightedSignificanceEvaluator2 : public TQSimpleSignificanceEvaluator {
// protected:
//   virtual double getSignificance2(size_t i) override;
// public:
//   TQWeightedSignificanceEvaluator2(TQSampleFolder* sf=NULL, TString signal="sig", TString background="bkg", TString name="s/sqrt(ds2+db2)");

//   ClassDefOverride(TQWeightedSignificanceEvaluator2,1)
// };


#endif
