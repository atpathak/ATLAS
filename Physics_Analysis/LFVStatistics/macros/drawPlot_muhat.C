#include "macros/drawPlot.C"

#include "TLatex.h"
#include "TLine.h"
#include "TCanvas.h"

double plot_min = -0.5;
double plot_max = -1;

bool overlayBands = 1;
bool doCoarse     = 0;
bool doreread     = 0;
string cardOpts   = "";

void drawPlot_muhat2(string cardName);

void drawPlot_muhat(string cardName, bool rereadAscii = 0, bool showZoom = 0, string overlayCard = "", string overlayCard2 = "", string overlayCard3 = "") {
  vector<string> parsed = parseString(cardName, ":");
  if (parsed.size() > 1) {
    cardOpts = parsed[1];
  }
  cardName = parsed[0]+"_mu";
  applyOverlay(overlayCard , overlay , "_mu");
  applyOverlay(overlayCard2, overlay2, "_mu");
  applyOverlay(overlayCard3, overlay3, "_mu");

  if (cardName.find("inj") != string::npos) {
    doInj = 1;
  }

  dozoom = showZoom;
  computeFlags(cardName);
  doreread = rereadAscii;

  if (dozoom) {
    overrideMass = 1;
    maxMass      = 150;
  }

  bool truncate = 0;

  if (truncate) {
    plot_max = 3;
    plot_min = 0;
  }

  if (!dozoom) {
    plot_max = 2.5;
  }
  else {
    plot_max = 2.5;
  }

  showLabel = 1;
  // labelTxt = "Private";

  TCanvas* c1 = new TCanvas("c1","c1",1024,768);
  if (dogg_4l) {
    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    plot_max   = 4;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, channel_label.c_str());
  }
  else if (dogg) {
    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    plot_max   = 4;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#gamma#gamma");
  }
  else if (dollll) {
    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 600;

    if (dozoom) {
      maxMass = min(maxMass, 150);
    }

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllll");
  }
  else if (dollqq) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.86;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.28;
    txtPosY    = ymax_leg - 0.12;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = 600;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllqq");
  }
  else if (dollvv) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.28;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.88;
    ydiff_leg  = 0.13;

    txtPosX    = 0.56;
    txtPosY    = 0.72;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = 600;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowll#nu#nu");
  }
  else if (dolvlv) {

    labelPosX = 0.35;

    if (cardName.find("_BK_") != string::npos) {
      lumi = "2.3";
    }
    else if (cardName.find("_LM_") != string::npos) {
      lumi = "2.4";
    }

    if (overlay != "") {
      ymax_leg -= 0.01;
      ydiff_leg += 0.05;
    }

    if (overlay2 != "") {
      ydiff_leg += 0.05;
    }

    if (overlay3 != "") {
      ydiff_leg += 0.05;
    }

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 200;

    if (cardName.find("_lpt_") != string::npos) {
      maxMass = 200;
    }

    if (dozoom) {
      minMass  = 115;
      maxMass  = 150;
    }

    maxMass    = min(maxMass, 600);

    if (overlay != "") {
      plot_max = 6;
    }

    if (overlay2 != "") {
      plot_min = -2;
      plot_max = 12;
    }

    if (truncate) {
      plot_max = 8;
      plot_min = 0;
    }

    drawPlot_muhat2(cardName);

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
    }
    else t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
  }
  else if (dolvqq) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.25;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = 0.553;
    txtPosY    = 0.745;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = 600;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW#rightarrowl#nuqq");
  }
  else if (dolh) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "1.063";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.2;
    txtPosY    = ymax_leg - 0.065;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = 600;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{h}");
  }
  else if (doll) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "1.063";

    xmin_leg   = 0.35;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.45;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.2;
    txtPosY    = ymax_leg - 0.065;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 140;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau_{l}#tau_{l}+1j");
  }
  else if (dozz) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7-4.9";

    xmin_leg   = 0.25;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.29;
    txtPosY    = ymax_leg - 0.13;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 600;

    if (dozoom) {
      maxMass   = min(maxMass, 150);

      xmin_leg  = 0.25;
      xdiff_leg = 0.22;
      ymax_leg  = 0.85;
      ydiff_leg = 0.13;

      txtPosX   = xmin_leg + 0.29;
      txtPosY   = ymax_leg - 0.13;

      plot_max  = 8;
    }
    else {
      plot_min = -1.8;
      plot_max = 4;
    }

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    if (dozoom) {
      t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ^{(*)}#rightarrowllll");
    }
    else {
      t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ^{(*)}");
    }
  }
  else if (doww) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.25;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg+0.29;
    txtPosY    = ymax_leg-0.13;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 600;

    if (dozoom) {
      maxMass   = min(maxMass, 150);
      doCoarse  = 1;
      xmin_leg  = 0.25;
      xdiff_leg = 0.22;
      ymax_leg  = 0.85;
      ydiff_leg = 0.13;

      txtPosX   = xmin_leg + 0.29;
      txtPosY   = ymax_leg - 0.13;

      plot_max  = 8;
    }
    else {
      plot_max = 3.2;
      plot_min = -2;
    }

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    if (dozoom) t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
    else t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW^{(*)}");
  }
  else if (dott) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    plot_max   = 4;

    lumi       = "4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.12;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;
    doCoarse   = 1;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau#tau");
  }
  else if (dovh) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;
    doCoarse   = 1;
    plot_max   = 5;

    lumi       = "4.6-4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.85;
    ydiff_leg  = 0.13;

    txtPosX    = xmin_leg + 0.4;
    txtPosY    = ymax_leg - 0.12;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 130;

    drawPlot_muhat2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowbb");
  }
  else if (docb) {
    if (dozoom) {
      if (truncate) {
        xmin_leg  = 0.2;
        xdiff_leg = 0.22;
        ymax_leg  = 0.78;
        ydiff_leg = 0.13;

        txtPosX   = xmin_leg + 0.4;
        txtPosY   = ymax_leg - 0.065;
      }
    }

    markerSize = 0.8;

    minMass = max(minMass, 110);
    maxMass = min(maxMass, 600);

    if (overlay != "") {
      ydiff_leg += 0.05;
    }

    drawPlot_muhat2(cardName);

    drawTopRight(year+" Data");
  }
  else {

  }
  
  string saveName = cardName;
  
  if (overlay != "") {
    saveName+="_comp";
  }

  if (dozoom) {
    saveName += "_zoom";
  }
  
  save(saveName, "eps", c1);
  save(saveName, "pdf", c1);
  save(saveName, "C", c1);
}

