#include "QFramework/TQStringUtils.h"
#include "QFramework/TQSample.h"
#include "QFramework/TQUtils.h"
#include "QFramework/TQIterator.h"
#include "TFile.h"

#include "definitions.h"

// #define _DEBUG_
#include "QFramework/TQLibrary.h"

#ifdef ROOTCORE
#include <RootCore/Packages.h>
#endif

#ifdef ROOTCORE_PACKAGE_xAODRootAccess
#define XAOD_STANDALONE 1
#include "TChain.h"
#include "xAODRootAccessInterfaces/TActiveEvent.h"
#include "xAODRootAccess/TEvent.h"
#include "xAODRootAccessInterfaces/TVirtualEvent.h"
#include "xAODRootAccess/MakeTransientTree.h"
bool TQSample::gUseTransientTree(true);
#else 
bool TQSample::gUseTransientTree(false);
#endif
bool TQSample::gUseAthenaAccessMode(false);

#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdlib>

////////////////////////////////////////////////////////////////////////////////////////////////
//
// TQSample:
//
// The TQSample class is a representation of a sample. It is a subclass of a TQSampleFolder and
// can therefore additionally serve as container for objects (e.g. analysis results) associated
// with it. An instance of this class can point to a TTree object which will be used as the
// source of event data. This tree may either be contained in this sample itself, or it may be
// contained in an external ROOT file. Either way, you tell the sample where to find the tree
// by passing an argument to setTreeLocation(...).
//
// As the tree will probably be used by several classes, accessing the tree is managed by the
// sample. To get a pointer to the tree, use getTreeToken(...)
//
////////////////////////////////////////////////////////////////////////////////////////////////

ClassImp(TQSample)


//__________________________________________________________________________________|___________

TList * TQSample::splitTreeLocations(TString treeLocations) {

  /* the list of tree locations to return */
  TList * list = 0;

  while (!treeLocations.IsNull()) {
    /* read one tree location */
    TString location;
    TQStringUtils::readUpTo(treeLocations, location, "<");
    TQStringUtils::readBlanks(treeLocations);

    if (TQStringUtils::removeLeading(treeLocations, "<", 2) != 1) {
      /* create the list */
      if (!list) {
        list = new TList();
        list->SetOwner(true);
      }
      /* add the new item to the list */
      list->Add(new TObjString(TQStringUtils::trim(location).Data()));
    } else {
      /* found a single "<" character: stop */
      if (list)
        delete list;
      return 0;
    }
  }

  /* return the list */
  return list;
}


//__________________________________________________________________________________|___________

TString TQSample::extractFilename(const TString& treeLocation) {

  // find last occurence of ":" ...
  int pos = treeLocation.Last(':');
  if (pos != kNPOS) {
    // ... return string before it
    return treeLocation(0, pos);
  } else {
    // ... or an empty string in case no ':' has been found
    return TString("");
  }
}


//__________________________________________________________________________________|___________

TString TQSample::extractTreename(const TString& treeLocation) {

  // find last occurence of ":" ...
  int pos = treeLocation.Last(':');
  if (pos != kNPOS) {
    // ... return string after it
    return treeLocation(pos + 1, treeLocation.Length());
  } else {
    // ... or the full string in case no ':' has been found
    return treeLocation;
  }
}


//__________________________________________________________________________________|___________

TQSample::TQSample() : TQSampleFolder() {
  // Default constructor of TQSample class: a new instance of TQSample is created
  // and initialized. It will be emtpy and have no base folder and its name will
  // be set to "unkown"
}


//__________________________________________________________________________________|___________

TQSample::TQSample(const TString& name) : TQSampleFolder(name) {
  // Constructor of TQSample class: a new instance of TQSample is created and
  // initialized. It will be emtpy and have no base folder and its name will
  // be set to the value of the parameter "name_" if it is a valid name and
  // "unknown" otherwise
}

//__________________________________________________________________________________|___________

void TQSample::setNormalisation(double normalisation_) {
  // Set the normalisation factor of this sample. This factor will be used by any
  // analysis to scale its results

  fNormalisation = normalisation_;
}


//__________________________________________________________________________________|___________

double TQSample::getNormalisation() {
  // Get the normalisation factor of this sample (= 1. by default)
 
  return fNormalisation;
}

