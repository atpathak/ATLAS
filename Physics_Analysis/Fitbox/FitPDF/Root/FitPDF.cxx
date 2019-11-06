#include "FitPDF/FitPDF.h"

///<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
RooFitResult* FitPDF::FitResult( RooStats::ModelConfig* model, 
				 RooAbsPdf* fitpdf, 
				 RooAbsData* data,
                                 int &MinuitStatus, 
				 int &ErrorStatus, 
				 double &Edm)
{

  //constrain the params
  RooArgSet* constrainedParams = fitpdf->getParameters(*data);
  RooStats::RemoveConstantParameters(constrainedParams);
  RooFit::Constrain(*constrainedParams);

  //global observables
  const RooArgSet* glbObs = model->GetGlobalObservables();

  RooRealVar *poi = (RooRealVar*) model->GetParametersOfInterest()->first();
  Info("FitPDF::FitResult",  "POI const = %i value = %f", poi->isConstant(), poi->getVal() );

  //NLL
  RooAbsReal * nll = m_nCPUs ?
    fitpdf->createNLL(*data, RooFit::Constrain(*constrainedParams), RooFit::GlobalObservables(*glbObs), RooFit::Offset(1), RooFit::NumCPU(m_nCPUs) ) :
    fitpdf->createNLL(*data, RooFit::Constrain(*constrainedParams), RooFit::GlobalObservables(*glbObs), RooFit::Offset(1));

  //allow offset?
  //nll->enableOffsetting(true);
  
  //print pdf params
  if(m_print_fit_pdf_init_params){
    Info("FitPDF::FitResult", "Initial pdf parameters:");
    constrainedParams->Print("v");
  }

  //print pdf model
  if(m_print_fit_pdf_model){
    Info("FitPDF::FitResult", "Initial model parameters:");
    model->Print("");
  }

  return this->FitResult(nll, MinuitStatus, ErrorStatus, Edm);

}

