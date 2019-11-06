#include <WorkspaceBuilder/NormFactor.h>
//#include <RooStats/HistFactory/Systematics.h>

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------
  bool NormFactor::readNumbers(UserTools* usertools){
    m_value=usertools->getNormFactor( m_nameInFile,0);
    m_high=usertools->getNormFactor(m_nameInFile,1);
    m_low=usertools->getNormFactor(m_nameInFile,-1);

    if(m_high<m_low || m_value<m_low || m_value>m_high )
      std::cout << "ERROR: NormFactor::readNumbers(), the high="<<m_high << ", low="<<m_low << " and start="<<m_value<< " values for NormFactor "<< m_name << " are inconsistent." << std::endl;

    return true;
  }  

 //-------------------------------------------------------------------------------
  double NormFactor::getNormFactorValue(std::map<std::string,double> parameters){
    std::map<std::string,double>::iterator it=parameters.find(m_name);
    if(it!=parameters.end())
      return it->second;

    if(m_value==-999.){
      std::cout << "ERROR: getNormFactorValue failed for normfactor \"" << m_name << "\" as value was not correctly initialized. Call readNumbers first." << std::endl;
      throw NormFactorNotInitialized(m_name);
    }

    return m_value;
  }
 //-------------------------------------------------------------------------------

  RooStats::HistFactory::NormFactor NormFactor::getHistFactoryNormFactor(){
    if(m_value==-999. || m_low==-999. || m_high==-999.){
      std::cout << "ERROR: getHistFactoryNormFactor failed for normfactor \"" << m_name << "\" as value/low/high were not correctly initialized. Call readNumbers first." << std::endl;
      throw NormFactorNotInitialized(m_name);
    }
    RooStats::HistFactory::NormFactor normfactor;
    normfactor.SetName(m_name);
    normfactor.SetVal(m_value);
    //normfactor.SetConst(m_const); //deprecated
    normfactor.SetLow(m_low);
    normfactor.SetHigh(m_high);
    return normfactor;
  }

}
