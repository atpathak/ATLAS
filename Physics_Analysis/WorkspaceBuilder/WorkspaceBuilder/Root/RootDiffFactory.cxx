#include <WorkspaceBuilder/RootDiffFactory.h>
#include <iostream>
#include <set>
#include <vector>
#include <algorithm>
#include <TKey.h>
#include <TROOT.h>
#include <TClass.h>

namespace RootDiffNS{

  //-------------------------------------------------------------------------------
  void RootDiffFactory::printDiffForTwoDirectories(TDirectory* dir1, TDirectory* dir2, TString path){
    //std::cout << "dir1: " << dir1->GetName() << " dir2: " << dir2->GetName() <<  std::endl;

    //first get all keys
    std::set<std::string> s_allkeys;
    std::vector<std::string> v_allkeys;

    TIter next1(dir1->GetListOfKeys());
    TKey *key1;
    while ((key1 = (TKey*)next1())) s_allkeys.insert(key1->GetName());

    TIter next2(dir2->GetListOfKeys());
    TKey *key2;
    while ((key2 = (TKey*)next2())) s_allkeys.insert(key2->GetName());
    

    std::copy(s_allkeys.begin(), s_allkeys.end(), std::back_inserter(v_allkeys));
    std::sort(v_allkeys.begin(), v_allkeys.end());

    for(unsigned int i=0; i<v_allkeys.size(); i++){
      TKey *key1=dir1->FindKeyAny(v_allkeys.at(i).c_str());
      TKey *key2=dir2->FindKeyAny(v_allkeys.at(i).c_str());

      if(key1!=0 && key2!=0){
	if(gROOT->GetClass(key1->GetClassName())->InheritsFrom("TDirectory") && gROOT->GetClass(key2->GetClassName())->InheritsFrom("TDirectory")){
	  TObject* obj1=key1->ReadObj();
	  TObject* obj2=key2->ReadObj();
	  printDiffForTwoDirectories((TDirectory*)obj1, (TDirectory*)obj2, path+"/"+key1->GetName());
	}
	else if(gROOT->GetClass(key1->GetClassName())->InheritsFrom("TH1")){
	  TObject* obj1=key1->ReadObj();
	  TObject* obj2=key2->ReadObj();
	  printDiffForTwoHistograms((TH1*)obj1, (TH1*)obj2, path+"/"+key1->GetName());
	}
	else if(key1->GetClassName()!=key2->GetClassName()){
	  std::cout << path <<"/" << key1->GetName() << " has type " << key1->GetClassName() << " in " << m_rootfilename1 << " and type " << key2->GetClassName() << " in " << m_rootfilename2 << std::endl;
	}
	else{
	  std::cout << path <<"/" << key1->GetName() << " has unkown type " << key1->GetClassName() << ", will ignore it."<< std::endl;
	}


      }
      else if(key1!=0){
	std::cout << path <<"/" << key1->GetName() << " only found in " << m_rootfilename1 << std::endl;
      }
      else if(key2!=0){
	std::cout << path <<"/" << key2->GetName() << " only found in " << m_rootfilename2 << std::endl;
      }
    }
  }

  //-------------------------------------------------------------------------------
  void RootDiffFactory::printDiffForTwoHistograms(TH1* hist1, TH1* hist2, TString path){
    if(hist1->GetNbinsX()!=hist2->GetNbinsX()){
      std::cout << "Histogram " << path << " has different number of bins: " << hist1->GetNbinsX() << " != " << hist2->GetNbinsX() << std::endl;
      return;
    }

    std::vector<int> differingbinscontent;
    std::vector<int> differingbinserror;
    for(int i=0; i<hist1->GetNbinsX()+2; i++){
      double diff=hist1->GetBinContent(i)-hist2->GetBinContent(i);
      double average=0.5*(hist1->GetBinContent(i)+hist2->GetBinContent(i));
      if(diff/average>m_threshold){
	differingbinscontent.push_back(i);
	if(m_verbose) std::cout << "Histogram " << path << ": content differs in bin " << i << " value1: " << hist1->GetBinContent(i) << " value2: " << hist2->GetBinContent(i) << std::endl;
      }

      double differror=hist1->GetBinError(i)-hist2->GetBinError(i);
      double averageerror=0.5*(hist1->GetBinError(i)+hist2->GetBinError(i));
      if(differror/averageerror>m_threshold){
	differingbinserror.push_back(i);
	if(m_verbose) std::cout << "Histogram " << path << ": errors differ in bin " << i << " value1: " << hist1->GetBinError(i) << " value2: " << hist2->GetBinError(i) << std::endl;
      }
    }

    if(differingbinscontent.size()>0 || differingbinserror.size()>0){
      std::cout << "Histogram " << path << ": content differs in bins ";
      for(unsigned int i=0; i<differingbinscontent.size(); i++)
	std::cout << differingbinscontent[i]<< ", ";
      std::cout << " error differs in bins ";
      for(unsigned int i=0; i<differingbinserror.size(); i++)
	std::cout << differingbinserror[i]<< ", ";
      std::cout << std::endl;
    }
  }

}