//__________________________________________________________________________________|___________

void TQSample::setTree(TFile* f, const TString& treename){
  // set the TTree of this sample from the fiel and the treename given
  DEBUGclass("checking status of file");
  if(!f || f->IsZombie() || !f->IsOpen()){
    INFO("Encountered possible zombie file in sample '%s'!",this->getPath().Data());
    this->fTree = NULL;
    return;
  }
  DEBUGclass("attempting to retrieve tree");
  TTree* tree = dynamic_cast<TTree*>(fFile->Get(treename.Data()));
  if(!tree){
    this->fTree = NULL;
    return;
  }
  DEBUGclass("function called on file '%s' for tree '%s'",f->GetName(),treename.Data());
  // this function is used to encapsule TTree post-processing
#ifdef ROOTCORE_PACKAGE_xAODRootAccess
#warning "using ASG_RELEASE compilation scheme"
  // if we are inside an ASG release, this tree might require special treatment
  // we check this by looking if any branch type includes the string "xAOD"
  TQIterator itr(tree->GetListOfBranches());
  bool isxAOD = false;
  while(itr.hasNext()){
    TBranch* obj = (TBranch*)(itr.readNext());
    if(TQStringUtils::find(obj->GetClassName(),"xAOD") != kNPOS){
      isxAOD = true;
      break;
    }
  }
  // if we found anything, we use the xAOD::MakeTransientTree method
  if(isxAOD){
    DEBUGclass("identified tree of sample '%s' as xAOD, making transient",this->getPath().Data());
    DEBUGclass("creating new xAOD::TEvent");
    xAOD::TVirtualEvent* evt = xAOD::TActiveEvent::event();
    if(evt){
      DEBUGclass("re-using existing instance of xAOD::TEvent");
      this->fEvent = dynamic_cast<xAOD::TEvent*>(evt);
      if(!this->fEvent) throw std::runtime_error("active instance of TVirtualEvent is not of type TEvent!");
    } else {
      DEBUGclass("creating new instance of xAOD::TEvent");
      this->fEvent = new xAOD::TEvent(gUseAthenaAccessMode? xAOD::TEvent::kAthenaAccess : xAOD::TEvent::kClassAccess);
      this->fEvent->setActive();
    }
    bool ok = (this->fEvent->readFrom( this->fFile, kTRUE, treename ).isSuccess());
    DEBUGclass("calling xAOD::MakeTransientTree on event %x with treename '%s'",this->fEvent,treename.Data());
    if(ok){
      if(TQSample::gUseTransientTree){
        int oldErrorIgnoreLevel = gErrorIgnoreLevel;
        gErrorIgnoreLevel = 2000;
        this->fTree = xAOD::MakeTransientTree( *this->fEvent, treename );
        gErrorIgnoreLevel = oldErrorIgnoreLevel;
        DEBUGclass("retrieved transient tree %x",this->fTree);
        this->fTreeIsTransient = true;
      } else {
        this->fTree = tree;
        this->fTreeIsTransient = false;
      }
    } else {
      //      delete this->fEvent;
      this->fEvent = 0;
    }
    return;
  }
#else
#warning "using plain ROOT compilation scheme"
#endif
  // if control reaches this point, we are either in plain ROOT or at least
  // have a plain ROOT compatible tree. there's not much to do in that case...
  DEBUGclass("identified tree of sample '%s' as plain ROOT, setting data member",this->getPath().Data());
  this->fTree = tree;
  fTreeIsTransient = false;
}

//__________________________________________________________________________________|___________

void TQSample::clearTree(){
  // clear the tree belonging to this sample
  // is called upon returning all tree tokens
  if(fFile){
    this->retractTreeFromFriends();
    if (fFile->IsOpen()){
      DEBUGclass("closing file");
      fFile->Close();
    }
    DEBUGclass("deleting file pointer");
    delete fFile;
  }
#ifdef ROOTCORE_PACKAGE_xAODRootAccess
  fEvent = NULL;
#endif
  fFile = NULL;
  fTree = NULL;
}

//__________________________________________________________________________________|___________

