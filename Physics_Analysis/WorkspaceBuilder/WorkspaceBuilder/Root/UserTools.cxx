#include <WorkspaceBuilder/UserTools.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  void UserTools::produceEquidistantHistogram(TH1F* input){
    bool isequidistant=input->GetXaxis()->GetXbins()->GetSize()<=0;

    //if(!isequidistant){
    std::vector<float> bincontent;
    std::vector<float> binerror;
    int entries=input->GetEntries();
    for(int i=1; i<input->GetNbinsX()+1; i++){
      bincontent.push_back(input->GetBinContent(i));
      binerror.push_back(input->GetBinError(i));
    }
    input->SetDirectory(0);
    if(input->GetNbinsX()+2!=input->GetSumw2N())
      input->Sumw2();
    if(isequidistant)
      input->SetBins(bincontent.size(),input->GetXaxis()->GetXmin(),input->GetXaxis()->GetXmax());
    else
      input->SetBins(bincontent.size(),-0.5,bincontent.size()-0.5);
    input->SetEntries(entries);
    for(unsigned int i=0; i<bincontent.size(); i++){
      input->SetBinContent(i+1,bincontent[i]);
      input->SetBinError(i+1,binerror[i]);
    }	  
  }
  //-------------------------------------------------------------------------------  
  bool UserTools::hasParameter(std::string para){
    if(m_parameters.find(para.c_str())!=m_parameters.end()) return true;
    return false;
  }

} // namespace
