
#include "macros/drawPlot.C"

#include "TLatex.h"
#include "TLine.h"
#include "TCanvas.h"

bool oldWay=0;
double maxLimit = 400;
double minLimit = 0.00005;
bool drawBands = 1;
bool doreread = 0;
string cardOpts = "";

void drawPlot_cls2(string cardName);
void drawPlot_cls(string cardName, bool rereadAscii = 0, bool showZoom = 0, string overlayCard="", string overlayCard2="")
{
  vector<string> parsed = parseString(cardName, ":");
  if (parsed.size() > 1)
  {
    cardOpts = parsed[1];
  }

  cardName = parsed[0]+"_pv";
  applyOverlay(overlayCard, overlay, "_pv");
  applyOverlay(overlayCard2, overlay2, "_pv");
  //applyOverlay(overlayCard2, overlay2, "_pv");
  //applyOverlay(overlayCard3, overlay3, "_pv");

  computeFlags(cardName);
  dozoom = showZoom;
  doreread = rereadAscii;
  //overlay=overlayCard;
  //if (overlay != "") overlay += "_pv";

  if (dozoom)
  {
    overrideMass = 1;
    maxMass = 150;
  }

  //labelTxt="Private";
  ydiff_leg = 0.13;
  if (overlay != "") ydiff_leg = 0.2;
  if (drawBands) ydiff_leg += 0.03;
  labelPosX = 0.2;
  if (dozoom) labelPosX = 0.2;
  labelPosY = 0.89;
  TCanvas* c1 = new TCanvas("c1","c1",1024,768);







//   xmin_leg = 0.18;
//   xdiff_leg = 0.22;
//   ymax_leg = 0.89;

//   if (overlay != "") ydiff_leg += 0.05;
//   if (overlay2 != "") ydiff_leg += 0.05;
// //   if (overlay2 != "") ydiff_leg += 0.05;
// //   if (overlay3 != "") ydiff_leg += 0.05;
  
//   //txtPosX = 0.55;
//   txtPosY = 0.79;
//   txtPosX = 0.49;
//   if (do2011) txtPosX = 0.45;





  if (dogg_4l)
  {

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass = 110;
      maxMass = 150;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, channel_label.c_str());
  }

  else if (dogg)
  {
    lumi = "4.9";

    xmin_leg = 0.25;
    xdiff_leg = 0.22;
    ymax_leg = 0.36;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass = 110;
      maxMass = 150;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#gamma#gamma");
  }
  else if (dollll)
  {

    lumi = "4.9";

    xmin_leg = 0.62;
    xdiff_leg = 0.22;
    ymax_leg = 0.55;

    txtPosX = xmin_leg;
    txtPosY = 0.30;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllll");
  }
  else if (dollqq)
  {
    lumi = "2.05";

    xmin_leg = 0.2;
    xdiff_leg = 0.22;
    ymax_leg = 0.38;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.06;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=200;
      maxMass=600;
    }

    minLimit = 0.008;

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllqq");
  }
  else if (dollvv)
  {
    lumi = "2.05";

    xmin_leg = 0.25;
    xdiff_leg = 0.22;
    ymax_leg = 0.88;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=200;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowll#nu#nu");
  }
  else if (dolvlv)
  {
//     lumi = "4.7";
//     if (cardName.find("_BK_") != string::npos) lumi = "2.3";
//     else if (cardName.find("_LM_") != string::npos) lumi = "2.4";

//     xmin_leg = 0.58;
//     xdiff_leg = 0.22;
//     ymax_leg = 0.55;

//     txtPosX = 0.58;
//     txtPosY = 0.30;

//     minLimit = 0.0000005;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=110;
      maxMass=600;
    }

    if (cardName.find("_lpt_") != string::npos) 
    {
      maxMass = 190;
    }

//     if (dozoom)
//     {
//       xmin_leg = 0.28;
//       xdiff_leg = 0.22;
//       ymax_leg = 0.55;