bool TQSample::getTree(){
  // retrieves the tree from the file to store it in the sample
    DEBUGclass("trying to retrieve tree");
    /* get and split the tree locations */
    TList * treeLocations = splitTreeLocations(getTreeLocation());

    if (!treeLocations) return false;

    /* the default file- and treename */
    TString defFilename;
    TString defTreename;
 
    /* iterate over tree locations */
    int nTreeLocations = treeLocations->GetEntries();
    for (int i = 0; i < nTreeLocations && (fTree || i == 0); i++) {
 
      /* the tree location */
      TString location = treeLocations->At(i)->GetName();
      /* extract file- and treename */
      TString filename = extractFilename(location);
      TString treename = extractTreename(location);
 
      /* set default file- and treenames */
      if (filename.IsNull() && !defFilename.IsNull())
        filename = defFilename;
      else if (!filename.IsNull() && defFilename.IsNull())
        defFilename = filename;
      if (treename.IsNull() && !defTreename.IsNull())
        treename = defTreename;
      else if (!treename.IsNull() && defTreename.IsNull())
        defTreename = treename;
 
 
      if (!filename.IsNull()) {
        /* check whether file exists */
        if (TQUtils::fileExists(filename)) {
          if (i == 0) {
            /* open the file and the tree */
            fFile = TFile::Open(filename.Data());
	    DEBUGclass("setting tree");
            this->setTree(fFile, treename);
            DEBUGclass("got tree %x",this->fTree);
            if (!fTree) {
              if(fFile && fFile->IsOpen()){
                fFile->Close();
                delete fFile;
              }
              fFile = 0;
              return false;
            }
          } else {
            fTree->AddFriend(treename.Data(), filename.Data());
          }
        } 
      } else {
        /* look for the tree inside this folder */
        TTree* tree = dynamic_cast<TTree*>(getObject(treename.Data()));
        if (tree){
          /* we found the tree: keep the pointer */
          this->fTree = tree;
          this->fTreeIsTransient = false;
        } else {
          /* we couldn't find the tree */
          fTree = NULL;
          return false;
        }
      }
    }
 
    delete treeLocations;
    DEBUGclass("successfully completed function");
    return true;
}

//__________________________________________________________________________________|___________

void TQSample::promoteTreeToFriends(){
  // promote the tree pointer to some friends
  TQSampleFolderIterator itr(this->fFriends);
  while(itr.hasNext()){
    TQSample* myFriend = dynamic_cast<TQSample*>(itr.readNext());
    if (!myFriend) throw std::runtime_error("Attempt to promote tree to friend which is not of type TQSample. A SampleFolder within a Sample should never happen, check your analysis setup!");
    DEBUG("promoting tree from sample '%s' to sample '%s'",this->getPath().Data(),myFriend->getPath().Data());
    if(!myFriend->getFile()){
      myFriend->fTree = this->fTree;
#ifdef ROOTCORE_PACKAGE_xAODRootAccess
      myFriend->fEvent = this->fEvent;
#endif
    }
  }
}

void TQSample::retractTreeFromFriends(){
  // retract a tree (and TEvent) pointer from friends
  TQSampleFolderIterator itr(this->fFriends);
  while(itr.hasNext()){
    TQSample* myFriend = dynamic_cast<TQSample*>(itr.readNext());
    if (!myFriend) throw std::runtime_error("Attempt to promote tree to friend which is not of type TQSample. A SampleFolder within a Sample should never happen, check your analysis setup!");
    if (myFriend==this) continue; //don't remove the pointer of this instance (yet)
    DEBUG("retracting tree of sample '%s' from sample '%s'",this->getPath().Data(),myFriend->getPath().Data());
    if(!myFriend->getFile()){
      myFriend->fTree = NULL;
#ifdef ROOTCORE_PACKAGE_xAODRootAccess
      myFriend->fEvent = NULL;
#endif
    }
  }
  
  
  
}

//__________________________________________________________________________________|___________

