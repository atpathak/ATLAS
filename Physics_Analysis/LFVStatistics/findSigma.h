// Author: Aaron Armbruster
// Date:   2012-05-30
// Email:  armbrusa@umich.edu
// Description: Find the value of sigma evaluated at a specified N

#ifndef FINDSIGMA
#define FINDSIGMA

#include "TMath.h"
#include "Math/ProbFuncMathCore.h"
#include "Math/QuantFuncMathCore.h"

#include "RooRealVar.h"
#include "Math/MinimizerOptions.h"
#include "RooMinimizerFcn.h"
#include "RooMinimizer.h"

//#include "macros/minimize.C"
#include "minimize.h"

#include <map>
#include <iostream>

using namespace std;
using namespace RooFit;

double getSigma(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val, double val_true, double& tmu);
void setVal(RooRealVar* par, double val);
double getNLL(RooAbsReal* nll);
double getTpar(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val);

// ____________________________________________________________________________|__________
// get sigma assuming nll -2logLR is parabolic in par
double findSigma(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val_guess, double val_true, double N, double precision) {
    bool isConst = par->isConstant();
    par->setConstant(true);
    int direction = int(N/fabs(N));
    double tmu;
    int nrDamping = 1;
    map<double, double> guess_to_corr;
    double damping_factor = 1.0;
    double damping_factor_pre = damping_factor;
    int nrItr = 0;
    int nretry= 0;
 tryAgain: 
    double val_pre = val_guess-10*precision;
    while (fabs(val_pre-val_guess) > precision) {
        cout << "----------------------" << endl;
        cout << "Starting iteration " << nrItr << " of " << nll->GetName() << " and parameter " << par->GetName() << " at val_guess = " << val_guess << endl;
        val_pre = val_guess;
        damping_factor_pre = damping_factor;

        double sigma_guess = getSigma(nll, nll_val_true, par, val_guess, val_true, tmu);
	//
	if (std::isnan(sigma_guess) || std::isinf(sigma_guess) ) {
	  if (std::isnan(sigma_guess) ) cout << "sigma_guess is nan for nretry = " << nretry << endl;
	  if (std::isinf(sigma_guess) ) cout << "sigma_guess is inf for nretry = " << nretry << endl;
	  if (nretry==0) {
	    val_guess=10*precision*direction;
	  } else if (nretry==1) {
	    val_guess=50*precision*direction;
	  } else if (nretry==2) {
	    val_guess=100*precision*direction;
	  } else if (nretry==3) {
	    val_guess=200*precision*direction;
	  } else if (nretry==4) {
	    val_guess=300*precision*direction;
	  } else if (nretry==5) {
	    val_guess=10*precision*direction;
	  } else if (nretry==6) {
	    val_guess=50*precision*direction;
	  } else if (nretry==7) {
	    val_guess=100*precision*direction;
	  } else if (nretry==8) {
	    val_guess=200*precision*direction;
	  } else if (nretry==9) {
	    val_guess=300*precision*direction;
	  }
	  if (nrItr< 50) {
	    nretry++;
	    if (nretry<10) {
	      nrItr++;
	      goto tryAgain;
	    }
	  }
	}
	//
        double corr = damping_factor*(val_pre - val_true - N*sigma_guess);

        for (map<double, double>::iterator itr=guess_to_corr.begin();itr!=guess_to_corr.end();itr++) {
            if (fabs(itr->first - val_pre) < direction*val_pre*0.02) {
                damping_factor *= 0.8;
                cout << "Changing damping factor to " << damping_factor << ", nrDamping = " << nrDamping << endl;
                if (nrDamping++ > 10) {
                    nrDamping = 1;
                    damping_factor = 1.0;
                }
                corr *= damping_factor;
                break;
            }
        }

        // subtract off the difference in the new and damped correction
        val_guess -= corr;
        guess_to_corr[val_pre] = corr;

        cout << "NLL:            " << nll->GetName() << " = " << nll->getVal() << endl;
        cout << "delta(NLL):     " << nll->getVal()-nll_val_true << endl;
        cout << "N*sigma(pre):   " << fabs(val_pre-val_true) << endl;
        cout << "sigma(guess):   " << sigma_guess << endl;
        cout << "par(guess):     " << val_guess+corr << endl;
        cout << "true val:       " << val_true << endl;
        cout << "tmu:            " << tmu << endl;
        cout << "Precision:      " << direction*val_guess*precision << endl;
        cout << "Correction:     " << (-corr<0?" ":"") << -corr << endl;
        cout << "N*sigma(guess): " << fabs(val_guess-val_true) << endl;
        cout << endl;

        nrItr++;
        if (nrItr > 50) {
            cout << "Infinite loop detected in getSigma(). Please intervene." << endl;
            break;
        }
    }

    cout << "Found sigma for nll " << nll->GetName() << ": " << (val_guess-val_true)/N << endl;
    cout << "Finished in " << nrItr << " iterations." << endl;
    cout << endl;
    par->setConstant(isConst);
    
    return (val_guess-val_true)/N;
}

// ____________________________________________________________________________|__________
double getSigma(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val, double val_true, double& tmu) {
    tmu = getTpar(nll, nll_val_true, par, val);
    return fabs(val-val_true)/sqrt(tmu);
}

// ____________________________________________________________________________|__________
double getTpar(RooAbsReal* nll, double nll_val_true, RooRealVar* par, double val) {
    setVal(par, val);
    double nll_val = getNLL(nll);
    return 2*(nll_val-nll_val_true);
}

// ____________________________________________________________________________|__________
double getNLL(RooAbsReal* nll) {
    minimize(nll);
    double val = nll->getVal();
    return val;
}

// ____________________________________________________________________________|__________
void setVal(RooRealVar* par, double val) {
    if (val > 0 && par->getMax() < val) par->setMax(2*val);
    if (val < 0 && par->getMin() > val) par->setMin(2*val);
    par->setVal(val);
}

#endif
