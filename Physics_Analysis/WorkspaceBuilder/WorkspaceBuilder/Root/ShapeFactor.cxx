#include <WorkspaceBuilder/ShapeFactor.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------

  RooStats::HistFactory::ShapeFactor ShapeFactor::getHistFactoryShapeFactor(){
    RooStats::HistFactory::ShapeFactor shapefactor;
    shapefactor.SetName(m_name);
    //shapefactor.SetConst(m_const); //deprecated
    return shapefactor;
  }

  //-------------------------------------------------------------------------------

  double ShapeFactor::getGammaScaleFactor(std::map<std::string,double> parameters, int ibin){
    if(ibin<=0){
      std::cout << "ERROR: ShapeFactor::getGammaScaleFactor called with ibin<=0, but only bins in visible range have gamma factors, ibin="<< ibin << std::endl;
      return 1.;
    }

    double sf=1.;

    TString parname="gamma_"+m_name+"_bin_";
    parname+=(ibin-1);
    std::map<std::string,double>::iterator it=parameters.find(parname.Data());
    if(it!=parameters.end())
      return it->second;
    
    return sf;
  }

}
