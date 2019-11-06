#include "QFramework/TQAlgorithm.h"

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQAlgorithm
//
// The TQAlgorithm class is a hook in order to precompute arbitrary
// event-based quantities and cache their values. The entire list of
// algorithms is executed on every event.
//
// every algorithm should implement the initialize, finalize and execute methods.
// initialize will be called on every sample and will take the sample as an argument
// finalize and execute will not receive arguments
// all three functions should return true in case of success, false in case of failure
// failure of any algorithm will cause the event loop to halt
// additional error handling may be done by raising exceptions
//
// all input/output managing is entirely up to the user and may be
// done via filling the TStore or creating decorations to objects in
// the TEvent
//
////////////////////////////////////////////////////////////////////////////////////////////////

bool TQAlgorithm::Manager::addAlgorithm(TQAlgorithm* newAlgorithm){
  // add a new cache
  // the function takes ownership of the object, you shall not delete it!
  for(size_t i=0; i<this->gAlgorithmList.size(); ++i){
    if(TQStringUtils::equal(gAlgorithmList[i]->GetName(),newAlgorithm->GetName())){
      return false;
    }
  }
  this->gAlgorithmList.push_back(newAlgorithm);
  return true;
}

void TQAlgorithm::Manager::clearAlgorithms(){
  // clear the list of caches, deleting every one of them
  for(size_t i=0; i< this->gAlgorithmList.size(); ++i){
    delete this->gAlgorithmList[i];
  }
}

const std::vector<TQAlgorithm*>& TQAlgorithm::Manager::getAlgorithms(){
  // retrieve a const reference to the list of all caches
  return this->gAlgorithmList;
}

bool TQAlgorithm::Manager::initializeAlgorithms(TQSample*s){
  // initialize all algorithms
  for(size_t i=0; i<this->gAlgorithmList.size(); ++i){
    if(! this->gAlgorithmList[i]->initialize(s)) return false;
  }
  return true;
}

bool TQAlgorithm::Manager::finalizeAlgorithms(){
  // finalize all algorithms
  for(size_t i=0; i<this->gAlgorithmList.size(); ++i){
    if(! this->gAlgorithmList[i]->finalize()) return false;
  }
  return true;
}

bool TQAlgorithm::Manager::executeAlgorithms(){
  // execute all algorithms
  for(size_t i=0; i<this->gAlgorithmList.size(); ++i){
    if(! this->gAlgorithmList[i]->execute()) return false;
  }
  return true;
}

bool TQAlgorithm::Manager::cleanupAlgorithms(){
  // cleanup all algorithms
  for(size_t i=0; i<this->gAlgorithmList.size(); ++i){
    if(! this->gAlgorithmList[i]->cleanup()) return false;
  }
  return true;
}
