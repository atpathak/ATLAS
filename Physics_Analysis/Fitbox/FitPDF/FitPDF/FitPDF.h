/*

  Fit function definition

*/
#ifndef FITPDF_H
#define FITPDF_H

#include "FitIncludes/FitIncludes.h"

#include <iostream>

//ROOT
#include "TROOT.h"
#include "TH2D.h"


/*
  namespace RooStats {
  class ModelConfig;
  }
  class RooFitResult;
  class RooAbsPdf;
  class RooAbsData;
*/

class FitPDF  {

 
 public:

  explicit FitPDF() : FitPDF(false, false, -1, "Minuit", 102, false, false, 3, 0) 
    {
    }

  explicit FitPDF(TString minimType,
		  unsigned strategy_order,
		  bool useMinos,
		  bool minim_errorcalc_coupled, 
		  unsigned int maxItrRetries, 
		  unsigned int nCPUs ) : FitPDF(false, false, -1, minimType, strategy_order, useMinos, minim_errorcalc_coupled, maxItrRetries, nCPUs)
  {
  }

  explicit FitPDF(bool show_pdf_model, 
                  bool show_pdf_par, 
                  int print_lvl,
		  TString minimType,
		  unsigned strategy_order,
		  bool useMinos,
		  bool minim_errorcalc_coupled, 
		  unsigned int maxItrRetries, 
		  unsigned int nCPUs 
		  ) :
  m_print_fit_pdf_model(show_pdf_model),
    m_print_fit_pdf_init_params(show_pdf_par),
    m_RooMinimizer_print_lvl(print_lvl),
    m_minimType(minimType),
    m_useMinos(useMinos),
    m_minim_errorcalc_coupled(minim_errorcalc_coupled),
    m_maxRetries(maxItrRetries),
    m_nCPUs (nCPUs)
      {
	this->set_strategy_priority(strategy_order);
      }


  ~FitPDF() = default;

  /**
   * @brief specialized constructor
   */
 public:
  RooFitResult* FitResult( RooStats::ModelConfig* model, 
                           RooAbsPdf* fitpdf, 
                           RooAbsData* fitdata,
                           int &MinuitStatus, 
                           int &HessStatus, 
                           double &Edm
			   );

  RooFitResult* FitResult( RooAbsReal* nll,
                           int &MinuitStatus, 
                           int &HessStatus, 
                           double &Edm
			   );


 private:
  bool				m_print_fit_pdf_model;
  bool				m_print_fit_pdf_init_params;
  int				m_RooMinimizer_print_lvl;
  TString			m_minimType;
  std::vector<unsigned int>	m_strategies;
  bool				m_useMinos;
  bool				m_minim_errorcalc_coupled;
  unsigned int			m_maxRetries;
  unsigned int			m_nCPUs;

  
  inline void set_strategy_priority(unsigned int strat)
  {    
    if(strat == 12)
      m_strategies = {0, 1, 2};
    else if(strat == 21)
      m_strategies = {0, 2, 1};
    else if(strat == 102)
      m_strategies = {1, 0, 2};
    else if(strat == 120)
      m_strategies = {1, 2, 0};
    else if(strat == 201)
      m_strategies = {2, 0, 1};
    else if(strat == 210)
      m_strategies = {2, 1, 0};
    else
      Warning("FitPDF::set_strategy_priority", "Strategy order %u is not acceptable. Will stick to the default order.", strat);
    std::ostringstream oss;
    std::copy(m_strategies.begin(), m_strategies.end(), 
	      std::ostream_iterator<unsigned int>(oss, " ") );
    
    Info("FitPDF::FitResult", "Fit strategy order: %s", oss.str().c_str());

  }
  
};

#endif
