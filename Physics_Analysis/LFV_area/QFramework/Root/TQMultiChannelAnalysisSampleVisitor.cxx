#include "QFramework/TQMultiChannelAnalysisSampleVisitor.h"

#include "QFramework/TQCounter.h"
#include "QFramework/TQUtils.h"
#include "QFramework/TQSample.h"
#include "QFramework/TQCut.h"
#include "QFramework/TQStringUtils.h"
#include "QFramework/TQIterator.h"

#include "TList.h"
#include "TObjString.h"
#include "TStopwatch.h"

// #define _DEBUG_
#include "QFramework/TQLibrary.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <stdexcept>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQMultiChannelAnalysisSampleVisitor:
//
// Visit samples and execute analysis jobs at cuts. 
//
////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(TQMultiChannelAnalysisSampleVisitor)


//__________________________________________________________________________________|___________

TQMultiChannelAnalysisSampleVisitor::TQMultiChannelAnalysisSampleVisitor() : 
TQAnalysisSampleVisitorBase("mcasv",false)
{
  // Default constructor of the TQMultiChannelAnalysisSampleVisitor class
  this->setVisitTraceID("analysis");
  this->setTagString("tagKey","~.mcasv.channel");
}

//__________________________________________________________________________________|___________

TQMultiChannelAnalysisSampleVisitor::~TQMultiChannelAnalysisSampleVisitor(){
  // default destructor
}

//__________________________________________________________________________________|___________

int TQMultiChannelAnalysisSampleVisitor::visitFolder(TQSampleFolder * sampleFolder, TString& message) {
  //if this was already visited, we don't need to do so again
  if(!sampleFolder) return visitFAILED;
  DEBUG("checking visit on '%s'",sampleFolder->getPath().Data());
  if (this->checkVisit(sampleFolder)){
    DEBUG("folder '%s' has been visited before...",sampleFolder->getPath().Data());
    return visitIGNORE;
  }
  const TString tagKey = this->getTagStringDefault("tagKey","~.mcasv.channel");
  updateFriends(sampleFolder);
  std::set<TString> foundChannels;
  TQSampleFolderIterator itr(sampleFolder->getFriends());
  while(itr.hasNext()) {
    TQSampleFolder* sf = itr.readNext();
    DEBUG("checking if '%s' has been visited before...",sf->getPath().Data());
    if(this->checkVisit(sf)) continue;
    TString channel = sf->getTagStringDefault(tagKey,"");
    if(foundChannels.find(channel) != foundChannels.end()){
      DEBUG("skipping '%s' due to channel conflict in channel '%s'...",sf->getPath().Data(),channel.Data());
      continue;
    }
    TQCut* cut = this->fChannels[channel];
    if(!cut){
      DEBUG("no cut avialable for channel '%s' required by '%s'- skipping",channel.Data(),sf->getPath().Data());
      return visitFAILED;
    }
    DEBUG("trying to initialize SampleFolder with path '%s'",sf->getPath().Data());
    if(!cut->initializeSampleFolder(sf)) return visitFAILED;
    this->stamp(sf);
    foundChannels.insert(channel);
  }
  return visitLISTONLY;
}

//__________________________________________________________________________________|___________

int TQMultiChannelAnalysisSampleVisitor::revisitSample(TQSample * sample, TString& message) {
  // revisit an instance of TQSample on the way out
  return visitIGNORE;
}



//__________________________________________________________________________________|___________

