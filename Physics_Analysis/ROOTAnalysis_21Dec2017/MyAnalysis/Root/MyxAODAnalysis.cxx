// Infrastructure include(s):
#include "xAODRootAccess/Init.h"
#include "xAODRootAccess/TEvent.h"
// ASG status code check
#include <AsgTools/MessageCheck.h>
#include <EventLoop/Job.h>
#include <EventLoop/StatusCode.h>
#include <EventLoop/Worker.h>
#include <MyAnalysis/MyxAODAnalysis.h>
// EDM includes:
#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauDefs.h" 
#include "xAODTau/TauJet.h"
#include "xAODTruth/TruthEventContainer.h"
#include "xAODTruth/TruthParticleContainer.h"
#include "TString.h"
#include "TH1.h"
#include "TH2.h"
#include <string>
#include <TFile.h>

//#include "xAODTruth/TruthTaus.h"

// this is needed to distribute the algorithm to the workers
ClassImp(MyxAODAnalysis)



MyxAODAnalysis :: MyxAODAnalysis ()
{
  // Here you put any code for the base initialization of variables,
  // e.g. initialize all pointers to 0.  Note that you should only put
  // the most basic initialization here, since this method will be
  // called on both the submission and the worker node.  Most of your
  // initialization code will go into histInitialize() and
  // initialize().
}



