// Author: Aaron Armbruster
// Date:   2012-05-30
// Email:  armbrusa@umich.edu
// Description: minimize a function with a smart retry strategy, eventually compute Minos error

#ifndef MINIMIZE
#define MINIMIZE

#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"
#include "TStopwatch.h"
#include "RooFitResult.h"

#include <iostream>
#include <string>

using namespace std;
using namespace RooFit;

RooFitResult* minimize(RooAbsReal* fcn, RooArgSet* minosSet = NULL) {
    TStopwatch timer;
    timer.Start();

    int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
    RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
    if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

    int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
    int save_strat = strat;
    
    RooMinimizer minim(*fcn);
    minim.optimizeConst(2);
    minim.setStrategy(strat);
    minim.setPrintLevel(printLevel);
    minim.setProfile(1);

    int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    cout << "minimize with strat = " << strat << " status = " << status << endl;
    // up the strategy
    if (status != 0 && status != 1 && strat < 2) {
        strat++;
        cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
        minim.setStrategy(strat);
        status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
	cout << "minimize with strat = " << strat << " status = " << status << endl;
    }

    if (status != 0 && status != 1) {
      cout << "Fit failed with status " << status << endl;
    } else {
      cout << "Fit succeeded ... " << endl;
    }

    if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(save_strat);

    string name = Form("fitresult_%s",fcn->GetName());
    string title = Form("fitresult_%s",fcn->GetName());

    RooFitResult* fitresult = minim.save(name.c_str(), title.c_str());
    minim.hesse();
    if (minosSet != NULL) {
      cout << "BEFORE MINOS::"; minosSet->Print("v");
      minim.minos(*minosSet);
      cout << "AFTER MINOS:: "; minosSet->Print("v");
    }

    timer.Stop();
    timer.Print();

    return fitresult;
}

RooFitResult* minimize(int & status, int & HesseStatus, RooAbsReal* fcn, RooArgSet* minosSet = NULL) {
    TStopwatch timer;
    timer.Start();

    int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
    RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
    if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

    int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
    int save_strat = strat;
    
    RooMinimizer minim(*fcn);
    minim.optimizeConst(2);
    minim.setStrategy(strat);
    minim.setPrintLevel(printLevel);
    minim.setProfile(1);

    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());

    // up the strategy
    if (status != 0 && status != 1 && strat < 2) {
        strat++;
        cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
        minim.setStrategy(strat);
        status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
    }

    if (status != 0 && status != 1) {
        cout << "Fit failed with status " << status << endl;
    }

    if (printLevel < 0) RooMsgService::instance().setGlobalKillBelow(msglevel);
    ROOT::Math::MinimizerOptions::SetDefaultStrategy(save_strat);

    string name = Form("fitresult_%s",fcn->GetName());
    string title = Form("fitresult_%s",fcn->GetName());

    RooFitResult* fitresult = minim.save(name.c_str(), title.c_str());
    cout << "fitresult status = " << fitresult->status();
    HesseStatus= minim.hesse();
    cout << "HesseStatus = " << HesseStatus << endl;
    if (minosSet != NULL) {
        minim.minos(*minosSet);
    }

    timer.Stop();
    timer.Print();

    return fitresult;
}

#endif