int TQMultiChannelAnalysisSampleVisitor::revisitFolder(TQSampleFolder * sampleFolder, TString& message) {
  
  const TString tagKey = this->getTagStringDefault("tagKey","~.mcasv.channel");
  bool finalized = true;
  
  TQSampleFolderIterator itr(sampleFolder->getFriends());
  
  while(itr.hasNext()) {
    TQSampleFolder* sf = itr.readNext();
    TString channel = sf->getTagStringDefault(tagKey,"");
    TQCut* cut = this->fChannels[channel];
    if(!cut) continue;
  
    finalized = finalized && cut->finalizeSampleFolder(sf);
    
    //generalization of objects is not yet ported to TQMultiChannelAnalysisSampleVisitor. the Following lines are just a copy from TQAnalysisSampleVisitor to give a rough guideline
    /* 
    bool generalizeHistograms = false;
    bool generalizeCounter = false;
    bool generalizeCounterGrids = false;
    //@tag: [.asv.generalize.<objectType>] These folder tags determine if objects of type "<objectType>" (=histograms,counter,countergrid) should be generalized using TQSampleFolder::generalizeObjects. Default: true
    sampleFolder->getTagBool(".asv.generalize.histograms", generalizeHistograms, true);
    sampleFolder->getTagBool(".asv.generalize.counter", generalizeCounter, true);
    sampleFolder->getTagBool(".asv.generalize.countergrid", generalizeCounterGrids, true);
  
    bool generalized = false;
    bool doGeneralize = generalizeHistograms || generalizeCounter || generalizeCounterGrids;
  
    if (doGeneralize){
  
      // start the stop watch 
      TStopwatch * timer = new TStopwatch();
  
      // generalize all histograms 
      int nHistos = 0;
      if (generalizeHistograms)
        nHistos = sampleFolder->generalizeHistograms();
  
      // generalize all counter 
      int nCounter = 0;
      if (generalizeCounter)
        nCounter = sampleFolder->generalizeCounter();
  
      // generalize all counter 
      int nCounterGrid = 0;
      if (generalizeCounterGrids)
        nCounterGrid = sampleFolder->generalizeCounterGrid();
  
      // stop the timer 
      timer->Stop();
  
      message.Append(" ");
      message.Append(TQStringUtils::fixedWidth("--", 12,"r"));
      message.Append(TQStringUtils::fixedWidth(TString::Format("%.2f", timer->RealTime()), 12,"r"));
      message.Append(" ");
      
      if (nHistos == 0 && nCounter == 0 && nCounterGrid == 0)
        message.Append("nothing to generalize");
      else{
        message.Append("generalized: ");
        if(nHistos > 0){
          if (generalizeHistograms) message.Append(TString::Format("%d histograms", nHistos));
        }
        if(nCounter + nCounterGrid > 0 && nHistos > 0){
          message.Append(", ");
        }
        if(nCounter > 0){
          message.Append(TString::Format("%d counters", nCounter));
        }
        if(nCounterGrid > 0 && nCounter > 0){
          message.Append(", ");
        }
        if(nCounterGrid > 0){
          message.Append(TString::Format("%d grids", nCounterGrid));
        }
      }
   
      // delete the timer 
      delete timer;
  
      if (nHistos > 0 || nCounter > 0 || nCounterGrid > 0)
        generalized = true;
  
    } else {
      if(finalized)
        return visitIGNORE;
    }
    if(generalized || !doGeneralize){
      if(finalized){
        return visitOK;
      } else
        return visitWARN;
    } else {
      if(finalized)
        return visitWARN;
    }
    return visitFAILED;
    */

  }

  return finalized ? visitLISTONLY : visitWARN;

}


//__________________________________________________________________________________|___________

int TQMultiChannelAnalysisSampleVisitor::visitSample(TQSample * sample, TString& message) {
  // Run the analysis jobs on a sample
  if(this->checkVisit(sample)){
    return visitSKIPPED;
  } else {
    #ifdef _DEBUG_
    sample->printTags();
    #endif
  }
  
  TStopwatch * timer = new TStopwatch();

  /* analyse the tree */
  TString analysisMessage;
  #ifndef _DEBUG_
  //TQLibrary::redirect_stderr("/dev/null");
  #endif
  DEBUGclass("analysing tree");
  int nEntries = this->analyseTree(sample, analysisMessage);
  #ifndef _DEBUG_
  TQLibrary::restore_stderr();
  #endif
  /* stop the timer */
  timer->Stop();

  /* compile the message */
  message.Append(" ");

  /* save the number of entries in tree analyzed */
  sample->setTagInteger(TString::Format(".%s.analysis.nentries",this->GetName()),nEntries);

  if (nEntries >= 0) {
    message.Append(TQStringUtils::fixedWidth(TQStringUtils::getThousandsSeparators(nEntries), 12,"r"));
  } else {
    message.Append(TQStringUtils::fixedWidth("--", 12,"r"));
  }
 
  message.Append(TQStringUtils::fixedWidth(TString::Format("%.2f", timer->RealTime()), 12,"r"));
  message.Append(" ");
  message.Append(TQStringUtils::fixedWidth(analysisMessage, 40, "l"));

  /* delete the timer */
  delete timer;

  if(nEntries > 0){
    return visitOK;
  }
  else if (nEntries == 0)
    return visitWARN;

  return visitFAILED;
 
}

