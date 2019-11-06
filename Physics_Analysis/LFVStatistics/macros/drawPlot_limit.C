#include "macros/drawPlot.C"

#include "TLatex.h"
#include "TLine.h"
#include "TCanvas.h"

bool doObs                = 1;
bool overlayBands         = 0;
bool overlayNegativeBands = 1;
bool overlayExp           = 1;
bool overlayObs           = 1;
bool doProgressive        = 0;
bool doreread             = 0;
double minLimit           = 0.5;
double maxLimit           = 30;
string cardOpts           = "";

void drawPlot_limit2(string cardName);

void drawPlot_limit(string cardName, bool rereadAscii = 0, bool plotZoom = 0, string overlayCard = "", string overlayCard2 = "", string overlayCard3 = "") {
  vector<string> parsed = parseString(cardName, ":");
  if (parsed.size() > 1) {
    cardOpts = parsed[1];
  }
  cardName = parsed[0]+"_cls";
  applyOverlay(overlayCard , overlay , "_cls");
  applyOverlay(overlayCard2, overlay2, "_cls");
  applyOverlay(overlayCard3, overlay3, "_cls");

  dozoom = plotZoom;
  computeFlags(cardName);
  doreread = rereadAscii;
  // dolvlv=0;
  // docb=0;

  if (dozoom) {
    overrideMass = 1;
    maxMass = 150;
  }

  showLabel = 1;
  // labelTxt = "Private";

  TCanvas* c1 = new TCanvas("c1","c1",1024,768);

  ymax_leg = 0.5;
  ydiff_leg = 0.2;

  if (dogg_4l) {
    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    minLimit   = 0.15;
    maxLimit   = 10;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, channel_label.c_str());
  }
  else if (dogg) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    txtPosX    = 0.55;
    txtPosY    = 0.79;;

    lumi       = "4.9";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.87;
    ydiff_leg  = 0.2;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    minLimit   = 0.5;
    maxLimit   = 20;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#gamma#gamma");
  }
  else if (dollll) {
    labelPosX = 0.25;
    labelPosY = 0.89;

    lumi      = "4.9";

    xmin_leg  = 0.25;
    xdiff_leg = 0.22;
    ymax_leg  = 0.88;
    ydiff_leg = 0.2;

    txtPosX   = xmin_leg + 0.35;
    txtPosY   = ymax_leg - 0.16;

    markerSize = 0.8;

    minMass = 110;
    maxMass = 600;

    if (dozoom) {
      labelPosX = 0.3;
      xmin_leg  = 0.35;
      maxMass   = 150;
    }

    minLimit = 0.2;
    maxLimit = 100;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllll");
  }
  else if (dollqq) {
    labelPosX = 0.2;
    labelPosY = 0.89;

    lumi      = "4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.86;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.37;
    txtPosY    = ymax_leg - 0.15;

    markerSize = 0.8;

    minMass    = 200;
    maxMass    = 600;

    minLimit   = 0.2;
    maxLimit   = 30;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowllqq");
  }
  else if (dovh) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.6-4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.86;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.37;
    txtPosY    = ymax_leg - 0.15;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 130;

    minLimit   = 0.7;
    maxLimit   = 30;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowbb");
  }
  else if (dollvv) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.88;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.37;
    txtPosY    = ymax_leg - 0.15;

    markerSize = 0.8;

    minMass    = 200;
    maxMass    = 600;

    minLimit   = 0.1;
    maxLimit   = 35;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ#rightarrowll#nu#nu");
  }
  else if (dolvlv) {
    if (cardName.find("_BK_") != string::npos) {
      lumi = "2.3";
    }
    else if (cardName.find("_LM_") != string::npos) {
      lumi = "2.4";
    }

    // labelPosX = 0.2;
    // labelPosY = 0.81;

    // xmin_leg = 0.26;
    // xdiff_leg = 0.22;
    // ymax_leg = 0.5;
    // ydiff_leg = 0.2;
    // if (overlay != "") ydiff_leg += 0.05;
    // if (overlay2 != "") ydiff_leg += 0.05;
    // if (overlay3 != "") ydiff_leg += 0.05;

    // txtPosX = xmin_leg+0.3;
    // txtPosY = ymax_leg-0.13;

    // txtPosX = 0.55;
    // txtPosY = 0.73;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 200;

    if (overlay != "") {
      maxLimit = 500;
    }

    if (dozoom) {
      minMass  = 115;
      maxMass  = 150;

      minLimit = 0.10;
      maxLimit = 150;
    }
    else {
      minLimit = 0.05;
      maxLimit = 70;
    }

    maxMass = min(maxMass, 600);

    if (overlay != "") {
      ydiff_leg += 0.03;
    }

    if (overlay2 != "") {
      ydiff_leg += 0.03;
    }

    if (overlay3 != "") {
      ydiff_leg += 0.03;
    }

    // if (cardName.find("paper") != string::npos) {
    //   lumi     = "2.1";
    //   maxLimit = 200;
    //   minLimit = 0.1;
    // }

    if (cardName.find("_lpt_") != string::npos) {
      maxMass  = 200;
      minLimit = 0.5;
      maxLimit = 200;
    }

    if (cardName.find("_2j") != string::npos) {
      minLimit = 0.4;
      maxLimit = 100;
    }

    drawPlot_limit2(cardName);

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
    else if (cardName.find("_SF2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (2 jets)");
    else if (cardName.find("_SF01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0/1 jets)");
    else if (cardName.find("_OF_")     != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu");
    else if (cardName.find("_OF0j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0 jets)");
    else if (cardName.find("_OF1j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (1 jet)");
    else if (cardName.find("_OF01j_")  != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0/1 jets)");
    else if (cardName.find("_OF2j_")   != string::npos) t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (2 jets)");
    else if (cardName.find("_cuts_")   != string::npos) {
      t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "m_{T} Cut");
    }
    else if (cardName.find("_lpt_") != string::npos) {
      t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "Low pT alone");
    }
    else if (cardName.find("_hpt_") != string::npos) {
      t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
      t.DrawLatex(0.67, 0.2, "High pT alone");
    }
    else {
      t.DrawLatex(labelPosX, labelPosY-0.065, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
    }
  }
  else if (dolvqq) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.2;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.87;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg+0.38;
    txtPosY    = ymax_leg-0.15;

    markerSize = 0.8;

    minMass    = 240;
    maxMass    = 600;

    minLimit   = 0.7;
    maxLimit   = 50;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW#rightarrowl#nuqq");
  }
  else if (dolh) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "1.063";

    xmin_leg   = 0.19;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.87;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.38;
    txtPosY    = ymax_leg - 0.065;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    minLimit = 0.7;
    maxLimit = 1000;

    drawPlot_limit2(cardName);

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
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.2;
    txtPosY    = ymax_leg - 0.065;

    markerSize = 0.8;

    minMass    = 120;
    maxMass    = 600;

    minLimit   = 0.7;
    maxLimit   = 10;

    drawPlot_limit2(cardName);

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
    ymax_leg   = 0.86;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg + 0.3;
    txtPosY    = ymax_leg - 0.13;

    markerSize = 0.8;

    minLimit   = 0.15;
    maxLimit   = 200;

    minMass    = max(minMass,110);
    maxMass    = min(maxMass,600);

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowZZ");
  }
  else if (doww) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7-4.9";

    xmin_leg   = 0.25;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.86;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg+0.3;
    txtPosY    = ymax_leg-0.065;

    markerSize = 0.8;

    minLimit   = 0.15;
    maxLimit   = 100;

    minMass    = max(minMass,110);
    maxMass    = min(maxMass,600);

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrowWW");

  }
  else if (dott) {
    labelPosX  = 0.2;
    labelPosY  = 0.89;

    lumi       = "4.7";

    xmin_leg   = 0.19;
    xdiff_leg  = 0.22;
    ymax_leg   = 0.87;
    ydiff_leg  = 0.2;

    txtPosX    = xmin_leg+0.38;
    txtPosY    = ymax_leg-0.13;

    markerSize = 0.8;

    minMass    = 110;
    maxMass    = 150;

    minLimit   = 0.7;
    maxLimit   = 50;

    drawPlot_limit2(cardName);

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX + 0.4, labelPosY, "H#rightarrow#tau#tau");
  }
  else if (docb) {
    minLimit = 0.05;
    maxLimit = 20;
    
    if (dozoom) {
      minLimit = 0.1;
      maxLimit = 10;
    }
    
    if (overlay != "")
    {
      maxLimit = 40;
    }
    
    if (do2011 || do2012) {
      maxLimit = 20;
    }

    if (overlay != "") {
      ydiff_leg += 0.05;
    }

    if (overlay2 != "") {
      ydiff_leg += 0.05;
    }

    if (doProgressive) {
      xmin_leg  = 0.525;
      ymax_leg  = 0.874;
      ydiff_leg = 0.874-0.525;

      txtPosX   = 0.193;
      txtPosY   = 0.721;
    }

    drawPlot_limit2(cardName);

    drawTopRight(year+" Data");

    TLatex t;
    t.SetNDC();
    t.DrawLatex(labelPosX, labelPosY-0.7, "#it{CLs Limits}");
  }
  else {
  }

  string saveName = cardName;
  
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

