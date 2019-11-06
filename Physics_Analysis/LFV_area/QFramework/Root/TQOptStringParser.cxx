#include "QFramework/TQOptStringParser.h"
#include "QFramework/TQStringUtils.h"
#include "QFramework/TQIterator.h"
#include "TList.h"
#include <iostream>
#include <cstdlib>

ClassImp(TQOptStringParser)

TQOptStringParser::TQOptStringParser() : 
fOpts(new TList())
{}

TQOptStringParser::~TQOptStringParser(){}

void TQOptStringParser::addFlag(const TString& opt, const TString& tagname, bool invert){
  TQNamedTaggable* tags = new TQNamedTaggable(opt);
  tags->setTagString("opt",opt);
  tags->setTagString("name",tagname);
  tags->setTagString("type","flag");
  tags->setTagBool("invert",invert);
  this->fOpts->Add(tags);
}
void TQOptStringParser::addFlag(const TString& opt, bool invert){
  this->addFlag(opt,opt,invert);
}

void TQOptStringParser::addIntegerOption(const TString& opt, const TString& tagname, int defaultValue){
  TQNamedTaggable* tags = new TQNamedTaggable(opt);
  tags->setTagString("opt",opt);
  tags->setTagString("name",tagname);
  tags->setTagString("type","integer");
  tags->setTagInteger("default",defaultValue);
  this->fOpts->Add(tags);
}
void TQOptStringParser::addIntegerOption(const TString& opt, int defaultValue){
  this->addIntegerOption(opt,opt,defaultValue);
}


void TQOptStringParser::addBlockOption(const TString& opt, const TString& tagname, const TString& defaultValue, const TString& blocks){
  TQNamedTaggable* tags = new TQNamedTaggable(opt);
  tags->setTagString("opt",opt);
  tags->setTagString("name",tagname);
  tags->setTagString("type","block");
  tags->setTagString("blocks",blocks);
  tags->setTagString("default",defaultValue);
  this->fOpts->Add(tags);
}
void TQOptStringParser::addBlockOption(const TString& opt){
  this->addBlockOption(opt,opt);
}

TQTaggable* TQOptStringParser::parse(TString& left) const {
  TQTaggable* result = new TQTaggable();
  TString opts(left);
  left = "";
  TQIterator itr(fOpts);
  while(opts.Length() > 0){
    itr.reset();
    bool found = false;
    while(!found && itr.hasNext()){
      TObject* obj = itr.readNext();
      TString type("");
      TString name(obj->GetName());
      if(!TQStringUtils::removeLeadingText(opts,name)){
        continue;
      }
      TQTaggable* tags = dynamic_cast<TQTaggable*>(obj);
      tags->getTagString("name",name);
      tags->getTagString("type",type);
      if(type == "flag"){
        result->setTagBool(name,!tags->getTagBoolDefault("invert",false));
        found = true;
      } else if(type == "integer"){
        TString num("");
        if(TQStringUtils::readToken(opts,num,TQStringUtils::getNumerals()) > 0){
          result->setTagInteger(name,atoi(num.Data()));
        } else {
          result->setTagInteger(name,tags->getTagIntegerDefault("default",0));
        }
        found = true;
      } else if(type == "block"){
        TString content("");
        if(TQStringUtils::readBlock(opts,content,tags->getTagStringDefault("block","[]")) > 0){
          result->setTagString(name,content);
        } else {
          result->setTagString(name,tags->getTagStringDefault("default",""));
        }
        found = true;
      }
    }
    if(!found){
      left.Append(opts(0,1));
      opts.Remove(0,1);
    }
  }
  return result;
}

TString TQOptStringParser::serialize(){
  TString retval("");
  TQIterator itr(fOpts);
  while(itr.hasNext()){
    TQTaggable* tags = dynamic_cast<TQTaggable*>(itr.readNext());
    if(!tags) continue;
    retval.Append(tags->exportTagsAsString());
    retval.Append("\n");
  }
  return retval;
}

void TQOptStringParser::print(){
  TQIterator itr(fOpts);
  while(itr.hasNext()){
    TQTaggable* tags = dynamic_cast<TQTaggable*>(itr.readNext());
    if(!tags) continue;
    tags->printTags();
  }
}

TQOptStringParser::TQOptStringParser(const TString& serialization) : 
  fOpts(new TList())
{
  TList* opts = TQStringUtils::tokenize(serialization,"\n");
  opts->SetOwner(true);
  TQIterator itr(opts,true);
  while(itr.hasNext()){
    TObject* opt = itr.readNext();
    TQNamedTaggable* tags = new TQNamedTaggable(opt->GetName());
    TString optName("");
    tags->getTagString("opt",optName);
    if(!optName.IsNull()){
      tags->SetName(optName);
      this->fOpts->Add(tags);
    } else {
      delete tags;
    }
  }
}