TQToken * TQSample::getTreeToken() {
  // Request a tree token (TQToken). Return a tree token containing a pointer to
  // the TTree object if the tree is accessible, return a null pointer otherwise.
  // Use returnTreeToken(...) to return the tree token after you don't need the
  // tree anymore. The first request of a tree token triggers reading of the tree
  // (opening the ROOT file, in case it is contained in an external file). After
  // every tree token was returned, the tree is released (closing the ROOT file,
  // in case it is contained in an external file).
  //
  // Your code using the tree might look like this:
  //
  // /* try to get a tree token */
  // treeToken = sample->getTreeToken();
  //
  // if (treeToken) {
  // /* get the pointer to the tree */
  // TTree* tree = (TTree*)treeToken->getContent();
  // /* optional: the sample should know who is using the token */
  // treeToken->setOwner(this);
  //
  // /* <use your tree here> */
  //
  // /* return the tree token */
  // sample->returnTreeToken(treeToken);
  // } else {
  // /* for any reason we didn't get a tree
  // * token, so we cannot use the tree */
  // }
  DEBUGclass("entering function");
  if (!fTree) {
    if(this->getTree()){
      this->promoteTreeToFriends();
    }
  }

  if (fTree) {
    DEBUGclass("creating tree token");
    /* create a new tree token */
    TQToken * treeToken = new TQToken();
    treeToken->setContent(fTree);
    /* create the list of tree tokens */
    if (!fTokens) {
      fTokens = new TList();
      fTokens->SetOwner(true);
    }
    /* keep it in our list */
    fTokens->Add(treeToken);
    /* dispense the token */
    DEBUGclass("returning tree token");
    return treeToken;
  } 

  /* no tree and thus no tree token to dispense */
  return NULL;
}

//__________________________________________________________________________________|___________

TQToken * TQSample::getFileToken() {
  // Request a file token (TQToken). Return a file token containing a pointer to
  // the TFile object if it is accessible, return a null pointer otherwise.
  // Use returnToken(...) to return the token after you don't need the file anymore.
  DEBUGclass("entering function");
  if (!fFile) {
    if(this->getTree()){
      this->promoteTreeToFriends();
    }
  }
  if (fFile) {
    DEBUGclass("returning file");
    TQToken * fileToken = new TQToken();
    fileToken->setContent(fFile);
    if (!fTokens) {
      fTokens = new TList();
      fTokens->SetOwner(true);
    }
    fTokens->Add(fileToken);
    return fileToken;
  } 
  return NULL;
}


//__________________________________________________________________________________|___________

TQToken * TQSample::getEventToken() {
  // Request an event token (TQToken). Return a tree token containing a pointer to
  // the TEvent object if the tree is accessible, return a null pointer otherwise.
  // Use returnToken(...) to return the token after you don't need the
  // TEvent anymore. The first request of a tree token triggers reading of the tree
  // (opening the ROOT file, in case it is contained in an external file). After
  // every tree token was returned, the tree is released (closing the ROOT file,
  // in case it is contained in an external file).
#ifdef ROOTCORE_PACKAGE_xAODRootAccess
  DEBUGclass("entering function");
  if (!fEvent) {
    if(this->getTree()){
      this->promoteTreeToFriends();
    }
  }

  if (fEvent) {
    DEBUGclass("returning TEvent");
    /* create a new token */
    TQToken * eventToken = new TQToken();
    eventToken->setContent(fEvent);
    /* create the list of event tokens */
    if (!fTokens) {
      fTokens = new TList();
      fTokens->SetOwner(true);
    }
    /* keep it in our list */
    fTokens->Add(eventToken);
    /* dispense the token */
    return eventToken;
  } 

  /* no tree and thus no event token to dispense */
  return NULL;
  #else
  ERRORclass("event token was requested, but TEvent class unavailable in standalone release!");
  return NULL;
  #endif
}

//__________________________________________________________________________________|___________

bool TQSample::returnTreeToken(TQToken * &token_) {
  // Return and delete a tree token that is not needed anymore (for
  // details check documentation of getTreeToken()).
  return this->returnToken(token_);
}

//__________________________________________________________________________________|___________

bool TQSample::returnToken(TQToken * &token_) {
  // Return and delete a tree or event token that is not needed
  // anymore
  if (fTokens && fTokens->FindObject(token_)) {

    /* remove and delete this token
     * and reset the pointer to it */
    fTokens->Remove(token_);
    token_ = 0;

    /* close file if all tokens have been returned */
    if (getNTreeTokens() == 0) {
      DEBUGclass("returning tree token - no tokens left, clearing tree");
      this->clearTree();
    } else {
      DEBUGclass("returning tree token - there are still other active tokens");
    }

    /* successfully returned tree token */
    return true;
  } else {
    /* we don't know this token, so ignore it */
    return false;
  }
}


