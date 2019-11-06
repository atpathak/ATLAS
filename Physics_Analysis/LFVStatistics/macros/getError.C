#include "RooWorkspace.h"
#include "RooStats/ModelConfig.h"
#include "RooDataSet.h"
#include "RooRealVar.h"
#include "RooMinimizer.h"
#include "RooNLLVar.h"

#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
#include <vector>
#include <map>

using namespace std;
using namespace RooFit;
using namespace RooStats;

void getError(int direction, RooNLLVar* nll, RooRealVar* firstPOI, double& err);

map<string, RooNLLVar*> nll_map;

// previously used for number counting
double interp(double x, double x1, double x2, double y1, double y2)
{
  double m = (y2-y1)/(x2-x1);
  double b = y2 - m*x2;

  cout << "Interpolating between point1 = (" << x1 << ", " << y1 << "), point2 = (" << x2 << ", " << y2 << ")" << endl;

  return m*x+b;
}

void minimize(RooNLLVar* nll, bool minos=0, RooArgSet* minosSet=0)
{
  int strat = ROOT::Math::MinimizerOptions::DefaultStrategy();
  RooMinimizer minim(*nll);
  minim.setStrategy(strat);
  minim.setPrintLevel(ROOT::Math::MinimizerOptions::DefaultPrintLevel());
  // minim.setPrintLevel(3);
  // RooFit::MsgLevel msglevel = RooMsgService::instance().globalKillBelow();
  // RooMsgService::instance().setGlobalKillBelow(RooFit::FATAL);

  int status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());

  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (status != 0 && status != 1 && strat < 2)
  {
    strat++;
    cout << "Fit failed with status " << status << ". Retrying with strategy " << strat << endl;
    minim.setStrategy(strat);
    status = minim.minimize(ROOT::Math::MinimizerOptions::DefaultMinimizerType().c_str(), ROOT::Math::MinimizerOptions::DefaultMinimizerAlgo().c_str());
  }

  if (minos)
  {
    // int runopt = 0;
    // if (isatboundary) runopt = 2;
    if (minosSet)
    {
      minim.minos(*minosSet/*, runopt*/);
    }
    else
    {
      minim.minos(/*runopt*/);
    }
  }
  // RooMsgService::instance().setGlobalKillBelow(msglevel);
}

void getError(int direction, RooNLLVar* nll, RooRealVar* firstPOI, double& err)
{
  if (direction != -1 && direction != 1)
  {
    cout << "ERROR::Invalid direction: " << direction << endl;
    exit(1);
  }
  int printLevel = ROOT::Math::MinimizerOptions::DefaultPrintLevel();
  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(-1);

  bool isconst = firstPOI->isConstant();
  firstPOI->setConstant(1);
  
  // see if its at a wall
  double mu_val = firstPOI->getVal();

  if (direction == -1)
  {
    firstPOI->setVal(firstPOI->getVal()- 0.02);
    minimize(nll);
    if (nll->getVal() < -pow(10., 27.) || nll->getVal() != nll->getVal())
    {
      err = 0;
      return;
    }
  }

  firstPOI->setVal(mu_val);

  double last_good_mu = firstPOI->getVal();
  double target = 0.5;
  int itr = 0;
  double nll_val = nll->getVal();
  double thisNll_val = nll->getVal();

  double err_pre = direction == 1 ? firstPOI->getErrorHi() : firstPOI->getErrorLo();

  double step_size = 0.1;//max(0.1, err_pre*0.5);
  double step_size_min = step_size/10;
  double oldNll_val = nll->getVal();
  while (fabs(thisNll_val - nll_val) <= target || thisNll_val != thisNll_val)
  {
    oldNll_val = thisNll_val;
    double old_mu = firstPOI->getVal();
    firstPOI->setVal(old_mu + step_size*direction);
    double this_mu = firstPOI->getVal();
    minimize(nll);
    thisNll_val = nll->getVal();
    if (thisNll_val != thisNll_val) // back off until we get back to normal
    {
      step_size *= 0.2;
      if (step_size < step_size_min)
      {
        thisNll_val = nll_val + target+0.000000001;
        break;
      }
      firstPOI->setVal(last_good_mu+step_size);
      cout << "step size is now " << step_size << endl;
    }
    else
    {
      last_good_mu = old_mu;
      if (fabs(thisNll_val - nll_val) <= target)
      {
        // step_size = step_size/(thisNll_val - oldNll_val)*target;
        // cout << "Using new step size = " << step_size << endl;
      }
    }

    itr++;
    cout << "Iterator " << itr << " gives nll val = " << thisNll_val << " for par = " << this_mu << ", delta(pll) = " << nll_val - thisNll_val << ", delta(par) = " << this_mu - mu_val << endl;
  }
  if (direction == 1)
  {

  }
  // interpolate
  err = interp(target+nll_val, thisNll_val, oldNll_val, firstPOI->getVal(), firstPOI->getVal() - direction*step_size) - mu_val;//

  ROOT::Math::MinimizerOptions::SetDefaultPrintLevel(printLevel);
  firstPOI->setConstant(isconst);
}

map<string, double> pllVals()
{
  map<string, double> vals;
  for (map<string, RooNLLVar*>::iterator itr=nll_map.begin();
       itr!=nll_map.end();itr++)
  {
    vals[itr->first] = itr->second->getVal();
  }
  return vals;
}

void printDeltaPll(map<string, double>& map1, map<string, double>& map2)
{
  cout << setprecision(3);
  for (map<string, RooNLLVar*>::iterator itr=nll_map.begin();
       itr!=nll_map.end();itr++)
  {
    cout << itr->first << ", delta(pll) = " << map1[itr->first] - map2[itr->first] << ", (pll1 = " << map1[itr->first] << ", pll2 = " << map2[itr->first] << ")" << endl;
  }
}

void printNlls()
{
  for (map<string, RooNLLVar*>::iterator itr=nll_map.begin();itr!=nll_map.end();itr++)
  {
    itr->second->Print("v");
  }
}
