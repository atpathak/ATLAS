#include "macros/drawPlot.C"

#include "TLatex.h"
#include "TLine.h"
#include "TCanvas.h"

bool drawObs       = 1;
bool doProgressive = 0;
bool doQtev        = 0;
bool uncap         = 1;
double minLimit    = 1-ROOT::Math::gaussian_cdf(5);
double maxLimit    = 5000000;
bool overlayExp    = 1;
bool overlayObs    = 1;
bool doreread      = 0;
string cardOpts    = "";

void drawPlot_p02(string cardName);

void drawPlot_p0(string cardName, bool rereadAscii = 0, bool showZoom = 0, string overlayCard="", string overlayCard2="", string overlayCard3="") {

  vector<string> parsed = parseString(cardName, ":");
  if (parsed.size() > 1) {
    cardOpts = parsed[1];
  }

  if (!doQtev) {
    cardName = parsed[0]+"_sig";
    applyOverlay(overlayCard , overlay , "_sig");
    applyOverlay(overlayCard2, overlay2, "_sig");
    applyOverlay(overlayCard3, overlay3, "_sig");
  }
  else {
    cardName = parsed[0]+"_q";
    applyOverlay(overlayCard , overlay , "_q");
    applyOverlay(overlayCard2, overlay2, "_q");
    applyOverlay(overlayCard3, overlay3, "_q");
  }

  dozoom    = showZoom;
  computeFlags(cardName);
  doreread  = rereadAscii;
  uncap     = 1;
  drawBands = 0;

  if (cardName.find("inj") != string::npos) {
    uncap     = 1;
    doInj     = 1;
    drawBands = 1;
  }

  if (dozoom) {
    overrideMass = 1;
    maxMass      = 150;
  }

  showLabel = 1;
  // labelTxt = "Private";
  
  ydiff_leg = 0.11;
  
  if (drawBands) {
    ydiff_leg += 0.08;
  }

  labelPosX = 0.18;
  labelPosY = 0.87;
  
  TCanvas* c1 = new TCanvas("c1","c1",1024,768);

  if (dogg_4l_lvlv) {
    lumi       = "4.7-4.9";

    xmin_leg   = 0.16;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.42;

    txtPosX    = xmin_leg + 0.44;
    txtPosY    = ymax_leg - 0.15;

    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 110;
      maxMass  = 150;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "#gamma#gamma+4l+l#nul#nu");
  }
  else if (dogg_4l) {

    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 110;
      maxMass  = 150;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, channel_label.c_str());
  }
  else if (dogg) {
    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 110;
      maxMass  = 150;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#gamma#gamma");
  }
  else if (dollll) {
    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 110;
      maxMass  = 600;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllll");
  }
  else if (dollqq) {
    lumi       = "2.05";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.13;

    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 200;
      maxMass  = 600;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllqq");
  }
  else if (dollvv) {
    lumi       = "2.05";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 200;
      maxMass  = 600;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowll#nu#nu");
  }
  else if (dolvlv) {
    // labelPosY = 0.81;

    if (cardName.find("_BK_") != string::npos) {
      lumi = "2.3";
    }
    else if (cardName.find("_LM_") != string::npos) {
      lumi = "2.4";
    }

    if (overlay == "") {
      maxLimit = 5000;
    }

    maxLimit = 700000;
    minLimit = 0.01;

    if (overlay != "") {
      ydiff_leg += 0.04;
    }

    if (overlay2 != "") {
      ydiff_leg += 0.04;
    }

    if (overlay3 != "") {
      ydiff_leg += 0.04;
    }

    markerSize = 0.8;

    minMass    = 90;
    maxMass    = 200;
    
    if (dozoom) {
      minMass  = 115;
      maxMass  = 150;
    }

    maxMass=min(maxMass, 600);

    if (cardName.find("paper") != string::npos) {
      lumi     = "2.1";
      xmin_leg = 0.5 ;
      ymax_leg = 0.565;
      txtPosX  = 0.5;
    }

    if (cardName.find("_lpt_") != string::npos) {
      maxMass = 190;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    if (cardName.find("_ee_") != string::npos)          t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu");
    else if (cardName.find("_em_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu");
    else if (cardName.find("_me_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu");
    else if (cardName.find("_mm_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu");
    else if (cardName.find("_0j_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0 jets)");
    else if (cardName.find("_1j_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (1 jet)");
    else if (cardName.find("_2j_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (2 jets)");
    else if (cardName.find("_BK_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu B-K");
    else if (cardName.find("_LM_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu L-M");
    else if (cardName.find("_ee0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0 jets)");
    else if (cardName.find("_em0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0 jets)");
    else if (cardName.find("_mm0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0 jets)");
    else if (cardName.find("_me0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0 jets)");
    else if (cardName.find("_ee1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (1 jet)");
    else if (cardName.find("_em1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (1 jet)");
    else if (cardName.find("_me1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (1 jet)");
    else if (cardName.find("_mm1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (1 jet)");
    else if (cardName.find("_ee01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0/1 jets)");
    else if (cardName.find("_em01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0/1 jets)");
    else if (cardName.find("_mm01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0/1 jets)");
    else if (cardName.find("_me01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0/1jets)");
    else if (cardName.find("_ee2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (2 jets)");
    else if (cardName.find("_em2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (2 jets)");
    else if (cardName.find("_me2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (2 jets)");
    else if (cardName.find("_mm2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (2 jets)");
    else if (cardName.find("_em0jBK_") != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (0 jets)");
    else if (cardName.find("_em0jLM_") != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (0 jets)");
    else if (cardName.find("_em1jBK_") != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (1 jet)");
    else if (cardName.find("_em1jLM_") != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (1 jet)");
    else if (cardName.find("_01j_")    != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0/1 jets)");
    else if (cardName.find("_SF_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu");
    else if (cardName.find("_SF0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0 jets)");
    else if (cardName.find("_SF1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (1 jet)");
    else if (cardName.find("_SF01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0/1 jets)");
    else if (cardName.find("_SF2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (2 jets)");
    else if (cardName.find("_OF_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu");
    else if (cardName.find("_OF0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0 jets)");
    else if (cardName.find("_OF1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (1 jet)");
    else if (cardName.find("_OF01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0/1 jets)");
    else if (cardName.find("_OF2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (2 jets)");
    else if (cardName.find("_cuts_") != string::npos) {
      t.DrawLatex(labelPosX + 0.4, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "m_{T} Cut");
    }
    else if (cardName.find("_lpt_") != string::npos) {
      t.DrawLatex(labelPosX + 0.4, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "Low pT alone");
    }
    else if (cardName.find("_hpt_") != string::npos) {
      t.DrawLatex(labelPosX + 0.4, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "High pT alone");
      // t.DrawLatex(0.67, 0.2, "MC11b");
    }
    else t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
  }
  else if (dolvqq) {
    lumi      = "1.04";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    minMass    = 240;
    maxMass    = min(maxMass, 600);

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW#rightarrowl#nuqq");
  }
  else if (dolh) {
    lumi       = "1.063";

    xmin_leg   = 0.19;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{h}");
  }
  else if (doll) {
    lumi       = "1.063";

    xmin_leg   = 0.35;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = min(maxMass, 600);

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{l}+1j");
  }
  else if (dott) {
    lumi       = "4.7";

    xmin_leg   = 0.35;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = min(maxMass, 600);

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau#tau");
  }
  else if (dovh) {
    lumi       = "4.7";

    xmin_leg   = 0.35;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.4;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.08;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = min(maxMass, 600);

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "VH, H#rightarrowbb");
  }
  else if (dozz) {
    lumi       = "4.7-4.9";

    xmin_leg   = 0.16;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.42;

    txtPosX    = xmin_leg+0.44;
    txtPosY    = ymax_leg-0.15;

    markerSize = 0.8;

    if (!overrideMass) {
      minMass  = 110;
      maxMass  = 600;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ");
  }
  else if (doww) {

    lumi = "4.7";
    if (cardName.find("_BK_") != string::npos) {
      lumi = "2.1";
    }
    else if (cardName.find("_LM_") != string::npos) {
      lumi = "2.6";
    }

    xmin_leg  = 0.16;
    xdiff_leg = 0.22;
    ymax_leg  = 0.42;

    txtPosX   = xmin_leg+0.44;
    txtPosY   = ymax_leg-0.15;

    if (overlay != "") {
      ydiff_leg += 0.05;
    }

    if (overlay2 != "") {
      ydiff_leg += 0.05;
    }

    if (overlay3 != "") {
      ydiff_leg += 0.05;
    }

    markerSize = 0.8;

    if (dozoom) {
      maxMass = 180;
    }

    minMass = 110;
    maxMass = min(maxMass, 600);

    if (cardName.find("paper") != string::npos) {
      lumi     = "2.1";
      xmin_leg = 0.5 ;
      ymax_leg = 0.565;
      txtPosX  = 0.5;
    }

    if (cardName.find("_lpt_") != string::npos) {
      maxMass = 190;
    }

    drawPlot_p02(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow WW");
  }
  else if (dott) {
  }
  else if (docb) {
    if (overlay != "") {
      if (overlayExp) {
        ydiff_leg += 0.06;
      }
      else {
        ydiff_leg += 0.025;
      }
    }

    if (overlay2 != "") {
      ydiff_leg += 0.06;
    }
    
    if (overlay3 != "") {
      ydiff_leg += 0.06;
    }

    if (doProgressive && !doInj) {
      // xmax_leg = 0.1814;
      ymax_leg  = 0.522;
      ydiff_leg = ymax_leg - 0.214;

      txtPosX = 0.61;
      txtPosY = 0.396;
    }

    if (doInj) {
      xmin_leg  = 0.168;
      xdiff_leg = 0.22;
      ymax_leg  = 0.42;

      txtPosX   = 0.60;
      txtPosY   = 0.269;
    }

    markerSize = 0.8;

    minMass = 110;
    maxMass = min(maxMass, 600);

    if (overlay == "") {
      maxLimit = 5000;
    }

    drawPlot_p02(cardName);

    drawTopRight(year+" Data");
  }
  else {
  }

  string saveName = cardName+"_p0";
  
  if (overlay != "") {
    saveName += "_comp";
  }
  
  if (dozoom) {
    saveName += "_zoom";
  }

  save(saveName, "eps", c1);
  save(saveName, "pdf", c1);
  save(saveName, "C", c1);
}


void drawPlot_p02(string cardName) {
  cout << "Drawing plot: " << cardName << endl;

  // see if file exists
  ifstream testFile(("ascii/"+cardName+".txt").c_str());
  if (testFile.fail() || doreread) {
    saveAscii(cardName);
  }

  fileHolder numbers;
  drawPlot("ascii/"+cardName+".txt", 2, numbers);

  int nrPoints = numbers.massPoints.size();
  
  if (nrPoints == 0) {
    // maybe ascii needs to be rewritten
    saveAscii(cardName);
    drawPlot("ascii/"+cardName+".txt", 2, numbers);
  }

  nrPoints = numbers.massPoints.size();

  fileHolder olFile;
  int nrOlPoints;
  double* olPoints;
  double* olObs;
  double* olExp;

  if (overlay != "") {
    string overlay_base = parseString(overlay,":")[0];
    if (doreread) {
      saveAscii(overlay_base);
    }
    drawPlot("ascii/"+overlay_base+".txt", 2, olFile);
    nrOlPoints = olFile.massPoints.size();

    if (nrOlPoints == 0) {
      saveAscii(overlay_base);
      drawPlot("ascii/"+overlay_base+".txt", 2, olFile);
      nrOlPoints = olFile.massPoints.size();
    }

    olPoints = getAry(olFile.massPoints);
    olObs    = getAry(olFile.getCol(0));
    olExp    = getAry(olFile.getCol(1));
  }

  fileHolder olFile2;
  int nrOlPoints2;
  double* olPoints2;
  double* olObs2;
  double* olExp2;

  if (overlay2 != "") {
    string overlay_base2 = parseString(overlay2,":")[0];
    if (doreread) {
      saveAscii(overlay_base2);
    }
    drawPlot("ascii/"+overlay_base2+".txt", 2, olFile2);
    nrOlPoints2 = olFile2.massPoints.size();

    if (nrOlPoints2 == 0) {
      saveAscii(overlay_base2);
      drawPlot("ascii/"+overlay_base2+".txt", 2, olFile2);
      nrOlPoints2 = olFile2.massPoints.size();
    }

    olPoints2 = getAry(olFile2.massPoints);
    olObs2    = getAry(olFile2.getCol(0));
    olExp2    = getAry(olFile2.getCol(1));
  }

  fileHolder olFile3;
  int nrOlPoints3;
  double* olPoints3;
  double* olObs3;
  double* olExp3;
  if (overlay3 != "") {
    string overlay_base3 = parseString(overlay3,":")[0];
    if (doreread) {
      saveAscii(overlay_base3);
    }
    drawPlot("ascii/"+overlay_base3+".txt", 2, olFile3);
    nrOlPoints3 = olFile3.massPoints.size();

    if (nrOlPoints3 == 0) {
      saveAscii(overlay_base3);
      drawPlot("ascii/"+overlay_base3+".txt", 2, olFile3);
      nrOlPoints3 = olFile3.massPoints.size();
    }

    olPoints3 = getAry(olFile3.massPoints);
    olObs3    = getAry(olFile3.getCol(0));
    olExp3    = getAry(olFile3.getCol(1));
  }

  double* massPoints = getAry(numbers.massPoints);
  double* obs = getAry(numbers.getCol(0));
  double* exp = getAry(numbers.getCol(1));
  double* p2s = new double[nrPoints];
  double* p1s = new double[nrPoints];
  double* n1s = new double[nrPoints];
  double* n2s = new double[nrPoints];

  for (int i=0;i<nrPoints;i++) {
    // obs[i] = 1-ROOT::Math::gaussian_cdf(obs[i]);
    exp[i] = ROOT::Math::gaussian_quantile(1 - exp[i], 1);
    p2s[i] = 1-ROOT::Math::gaussian_cdf(exp[i]+2);
    p1s[i] = 1-ROOT::Math::gaussian_cdf(exp[i]+1);
    if (!uncap) {
      n1s[i] = min(0.5, 1-ROOT::Math::gaussian_cdf(exp[i]-1));
      n2s[i] = min(0.5, 1-ROOT::Math::gaussian_cdf(exp[i]-2));
      obs[i] = min(0.5, obs[i]);
      if (overlay  != "") olObs[i]  = min(0.5, olObs[i]);
      if (overlay2 != "") olObs2[i] = min(0.5, olObs2[i]);
      if (overlay3 != "") olObs3[i] = min(0.5, olObs3[i]);
    }
    else {
      n1s[i] = 1-ROOT::Math::gaussian_cdf(exp[i]-1);
      n2s[i] = 1-ROOT::Math::gaussian_cdf(exp[i]-2);
    }

    exp[i] = 1-ROOT::Math::gaussian_cdf(exp[i]);

    p2s[i] = p2s[i] - exp[i];
    p1s[i] = p1s[i] - exp[i];
    n1s[i] = exp[i] - n1s[i];
    n2s[i] = exp[i] - n2s[i];

    minLimit = min(minLimit, obs[i]);
  }

  minLimit = 0.1*min(minLimit, (1-ROOT::Math::gaussian_cdf(6)));
  minLimit = max(minLimit, pow(10, -3));
  minLimit *= 0.01;
  if (overlay != "") minLimit *= 10;

  int c_2s = kYellow;
  int c_1s = kGreen;

  maxLimit = 1000;
  minLimit = 0.000001;

  string ytitle = "Local p_{0}";
  TGraphAsymmErrors* g_2s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n2s, p2s);
  g_2s->SetMinimum(minLimit);
  g_2s->SetMaximum(maxLimit);
  g_2s->SetFillColor(c_2s);
  g_2s->GetYaxis()->SetTitleOffset(1.2);
  if (doLogx) {
    g_2s->GetXaxis()->SetMoreLogLabels(1);
    g_2s->GetXaxis()->SetNoExponent(1);
    g_2s->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
  }
  else {
    g_2s->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
  }
  if (drawBands) g_2s->Draw("al3");

  TGraphAsymmErrors* g_1s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n1s, p1s);
  g_1s->SetFillColor(c_1s);
  g_1s->SetMinimum(minLimit);
  g_1s->SetMaximum(maxLimit);
  if (drawBands) g_1s->Draw("l3");

  if (overlay != "") {
    TGraph* g_ol_obs = makeGraph(ytitle, nrOlPoints, olPoints, olObs);
    g_ol_obs->SetLineColor(kRed);
    g_ol_obs->SetMarkerColor(kRed);
    g_ol_obs->SetLineStyle(1);
    g_ol_obs->SetLineWidth(2);
    g_ol_obs->SetMinimum(minLimit);
    g_ol_obs->SetMaximum(maxLimit);
    g_ol_obs->SetMarkerSize(markerSize);
    g_ol_obs->GetYaxis()->SetTitleOffset(1.2);
    // if (drawBands) g_ol_obs->Draw("lp");
    // else g_ol_obs->Draw("alp");
    if (drawObs && overlayObs) {
      if (doLogx) {
        g_ol_obs->GetXaxis()->SetMoreLogLabels(1);
        g_ol_obs->GetXaxis()->SetNoExponent(1);
        g_ol_obs->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
      }
      else {
        g_ol_obs->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
      }
      g_ol_obs->Draw("alp");
    }

    if (overlayExp) {
      TGraph* g_ol_exp = makeGraph(ytitle, nrOlPoints, olPoints, olExp);
      g_ol_exp->SetLineColor(kRed);
      g_ol_exp->SetMarkerColor(kRed);
      g_ol_exp->SetMinimum(minLimit);
      g_ol_exp->SetMaximum(maxLimit);
      g_ol_exp->SetLineStyle(2);
      g_ol_exp->SetLineWidth(2);
      if ((drawObs || drawBands) && overlayObs) {
        g_ol_exp->Draw("l");
      }
      else g_ol_exp->Draw("al");
    }
  }

  if (overlay2 != "") {
    TGraph* g_ol_obs2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olObs2);
    g_ol_obs2->SetLineColor(kBlue);
    g_ol_obs2->SetMarkerColor(kBlue);
    g_ol_obs2->SetLineStyle(1);
    g_ol_obs2->SetLineWidth(2);
    g_ol_obs2->SetMinimum(minLimit);
    g_ol_obs2->SetMaximum(maxLimit);
    g_ol_obs2->SetMarkerSize(markerSize);
    g_ol_obs2->GetYaxis()->SetTitleOffset(1.2);

    if (drawObs && overlayObs) {
      g_ol_obs2->Draw("lp");
    }

    TGraph* g_ol_exp2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olExp2);
    g_ol_exp2->SetLineColor(kBlue);
    g_ol_exp2->SetMarkerColor(kBlue);
    g_ol_exp2->SetLineStyle(2);
    g_ol_exp2->SetLineWidth(2);
    g_ol_exp2->Draw("l");
  }

  if (overlay3 != "") {
    TGraph* g_ol_obs3 = makeGraph(ytitle, nrOlPoints3, olPoints3, olObs3);
    g_ol_obs3->SetLineColor(kGreen);
    g_ol_obs3->SetMarkerColor(kGreen);
    g_ol_obs3->SetLineStyle(1);
    g_ol_obs3->SetLineWidth(2);
    g_ol_obs3->SetMinimum(minLimit);
    g_ol_obs3->SetMaximum(maxLimit);
    g_ol_obs3->SetMarkerSize(markerSize);
    g_ol_obs3->GetYaxis()->SetTitleOffset(1.2);

    if (drawObs && overlayObs) {
      g_ol_obs3->Draw("lp");
    }

    TGraph* g_ol_exp3 = makeGraph(ytitle, nrOlPoints3, olPoints3, olExp3);
    g_ol_exp3->SetLineColor(kGreen);
    g_ol_exp3->SetMarkerColor(kGreen);
    g_ol_exp3->SetLineStyle(2);
    g_ol_exp3->SetLineWidth(2);
    g_ol_exp3->Draw("l");
  }

  TGraph* g_obs = makeGraph(ytitle, nrPoints, massPoints, obs);
  g_obs->SetLineStyle(1);
  g_obs->SetLineWidth(2);
  g_obs->SetMinimum(minLimit);
  g_obs->SetMaximum(maxLimit);
  g_obs->SetMarkerSize(markerSize);
  g_obs->GetYaxis()->SetTitleOffset(1.2);

  if (doLogx) {
    g_obs->GetXaxis()->SetMoreLogLabels(1);
    g_obs->GetXaxis()->SetNoExponent(1);
    g_obs->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
  }
  else {
    g_obs->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
  }
  // g_obs->Draw("alp");

  if (drawObs) {
    if (drawBands || (overlay != "" && (overlayObs || overlayExp))) {
      g_obs->Draw("lp");
    }
    else {
      g_obs->Draw("alp");
    }
  }

  TGraph* g_exp = makeGraph(ytitle, nrPoints, massPoints, exp);
  g_exp->SetLineStyle(2);
  g_exp->SetLineWidth(2);
  g_exp->SetMinimum(minLimit);
  g_exp->SetMaximum(maxLimit);
  
  if (drawObs || drawBands || overlay != "") {
    g_exp->Draw("l");
  }
  else {
    g_exp->Draw("al");
  }

  TLegend* leg = makeLeg();
  TLegend* leg2 = makeLeg2();

  if (doProgressive) {
    if (overlay != "") {
      // leg->AddEntry(g_ol_obs, "Observed old llvv", "lp");
      // leg->AddEntry(g_ol_exp, "Expected old llvv", "lp");
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs, "Obs. #gamma#gamma+4l", "lp");
      }
      leg->AddEntry(g_ol_exp, "Exp. #gamma#gamma+4l", "l");
    }

    if (overlay2 != "") {
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs2, "Obs. #gamma#gamma+4l+l#nul#nu", "lp");
      }
      leg->AddEntry(g_ol_exp2, "Exp. #gamma#gamma+4l+l#nul#nu", "l");
    }

    if (overlay3 != "") {
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs3, "Observed lvlv+tt+vh", "lp");
      }
      leg->AddEntry(g_ol_exp3, "Expected lvlv+tt+vh", "l");
    }
  }

  if (drawObs) {
    leg->AddEntry(g_obs, (string("Obs. ")+cardOpts).c_str(),"lp");
  }

  if (!doInj) {
    leg->AddEntry(g_exp, (string("Exp. ")+cardOpts).c_str(),"l");
  }
  else {
    vector<string> parsed = parseString(cardName,"_");
    string injPart = "";
    for (int i=0;i<(int)parsed.size();i++) {
      if (parsed[i].find("inj") != string::npos) {
        injPart = parsed[i].substr(parsed[i].find("inj")+string("inj").size(),parsed[i].size());
      }
    }

    if (injPart == "") injPart = "125";

    leg->AddEntry(g_exp, (string("Exp. ")+cardOpts + " m_{H} = " + injPart + " GeV").c_str(),"l");
  }

  if (drawBands) {
    leg2->AddEntry(g_1s, "#pm1 #sigma","f");
    leg2->AddEntry(g_2s, "#pm2 #sigma","f");
  }

  if (!doProgressive) {
    if (overlay != "") {
      string suf = "Conf";
      applyOverlay(overlay, suf);
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs, (string("Obs. ")+suf).c_str(), "lp");
      }

      if (overlayExp) {
        leg->AddEntry(g_ol_exp, (string("Exp. ")+suf).c_str(), "l");
      }
    }

    if (overlay2 != "") {
      string suf = "Conf";
      applyOverlay(overlay2, suf);
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs2, (string("Obs. ")+suf).c_str(), "lp");
      }
      leg->AddEntry(g_ol_exp2, (string("Exp. ")+suf).c_str(), "l");
    }

    if (overlay3 != "") {
      string suf = "Conf";
      applyOverlay(overlay3, suf);
      if (drawObs && overlayObs) {
        leg->AddEntry(g_ol_obs3, (string("Obs. ")+suf).c_str(), "lp");
      }
      leg->AddEntry(g_ol_exp3, (string("Exp. ")+suf).c_str(), "l");
    }
  }

  leg->Draw();
  leg2->Draw();
  drawInsert();

  TLine l;
  l.SetLineWidth(2);
  l.SetLineStyle(2);
  l.SetLineColor(kRed);
  double extra = 10;
  double lposx_lo = g_obs->GetXaxis()->GetXmin();
  double lposx_hi = g_obs->GetXaxis()->GetXmax();
  if (doLogx) {
    lposx_lo=massPoints[0]-10;
    lposx_hi=massPoints[nrPoints-1]+20;
  }
  else {
    lposx_lo=massPoints[0];
    lposx_hi=massPoints[nrPoints-1];
  }
  if (uncap) {
    l.SetLineColor(kBlack);
    l.DrawLine(lposx_lo,1,lposx_hi,1);

    l.SetLineColor(kRed);
    l.DrawLine(lposx_lo,1-ROOT::Math::gaussian_cdf(0),lposx_hi,1-ROOT::Math::gaussian_cdf(0));
  }

  TLatex t;
  double delta = (lposx_hi-lposx_lo)*0.02;
  if (uncap) {
    t.DrawLatex(lposx_hi+delta, (1-ROOT::Math::gaussian_cdf(0))*0.8, "#color[2]{0#sigma}");
  }

  int n = 1;
  while (minLimit < 1-ROOT::Math::gaussian_cdf(n)) {
    l.DrawLine(lposx_lo,1-ROOT::Math::gaussian_cdf(n),lposx_hi,1-ROOT::Math::gaussian_cdf(n));
    stringstream str;
    str << "#color[2]{" << n << "#sigma}";
    t.DrawLatex(lposx_hi+delta, (1-ROOT::Math::gaussian_cdf(n))*0.4, str.str().c_str());
    // cout << n << endl;
    n++;
  }
  c1->SetLogy(1);
}