void drawPlot_muhat2(string cardName) {
  cout << "Drawing plot: " << cardName << endl;

  // see if file exists
  if (doreread) {
    saveAscii(cardName);
  }

  ifstream testFile(("ascii/"+cardName+".txt").c_str());
  if (testFile.fail()) {
    saveAscii(cardName);
  }

  fileHolder numbers;
  drawPlot("ascii/"+cardName+".txt", 3, numbers);

  int nrPoints = numbers.massPoints.size();
  if (nrPoints == 0) {
    // maybe ascii needs to be rewritten
    saveAscii(cardName);
    drawPlot("ascii/"+cardName+".txt", 3, numbers);
  }
  nrPoints = numbers.massPoints.size();

  fileHolder olFile;
  int nrOlPoints = 0;
  double* olPoints;
  double* olObs;
  double* olP1s;
  double* olN1s;
  if (overlay != "") {
    string overlay_base = parseString(overlay,":")[0];
    if (doreread) {
      saveAscii(overlay_base);
    }
    drawPlot("ascii/"+overlay_base+".txt", 3, olFile);
    nrOlPoints = olFile.massPoints.size();

    if (nrOlPoints == 0) {
      drawPlot("ascii/"+overlay_base+".txt", 3, olFile);
      saveAscii(overlay_base);
      nrOlPoints = olFile.massPoints.size();
    }

    olPoints = getAry(olFile.massPoints);
    olObs = getAry(olFile.getCol(0));
    olP1s = getAry(olFile.getCol(1));
    olN1s = getAry(olFile.getCol(2));

    for (int i=0;i<nrOlPoints;i++) {
      olP1s[i] = olObs[i]+olP1s[i];
      olN1s[i] = olObs[i]+olN1s[i];
    }
  }

  fileHolder olFile2;
  int nrOlPoints2 = 0;
  double* olPoints2;
  double* olObs2;
  double* olP1s2;
  double* olN1s2;
  if (overlay2 != "") {
    string overlay_base = parseString(overlay2,":")[0];
    if (doreread) {
      saveAscii(overlay_base);
    }
    drawPlot("ascii/"+overlay_base+".txt", 3, olFile2);
    nrOlPoints2 = olFile2.massPoints.size();

    if (nrOlPoints2 == 0) {
      drawPlot("ascii/"+overlay_base+".txt", 3, olFile2);
      saveAscii(overlay_base);
      nrOlPoints2 = olFile2.massPoints.size();
    }

    olPoints2 = getAry(olFile2.massPoints);
    olObs2 = getAry(olFile2.getCol(0));
    olP1s2 = getAry(olFile2.getCol(1));
    olN1s2 = getAry(olFile2.getCol(2));

    for (int i=0;i<nrOlPoints2;i++) {
      olP1s2[i] = olObs2[i]+olP1s2[i];
      olN1s2[i] = olObs2[i]+olN1s2[i];
    }
  }

  double* massPoints = getAry(numbers.massPoints);
  double* obs = getAry(numbers.getCol(0));
  double* p1s = getAry(numbers.getCol(1));
  double* n1s = getAry(numbers.getCol(2));
  for (int i=0;i<nrPoints;i++) {
    if (!(doww || dozz)) {
      plot_max = max(plot_max, obs[i]+p1s[i]);
      plot_min = min(plot_min, obs[i]+n1s[i]);
    }
    //if (massPoints[i] < 120) {p1s[i] = 0;n1s[i] = 0;obs[i]=-50;}
    p1s[i] = p1s[i];
    n1s[i] = -n1s[i];
  }

  if (doCoarse) {
    int nrPoints2 = 0;
    for (int i=0;i<nrPoints;i++) {
      if (int(massPoints[i]) != massPoints[i] || int(massPoints[i]) % 5 != 0) continue;
      nrPoints2++;
    }
    double* massPoints2 = new double[nrPoints2];
    double* obs2 = new double[nrPoints2];
    double* p1s2 = new double[nrPoints2];
    double* n1s2 = new double[nrPoints2];
    int counter=0;
    for (int i=0;i<nrPoints;i++) {
      if (int(massPoints[i]) != massPoints[i] || int(massPoints[i]) % 5 != 0) continue;
      massPoints2[counter] = massPoints[i];
      obs2[counter] = obs[i];
      p1s2[counter] = p1s[i];
      n1s2[counter] = n1s[i];
      counter++;
    }
    nrPoints=nrPoints2;
    massPoints=massPoints2;
    obs=obs2;
    p1s=p1s2;
    n1s=n1s2;
  }

  if (!(doww || dozz)) {
    plot_min *= 1.1;
    plot_max *= 1.1;
  }

  string ytitle = "Signal strength (#mu)";

  TGraph* g_obs = makeGraph(ytitle, nrPoints, massPoints, obs);
  TGraphAsymmErrors* g_err = makeGraphErr(ytitle, nrPoints, massPoints, obs, n1s, p1s);
  if (plot_min != -1) g_err->SetMinimum(plot_min);
  if (plot_max != -1) g_err->SetMaximum(plot_max);
  //g_err->GetXaxis()->SetRangeUser(110,150);
  g_err->SetFillColor(7);
  g_err->GetXaxis()->SetMoreLogLabels(1);
  g_err->GetXaxis()->SetNoExponent(1);
  if (doLogx) {
    g_err->GetXaxis()->SetMoreLogLabels(1);
    g_err->GetXaxis()->SetNoExponent(1);
    g_err->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
  }
  else {
    g_err->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
  }
  g_err->Draw("al3");

  if (overlay != "") {
    TGraph* g_ol_obs = makeGraph(ytitle, nrOlPoints, olPoints, olObs);
    g_ol_obs->SetLineColor(kRed);
    g_ol_obs->SetMarkerColor(kRed);
    g_ol_obs->SetLineStyle(1);
    g_ol_obs->SetLineWidth(2);
    g_ol_obs->SetMarkerSize(markerSize);
    g_ol_obs->GetYaxis()->SetTitleOffset(1.2);
    g_ol_obs->Draw("lp");

    if (overlayBands) {
      TGraph* g_ol_p1s = makeGraph(ytitle, nrOlPoints, olPoints, olP1s);
      g_ol_p1s->SetLineColor(kRed);
      g_ol_p1s->SetMarkerColor(kRed);
      g_ol_p1s->SetLineStyle(10);
      g_ol_p1s->SetLineWidth(2);
      g_ol_p1s->Draw("l");

      TGraph* g_ol_n1s = makeGraph(ytitle, nrOlPoints, olPoints, olN1s);
      g_ol_n1s->SetLineColor(kRed);
      g_ol_n1s->SetMarkerColor(kRed);
      g_ol_n1s->SetLineStyle(10);
      g_ol_n1s->SetLineWidth(2);
      g_ol_n1s->Draw("l");
    }
  }

  if (overlay2 != "") {
    TGraph* g_ol_obs2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olObs2);
    g_ol_obs2->SetLineColor(kBlue);
    g_ol_obs2->SetMarkerColor(kBlue);
    g_ol_obs2->SetLineStyle(1);
    g_ol_obs2->SetLineWidth(2);
    g_ol_obs2->SetMarkerSize(markerSize);
    g_ol_obs2->GetYaxis()->SetTitleOffset(1.2);
    g_ol_obs2->Draw("lp");

    if (overlayBands) {
      TGraph* g_ol_p1s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olP1s2);
      g_ol_p1s2->SetLineColor(kBlue);
      g_ol_p1s2->SetMarkerColor(kBlue);
      g_ol_p1s2->SetLineStyle(10);
      g_ol_p1s2->SetLineWidth(2);
      g_ol_p1s2->Draw("l");

      TGraph* g_ol_n1s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olN1s2);
      g_ol_n1s2->SetLineColor(kBlue);
      g_ol_n1s2->SetMarkerColor(kBlue);
      g_ol_n1s2->SetLineStyle(10);
      g_ol_n1s2->SetLineWidth(2);
      g_ol_n1s2->Draw("l");
    }
  }

  if (doInj) {
    g_obs->Draw("l");
  }
  else {
    g_obs->Draw("lp");
  }

  g_obs->SetMarkerSize(markerSize);

  TLegend* leg = makeLeg();
  if (!doInj) {
    leg->AddEntry(g_obs, (string("Obs. best fit ")+cardOpts).c_str(),"lp");
  }
  else {
    vector<string> parsed = parseString(cardName,"_");
    string injPart = "";
    for (int i=0;i<(int)parsed.size();i++) {
      if (parsed[i].find("inj") != string::npos) {
        injPart = parsed[i].substr(parsed[i].find("inj")+string("inj").size(),parsed[i].size());
      }
    }

    if (injPart == "") {
      injPart = "125";
    }

    leg->AddEntry(g_obs, (string("Exp. best fit ")+cardOpts + " m_{H} = " + injPart + " GeV").c_str(),"l");
  }
  leg->AddEntry(g_err, (string("-2 ln #lambda(#mu) < 1 ")+cardOpts).c_str(),"f");

  if (overlay != "") {
    // string suf = "1";
    // applyOverlay(overlay, suf);
    // leg->AddEntry(g_ol_obs, (string("Best fit ")+suf).c_str(),"lp");
    // if (overlayBands) leg->AddEntry(g_ol_p1s, (string("-2 ln #lambda(#mu) < 1 ")+suf).c_str(),"l");

    string suf = "1";
    string injPart = "";
    applyOverlay(overlay, suf);
    leg->AddEntry(g_ol_obs, (string("Best fit ")+suf).c_str(),"lp");
    
    if (injPart == "") {
      injPart = "125";
    }

    // leg->AddEntry(g_ol_obs, (string("Exp. best fit ")+cardOpts + " m_{H} = " + injPart + " GeV").c_str(),"lp");

    if (overlayBands) {
      leg->AddEntry(g_ol_p1s, (string("-2 ln #lambda(#mu) < 1 ")+cardOpts).c_str(),"l");
    }
  }

  if (overlay2 != "") {
    string suf = "2";
    applyOverlay(overlay2, suf);
    leg->AddEntry(g_ol_obs2, (string("Best fit ")+suf).c_str(),"lp");
    if (overlayBands) {
      leg->AddEntry(g_ol_p1s2, (string("-2 ln #lambda(#mu) < 1 ")+suf).c_str(),"l");
    }
  }

  leg->Draw();

  TLine l;
  l.SetLineWidth(2);
  l.SetLineColor(13);
  l.SetLineStyle(2);
  if (!doLogx) {
    l.DrawLine(massPoints[0], 0, massPoints[nrPoints-1], 0);
    l.SetLineColor(kRed);
    l.DrawLine(massPoints[0], 1, massPoints[nrPoints-1], 1);
  }
  else {
    l.DrawLine(massPoints[0]-10, 0, massPoints[nrPoints-1]+20, 0);
    l.SetLineColor(kRed);
    l.DrawLine(massPoints[0]-10, 1, massPoints[nrPoints-1]+20, 1);
  }

  vector<double> masses;
  masses.push_back(305);
  masses.push_back(310);
  masses.push_back(315);
  masses.push_back(325);
  masses.push_back(330);
  masses.push_back(335);
  masses.push_back(345);
  masses.push_back(350);
  masses.push_back(360);
  masses.push_back(370);
  masses.push_back(390);
  masses.push_back(400);

  TMarker m;
  m.SetNDC(false);
  m.SetMarkerColor(kRed);
  m.SetMarkerSize(0.4);
  for (int i=0;i<(int)masses.size();i++) {
    // m.DrawMarker(masses[i], 0.0);
  }
  drawInsert();
}
