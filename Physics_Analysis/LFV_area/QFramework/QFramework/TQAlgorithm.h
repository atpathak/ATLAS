//this file looks like plain C, but it's actually -*- c++ -*-
#ifndef __TQAlgorithm__
#define __TQAlgorithm__

#include "TQNamedTaggable.h"

class TQSample;

class TQAlgorithm : public TQNamedTaggable {
public:
  class Manager { // nested
  protected:
    std::vector<TQAlgorithm*> gAlgorithmList;
  public:
    bool addAlgorithm(TQAlgorithm* newAlgorithm);
    void clearAlgorithms();
    const std::vector<TQAlgorithm*>& getAlgorithms();
    void printAlgorithms();
    bool initializeAlgorithms(TQSample*s);
    bool finalizeAlgorithms();
    bool executeAlgorithms();
    bool cleanupAlgorithms();
  };
  
  virtual bool initialize(TQSample* s) = 0;
  virtual bool finalize() = 0;
  virtual bool execute() = 0;
  
  virtual bool cleanup() = 0;
};
#endif