//__________________________________________________________________________________|___________

bool TQSample::setTreeLocation(TString treeLocation_) {
  // Set the tree location. Setting a new tree location will fail, if there are
  // still unreturned tree tokens. In that case, false is returned, and true other-
  // wise.
  //
  // A tree may either be contained in this sample it self, or it may be contained
  // in an external ROOT file:
  //
  // - if the tree is contained in the sample use
  //
  // setTreeLocation("<name_of_the_tree>");
  //
  // - if the tree is contained in an external ROOT file, use:
  //
  // setTreeLocation("<name_of_the_file>:<name_of_the_tree>");
  //
  // Please note: the name of the tree is NOT the name of the pointer, but it is
  // the name of the object returned by tree->GetName() (or in case of an external
  // file, the name of the key).

  if (getNTreeTokens() == 0) {
    /* set the new tree location */
    fTreeLocation = treeLocation_;
    /* we successfully set the tree location */
    return true;
  } else {
    /* there are still tree tokens unreturned */
    return false;
  }

}


//__________________________________________________________________________________|___________

int TQSample::getNTreeTokens() {
  // Return the number of unreturned tree tokens

  if (fTokens)
    return fTokens->GetEntries();
  else
    return 0;
}


//__________________________________________________________________________________|___________

void TQSample::printTreeTokens() {
  // Print a summary of tree tokens

  TQIterator itr(fTokens);
  while (itr.hasNext()) {
    TQToken * token = dynamic_cast<TQToken*>(itr.readNext());
    if (token) {
      token->print();
    }
  }
}


//__________________________________________________________________________________|___________

bool TQSample::checkTreeAccessibility() {
  // Return true if the TTree is accessible and false otherwise

  bool isAccessible = false;

  /* try to access the tree */
  TQToken * token = getTreeToken();
 
  if (token) {
    isAccessible = true;
    returnTreeToken(token);
  }

  return isAccessible;
}


//__________________________________________________________________________________|___________

TString TQSample::getTreeLocation() {
  // Return the tree location
  if(fTreeLocation.IsNull()){
    TString filepath,treename;
    if(this->getTagString(".xsp.filepath",filepath) && this->getTagString(".xsp.treename",treename)){
      this->fTreeLocation = filepath + ":" + treename;
    }
  }
  return fTreeLocation;
}


//__________________________________________________________________________________|___________

TString TQSample::getFilename() {
  // retrieve the filename associated with this sample
  TList * treeLocations = splitTreeLocations(fTreeLocation);
  TString treeLocation;
  if (treeLocations) {
    if (treeLocations->GetEntries() > 0)
      treeLocation = TString(treeLocations->First()->GetName());
    delete treeLocations;
  }
 
  return extractFilename(treeLocation);
}


//__________________________________________________________________________________|___________

TString TQSample::getTreename() {
  // retrieve the treename associated with this sample
  TList * treeLocations = splitTreeLocations(fTreeLocation);
  TString treeLocation;
  if (treeLocations) {
    if (treeLocations->GetEntries() > 0)
      treeLocation = TString(treeLocations->First()->GetName());
    delete treeLocations;
  }

  return extractTreename(treeLocation);
}


//__________________________________________________________________________________|___________

bool TQSample::addSubSample(TQSample * subSample) {
  // add a subsample to this sample
  // can only be done if this sample is not already a subsample
  if(this->isSubSample()){
    return false;
  }
  if (!subSample) {
    return false;
  }
  if(!this->addFolder(subSample)) {
    return false;
  }
  return true;
}


//__________________________________________________________________________________|___________

TQSample* TQSample::addSelfAsSubSample(const TString& name) {
  // add a copy of yourself as a subsample

  if(name.IsNull()) return NULL;
  TQSample* subSample = new TQSample(name);
  if(!this->addSubSample(subSample)){
    delete subSample;
    return NULL;
  }
 
  subSample->importTags(this);
  return subSample;
}



//__________________________________________________________________________________|___________