///<><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><>
RooFitResult* FitPDF::FitResult( RooAbsReal* nll, 
				 int &MinuitStatus, 
				 int &ErrorStatus, 
				 double &Edm)
{

  Info("FitPDF::FitResult", "Starting fit...");
  TStopwatch sw; sw.Start();
  Info("FitPDF::FitResult", "Initial NLL: %f", nll->getVal() );

  //verbosity
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Eval);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Generation);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Minimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Plotting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Fitting);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Integration);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::LinkStateMgmt);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Caching);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Optimization);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::ObjectHandling);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::InputArguments);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Tracing);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::Contents);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::DataHandling);
  RooMsgService::instance().getStream(1).removeTopic(RooFit::NumIntegration);

  //minimizer options
  ROOT::Math::MinimizerOptions::SetDefaultMinimizer(m_minimType);
  //ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(m_RooMinimizer_print_lvl);

  //Strategy
  unsigned int default_minim_startegy = ROOT::Math::MinimizerOptions::DefaultStrategy(); // default = 1

  if(m_strategies.at(0) != default_minim_startegy)
    Info("FitPDF::FitResult", "Fit strategy chosen before entering minimization loop %u is not the default one...", m_strategies.at(0));

  //messages printing
  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  if (printLevel < 0) 
    RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  //RooMinimizer
  RooMinimizer minim(*nll);
  //minim.setPrintLevel(m_RooMinimizer_print_lvl);
  minim.setStrategy(m_strategies.at(0));
  minim.setEps(1);

  //Minimization results
  int status		= -99;
  ErrorStatus		= -99;
  Edm			= -99;
  RooFitResult  *rfr	= NULL;
  unsigned  int nrItr	= 0;
  bool FitIsNotGood	= true;

  //fit trials
  while (nrItr < m_maxRetries && FitIsNotGood) {

   Info("FitPDF::FitResult", "Fit try n° %u out of %u.", nrItr+1, m_maxRetries);

    //fit strategies
    for(auto istrategy : m_strategies){

      //update strategy and minimize
      Info("FitPDF::FitResult", "Fitting with strategy %u", istrategy);
      ROOT::Math::MinimizerOptions::SetDefaultStrategy(istrategy);
      
      status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), // if is "" (no default is set) read from /etc/system.rootrc
			      ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str() //"Migrad", call this to minimize the likelihood
			      );
      rfr = minim.save();
      Edm = rfr->edm();

      //check status with or without error calculation
      if(m_minim_errorcalc_coupled){
	ErrorStatus = m_useMinos ? minim.minos() : minim.hesse(); //error calculation method
	FitIsNotGood = ( (status!=0 && status!=1) || Edm>1.0 || (ErrorStatus!=0 && ErrorStatus!=1) );
      }else{
	FitIsNotGood = ((status!=0 && status!=1) || Edm>1.0);
      }

      //dump some useful info
      if(FitIsNotGood){
	Warning("FitPDF::FitResult", "Fit try with strategy %u, MINUIT status %d, EDM %f, MINOS %s failed!", istrategy, status, Edm, (m_useMinos?"yes":"no") );
      }else{
	Info("FitPDF::FitResult", "Fit try with strategy %u, MINUIT status %d, EDM %f, MINOS %s succeeded!", istrategy, status, Edm, (m_useMinos?"yes":"no") );
	break;
      }
    }//loop over strategies

    //update trial if fit is still not good
    if(FitIsNotGood) nrItr++;

  }//

  //check if fit is good
  if (FitIsNotGood ){
    Error("FitPDF::FitResult", "Fit failure unresolved with maximum tries %u. Please investigate your workspace!", m_maxRetries);
  }

  //run error finding if decoupled from minimization
  if(!m_minim_errorcalc_coupled){
    Info("FitPDF::FitResult", "Trying error calculation with MINOS %s or HESSE %s, decoupled from minimization", (m_useMinos?"on":"off"), (m_useMinos?"off":"on"));
    ErrorStatus = m_useMinos ? minim.minos() : minim.hesse(); //error calculation method
    if( (ErrorStatus!=0 && ErrorStatus!=1) )
      Info("FitPDF::FitResult", "Error calculation failure with status %d!", ErrorStatus);
  }
  
  //inform minut status for user
  MinuitStatus = status;

  //extract and dump fit info
  bool fit_status               = minim.fitter()->Result().Status();
  bool valid_fit                = minim.fitter()->Result().IsValid();
  bool empty_fit                = minim.fitter()->Result().IsEmpty();
  std::string minimizer_type    = minim.fitter()->Result().MinimizerType();
  unsigned int n_tot_params     = minim.fitter()->Result().NTotalParameters();
  int cov_matrix_status         = minim.fitter()->Result().CovMatrixStatus();

  std::cout << std::endl;
  std::cout << "***********************************************************" << std::endl;
  std::cout << " FIT RESULTS " << std::endl;
  std::cout << std::endl;
  std::cout << "\tN parameters      " << n_tot_params << std::endl;
  std::cout << "\tminimizer type    " << minimizer_type << std::endl;
  std::cout << "\tfit_trials        " << nrItr + 1 << std::endl;
  std::cout << "\tempty fit         " << empty_fit << std::endl;
  std::cout << "\tvalid fit         " << valid_fit << std::endl;
  std::cout << "\tfit status        " << fit_status << std::endl;
  std::cout << "\tcov matrix status " << cov_matrix_status << std::endl;
  std::cout << std::endl;
  std::cout << "\tminuit status     " << status << std::endl;
  std::cout << "\tMinos is used     " << (m_useMinos?"yes":"no")<<std::endl;
  std::cout << "\terror status      " << ErrorStatus << std::endl;
  std::cout << "\tEdm               " << Edm << std::endl;
  std::cout << "\tTotal Time        "; sw.Print();
  std::cout << "***********************************************************" << std::endl;
  std::cout << std::endl;

  return rfr;
}

/*
  Ref:
  1.  RooMinimizer::minimize is using: _theFitter->Result().Status() https://root.cern.ch/doc/v608/RooMinimizer_8cxx_source.html#l00313
  2.  Fit::Fitter https://root.cern.ch/doc/v608/classROOT_1_1Fit_1_1Fitter.html
  3.  Fit::Fitter::Result() of type FitResult https://root.cern.ch/doc/v608/classROOT_1_1Fit_1_1Fitter.html#a0ecc4e8ab99843983080c3e707c9b375
  4.  FitResult https://root.cern.ch/doc/v608/classROOT_1_1Fit_1_1FitResult.html
*/