//__________________________________________________________________________________|___________

bool TQMultiChannelAnalysisSampleVisitor::checkCut(TQCut * baseCut) {
  // checks if a given cut is already added to some channel
  for (auto it=this->fChannels.begin(); it!=this->fChannels.end(); ++it){
    if(it->second == baseCut) return true;
  }
  return false;
}

//__________________________________________________________________________________|___________

bool TQMultiChannelAnalysisSampleVisitor::checkChannel(const TString& channelName) {
  // checks if a channel is already added to the iterator
  auto it= this->fChannels.find(channelName);
  if(it==this->fChannels.end()){
    return false;
  }
  return true;
}

//__________________________________________________________________________________|___________

void TQMultiChannelAnalysisSampleVisitor::printChannels() {
  // print the currently scheduled channel
  for (auto it=this->fChannels.begin(); it!=this->fChannels.end(); ++it){
    std::cout << TQStringUtils::fixedWidth(it->first,40) << " " << TQStringUtils::fixedWidth(it->second->GetName(),20) << " @" << it->second << std::endl;
  }
}

//__________________________________________________________________________________|___________

void TQMultiChannelAnalysisSampleVisitor::addChannel(const TString& channelName, TQCut * baseCut) {
  // add a new channel to this visitor, scheduling the visit of the given channelName with the given basecut
  if(channelName.IsNull()){
    ERRORclass("unable to use empty channel name");
    return;
  }
  if(baseCut){
    if(this->checkCut(baseCut)){
      ERRORclass("not adding basecut '%s' -- this cut was already scheduled for another channel!",baseCut->GetName());
      return;
    }
    if(this->checkChannel(channelName)){
      ERRORclass("channel '%s' already added to this visitor!",channelName.Data());
      return;
    }
    this->fChannels[channelName] = baseCut;
  } else {
    if(this->checkChannel(channelName)){
      this->fChannels.erase(channelName);
      return;
    } else {
      WARNclass("attempt to add new channel '%s' with baseCut=NULL",channelName.Data());
      return;
    }
  }
}


//__________________________________________________________________________________|___________

TQCut * TQMultiChannelAnalysisSampleVisitor::getBaseCut(const TString& channelName) {
  // retrieve the basecut corresponding to that channelName
  if(this->checkChannel(channelName)){
    return this->fChannels[channelName];
  } else {
    return NULL;
  }
}


//__________________________________________________________________________________|___________