TQSample * TQSample::getSubSample(const TString& path) {
  // get the subsample matching a given path

  TQSampleFolder * sampleFolder = getSampleFolder(path);
  if (sampleFolder && sampleFolder->InheritsFrom(TQSample::Class()))
    return dynamic_cast<TQSample*>(sampleFolder);
  else 
    return 0;
}

//__________________________________________________________________________________|___________

bool TQSample::isSubSample(){
  // return true if this sample is the direct child of some other sample
  // false otherwise
  return this->getBaseSample();
}

//__________________________________________________________________________________|___________

TQSample * TQSample::getBaseSample() {
  // retrieve the base sample of this sample
  return dynamic_cast<TQSample*>(this->getBase());
}

//__________________________________________________________________________________|___________

TFile* TQSample::getFile(){
  // retrieve the file pointer of this sample
  return this->fFile;
}

//__________________________________________________________________________________|___________

TQSample::~TQSample() {
  // default destructor
  if (fTokens)
    delete fTokens;
}

//__________________________________________________________________________________|___________


bool TQSample::hasSubSamples() {
  // determine if this sample has any sub-samples
  bool retval = false;
  /* try to find a sub sample */
  TCollection * subSamples = this->getListOfSamples("?");
  DEBUGclass("subsamples: %x (len=%d)",subSamples,(subSamples ? subSamples->GetEntries() : 0));
  #ifdef _DEBUG_
  this->print("d");
  #endif
  if(subSamples){
    if(subSamples->GetEntries() > 0){
      retval = true;
    }
    delete subSamples;
  }
  return retval;
}

//__________________________________________________________________________________|___________

bool TQSample::mergeSubSamples(TQSample* other, bool sumElements, bool verbose){
  // merge all sub samples from the other sample into this one
  // NOT YET IMPLEMENTED
//  if(!other->hasSubSamples()){
//    return true;
//  }
//  TQSampleIterator itr(this->getListOfSamples("?"));
//  while(itr.hasNext()){
//    TQSample* otherSubSample = itr.readNext();
//    TObject* thisObj = this->getObject(otherSubSample->GetName());
//  }
  return false;
}

//__________________________________________________________________________________|___________


void TQSample::findFriendsInternal(TQSampleFolder* otherSF, bool forceUpdateSubsamples){
  // find the friends of this sample folder in some other sample folder
  TQSampleIterator itr(otherSF->getListOfSampleFolders("*",TQSample::Class()));
  const TString myTreeLocation = this->getTreeLocation();
  if (myTreeLocation.Length()>0) { //we don't want to consider super-samples which don't have a tree themselves (this would cause a lot of mis-matching!)
    while(itr.hasNext()){
      TQSample* s = itr.readNext();
      TString treelocation = s->getTreeLocation();
      DEBUGclass("looking at friend candidate '%s'",s->getPath().Data());
      if(TQStringUtils::equal(treelocation,myTreeLocation) ){
        DEBUGclass("found friend '%s'",s->getPath().Data());
        this->befriend(s);
      }
    }
  }
  //now check if all sub-samples are friends and if so, befriend with their sup-folder
  if (this->hasSubSamples()) {
    itr.reset();
    TQSampleIterator subitr(this->getListOfSamples("?"));
    while(itr.hasNext()){
      TQSample* other = itr.readNext();
      //we apply the requirements: existence (not a null pointer), has sub samples, same number of (sub) samples as this sample, 
      if (!other || !other->hasSubSamples() || this->getNSamples() != other->getNSamples() ) continue;
      bool befriend = true;
      while(subitr.hasNext() && befriend) {
        TQSample* thisSub = subitr.readNext();
        if (!thisSub) continue;
        TQSample* otherSub = other->getSubSample(thisSub->GetName());
        if (!otherSub) {befriend = false; break;}
        if (forceUpdateSubsamples || (thisSub->countFriends() == 0) ) thisSub->findFriends(otherSF, forceUpdateSubsamples);
        if (forceUpdateSubsamples || (otherSub->countFriends() == 0) ) otherSub->findFriends(otherSF, forceUpdateSubsamples);
        
        befriend = befriend && thisSub->isFriend(otherSub);
      }
      subitr.reset();
      if (befriend) {
        this->befriend(other);
      }
    }
  }
}




