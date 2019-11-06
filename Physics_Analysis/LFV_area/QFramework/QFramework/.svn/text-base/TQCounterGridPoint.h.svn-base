//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQ_COUNTERGRIDPOINT__
#define __TQ_COUNTERGRIDPOINT__

#include "QFramework/TQCounterGridNode.h"

class TQCounterGridPoint : public TQCounterGridNode {
protected:
  int sum;
  Double32_t sumW;
  Double32_t sumW2;

  TQCounterGridPoint* copy();
public:
  TQCounterGridPoint(double s);
  TQCounterGridPoint(const TQCounterGridPoint* other);
  TQCounterGridPoint(const TQCounterGridPoint& other);
  TQCounterGridPoint();

  size_t getMemSize();
  bool isTrivial();
  bool add(TQCounterGridNode* addition, double factor=1, double thisFactor=1, bool statError=true);
  bool add(TQCounterGridPoint* addition, double factor=1, double thisFactor=1, bool statError=true);

  void fill(double weight = 1);
  double value();
  int valueRaw();
  double uncertainty();
  double variance();
  double uncertaintyRel();
  double relUncertainty();
  void print(unsigned int indent = 0);

  void fillFromTree(double weight = 1);
  void clear();

  bool isEqual(const TQCounterGridPoint& g1) const;
  bool isEqual(const TQCounterGridNode* g1) const;
  friend bool operator==(const TQCounterGridPoint& g1, const TQCounterGridPoint& g2);

  //  ClassDef(TQCounterGridPoint, 2);
};

#endif
