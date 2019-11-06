#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <algorithm>

#include "TCanvas.h"
#include "TLegend.h"
#include "TFile.h"
#include "TH1D.h"
#include "TGraph.h"
#include "TLine.h"

#include "parseString.C"

using namespace std;

void plotMany(vector<string> folderNames);
void plotMany(string folder1, string folder2, string folder3 = "", string folder4 = "", string folder5 = "", string folder6 = "", string folder7 = "", string folder8 = "")
{
  vector<string> folderNames;
  folderNames.push_back(folder1);
  folderNames.push_back(folder2);
  if (folder3 != "") folderNames.push_back(folder3);
  if (folder4 != "") folderNames.push_back(folder4);
  if (folder5 != "") folderNames.push_back(folder5);
  if (folder6 != "") folderNames.push_back(folder6);
  if (folder7 != "") folderNames.push_back(folder7);
  if (folder8 != "") folderNames.push_back(folder8);
  plotMany(folderNames);
}

void plotMany(vector<string> folderNames)
{
  bool dolimit = 0;
  bool drawNorm = 1;
  bool drawObs = 0;

  int nrFolders = folderNames.size();
  TCanvas* c1 = new TCanvas("c1","c1",1024,768);

  double xmin_leg = 0.5;
  if (drawNorm) xmin_leg = 0.2;
  double xdiff_leg = 0.22;
  double ymax_leg = 0.94;
  double ydiff_leg = 0.06*nrFolders;

  TLegend* leg = new TLegend(xmin_leg, ymax_leg-ydiff_leg, xmin_leg+xdiff_leg, ymax_leg, "", "NDC");
  leg->SetFillStyle(0);
  leg->SetTextSize(0.05);
  leg->SetBorderSize(0);
  leg->SetFillColor(0);

  const int nrPoints = 41;
  double massPoints[nrPoints];
  for (int i=0;i<nrPoints;i++)
  {
    massPoints[i] = 110+i;
  }

  //const int nrPoints = 17;
//   const int nrPoints = 15;
//   int massPoints[nrPoints] = {110, 115, 120, 125, 130, 135, 145, 150, 165, 175, 180, 185, 190, 195, 200};//, 220, 300, 360, 400};

  //int massPoints[nrPoints] = {110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190,195,200,220,240,260,280,300,320,340,360,380,400,420,440,460,500,520,540,560,580,600};
  //int massPoints[nrPoints] = {110,115,120,125,130,135,140,145,150,155,160,165,170,175,180,185,190};//,195,200,220,240,260,280,300,320,340,360,400,420,440,460,480,500,520,540,560,580,600};
  double massPointsD[nrPoints];
  double** exp = new double*[nrFolders];
  double** obs = new double*[nrFolders];

  stringstream axisLabel;
  if (!drawNorm) 
  {
    if (dolimit)
    {
      axisLabel << "95% CL Limit on #sigma/#sigma_{SM}";
    }
    else
    {
      axisLabel << "Statistical Significance";
    }
  }
  else
  {
    string folderName = folderNames[0];
    string legTitle = folderName;
    vector<string> parsed = parseString(folderName,":");
    if (parsed.size() > 1)
    {
      folderName = parsed[0];
      legTitle = parsed[1];
    }

    if (dolimit)
    {
      axisLabel << "Limit / " << legTitle;
    }
    else
    {
      axisLabel << "Significance / " << legTitle;
    }
  }

  double maxLim = -10e9;
  double minLim = 10e9;
  for (int ifo=0;ifo<nrFolders;ifo++)
  {
    string folderName = folderNames[ifo];
    string legTitle = folderName;
    vector<string> parsed = parseString(folderName,":");
    if (parsed.size() > 1)
    {
      folderName = parsed[0];
      legTitle = parsed[1];
    }
    exp[ifo] = new double[nrPoints];
    obs[ifo] = new double[nrPoints];
    for (int ip=0;ip<nrPoints;ip++)
    {
      massPointsD[ip] = massPoints[ip];
      stringstream fileName;
      if (dolimit)
      {
	fileName << "root-files/" << folderName << "_cls/" << massPoints[ip] << ".root";
      }
      else
      {
	fileName << "root-files/" << folderName << "_sig/" << massPoints[ip] << ".root";
      }

      TFile f(fileName.str().c_str());

      TH1D* limit;
      if (dolimit)
      {
	limit = (TH1D*)f.Get("limit");
      }
      else
      {
	limit = (TH1D*)f.Get("hypo");
      }
      if (!limit)
      {
	cout << "ERROR::File doesn't exist: " << fileName.str() << endl;
	exp[ifo][ip] = 1;
	obs[ifo][ip] = 1;
	continue;
      }
      exp[ifo][ip] = limit->GetBinContent(2);
      obs[ifo][ip] = limit->GetBinContent(1);
      if (drawNorm && ifo != 0) 
      {
	exp[ifo][ip] /= exp[0][ip];
	obs[ifo][ip] /= obs[0][ip];
      }
      if (!(drawNorm && ifo == 0))
      {
	minLim = min(minLim, exp[ifo][ip]);
	maxLim = max(maxLim, exp[ifo][ip]);
	minLim = min(minLim, obs[ifo][ip]);
	maxLim = max(maxLim, obs[ifo][ip]);
      }
      //cout << "minLim: " << minLim << endl;
    }
  }
  if (drawNorm)
  {
    for (int ip=0;ip<nrPoints;ip++) 
    {
      exp[0][ip] = 1.0;
      obs[0][ip] = 1.0;
    }
  }
  minLim *= 0.5;
  maxLim *= 1.4;

  if (drawNorm)
  {
    maxLim = 4.5;
    minLim = 0.3;
  }

  maxLim = 1.5;
  minLim = 0.75;

  int color = 1;
  for (int ifo=0;ifo<nrFolders;ifo++)
  {
    string folderName = folderNames[ifo];
    string legTitle = folderName;
    vector<string> parsed = parseString(folderName,":");
    if (parsed.size() > 1)
    {
      folderName = parsed[0];
      legTitle = parsed[1];
    }
    double* exp_ary = exp[ifo];
    double* obs_ary = obs[ifo];
    TGraph* graph = new TGraph(nrPoints, massPointsD, exp_ary);
    TGraph* graph_obs = new TGraph(nrPoints, massPointsD, obs_ary);
    if (drawNorm)
    {
      graph->SetMinimum(minLim);
      graph->SetMaximum(maxLim);
    }
    graph->SetTitle("");
    graph->GetYaxis()->SetTitleOffset(1.5);
    graph->SetLineColor(color);
    graph->SetMarkerColor(color);
    graph->GetXaxis()->SetTitle("m_{H} [GeV]");
    graph->GetYaxis()->SetTitle(axisLabel.str().c_str());

    graph_obs->SetTitle("");
    graph_obs->GetYaxis()->SetTitleOffset(1.5);
    graph_obs->SetLineColor(color);
    graph_obs->SetMarkerColor(color);
    graph_obs->GetXaxis()->SetTitle("m_{H} [GeV]");
    graph_obs->GetYaxis()->SetTitle(axisLabel.str().c_str());

    if (ifo == 0) graph->Draw("al");
    else 
    {
      graph->Draw("l");
      if (drawObs) graph_obs->Draw("lp");
    }

    leg->AddEntry(graph, legTitle.c_str(), "l");
    color++;
    if (color == 5) color++;
    if (color == 8) {color++;color++;color++;}
  }
  
  TLine l;
  l.SetLineWidth(2);
  l.SetLineColor(13);
  l.SetLineStyle(2);
  if (!drawNorm) l.DrawLine(massPoints[0], 1, massPoints[nrPoints-1], 1);

  //l.DrawLine(195, 2, 195, 0.2);



  leg->Draw();



  if (!drawNorm) c1->SetLogy(1);
}

// vector<string> parseString(string str, string sep)
// {
//   vector<string> parsed;
//   int pos = 0;
//   bool first = true;
//   if (str.size() == 0) return parsed;
//   if (str.find(sep) == string::npos)
//   {
//     parsed.push_back(str);
//     return parsed;
//   }
//   while (true)
//   {
//     int newPos = str.find(sep, pos);
//     if (str.find(sep, pos) == string::npos)
//     {
//       if (!first) parsed.push_back(str.substr(pos, newPos-pos));
//       break;
//     }
//     //cout << "pos: " << pos << ", newpos: " << newPos << endl;
//     string sub = str.substr(pos, newPos-pos);
//     parsed.push_back(sub);
//     pos = newPos+1;
//     first = false;
//     //cout << "Adding string: " << sub << endl;
//   }
// //   for (int i=0;i<(int)parsed.size();i++)
// //   {
// //     cout << "str: " << parsed[i] << endl;
// //   }
//   return parsed;
// }