//       txtPosX = 0.28;
//       txtPosY = 0.30;
//     }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    if (cardName.find("_ee_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu");
    else if (cardName.find("_em_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu");
    else if (cardName.find("_mm_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu");
    else if (cardName.find("_0j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu+0j");
    else if (cardName.find("_1j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu+1j");
    else if (cardName.find("_2j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu+2j");
    else if (cardName.find("_BK_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu B-K");
    else if (cardName.find("_LM_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu L-M");
    else if (cardName.find("_ee0j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu+0j");
    else if (cardName.find("_em0j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu+0j");
    else if (cardName.find("_mm0j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu+0j");
    else if (cardName.find("_ee1j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu+1j");
    else if (cardName.find("_em1j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu+1j");
    else if (cardName.find("_mm1j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu+1j");
    else if (cardName.find("_ee2j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu+2j");
    else if (cardName.find("_em2j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu+2j");
    else if (cardName.find("_mm2j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu+2j");
    else if (cardName.find("_01j_") != string::npos) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu+0+1j");
    else if (cardName.find("_cuts_") != string::npos) 
    {
      t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "m_{T} Cut");
    }
    else if (cardName.find("_lpt_") != string::npos) 
    {
      t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "Low pT alone");
    }
    else if (cardName.find("_hpt_") != string::npos) 
    {
      t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "High pT alone");
    }
    else t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
  }
  else if (dolvqq)
  {
    lumi = "1.04";

    xmin_leg = 0.2;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=240;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW#rightarrowl#nuqq");
  }
  else if (dolh)
  {
    lumi = "1.063";

    xmin_leg = 0.19;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=110;
      maxMass=150;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{h}");
  }
  else if (doll)
  {
    lumi = "1.063";

    xmin_leg = 0.35;
    xdiff_leg = 0.22;
    ymax_leg = 0.4;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{l}+1j");
  }
  else if (dowh)
  {
    lumi = "1.063";

    xmin_leg = 0.2;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "WH, H#rightarrowbb");
  }
  else if (dozh)
  {
    lumi = "1.063";

    xmin_leg = 0.2;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "ZH, H#rightarrowbb");
  }
  else if (dovh)
  {
    lumi = "1.063";

    xmin_leg = 0.2;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "VH, H#rightarrowbb");
  }
  else if (dozz)
  {
    lumi = "4.7-4.9";

    xmin_leg = 0.32;
    xdiff_leg = 0.22;
    ymax_leg = 0.52;

    txtPosX = xmin_leg;
    txtPosY = 0.24;

    if (!dozoom)
    {
      txtPosX = 0.3;
      txtPosY = 0.74;
      xmin_leg = 0.6;
      ymax_leg = 0.87;
    }


    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=120;
      maxMass=600;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ");
  }
  else if (doww)
  {
    lumi = "1.04-2.05";

    xmin_leg = 0.58;
    xdiff_leg = 0.22;
    ymax_leg = 0.55;

    txtPosX = 0.58;
    txtPosY = 0.30;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=110;
      maxMass=300;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW#rightarrow");
  }
  else if (dott)
  {
    lumi = "1.063";

    xmin_leg = 0.19;
    xdiff_leg = 0.22;
    ymax_leg = 0.45;

    txtPosX = xmin_leg+0.3;
    txtPosY = ymax_leg-0.08;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=110;
      maxMass=150;
    }

    drawPlot_cls2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau#tau");
  }
  else if (docb)
  {
//     lumi = "4.6-4.9";

//     xmin_leg = 0.25;
//     xdiff_leg = 0.22;
//     ymax_leg = 0.86;

//     txtPosX = 0.49;
//     txtPosY = 0.72;

    markerSize = 0.8;

    if (!overrideMass)
    {
      minMass=110;
      maxMass=600;
    }


    maxLimit = 50000;
    minLimit = 0.000000005;

    if (dozoom)
    {
//       xmin_leg = 0.2;
//       xdiff_leg = 0.22;
//       ymax_leg = 0.57;

//       txtPosX = 0.196;
//       txtPosY = 0.281;
      //maxMass = 135;
      maxLimit = 2000000;
      minLimit = 0.000000000005;
    }


    drawPlot_cls2(cardName);

    drawTopRight(year+" Data");
  }
  else
  {

  }

  if (doSave)
  {
    string saveName = cardName+"_cls";
    if (overlay != "") saveName+="_comp";
    if (dozoom) saveName += "_zoom";
    save(saveName, "eps", c1);
    save(saveName, "pdf", c1);
  }
}



void drawPlot_cls2(string cardName)
{
  cout << "Drawing plot: " << cardName << endl;

//see if file exists
  ifstream testFile(("ascii/"+cardName+".txt").c_str());
  if (testFile.fail() || doreread)
  {
    saveAscii(cardName);
  }

  fileHolder numbers;
  drawPlot("ascii/"+cardName+".txt", 6, numbers);

  int nrPoints = numbers.massPoints.size();
  if (nrPoints == 0)
  {
//maybe ascii needs to be rewritten
    saveAscii(cardName);
    drawPlot("ascii/"+cardName+".txt", 6, numbers);
  }
  nrPoints = numbers.massPoints.size();

  fileHolder olFile;
  int nrOlPoints;
  double* olPoints;
  double* olObs;
  double* olExp;
  if (overlay != "")
  {
    string overlay_base = parseString(overlay,":")[0];
    if (doreread) saveAscii(overlay_base);
    drawPlot("ascii/"+overlay_base+".txt", 6, olFile);
    nrOlPoints = olFile.massPoints.size();

    if (nrOlPoints == 0) 
    {
      saveAscii(overlay_base);
      drawPlot("ascii/"+overlay_base+".txt", 6, olFile);
      nrOlPoints = olFile.massPoints.size();
    }

    olPoints = getAry(olFile.massPoints);
    olObs = getAry(olFile.getCol(0));
    olExp = getAry(olFile.getCol(1));
  }

  double* massPoints = getAry(numbers.massPoints);
  double* obs = getAry(numbers.getCol(0));
  double* exp = getAry(numbers.getCol(1));
  double* p2s = getAry(numbers.getCol(2));
  double* p1s = getAry(numbers.getCol(3));
  double* n1s = getAry(numbers.getCol(4));
  double* n2s = getAry(numbers.getCol(5));
  for (int i=0;i<nrPoints;i++)
  {
    obs[i] = oldWay ? 1 - obs[i] : obs[i];
    exp[i] = oldWay ? 1 - exp[i] : exp[i];
    p2s[i] = p2s[i] - exp[i];
    p1s[i] = p1s[i] - exp[i];
    n1s[i] = exp[i] - n1s[i];
    n2s[i] = exp[i] - n2s[i];

    if (overlay != "")
    {
      olObs[i] = oldWay ? 1 - olObs[i] : olObs[i];
      olExp[i] = oldWay ? 1 - olExp[i] : olExp[i];
    }
  }

  fileHolder olFile2;
  int nrOlPoints2;
  double* olPoints2;
  double* olObs2;
  double* olExp2;
  if (overlay2 != "")
  {
    string overlay_base = parseString(overlay2,":")[0];
    if (doreread) saveAscii(overlay_base);
    drawPlot("ascii/"+overlay_base+".txt", 6, olFile2);
    nrOlPoints2 = olFile2.massPoints.size();

    if (nrOlPoints2 == 0) 
    {
      saveAscii(overlay_base);
      drawPlot("ascii/"+overlay_base+".txt", 6, olFile2);
      nrOlPoints2 = olFile2.massPoints.size();
    }

    olPoints2 = getAry(olFile2.massPoints);
    olObs2 = getAry(olFile2.getCol(0));
    olExp2 = getAry(olFile2.getCol(1));
  }

  double* massPoints = getAry(numbers.massPoints);
  double* obs = getAry(numbers.getCol(0));
  double* exp = getAry(numbers.getCol(1));
  double* p2s = getAry(numbers.getCol(2));
  double* p1s = getAry(numbers.getCol(3));
  double* n1s = getAry(numbers.getCol(4));
  double* n2s = getAry(numbers.getCol(5));
  for (int i=0;i<nrPoints;i++)
  {
    obs[i] = oldWay ? 1 - obs[i] : obs[i];
    exp[i] = oldWay ? 1 - exp[i] : exp[i];
    p2s[i] = p2s[i] - exp[i];
    p1s[i] = p1s[i] - exp[i];
    n1s[i] = exp[i] - n1s[i];
    n2s[i] = exp[i] - n2s[i];

    if (overlay != "")
    {
      olObs[i] = oldWay ? 1 - olObs[i] : olObs[i];
      olExp[i] = oldWay ? 1 - olExp[i] : olExp[i];
    }
  }

  int c_2s = kYellow;
  int c_1s = kGreen;



  string ytitle = "CLs";
  TGraphAsymmErrors* g_2s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n2s, p2s);
  g_2s->SetMinimum(minLimit);
  g_2s->SetMaximum(maxLimit);
  g_2s->SetFillColor(c_2s);
  g_2s->GetYaxis()->SetTitleOffset(1.2);
  if (drawBands) g_2s->Draw("al3");

  TGraphAsymmErrors* g_1s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n1s, p1s);
  g_1s->SetFillColor(c_1s);
  g_1s->SetMinimum(minLimit);
  g_1s->SetMaximum(maxLimit);
  if (drawBands) g_1s->Draw("l3");

  TGraph* g_obs = makeGraph(ytitle, nrPoints, massPoints, obs);
  g_obs->SetLineStyle(1);
  g_obs->SetLineWidth(2);
  g_obs->SetMinimum(minLimit);
  g_obs->SetMaximum(maxLimit);
  g_obs->SetMarkerSize(markerSize);
  g_obs->GetYaxis()->SetTitleOffset(1.2);
  g_obs->GetXaxis()->SetMoreLogLabels(1);
  g_obs->GetXaxis()->SetNoExponent(1);
  if (!dozoom)
  {
    g_obs->GetXaxis()->SetMoreLogLabels(1);
    g_obs->GetXaxis()->SetNoExponent(1);
    g_obs->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
  }
  else
  {
    g_obs->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
  }
  if (drawBands) g_obs->Draw("lp");
  else g_obs->Draw("alp");

  TGraph* g_exp = makeGraph(ytitle, nrPoints, massPoints, exp);
  g_exp->SetLineStyle(2);
  g_exp->SetLineWidth(2);
  g_exp->Draw("l");






  TLegend* leg = makeLeg();
  leg->AddEntry(g_obs, (string("Obs. ")+cardOpts).c_str(),"lp");
  leg->AddEntry(g_exp, (string("Exp. ")+cardOpts).c_str(),"l");
  if (drawBands)
  {
    leg->AddEntry(g_1s, "#pm1 #sigma","f");
    leg->AddEntry(g_2s, "#pm2 #sigma","f");
  }

  if (overlay != "")
  {
    cout << "Drawing overlay" << endl;
    TGraph* g_ol_obs = makeGraph(ytitle, nrOlPoints, olPoints, olObs);
    g_ol_obs->SetLineColor(kRed);
    g_ol_obs->SetMarkerColor(kRed);
    g_ol_obs->SetLineStyle(1);
    g_ol_obs->SetLineWidth(2);
    g_ol_obs->SetMinimum(minLimit);
    g_ol_obs->SetMaximum(maxLimit);
    g_ol_obs->SetMarkerSize(markerSize);
    g_ol_obs->GetYaxis()->SetTitleOffset(1.2);
    g_ol_obs->Draw("lp");

    TGraph* g_ol_exp = makeGraph(ytitle, nrOlPoints, olPoints, olExp);
    g_ol_exp->SetLineColor(kRed);
    g_ol_exp->SetMarkerColor(kRed);
    g_ol_exp->SetLineStyle(2);
    g_ol_exp->SetLineWidth(2);
    g_ol_exp->Draw("l");

    string suf = "Conf";
    applyOverlay(overlay, suf);

    leg->AddEntry(g_ol_obs, (string("Obs. ")+suf).c_str(),"lp");
    leg->AddEntry(g_ol_exp, (string("Exp. ")+suf).c_str(),"l");
  }

  if (overlay2 != "")
  {
    cout << "Drawing overlay" << endl;
    TGraph* g_ol_obs2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olObs2);
    g_ol_obs2->SetLineColor(kBlue);
    g_ol_obs2->SetMarkerColor(kBlue);
    g_ol_obs2->SetLineStyle(1);
    g_ol_obs2->SetLineWidth(2);
    g_ol_obs2->SetMinimum(minLimit);
    g_ol_obs2->SetMaximum(maxLimit);
    g_ol_obs2->SetMarkerSize(markerSize);
    g_ol_obs2->GetYaxis()->SetTitleOffset(1.2);
    g_ol_obs2->Draw("lp");

    TGraph* g_ol_exp2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olExp2);
    g_ol_exp2->SetLineColor(kBlue);
    g_ol_exp2->SetMarkerColor(kBlue);
    g_ol_exp2->SetLineStyle(2);
    g_ol_exp2->SetLineWidth(2);
    g_ol_exp2->Draw("l");

    string suf = "Conf";
    applyOverlay(overlay2, suf);

    leg->AddEntry(g_ol_obs2, (string("Obs. ")+suf).c_str(),"lp");
    leg->AddEntry(g_ol_exp2, (string("Exp. ")+suf).c_str(),"l");
  }

  leg->Draw();


  double lposx_lo = g_obs->GetXaxis()->GetXmin();
  double lposx_hi = g_obs->GetXaxis()->GetXmax();

  if (!dozoom)
  {
    lposx_lo=massPoints[0]-10;
    lposx_hi=massPoints[nrPoints-1]+20;
  }
  else
  {
    lposx_lo=massPoints[0];
    lposx_hi=massPoints[nrPoints-1];
  }



  TLine l;
  l.SetLineWidth(2);
  l.SetLineStyle(2); 
  l.DrawLine(lposx_lo,1,lposx_hi,1);

  l.SetLineColor(kRed);

  //l.DrawLine(lposx_lo,0.1,lposx_hi,0.1);
  l.DrawLine(lposx_lo,0.05,lposx_hi,0.05);
  l.DrawLine(lposx_lo,0.01,lposx_hi,0.01);

  TLatex t;
  t.SetTextColor(kRed);
  double delta = (lposx_hi-lposx_lo)*0.02;
  //t.DrawLatex(lposx_hi+delta, 0.1-0.005, "90%");
  t.DrawLatex(lposx_hi+delta, 0.05-0.005, "95%");
  t.DrawLatex(lposx_hi+delta, 0.005, "99%");








  drawInsert();

  c1->SetLogy(dolog);
}