void drawPlot_limit2(string cardName) {
  cout << "Drawing plot: " << cardName << endl;

  // see if file exists
  ifstream testFile(("ascii/"+cardName+".txt").c_str());
  
  if (testFile.fail() || doreread) {
    saveAscii(cardName);
  }

  fileHolder numbers;
  drawPlot("ascii/"+cardName+".txt", 6, numbers);

  int nrPoints = numbers.massPoints.size();
  if (nrPoints == 0) {
    // maybe ascii needs to be rewritten
    saveAscii(cardName);
    drawPlot("ascii/"+cardName+".txt", 6, numbers);
  }
  nrPoints = numbers.massPoints.size();

  fileHolder olFile;
  int nrOlPoints = 0;
  double* olPoints;
  double* olObs;
  double* olExp;
  double* olP2s;
  double* olP1s;
  double* olN1s;
  double* olN2s;

  if (overlay != "") {
    string overlay_base = parseString(overlay,":")[0];
    if (doreread) {
      saveAscii(overlay_base);
    }
    drawPlot("ascii/"+overlay_base+".txt", 6, olFile);
    nrOlPoints = olFile.massPoints.size();

    if (nrOlPoints == 0) {
      drawPlot("ascii/"+overlay_base+".txt", 6, olFile);
      saveAscii(overlay_base);
      nrOlPoints = olFile.massPoints.size();
    }

    olPoints = getAry(olFile.massPoints);
    olObs    = getAry(olFile.getCol(0));
    olExp    = getAry(olFile.getCol(1));
    olP2s    = getAry(olFile.getCol(2));
    olP1s    = getAry(olFile.getCol(3));
    olN1s    = getAry(olFile.getCol(4));
    olN2s    = getAry(olFile.getCol(5));
  }

  fileHolder olFile2;
  int nrOlPoints2 = 0;
  double* olPoints2;
  double* olObs2;
  double* olExp2;
  double* olP2s2;
  double* olP1s2;
  double* olN1s2;
  double* olN2s2;

  if (overlay2 != "") {
    string overlay_base2 = parseString(overlay2,":")[0];
    if (doreread) {
      saveAscii(overlay_base2);
    }
    drawPlot("ascii/"+overlay_base2+".txt", 6, olFile2);
    nrOlPoints2 = olFile2.massPoints.size();

    if (nrOlPoints2 == 0) {
      drawPlot("ascii/"+overlay_base2+".txt", 6, olFile2);
      saveAscii(overlay_base2);
      nrOlPoints2 = olFile2.massPoints.size();
    }

    olPoints2 = getAry(olFile2.massPoints);
    olObs2    = getAry(olFile2.getCol(0));
    olExp2    = getAry(olFile2.getCol(1));
    olP2s2    = getAry(olFile2.getCol(2));
    olP1s2    = getAry(olFile2.getCol(3));
    olN1s2    = getAry(olFile2.getCol(4));
    olN2s2    = getAry(olFile2.getCol(5));
  }

  fileHolder olFile3;
  int nrOlPoints3 = 0;
  double* olPoints3;
  double* olObs3;
  double* olExp3;
  if (overlay3 != "") {
    string overlay_base3 = parseString(overlay3,":")[0];
    if (doreread) {
      saveAscii(overlay_base3);
    }
    drawPlot("ascii/"+overlay_base3+".txt", 6, olFile3);
    nrOlPoints3 = olFile3.massPoints.size();

    if (nrOlPoints3 == 0) {
      drawPlot("ascii/"+overlay_base3+".txt", 6, olFile3);
      saveAscii(overlay_base3);
      nrOlPoints3 = olFile3.massPoints.size();
    }

    olPoints3 = getAry(olFile3.massPoints);
    olObs3    = getAry(olFile3.getCol(0));
    olExp3    = getAry(olFile3.getCol(1));
  }

  vector<pair<double, double> > obs_excl;
  vector<pair<double, double> > exp_excl;

  bool inWindow = false;
  bool first = true;
  double prev_obs,prev_exp,prev_mass;
  double* massPoints = getAry(numbers.massPoints);
  double* obs        = getAry(numbers.getCol(0));
  double* exp        = getAry(numbers.getCol(1));
  double* p2s        = getAry(numbers.getCol(2));
  double* p1s        = getAry(numbers.getCol(3));
  double* n1s        = getAry(numbers.getCol(4));
  double* n2s        = getAry(numbers.getCol(5));
  
  for (int i=0;i<nrPoints;i++) {
    p2s[i] = p2s[i] - exp[i];
    p1s[i] = p1s[i] - exp[i];
    n1s[i] = exp[i] - n1s[i];
    n2s[i] = exp[i] - n2s[i];

    // find the excluded region
    if (obs[i] <= 1)
    {
      if (!inWindow) {

      }
      inWindow = true;
    }
    first = false;
  }

  int c_2s = kYellow;
  int c_1s = kGreen;

  string ytitle = "95% CL Limit on #sigma/#sigma_{SM}";

  TGraphAsymmErrors* g_2s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n2s, p2s);
  g_2s->SetMinimum(minLimit);
  g_2s->SetMaximum(maxLimit);
  g_2s->SetFillColor(c_2s);
  g_2s->GetXaxis()->SetMoreLogLabels(1);
  g_2s->GetXaxis()->SetNoExponent(1);
  if (doLogx) {
    g_2s->GetXaxis()->SetMoreLogLabels(1);
    g_2s->GetXaxis()->SetNoExponent(1);
    g_2s->GetXaxis()->SetRangeUser(massPoints[0]-10,massPoints[nrPoints-1]+20);
  }
  else {
    g_2s->GetXaxis()->SetRangeUser(massPoints[0],massPoints[nrPoints-1]);
  }
  g_2s->Draw("al3");

  TGraphAsymmErrors* g_1s = makeGraphErr(ytitle, nrPoints, massPoints, exp, n1s, p1s);
  g_1s->SetFillColor(c_1s);
  g_1s->Draw("l3");

  if (overlay != "") {
    if (overlayObs) {
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
    }

    if (overlayExp) {
      TGraph* g_ol_exp = makeGraph(ytitle, nrOlPoints, olPoints, olExp);
      g_ol_exp->SetLineColor(kRed);
      g_ol_exp->SetMarkerColor(kRed);
      g_ol_exp->SetLineStyle(2);
      g_ol_exp->SetLineWidth(2);
      g_ol_exp->Draw("l");
    }

    if (overlayBands) {
      TGraph* g_ol_p2s = makeGraph(ytitle, nrOlPoints, olPoints, olP2s);
      g_ol_p2s->SetLineColor(kRed);
      g_ol_p2s->SetMarkerColor(kRed);
      g_ol_p2s->SetLineStyle(9);
      g_ol_p2s->SetLineWidth(2);
      g_ol_p2s->Draw("l");

      TGraph* g_ol_p1s = makeGraph(ytitle, nrOlPoints, olPoints, olP1s);
      g_ol_p1s->SetLineColor(kRed);
      g_ol_p1s->SetMarkerColor(kRed);
      g_ol_p1s->SetLineStyle(10);
      g_ol_p1s->SetLineWidth(2);
      g_ol_p1s->Draw("l");

      if (overlayNegativeBands) {
        TGraph* g_ol_n1s = makeGraph(ytitle, nrOlPoints, olPoints, olN1s);
        g_ol_n1s->SetLineColor(kRed);
        g_ol_n1s->SetMarkerColor(kRed);
        g_ol_n1s->SetLineStyle(10);
        g_ol_n1s->SetLineWidth(2);
        g_ol_n1s->Draw("l");

        TGraph* g_ol_n2s = makeGraph(ytitle, nrOlPoints, olPoints, olN2s);
        g_ol_n2s->SetLineColor(kRed);
        g_ol_n2s->SetMarkerColor(kRed);
        g_ol_n2s->SetLineStyle(9);
        g_ol_n2s->SetLineWidth(2);
        g_ol_n2s->Draw("l");
      }
    }
  }

  if (overlay2 != "") {
    if (overlayObs) {
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
    }

    if (overlayExp) {
      TGraph* g_ol_exp2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olExp2);
      g_ol_exp2->SetLineColor(kBlue);
      g_ol_exp2->SetMarkerColor(kBlue);
      g_ol_exp2->SetLineStyle(2);
      g_ol_exp2->SetLineWidth(2);
      g_ol_exp2->Draw("l");
    }

    if (overlayBands) {
      TGraph* g_ol_p2s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olP2s2);
      g_ol_p2s2->SetLineColor(kBlue);
      g_ol_p2s2->SetMarkerColor(kBlue);
      g_ol_p2s2->SetLineStyle(9);
      g_ol_p2s2->SetLineWidth(2);
      g_ol_p2s2->Draw("l");

      TGraph* g_ol_p1s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olP1s2);
      g_ol_p1s2->SetLineColor(kBlue);
      g_ol_p1s2->SetMarkerColor(kBlue);
      g_ol_p1s2->SetLineStyle(10);
      g_ol_p1s2->SetLineWidth(2);
      g_ol_p1s2->Draw("l");

      if (overlayNegativeBands) {
        TGraph* g_ol_n1s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olN1s2);
        g_ol_n1s2->SetLineColor(kBlue);
        g_ol_n1s2->SetMarkerColor(kBlue);
        g_ol_n1s2->SetLineStyle(10);
        g_ol_n1s2->SetLineWidth(2);
        g_ol_n1s2->Draw("l");

        TGraph* g_ol_n2s2 = makeGraph(ytitle, nrOlPoints2, olPoints2, olN2s2);
        g_ol_n2s2->SetLineColor(kBlue);
        g_ol_n2s2->SetMarkerColor(kBlue);
        g_ol_n2s2->SetLineStyle(9);
        g_ol_n2s2->SetLineWidth(2);
        g_ol_n2s2->Draw("l");
      }
    }
  }

  if (overlay3 != "") {
    TGraph* g_ol_obs3 = makeGraph(ytitle, nrOlPoints3, olPoints3, olObs3);
    g_ol_obs3->SetLineColor(kMagenta);
    g_ol_obs3->SetMarkerColor(kMagenta);
    g_ol_obs3->SetLineStyle(1);
    g_ol_obs3->SetLineWidth(2);
    g_ol_obs3->SetMinimum(minLimit);
    g_ol_obs3->SetMaximum(maxLimit);
    g_ol_obs3->SetMarkerSize(markerSize);
    g_ol_obs3->GetYaxis()->SetTitleOffset(1.2);
    g_ol_obs3->Draw("lp");

    TGraph* g_ol_exp3 = makeGraph(ytitle, nrOlPoints3, olPoints3, olExp3);
    g_ol_exp3->SetLineColor(kMagenta);
    g_ol_exp3->SetMarkerColor(kMagenta);
    g_ol_exp3->SetLineStyle(2);
    g_ol_exp3->SetLineWidth(2);
    g_ol_exp3->Draw("l");
  }

  TGraph* g_obs = makeGraph(ytitle, nrPoints, massPoints, obs);
  g_obs->SetLineStyle(1);
  g_obs->SetLineWidth(2);
  g_obs->SetMarkerSize(markerSize);
  if (doObs) g_obs->Draw("lp");

  TGraph* g_exp = makeGraph(ytitle, nrPoints, massPoints, exp);
  g_exp->SetLineStyle(2);
  g_exp->SetLineWidth(2);
  g_exp->Draw("l");

  TLegend* leg = makeLeg();
  if (doObs) {
    leg->AddEntry(g_obs, (string("Obs. ")+cardOpts).c_str(),"lp");
  }
  leg->AddEntry(g_exp, (string("Exp. ")+cardOpts).c_str(),"l");
  leg->AddEntry(g_1s, "#pm1 #sigma","f");
  leg->AddEntry(g_2s, "#pm2 #sigma","f");
  
  if (overlay != "") {
    string suf = "Conf";
    applyOverlay(overlay, suf);
    if (overlayObs) {
      leg->AddEntry(g_ol_obs, (string("Obs. ")+suf).c_str(), "lp");
    }
    if (overlayExp) {
      leg->AddEntry(g_ol_exp, (string("Exp. ")+suf).c_str(), "l");
    }

    if (overlayBands) {
      if (overlayNegativeBands) {
        leg->AddEntry(g_ol_p2s, (string("#pm2#sigma ")+suf).c_str(), "l");
        leg->AddEntry(g_ol_p1s, (string("#pm1#sigma ")+suf).c_str(), "l");
      }
      else {
        leg->AddEntry(g_ol_p2s, (string("+2#sigma ")+suf).c_str(), "l");
        leg->AddEntry(g_ol_p1s, (string("+1#sigma ")+suf).c_str(), "l");
      }
    }
    // leg->AddEntry(g_ol_obs, "Obs. l#nul#nu+#tau#tau+bb", "lp");
    // leg->AddEntry(g_ol_exp, "Exp. l#nul#nu+#tau#tau+bb", "l");
  }
  if (overlay2 != "") {
    string suf = "Conf";
    applyOverlay(overlay2, suf);
    if (overlayObs) {
      leg->AddEntry(g_ol_obs2, (string("Obs. ")+suf).c_str(), "lp");
    }
    if (overlayExp) {
      leg->AddEntry(g_ol_exp2, (string("Exp. ")+suf).c_str(), "l");
    }

    if (overlayBands) {
      if (overlayNegativeBands) {
        leg->AddEntry(g_ol_p2s2, (string("#pm2#sigma ")+suf).c_str(), "l");
        leg->AddEntry(g_ol_p1s2, (string("#pm1#sigma ")+suf).c_str(), "l");
      }
      else {
        leg->AddEntry(g_ol_p2s2, (string("+2#sigma ")+suf).c_str(), "l");
        leg->AddEntry(g_ol_p1s2, (string("+1#sigma ")+suf).c_str(), "l");
      }
    }
  }

  if (overlay3 != "") {
    string suf = "Conf";
    applyOverlay(overlay3, suf);
    leg->AddEntry(g_ol_obs3, (string("Obs. ")+suf).c_str(), "lp");
    leg->AddEntry(g_ol_exp3, (string("Exp. ")+suf).c_str(), "l");
  }
  leg->Draw();

  TLine l;
  l.SetLineWidth(2);
  l.SetLineColor(13);
  l.SetLineStyle(2);
  l.DrawLine(massPoints[0], 1, massPoints[nrPoints-1], 1);

  drawInsert();

  c1->SetLogy(1);
}