EL::StatusCode MyxAODAnalysis :: setupJob (EL::Job& job)
{
  // Here you put code that sets up the job on the submission object
  // so that it is ready to work with your algorithm, e.g. you can
  // request the D3PDReader service or add output files.  Any code you
  // put here could instead also go into the submission script.  The
  // sole advantage of putting it here is that it gets automatically
  // activated/deactivated when you add/remove the algorithm from your
  // job, which may or may not be of value to you.

 // let's initialize the algorithm to use the xAODRootAccess package
  job.useXAOD ();
  ANA_CHECK_SET_TYPE (EL::StatusCode); // set type of return code you are expecting (add to top of each function once)
  ANA_CHECK(xAOD::Init());


  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: histInitialize ()
{
  // Here you do everything that needs to be done at the very
  // beginning on each worker node, e.g. create histograms and output
  // trees.  This method gets called before any input files are
  // connected.

  
  // get the output file, create a new TTree and connect it to that output
  // define what braches will go in that tree
  TFile *outputFile = wk()->getOutputFile (outputName);
  tree = new TTree ("tree", "tree");
  tree->SetDirectory (outputFile);
  tree->Branch("n_truth_tau",     &n_truth_tau);
  tree->Branch("angle_vis_neu",   &angle_vis_neu);
  tree->Branch("angle_tau_vis",   &angle_tau_vis);
  tree->Branch("angle_tau_neu",   &angle_tau_neu);
  tree->Branch("tau_momentum",    &tau_momentum);
  tree->Branch("tau_pt",          &tau_pt);
  tree->Branch("tau_m",           &tau_m);
  tree->Branch("tau_eta",         &tau_eta);
  tree->Branch("tau_phi",         &tau_phi);
  tree->Branch("vis_pt",          &vis_pt);
  tree->Branch("vis_m",           &vis_m);
  tree->Branch("vis_eta",         &vis_eta);
  tree->Branch("vis_phi",         &vis_phi);
  tree->Branch("vis_momentum",    &vis_momentum);
  tree->Branch("neu_pt",          &neu_pt);
  tree->Branch("neu_m",           &neu_m);
  tree->Branch("neu_eta",         &neu_eta);
  tree->Branch("neu_phi",         &neu_phi);
  tree->Branch("neu_momentum",    &neu_momentum);
  tree->Branch("IsHadronicTau",   &IsHadronicTau);
  tree->Branch("numCharged",      &numCharged);
  tree->Branch("numChargedPion",  &numChargedPion);
  tree->Branch("numNeutral",      &numNeutral);
  tree->Branch("numNeutralPion",  &numNeutralPion);
  tree->Branch("DecayMode",       &DecayMode);
  
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: fileExecute ()
{
  // Here you do everything that needs to be done exactly once for every
  // single file, e.g. collect a list of all lumi-blocks processed
  xAOD::TEvent* m_event = wk()->xaodEvent(); // you should have already added this as described before

  // as a check, let's see the number of events in our xAOD
  Info("fileExecute()", "Number of events = %lli", m_event->getEntries() ); // print long long int
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: changeInput (bool firstFile)
{
  // Here you do everything you need to do when we change input files,
  // e.g. resetting branch addresses on trees.  If you are using
  // D3PDReader or a similar service this method is not needed.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: initialize ()
{
  ANA_CHECK_SET_TYPE (EL::StatusCode); // set type of return code you are expecting (add to top of each function once)
  // Here you do everything that you need to do after the first input
  // file has been connected and before the first event is processed,
  // e.g. create additional histograms based on which variables are
  // available in the input files.  You can also create all of your
  // histograms and trees in here, but be aware that this method
  // doesn't get called if no events are processed.  So any objects
  // you create here won't be available in the output if you have no
  // input events.

  
  // count number of events
  m_eventCounter = 0;
  m_numCleanEvents = 0;

  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: execute ()
{
  ANA_CHECK_SET_TYPE (EL::StatusCode); // set type of return code you are expecting (add to top of each function once)
  // Here you do everything that needs to be done on every single
  // events, e.g. read input variables, apply cuts, and fill
  // histograms and trees.  This is where most of your actual analysis
  // code will go.
  
  // print every 100 events, so we know where we are:
  xAOD::TEvent* m_event = wk()->xaodEvent();
  if( (m_eventCounter % 10000) ==0 ) Info("execute()", "Event number = %i", m_eventCounter );
  m_eventCounter++;
 
  //----------------------------
  // Event information
  //--------------------------- 
  const xAOD::EventInfo* eventInfo = 0;
  ANA_CHECK(m_event->retrieve( eventInfo, "EventInfo"));  
  
  // check if the event is data or MC
  // (many tools are applied either to data or MC)
  bool isMC = false;
  // check if the event is MC
  if(eventInfo->eventType( xAOD::EventInfo::IS_SIMULATION ) ){
    isMC = true; // can do something with this later
  } 

  //-----------------------------------------------------------
  //----------------
  // TruthTaus
  //----------------

  const xAOD::TruthParticleContainer* truths = 0;
  
  if (isMC){
    if( !m_event->retrieve( truths, "TruthTaus" ).isSuccess() ){
      Error("execute()", "Failed to retrieve TruthParticle. Exiting." );
      return EL::StatusCode::FAILURE;
    }
    
    n_truth_tau = 0;
    const xAOD::TruthParticle* truth_tau_0 = 0;
    const xAOD::TruthParticle* truth_tau_1 = 0;
    double pt_max_0 = 0;
    double pt_max_1 = 0;

    for (auto  xTruthParticle=truths->begin(); xTruthParticle !=truths->end()  ; ++xTruthParticle) {
      int iAbsPdgId = (*xTruthParticle)->absPdgId();
      if ( iAbsPdgId == 15 ){
	n_truth_tau++;
	const xAOD::TruthParticle* this_truth_tau = *xTruthParticle;
	if (this_truth_tau->p4().Pt() > pt_max_0) {
	  pt_max_1 = pt_max_0;
	  truth_tau_1 = truth_tau_0;
	  pt_max_0 = this_truth_tau->p4().Pt();
	  truth_tau_0 = this_truth_tau;
	} else if (this_truth_tau->p4().Pt() > pt_max_1 && this_truth_tau->p4().Pt() <= pt_max_0) {
	  pt_max_1 = this_truth_tau->p4().Pt();
	  truth_tau_1 = this_truth_tau;
	}
      }
    }
    
    if (m_eventCounter<10) std::cout << "n_truth_tau: " << n_truth_tau << " pt_max_0 = " << pt_max_0/1000 << " pt_max_1 = " << pt_max_1/1000 << std::endl;

     //----------------
    // TAUS
    //----------------
    // get Taus container of interest
    
    int n_reco_tau = 0;
    
    double min_delta_r_0 = 1e9;
    double min_delta_r_1 = 1e9;
    const xAOD::TauJet* reco_tau_0 = 0;
    const xAOD::TauJet* reco_tau_1 = 0;
    
    const xAOD::TauJetContainer* taus = 0;
    ANA_CHECK(m_event->retrieve( taus, "TauJets" ));

    const xAOD::TauJet* reco_tau = 0;
    xAOD::TauJetContainer::const_iterator itS = taus->begin();
    xAOD::TauJetContainer::const_iterator itSE = taus->end();
    if (m_eventCounter<10)  std::cout << "taus->size(): " << taus->size() << std::endl;
    
    for (; itS != itSE; ++itS) {
      n_reco_tau++;
      const xAOD::TauJet* this_reco_tau = (*itS);
      if (m_eventCounter<10) std::cout << "reco_pt: " << this_reco_tau->pt()/1000 << " reco_eta = " << this_reco_tau->eta() << " reco_phi = " << this_reco_tau->phi() << std::endl;
      
      if (truth_tau_0) {
	Double_t deta_0 = this_reco_tau->eta()-truth_tau_0->p4().Eta();
	Double_t dphi_0 = TVector2::Phi_mpi_pi(this_reco_tau->phi()-truth_tau_0->p4().Phi());
	double delta_r_0 = TMath::Sqrt( deta_0*deta_0+dphi_0*dphi_0 );
	if (delta_r_0 < min_delta_r_0) {
	  min_delta_r_0 = delta_r_0;
	  reco_tau_0 = this_reco_tau;
	}
      }

      if (truth_tau_1) {
	Double_t deta_1 = this_reco_tau->eta()-truth_tau_1->p4().Eta();
	Double_t dphi_1 = TVector2::Phi_mpi_pi(this_reco_tau->phi()-truth_tau_1->p4().Phi());
	double delta_r_1 = TMath::Sqrt( deta_1*deta_1+dphi_1*dphi_1 );
	if (delta_r_1 < min_delta_r_1) {
	  min_delta_r_1 = delta_r_1;
	  reco_tau_1 = this_reco_tau;
	}
      }

    }

    if (min_delta_r_0 > 0.4) {
      reco_tau_0 = 0;
    } else {
      if (m_eventCounter<10) std::cout << "reco_tau_0 matched with truth_tau_0 " << std::endl;
    }

    if (min_delta_r_1 > 0.4) {
      reco_tau_1 = 0;
    } else {
      if (m_eventCounter<10) std::cout << "reco_tau_1 matched with truth_tau_1 " << std::endl;
    }

    if (m_eventCounter<10) std::cout << "n_reco_tau: " << n_reco_tau << std::endl;

    for (int i = 0 ; i < 2; ++i ) {
      
      if (
	  (i == 0 && n_truth_tau > 0 && pt_max_0/1000 > 0 && truth_tau_0) ||
	  (i == 1 && n_truth_tau > 1 && pt_max_1/1000 > 0 && truth_tau_1)
	  ) {
	
	const xAOD::TruthParticle* truth_tau = (i==0) ? truth_tau_0 : truth_tau_1;

	const xAOD::TauJet* reco_tau = (i==0) ? reco_tau_0 : reco_tau_1;
	
	//      std::cout << "isTau: " << truth_tau->isTau() << std::endl;
	
	if (truth_tau->isTau() && reco_tau) {
	}
	if (1) {
	  IsHadronicTau = truth_tau->auxdata<char>("IsHadronicTau");
	  //std::cout << "IsHadronicTau: " << IsHadronicTau << std::endl;
	  
	  TLorentzVector p4 = truth_tau->p4();
	  //	    std::cout << "p4 : "; p4.Print();
	  //	    std::cout << "p4.pt : " << p4.Pt() << std::endl;
	  //std::cout << "p4.Mass : " << p4.M() << std::endl;
	  //std::cout << "p4.Momentum : " << p4.P() << std::endl;
	  
	  vis_pt          =  truth_tau->auxdata<double>("pt_vis");
	  vis_m           =  truth_tau->auxdata<double>("m_vis");
	  vis_eta         =  truth_tau->auxdata<double>("eta_vis");
	  vis_phi         =  truth_tau->auxdata<double>("phi_vis");
	  std::vector<int> DecayModeVector =  truth_tau->auxdata<std::vector<int>>("DecayModeVector");
	  DecayMode       =  DecayModeVector[0] ;
	  //std::cout << "DecayModeVector_0 : " << DecayModeVector_0 << std::endl;
	  
	  /*
	    std::cout << "pt_vis : " << pt_vis << std::endl;
	    std::cout << "Eta_vis : " << Eta_vis << std::endl;
	    std::cout << "phi_vis : " << phi_vis << std::endl;
	    std::cout << "m_vis : " << m_vis << std::endl;
	  */
	  
	  TLorentzVector vis_p4;
	  vis_p4.SetPtEtaPhiM(vis_pt,vis_eta,vis_phi,vis_m);
	  //std::cout << "vis_p4.Mass : " << vis_p4.M() << std::endl;
	  //std::cout << "vis_p4.Momentum : " << vis_p4.P() << std::endl;
	  
	  TLorentzVector neutrino_p4 = p4 - vis_p4 ;
	  /*
	    std::cout << "p4.P : " << p4.P() << std::endl;
	    std::cout << "neutrino_p4.Mass : " << neutrino_p4.M() << std::endl;
	    std::cout << "neutrino_p4.Momentum : " << neutrino_p4.P() << std::endl;
	    
	    std::cout << "vis_p4 : "; vis_p4.Print();
	    std::cout << " neutrino_p4 : "; neutrino_p4.Print();
	    std::cout << "vis_p4.pt : " << vis_p4.Pt() << std::endl;
	    std::cout << "neutrino_p4.pt : " << neutrino_p4.Pt() << std::endl;
	  */
	  
	  //Double_t angle_vis_neu_1Prong = vis_p4.Angle(neutrino_p4.Vect());
	  //h_angle_vis_neu_1Prong->Fill(angle_vis_neu_1Prong);
	  //std::cout << "angle_vis_neu_1Prong =  " << angle_vis_neu_1Prong  << std::endl;
	  
	  
	  angle_vis_neu = vis_p4.Angle(neutrino_p4.Vect());
	  angle_tau_vis = p4.Angle(vis_p4.Vect());
	  angle_tau_neu = p4.Angle(neutrino_p4.Vect());
	  tau_momentum  = p4.P();
	  tau_pt        = p4.Pt();
	  tau_m         = p4.M();
	  tau_eta       = p4.Eta();
	  tau_phi       = p4.Phi();
	  vis_momentum  = vis_p4.P();
	  neu_pt        = neutrino_p4.Pt();
	  neu_m         = neutrino_p4.M();
	  neu_eta       = neutrino_p4.Eta();
	  neu_phi       = neutrino_p4.Phi();
	  neu_momentum  = neutrino_p4.P();
	  /*
	    std::cout << "angle_vis_neu: " << angle_vis_neu << std::endl;
	    std::cout << " Momentum : " <<  Momentum << std::endl;
	    std::cout << " Angle_tau_vis : " <<  Angle_tau_vis << std::endl;
	    std::cout << "Pt_tau : " << Pt_tau << std::endl;
	  */
	  
	  numCharged = truth_tau->auxdata<size_t>("numCharged");
	  
	  
	  numChargedPion = truth_tau->auxdata<size_t>("numChargedPion");
	  
	  
	  numNeutral = truth_tau->auxdata<size_t>("numNeutral");
	  numNeutralPion = truth_tau->auxdata<size_t>("numNeutralPion");
	  /*
	    std::cout << "numCharged: " << numCharged << std::endl;
	    std::cout << "numChargedPion: " << numChargedPion << std::endl;
	    std::cout << "numNeutral: " << numNeutral << std::endl;
	    std::cout << "numNeutralPion: " << numNeutralPion << std::endl;
	  */
	  
	  tree->Fill();
	  
	}
	
      }
      
    }
    
    
    
   
  } // end of isMC 
    
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: postExecute ()
{
  // Here you do everything that needs to be done after the main event
  // processing.  This is typically very rare, particularly in user
  // code.  It is mainly used in implementing the NTupleSvc.
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: finalize ()
{
  ANA_CHECK_SET_TYPE (EL::StatusCode); // set type of return code you are expecting (add to top of each function once)
  // This method is the mirror image of initialize(), meaning it gets
  // called after the last event has been processed on the worker node
  // and allows you to finish up any objects you created in
  // initialize() before they are written to disk.  This is actually
  // fairly rare, since this happens separately for each worker node.
  // Most of the time you want to do your post-processing on the
  // submission node after all your histogram outputs have been
  // merged.  This is different from histFinalize() in that it only
  // gets called on worker nodes that processed input events.
  Info("finalize()", "Number of total events = %i", m_eventCounter);
  //  Info("finalize()", "Number of clean events = %i", m_numCleanEvents);
  return EL::StatusCode::SUCCESS;
}



EL::StatusCode MyxAODAnalysis :: histFinalize ()
{
  // This method is the mirror image of histInitialize(), meaning it
  // gets called after the last event has been processed on the worker
  // node and allows you to finish up any objects you created in
  // histInitialize() before they are written to disk.  This is
  // actually fairly rare, since this happens separately for each
  // worker node.  Most of the time you want to do your
  // post-processing on the submission node after all your histogram
  // outputs have been merged.  This is different from finalize() in
  // that it gets called on all worker nodes regardless of whether
  // they processed input events.

  /* delete h_jetPt; h_jetPt = 0;
  delete h_tauPt; h_tauPt = 0;
  delete h_tauEta; h_tauEta = 0;
  delete h_numCharged; h_numCharged = 0;*/
  /*
  for (int ipbin = 0 ; ipbin < 14 ; ++ipbin){
    delete h_angle_vis_neu_1Prong[ipbin] ; h_angle_vis_neu_1Prong[ipbin] = 0; 
  }
  */


  return EL::StatusCode::SUCCESS;
}
