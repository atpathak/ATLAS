//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQOptStringParser__
#define __TQOptStringParser__

#include "QFramework/TQNamedTaggable.h"
#include "TList.h"


class TQOptStringParser : public TQNamedTaggable {
protected:
  TList* fOpts;
public:
  TQOptStringParser();
  TQOptStringParser(const TString& serialization);
  ~TQOptStringParser();
 
  void addFlag(const TString& opt, const TString& tagname, bool invert=false);
  void addFlag(const TString& opt, bool invert=false);
  void addIntegerOption(const TString& opt, const TString& tagname, int defaultValue=0);
  void addIntegerOption(const TString& opt, int defaultValue=0);
  void addBlockOption(const TString& opt, const TString& tagname, const TString& defaultValue="", const TString& blocks="[]");
  void addBlockOption(const TString& opt);
 
  TQTaggable* parse(TString& left) const;
  void print();
  TString serialize();

  ClassDefOverride(TQOptStringParser,0)

};

#endif