int TQMultiChannelAnalysisSampleVisitor::analyseTree(TQSample * sample, TString& message) {
  // analyse the tree in this sample
  DEBUGclass("entering function");
  DEBUGclass("testing sample");
  if (!sample) {
    message = "sample is NULL";
    DEBUGclass(message);
    return -1;
  }

  // let the sample find some friends
  if(!sample->hasFriends()){
    sample->findFriends();
  }
  if(!sample->hasFriends()){
    message = "no friends found";
    WARN("Sample '%s' has no friends. Please check for problems in your setup!",sample->getPath().Data());
  }
  TQToken* tok = sample->getTreeToken();
  if(!tok){
    message="failed to obtain tree token";
    return -1;
  }
  TTree* tree = static_cast<TTree*>(tok->getContent());
  if(!tree){
    message="failed to retrieve shared tree";
    return -1;
  }
  
  TQSampleIterator itr(sample->getFriends());
  DEBUGclass("retrieving number of entries");
  const Long64_t nEntries = std::min(tree->GetEntries(),this->fMaxEvents);

  std::vector<bool> useMCweights;
  std::vector<TQCut*> cuts;
  //@tag:tagKey: control which tag on the sample folders will be used to identify the cut set to be used (default: ~.mcasv.channel)
  const TString tagKey = this->getTagStringDefault("tagKey","~.mcasv.channel");
  tree->SetBranchStatus("*", 1);
  while(itr.hasNext()){
    TQSample* s = itr.readNext();
    if(!s) continue;
    TString channel;
    if(!s->getTagString(tagKey,channel)){
      message = TString::Format("sample '%s' has no channel set as '%s'",s->getPath().Data(),tagKey.Data());
      return -1;
    }
    if(!this->checkChannel(channel)){
      message = TString::Format("channel '%s' is unknown from %s:%s",channel.Data(),s->getPath().Data(),tagKey.Data());
      return -1;
    }
    if(this->fUseObservableSets){
      if(!TQObservable::manager.setActiveSet(channel)){
        TQObservable::manager.cloneActiveSet(channel);
      }
    }
  }
  
  itr.reset();
  std::set<TString> foundChannels;
  while(itr.hasNext()){
    TQSample* s = itr.readNext();
    if(!s) continue;
    if (this->checkVisit(s)) continue;
    TString channel;
    if(!s->getTagString(tagKey,channel)){
      throw std::runtime_error(TString::Format("no channel information set on sample '%s'",s->getPath().Data()).Data());
    }
    if(foundChannels.find(channel)!=foundChannels.end()){
      continue;
    }
    TQObservable::manager.setActiveSet(channel);
    TQCut* basecut = this->fChannels[channel];
    DEBUGclass("initializing sample '%s' with basecut '%p' for channel '%s'",s->getPath().Data(),basecut,channel.Data());
    if (!basecut->initialize(s)) {
      throw std::runtime_error(TString::Format("failed to initialize cuts for channel '%s' on sample '%s'",channel.Data(),s->getPath().Data()).Data());
    }
    // check wether to use MC weights
    DEBUGclass("testing for usemcweights tag");
    bool useWeights = false;
    s->getTagBool("usemcweights", useWeights, true);
    
    // push all required information into vectors
    useMCweights.push_back(useWeights);
    cuts.push_back(basecut);
    foundChannels.insert(channel);
  }

  // handle branch status
  tree->SetBranchStatus("*", 0);
  for(size_t i=0; i<cuts.size(); ++i){
    TCollection* bnames = cuts[i]->getListOfBranches();
    if(!this->setupBranches(tree,bnames)){
      throw std::runtime_error(TString::Format("failed to setup branches for sample '%s'",cuts[i]->getSample()->getPath().Data()).Data());
    }
    bnames->SetOwner(true);
    delete bnames;
  }

  // loop over tree entries
  DEBUGclass("entering event loop");
  #ifdef _DEBUG_
  std::cout<<"Tree adress: "<<tree<<std::endl;
  #endif
  const size_t n = cuts.size();
  const Long64_t nEventsPerPercent = ceil(nEntries/100.); //+1 is a simple cosmetic patch to actually show the expected percent values later
  const Long64_t progressInterval = nEventsPerPercent*this->getTagDoubleDefault("progressInterval",0.);
  for (Long64_t i = 0; i < nEntries; ++i) {
    //DEBUGclass(" visiting entry %d/%d",i,nEntries);
    tree->GetEntry(i);
    for(size_t j=0; j<n; ++j){
      //DEBUGclass("  friend %d",j);
      TRY(
        cuts[j]->analyse(1., useMCweights[j]);
      ,TString::Format("An error occured while evaluating entry %d of sample '%s'.",i,(cuts[j]->getSample()!=0?cuts[j]->getSample()->getPath().Data():"<undefined>"))
      )
    }
    if ( i%progressInterval == 0 ) {
      //TODO: update status buttom to reflect current status/progress
      this->updateLine(fStatusLine,message,visitPROGRESS,progressInterval<=0,((double)i)/nEntries  );
    }
  }

  // finalize the cuts
  for(size_t j=0; j<n; ++j){
    TQSampleFolder* sf = cuts[j]->getSample();
    this->stamp(sf);
    cuts[j]->finalize();
  }

  
  sample->returnTreeToken(tok);
 
  DEBUGclass("finished analyzing sample '%s'",sample->GetName());

  if (sample->getNTreeTokens() > 0) {
    std::cout << std::endl;
    message="sample left with # tree tokens > 0";
    sample->printTreeTokens();
    std::cout << std::endl;
    std::cout << "Message: " <<message<<std::endl;
  }
 
  return nEntries;
}

void TQMultiChannelAnalysisSampleVisitor::useObservableSets(bool useSets){
  // decide whether this visitor should use separate observable sets
  this->fUseObservableSets = useSets;
}

void TQMultiChannelAnalysisSampleVisitor::updateFriends(TQSampleFolder* sf) {
  if (!sf) return;
  if (sf->countFriends() > 0) return;
  sf->findFriends();
  return;
}


