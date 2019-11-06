// Author      : Stefan Gadatsch
// Email       : gadatsch@nikhef.nl
// Date        : 2013-04-13
// Description : Draw pulls of nuisance parameters, rank by importance

#include "TCanvas.h"
#include "TH2F.h"
#include "TGraphAsymmErrors.h"
#include "TLatex.h"
#include "TLine.h"
#include "TGaxis.h"
#include "TSystemDirectory.h"
#include "TList.h"
#include "TSystemFile.h"
#include "TPaveText.h"

#include "macros/drawPlot.C"

#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

// global style options
bool doHorizontal          = false; // produce a horizontal plot
bool drawInset             = false; // will cover legend but show the normalisation factors which are a priori unconstrained
bool drawErrorBars         = false; // draw bars visualising the total, stat and syst uncertainty
bool drawHatchedBands      = false; // draw hatched bands around delta muhat = 0 to visualise the total, stat and syst uncertainty
bool drawParamNames        = true;  // show the nuisance parameter labels
bool drawPostfitImpactBand = true;  // && (mode != error); // draw a band with the variation of muhat due to each theta (postfit uncertainty)
bool drawPrefitImpactBand  = true;  // && (mode != error); // draw a band with the variation of muhat due to each theta (prefit uncertainty)
bool useRelativeImpact     = false; // switch to delta muhat / delta muhat tot for the top axis
int useBreakdown           = 0;     // 0 = add, 1 = sub
double scale_poi           = 1;   // zoom the impact axis
double scale_theta         = 1;   // zoom the pull axis
bool removeHbb             = false; // remove Hbb parameters from the plot
bool removeHtt             = false; // remove Htt parameters from the plot
int showTopParameters      = -1;    // -1 to show all parameters
double showHighImpact      = 0.0;   // sigma_comp / sigma_tot threshold
Color_t color_standardband = kYellow-7;
Color_t color_standardband_ol = kGreen-8;
Color_t color_totalerror   = kBlue-4;
Color_t color_staterror    = kGreen+1;
Color_t color_systerror    = kMagenta-4;
Color_t color_pulls        = kGray+2;
Color_t color_prefit       = kBlue-4;
Color_t color_prefit_ol    = kOrange+1;
Color_t color_postfit      = kRed-4;
Color_t color_postfit_ol   = kMagenta+3;
bool rankNuis              = true; // sort the nuisance parameters by impact on POI
Int_t style_postfit        = 3353;
Int_t style_postfit_ol     = 3353;
Int_t style_prefit         = 3335;
Int_t style_prefit_ol      = 3335;

void drawPlot_pulls2(string cardName, string mass, TCanvas* c1, TPad* pad1, TPad* pad2);
void ROOT2Ascii(string folder);
void loadFile(const char* fileName, int cols, fileHolder file);
vector<string> getLabel(const char* fileName, int nrPars);
void drawPlot_pulls(bool _useRelativeImpact = 0, double use_scale_poi=1, double use_scale_theta = 1, string mass = "125", string cardName = "", bool remakeAscii = 0, string overlayCard="") {
    useRelativeImpact = _useRelativeImpact;
    scale_poi = use_scale_poi;
    scale_theta = use_scale_theta;
    cout << "useRelativeImpact = " << useRelativeImpact << " scale_poi = " << scale_poi << " scale_theta = " << scale_theta << endl;
    vector<string> parsed = parseString(cardName, ":");
    string cardOpts;
    if (parsed.size() > 1) {
        cardOpts = parsed[1];
    }
    cardName = parsed[0];
    computeFlags(cardName);

    applyOverlay(overlayCard , overlay , "");

    if (remakeAscii) {
        ROOT2Ascii(parsed[0]+"_pulls");
        ROOT2Ascii(parsed[0]+"_breakdown_add");

        if (overlay != "") {
            ROOT2Ascii(overlay+"_pulls");
            ROOT2Ascii(overlay+"_breakdown_add");
        }
    }

    showLabel = 1;

    TCanvas* c1 = new TCanvas("c1","c1",1024,1448);

    TPad *pad1 = new TPad("pad1", "pad1", 0.0  , 0.0  , 1.0 , 1.0  , 0);
    TPad *pad2 = new TPad("pad2", "pad2", 0.63, 0.1, 0.94, 0.22, 0);

    if (drawParamNames) pad1->SetLeftMargin(0.25);
    else pad1->SetLeftMargin(0.05);
    pad1->SetRightMargin(0.05);
    pad1->SetBottomMargin(0.09);
    if (drawErrorBars) pad1->SetTopMargin(0.10);
    else pad1->SetTopMargin(0.09);
    
    pad2->SetLeftMargin(0.35);
    pad2->SetRightMargin(0.01);

    pad1->Draw();
    if (drawInset) pad2->Draw();

    ydiff_leg = 0.15;

    labelPosX = 0.06;
    channelPosX = 0.28;
    channelPosY = 0.19;

    if (dolvlv) {
        markerSize = 0.8;

        minMass = 0;
        maxMass = 500;

        drawPlot_pulls2(cardName, mass, c1, pad1, pad2);

        pad1->cd();

        TLatex t;
        t.SetTextSize(0.03);
        t.SetNDC();
    
        if (cardName.find("_ee_") != string::npos)          t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu");
        else if (cardName.find("_em_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu");
        else if (cardName.find("_me_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu");
        else if (cardName.find("_mm_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu");
        else if (cardName.find("_0j_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0 jets)");
        else if (cardName.find("_1j_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (1 jet)");
        else if (cardName.find("_2j_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (2 jets)");
        else if (cardName.find("_BK_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu B-K");
        else if (cardName.find("_LM_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu L-M");
        else if (cardName.find("_ee0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0 jets)");
        else if (cardName.find("_em0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0 jets)");
        else if (cardName.find("_mm0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0 jets)");
        else if (cardName.find("_me0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0 jets)");
        else if (cardName.find("_ee1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (1 jet)");
        else if (cardName.find("_em1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (1 jet)");
        else if (cardName.find("_me1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (1 jet)");
        else if (cardName.find("_mm1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (1 jet)");
        else if (cardName.find("_ee01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (0/1 jets)");
        else if (cardName.find("_em01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_mm01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_me01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (0/1jets)");
        else if (cardName.find("_ee2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu (2 jets)");
        else if (cardName.find("_em2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu (2 jets)");
        else if (cardName.find("_me2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nue#nu (2 jets)");
        else if (cardName.find("_mm2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrow#mu#nu#mu#nu (2 jets)");
        else if (cardName.find("_em0jBK_") != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (0 jets)");
        else if (cardName.find("_em0jLM_") != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (0 jets)");
        else if (cardName.find("_em1jBK_") != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu BK (1 jet)");
        else if (cardName.find("_em1jLM_") != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu LM (1 jet)");
        else if (cardName.find("_01j_")    != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu (0/1 jets)");
        else if (cardName.find("_SF_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu");
        else if (cardName.find("_SF0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0 jets)");
        else if (cardName.find("_SF1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (1 jet)");
        else if (cardName.find("_SF01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (0/1 jets)");
        else if (cardName.find("_SF2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nue#nu/#mu#nu#mu#nu (2 jets)");
        else if (cardName.find("_OF_")     != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu");
        else if (cardName.find("_OF0j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0 jets)");
        else if (cardName.find("_OF1j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (1 jet)");
        else if (cardName.find("_OF01j_")  != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (0/1 jets)");
        else if (cardName.find("_OF2j_")   != string::npos) t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowe#nu#mu#nu/#mu#nue#nu (2 jets)");
        else if (cardName.find("_cuts_") != string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "m_{T} Cut");
        }
        else if (cardName.find("_lpt_") != string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "Low pT alone");
        }
        else if (cardName.find("_hpt_") != string::npos) {
            t.DrawLatex(channelPosX + 0.4, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
            t.DrawLatex(0.67, 0.2, "High pT alone");
        } else {
            t.DrawLatex(channelPosX, channelPosY, "H#rightarrowWW^{(*)}#rightarrowl#nul#nu");
        }
    } else {
        markerSize = 0.8;

        minMass = 0;
        maxMass = 500;

        drawPlot_pulls2(cardName, mass, c1, pad1, pad2);

        pad1->cd();

        TLatex t;
        t.SetTextSize(0.03);
        t.SetNDC();

	// t.DrawLatex(channelPosX, channelPosY, cardName.c_str());
	//        if (overlay != "") t.DrawLatex(channelPosX, channelPosY-0.025, overlay.c_str());


	TPaveText *tbox1 = new TPaveText(0.26,0.1,0.61,0.21,"BRNDC");
	tbox1->SetBorderSize(0);
	tbox1->SetFillStyle(1000);
	tbox1->SetFillColor(0);
	tbox1->SetLineColor(0);
	tbox1->SetTextSize(0.03);
	tbox1->SetTextAlign(21);
	TString tmpName=cardName;
	/*
	if (tmpName.BeginsWith("emb") || tmpName.BeginsWith("vis")) {
	  if (tmpName.EndsWith("125_0")) {
	    tmpName.ReplaceAll("_125_0","");	  
	  } else if (tmpName.EndsWith("125_1")) {
	    tmpName.ReplaceAll("_125_1"," (Asimov)");	  
	  }
	} else 
	*/
	{
	  if (tmpName.EndsWith("125_1")) {
	    tmpName.ReplaceAll("_125_1"," (ObsData)");
	  } else if  (tmpName.EndsWith("125_2")) {
	    tmpName.ReplaceAll("_125_2"," (Asimov)");
	  } else if  (tmpName.EndsWith("125_3")) {
	    tmpName.ReplaceAll("_125_3"," (Hybrid)");
	  }
	}
	//
	tbox1->AddText(.4,.6,tmpName);
	if (overlay != "") {
	  tmpName=overlay;
	  /*
	  if (tmpName.BeginsWith("emb") || tmpName.BeginsWith("vis")) {
	    if (tmpName.EndsWith("125_0")) {
	      tmpName.ReplaceAll("_125_0","");	  
	    } else if (tmpName.EndsWith("125_1")) {
	      tmpName.ReplaceAll("_125_1"," (Asimov)");	  
	    }
	  } else 
	  */
	  {
	    if (tmpName.EndsWith("125_1")) {
	      tmpName.ReplaceAll("_125_1"," (ObsData)");
	    } else if  (tmpName.EndsWith("125_2")) {
	      tmpName.ReplaceAll("_125_2"," (Asimov)");
	    } else if  (tmpName.EndsWith("125_3")) {
	      tmpName.ReplaceAll("_125_3"," (Hybrid)");
	    }
	  }
	  //
	  tbox1->AddText(.33, 0.4, Form("Alt %s",tmpName.Data()));
	}
	tbox1->Draw();
    }

    labelPosY = channelPosY-0.02;
    ATLASLabel(labelPosX,labelPosY,"",1);

    TLatex p;
    p.SetNDC();
    p.SetTextFont(42);
    p.DrawLatex(labelPosX,labelPosY-0.04,labelTxt.c_str());

    TLatex t3;
    t3.SetNDC();
    t3.SetTextSize(0.03);
    stringstream lumiLatex;
    lumiLatex << "#sqrt{s} = " << energy << " TeV: #lower[-0.2]{#scale[0.6]{#int}}Ldt = " << lumi << " fb^{-1}";
    // t3.DrawLatex(channelPosX,channelPosY-0.035,lumiLatex.str().c_str());

    TLatex t2;
    t2.SetTextSize(0.03);
    t2.SetNDC();
    t2.DrawLatex(channelPosX+.04, channelPosY-0.07, ("m_{H}="+mass+" GeV").c_str());

    stringstream saveName;
    saveName << cardName << "_pulls_" << mass;
    if (overlay != "") saveName << "_" << overlay << "_pulls_" << mass;
    save(saveName.str(), "eps", c1);
    save(saveName.str(), "pdf", c1);
    save(saveName.str(), "C", c1);
}

// ____________________________________________________________________________|__________
// The actual plotting goes on here
void drawPlot_pulls2(string cardName, string mass, TCanvas* c1, TPad* pad1, TPad* pad2) {
    cout << "INFO::Drawing pulls: " << cardName << " for mH = " << mass << " GeV";

    // load and initialize ascii files
    ifstream testFile(("ascii/"+cardName+"_pulls.txt").c_str());
    if (testFile.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_pulls.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder pulls;
    drawPlot("ascii/"+cardName+"_pulls.txt", 8, pulls);

    // overlay
    if (overlay != "") {
        ifstream testFile_ol(("ascii/"+overlay+"_pulls.txt").c_str());
        if (testFile_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_pulls.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder pulls_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_pulls.txt", 8, pulls_ol);

    // load and initialize the normalization factor ascii file
    ifstream testFile2(("ascii/"+cardName+"_pulls_nf.txt").c_str());
    if (testFile2.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_pulls_nf.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder nfs;
    drawPlot("ascii/"+cardName+"_pulls_nf.txt", 8, nfs);
    
    // overlay
    if (overlay != "") {
        ifstream testFile2_ol(("ascii/"+overlay+"_pulls_nf.txt").c_str());
        if (testFile2_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_pulls_nf.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder nfs_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_pulls_nf.txt", 8, nfs_ol);

    // load and initialize the category uncertainties
    ifstream testFile3(("ascii/"+cardName+"_breakdown_add.txt").c_str());
    if (testFile3.fail()) {
        cout << "ERROR::file " << ("ascii/"+cardName+"_breakdown_add.txt").c_str() << "does not exist.";
        exit(1);
    }
    fileHolder cats;
    drawPlot("ascii/"+cardName+"_breakdown_add.txt", 3, cats);
    
    // overlay
    if (overlay != "") {
        ifstream testFile3_ol(("ascii/"+overlay+"_breakdown_add.txt").c_str());
        if (testFile3_ol.fail()) {
            cout << "ERROR::file " << ("ascii/"+overlay+"_breakdown_add.txt").c_str() << "does not exist.";
            exit(1);
        }
    }
    fileHolder cats_ol;
    if (overlay != "") drawPlot("ascii/"+overlay+"_breakdown_add.txt", 3, cats_ol);

    // get the values from the ascii files
    int nrNuis = pulls.massPoints.size();
    int nrNFs = nfs.massPoints.size();
    int nrCats = cats.massPoints.size();

    int nrNuis_ol = pulls_ol.massPoints.size();
    int nrNFs_ol = nfs_ol.massPoints.size();
    int nrCats_ol = cats_ol.massPoints.size();
    
    vector<double> points_nuis = pulls.massPoints;
    vector<double> points_nf = nfs.massPoints;
    vector<double> points_cats = cats.massPoints;

    vector<double> points_nuis_ol = pulls_ol.massPoints;
    vector<double> points_nf_ol = nfs_ol.massPoints;
    vector<double> points_cats_ol = cats_ol.massPoints;

    for (int i = 0; i < nrNuis; i++) points_nuis[i]   = i + 0.5;
    for (int i = 0; i < nrNFs; i++) points_nf[i] = i + 0.5;

    for (int i = 0; i < nrNuis_ol; i++) points_nuis_ol[i]   = i + 0.25;
    for (int i = 0; i < nrNFs_ol; i++) points_nf_ol[i] = i + 0.25;

    if (overlay != ""){
        for (int i = 0; i < nrNuis; i++) points_nuis[i]   = i + 0.75;
        // TODO: nfs not needed at the moment
    }

    vector<double> val          = pulls.getCol(0);
    vector<double> up           = pulls.getCol(1);
    vector<double> down         = pulls.getCol(2);
    vector<double> poi_hat      = pulls.getCol(3);
    vector<double> poi_up       = pulls.getCol(4);
    vector<double> poi_down     = pulls.getCol(5);
    vector<double> poi_nom_up   = pulls.getCol(6);
    vector<double> poi_nom_down = pulls.getCol(7);

    vector<double> val_ol          = pulls_ol.getCol(0);
    vector<double> up_ol           = pulls_ol.getCol(1);
    vector<double> down_ol         = pulls_ol.getCol(2);
    vector<double> poi_hat_ol      = pulls_ol.getCol(3);
    vector<double> poi_up_ol       = pulls_ol.getCol(4);
    vector<double> poi_down_ol     = pulls_ol.getCol(5);
    vector<double> poi_nom_up_ol   = pulls_ol.getCol(6);
    vector<double> poi_nom_down_ol = pulls_ol.getCol(7);

    vector<double> nf_val          = nfs.getCol(0);
    vector<double> nf_up           = nfs.getCol(1);
    vector<double> nf_down         = nfs.getCol(2);
    vector<double> nf_poi_hat      = nfs.getCol(3);
    vector<double> nf_poi_up       = nfs.getCol(4);
    vector<double> nf_poi_down     = nfs.getCol(5);
    vector<double> nf_poi_nom_up   = nfs.getCol(6);
    vector<double> nf_poi_nom_down = nfs.getCol(7);

    vector<double> nf_val_ol          = nfs_ol.getCol(0);
    vector<double> nf_up_ol           = nfs_ol.getCol(1);
    vector<double> nf_down_ol         = nfs_ol.getCol(2);
    vector<double> nf_poi_hat_ol      = nfs_ol.getCol(3);
    vector<double> nf_poi_up_ol       = nfs_ol.getCol(4);
    vector<double> nf_poi_down_ol     = nfs_ol.getCol(5);
    vector<double> nf_poi_nom_up_ol   = nfs_ol.getCol(6);
    vector<double> nf_poi_nom_down_ol = nfs_ol.getCol(7);

    vector<double> cats_val  = cats.getCol(0);
    vector<double> cats_up   = cats.getCol(1);
    vector<double> cats_down = cats.getCol(2);

    vector<double> cats_val_ol  = cats_ol.getCol(0);
    vector<double> cats_up_ol   = cats_ol.getCol(1);
    vector<double> cats_down_ol = cats_ol.getCol(2);

    // set correct values for the poi
    for (int i = 0; i < nrNuis; i++) {
        val[i] *= scale_theta;

        poi_up[i]   = poi_up[i] - poi_hat[i];
        poi_down[i] = poi_down[i] - poi_hat[i];

        poi_nom_up[i]   = poi_nom_up[i] - poi_hat[i];
        poi_nom_down[i] = poi_nom_down[i] - poi_hat[i];
        
	//        if (poi_up[i] < 0) swap(poi_up[i], poi_down[i]);
	//        if (poi_nom_up[i] < 0) swap(poi_nom_up[i], poi_nom_down[i]);

        if (poi_up[i] < 0 && poi_down[i] > 0) {
	  poi_up[i] = 0;
	} else if (poi_up[i] > 0 && poi_down[i] < 0 ) {
	  poi_down[i] = 0;
	}

        if (poi_nom_up[i] < 0 && poi_nom_down[i] > 0) {
	  poi_nom_down[i] = 0;
	} else if (poi_nom_up[i] > 0 && poi_nom_down[i] < 0 ) {
	  poi_nom_up[i] = 0;
	}

        
        poi_up[i]   = fabs(poi_up[i]);
        poi_down[i] = fabs(poi_down[i]);

        poi_nom_up[i]   = fabs(poi_nom_up[i]);
        poi_nom_down[i] = fabs(poi_nom_down[i]);
        
        poi_hat[i] = 0;
    }

    for (int i = 0; i < nrNuis_ol; i++) {
        val_ol[i] *= scale_theta;

        poi_up_ol[i]   = poi_up_ol[i] - poi_hat_ol[i];
        poi_down_ol[i] = poi_down_ol[i] - poi_hat_ol[i];

        poi_nom_up_ol[i]   = poi_nom_up_ol[i] - poi_hat_ol[i];
        poi_nom_down_ol[i] = poi_nom_down_ol[i] - poi_hat_ol[i];
        
        if (poi_up_ol[i] < 0) swap(poi_up_ol[i], poi_down_ol[i]);
        if (poi_nom_up_ol[i] < 0) swap(poi_nom_up_ol[i], poi_nom_down_ol[i]);
        
        poi_up_ol[i]   = fabs(poi_up_ol[i]);
        poi_down_ol[i] = fabs(poi_down_ol[i]);

        poi_nom_up_ol[i]   = fabs(poi_nom_up_ol[i]);
        poi_nom_down_ol[i] = fabs(poi_nom_down_ol[i]);
        
        poi_hat_ol[i] = 0;
    }

    // find maximal error due to a single nuisance parameter
    double max_poi = 0.;
    for (int i = 0; i < nrNuis; ++i) {
        if (poi_up[i] > max_poi) max_poi = poi_up[i];
        if (poi_down[i] > max_poi) max_poi = poi_down[i];
    }

    // TODO: for overlay as well? maybe get rid of this...

    // get labels
    vector<std::string> labels;
    Int_t nlines = 0;
    ifstream idFile(("ascii/"+cardName+"_pulls_id.txt").c_str());
    while (1) {
        if (!idFile.good() || nlines > nrNuis-1) break;
        string label;
        idFile >> label;
        labels.push_back(label);
        cout << "added: " << label << endl;
        nlines++;
    }

    vector<string> labels_ol;
    if (overlay != "") {
        Int_t nlines_ol = 0;
        ifstream idFile_ol(("ascii/"+overlay+"_pulls_id.txt").c_str());
        while (1) {
            if (!idFile_ol.good() || nlines_ol > nrNuis_ol-1) break;
            string label;
            idFile_ol >> label;
            labels_ol.push_back(label);
            nlines_ol++;
        }
    }

    vector<string> nf_labels;
    Int_t nf_nlines = 0;
    ifstream idFile2(("ascii/"+cardName+"_pulls_nf_id.txt").c_str());
    while (1) {
        if (!idFile2.good() || nf_nlines > nrNFs-1) break;
        string nf_label;
        idFile2 >> nf_label;
        nf_labels.push_back(nf_label);
        nf_nlines++;
    }

    vector<string> nf_labels_ol;
    if (overlay != "") {
        Int_t nf_nlines_ol = 0;
        ifstream idFile2_ol(("ascii/"+overlay+"_pulls_nf_id.txt").c_str());
        while (1) {
            if (!idFile2_ol.good() || nf_nlines_ol > nrNFs_ol-1) break;
            string nf_label;
            idFile2_ol >> nf_label;
            nf_labels_ol.push_back(nf_label);
            nf_nlines_ol++;
        }
    }

    vector<string> cats_labels;
    Int_t cats_nlines = 0;
    ifstream idFile3(("ascii/"+cardName+"_breakdown_add_id.txt").c_str());
    while (1) {
        if (!idFile3.good() || cats_nlines > nrCats-1) break;
        string cat_label;
        idFile3 >> cat_label;
        cats_labels.push_back(cat_label);
        cats_nlines++;
    }

    vector<string> cats_labels_ol;
    if (overlay != "") {
        Int_t cats_nlines_ol = 0;
        ifstream idFile3_ol(("ascii/"+overlay+"_breakdown_add_id.txt").c_str());
        while (1) {
            if (!idFile3_ol.good() || cats_nlines_ol > nrCats_ol-1) break;
            string cat_label;
            idFile3_ol >> cat_label;
            cats_labels_ol.push_back(cat_label);
            cats_nlines_ol++;
        }
    }

    // map of category uncertainties
    map<string, vector<double> > cat_uncerts;
    for (int i = 0; i < nrCats; i++) {
        string index = cats_labels[i];
        cat_uncerts[index].push_back(cats_val[i]);
        cat_uncerts[index].push_back(cats_up[i]);
        cat_uncerts[index].push_back(cats_down[i]);
    }

    map<string, vector<double> > cat_uncerts_ol;
    if (overlay != "") {
        for (int i = 0; i < nrCats_ol; i++) {
            string index = cats_labels_ol[i];
            cat_uncerts_ol[index].push_back(cats_val_ol[i]);
            cat_uncerts_ol[index].push_back(cats_up_ol[i]);
            cat_uncerts_ol[index].push_back(cats_down_ol[i]);
        }
    }

    double sigma_tot_hi    = cat_uncerts["total"][1];
    double sigma_tot_lo    = cat_uncerts["total"][2];
    double sigma_tot_ol_hi;
    double sigma_tot_ol_lo;
    if (overlay != "") sigma_tot_ol_hi = cat_uncerts_ol["total"][1];
    if (overlay != "") sigma_tot_ol_lo = cat_uncerts_ol["total"][2];

    // hardcoded for now
    double sigma_stat_hi = 0.;
    double sigma_stat_lo = 0.;
    double sigma_syst_hi = 0.;
    double sigma_syst_lo = 0.;
    // TODO: can probably drop this

    // dump everything in maps
    map<string, vector<double> > nuis_map;
    for (int i = 0; i < nrNuis; i++) {
        string index = labels[i];
	TString tmpname = index;
	//	if (tmpname.Contains("gamma_stat_")) {
	//	  nuis_map[index].push_back(val[i]-1);
	//	} else {
	  nuis_map[index].push_back(val[i]);
	  //	}
        nuis_map[index].push_back(up[i]);
        nuis_map[index].push_back(down[i]);
        nuis_map[index].push_back(poi_hat[i]);
        nuis_map[index].push_back(poi_up[i]);
        nuis_map[index].push_back(poi_down[i]);
        nuis_map[index].push_back(poi_nom_up[i]);
        nuis_map[index].push_back(poi_nom_down[i]);
    }

    map<string, vector<double> > nuis_map_ol;
    if (overlay != "") {
        for (int i = 0; i < nrNuis_ol; i++) {
            string index = labels_ol[i];
            nuis_map_ol[index].push_back(val_ol[i]);
            nuis_map_ol[index].push_back(up_ol[i]);
            nuis_map_ol[index].push_back(down_ol[i]);
            nuis_map_ol[index].push_back(poi_hat_ol[i]);
            nuis_map_ol[index].push_back(poi_up_ol[i]);
            nuis_map_ol[index].push_back(poi_down_ol[i]);
            nuis_map_ol[index].push_back(poi_nom_up_ol[i]);
            nuis_map_ol[index].push_back(poi_nom_down_ol[i]);
        }
    }

    // check that we have in both maps the same keys
    if (overlay != "") {

        std::vector<string> tmpstring;

        int counter = nrNuis;
        int counter_ol = nrNuis_ol;

        for (int i = 0; i < nrNuis; i++) {
            bool found = 0;
            for (int ii = 0; ii < nrNuis_ol; ii++) {
                if (labels[i] == labels_ol[ii]) found = 1;
            }
            if (!found) {
                nuis_map_ol[labels[i]].push_back(-999); val_ol.push_back(-999);
                nuis_map_ol[labels[i]].push_back(0); up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); down_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_hat_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_down_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_nom_up_ol.push_back(0);
                nuis_map_ol[labels[i]].push_back(0); poi_nom_down_ol.push_back(0);
                labels_ol.push_back(labels[i]);
                points_nuis_ol.push_back(counter_ol + 0.25); counter_ol++;
            }
        }

        for (int i = 0; i < nrNuis_ol; i++) {
            bool found = 0;
            for (int ii = 0; ii < nrNuis; ii++) {
                if (labels_ol[i] == labels[ii]) found = 1;
            }
            if (!found) {
                nuis_map[labels_ol[i]].push_back(-999); val.push_back(-999);
                nuis_map[labels_ol[i]].push_back(0); up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); down.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_hat.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_down.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_nom_up.push_back(0);
                nuis_map[labels_ol[i]].push_back(0); poi_nom_down.push_back(0);
                labels.push_back(labels_ol[i]);
                points_nuis.push_back(counter + 0.75); counter++;
            }
        }
    }

    // Getting the vectors back
    nrNuis    = labels.size();
    if (overlay != "") nrNuis_ol = labels_ol.size();

    for (int i = 0; i < nrNuis-1; i++) {
        for (int j = 0; j < nrNuis-1-i; j++) {
            if (strcmp(labels[i].c_str(),labels[i+1].c_str())) {
                string tmps;
                tmps        = labels[j];
                labels[j]   = labels[j+1];
                labels[j+1] = tmps;
                if (overlay != "") labels_ol[j+1] = labels[j+1];
                if (overlay != "") labels_ol[j] = labels[j];
            }
        }
    }

    for (int i = 0; i < nrNuis; i++) {
        val[i] = nuis_map[labels[i]][0];
        up[i] = nuis_map[labels[i]][1];
        down[i] = nuis_map[labels[i]][2];
        poi_hat[i] = nuis_map[labels[i]][3];
        poi_up[i] = nuis_map[labels[i]][4];
        poi_down[i] = nuis_map[labels[i]][5];
        poi_nom_up[i] = nuis_map[labels[i]][6];
        poi_nom_down[i] = nuis_map[labels[i]][7];
	cout << labels[i] << " : poi_up = " << poi_up[i] << " poi_down = " << poi_down[i] << " poi_nom_up = " << poi_nom_up[i] << " poi_nom_down = " << poi_nom_down[i] << endl;
    }

    

    for (int i = 0; i < nrNuis_ol; i++) {
        val_ol[i] = nuis_map_ol[labels_ol[i]][0];
        up_ol[i] = nuis_map_ol[labels_ol[i]][1];
        down_ol[i] = nuis_map_ol[labels_ol[i]][2];
        poi_hat_ol[i] = nuis_map_ol[labels_ol[i]][3];
        poi_up_ol[i] = nuis_map_ol[labels_ol[i]][4];
        poi_down_ol[i] = nuis_map_ol[labels_ol[i]][5];
        poi_nom_up_ol[i] = nuis_map_ol[labels_ol[i]][6];
        poi_nom_down_ol[i] = nuis_map_ol[labels_ol[i]][7];
    }

    if (overlay != "") {
        for (int i = 0; i < nrNuis; i++) {
            cout << "Matches: " << labels[i] << " " << labels_ol[i] << endl;
        }
    }

    // sort poi values by variation size
    if (rankNuis) {
        for (int i = 0; i < nrNuis-1; i++) {
            for (int j = 0; j < nrNuis-1-i; j++) {
                if (poi_up[j]+poi_down[j] > poi_up[j+1]+poi_down[j+1]) {

                    double tmp, tmp_up, tmp_down;
                    string tmps;

                    // swap postfit poi
                    tmp_up        = poi_up[j];
                    tmp_down      = poi_down[j];
                    poi_up[j]     = poi_up[j+1];
                    poi_down[j]   = poi_down[j+1];
                    poi_up[j+1]   = tmp_up;
                    poi_down[j+1] = tmp_down;

                    if (overlay != "") {
                        tmp_up        = poi_up_ol[j];
                        tmp_down      = poi_down_ol[j];
                        poi_up_ol[j]     = poi_up_ol[j+1];
                        poi_down_ol[j]   = poi_down_ol[j+1];
                        poi_up_ol[j+1]   = tmp_up;
                        poi_down_ol[j+1] = tmp_down;
                    }

                    // swap prefit poi
                    tmp_up            = poi_nom_up[j];
                    tmp_down          = poi_nom_down[j];
                    poi_nom_up[j]     = poi_nom_up[j+1];
                    poi_nom_down[j]   = poi_nom_down[j+1];
                    poi_nom_up[j+1]   = tmp_up;
                    poi_nom_down[j+1] = tmp_down;

                    if (overlay != "") {
                        tmp_up            = poi_nom_up_ol[j];
                        tmp_down          = poi_nom_down_ol[j];
                        poi_nom_up_ol[j]     = poi_nom_up_ol[j+1];
                        poi_nom_down_ol[j]   = poi_nom_down_ol[j+1];
                        poi_nom_up_ol[j+1]   = tmp_up;
                        poi_nom_down_ol[j+1] = tmp_down;
                    }

                    // swap pulls
                    tmp_up    = up[j];
                    tmp_down  = down[j];
                    tmp       = val[j];
                    up[j]     = up[j+1];
                    down[j]   = down[j+1];
                    val[j]    = val[j+1];
                    up[j+1]   = tmp_up;
                    down[j+1] = tmp_down;
                    val[j+1]  = tmp;

                    if (overlay != "") {
                        tmp_up    = up_ol[j];
                        tmp_down  = down_ol[j];
                        tmp       = val_ol[j];
                        up_ol[j]     = up_ol[j+1];
                        down_ol[j]   = down_ol[j+1];
                        val_ol[j]    = val_ol[j+1];
                        up_ol[j+1]   = tmp_up;
                        down_ol[j+1] = tmp_down;
                        val_ol[j+1]  = tmp;
                    }

                    // swap names
                    tmps        = labels[j];
                    labels[j]   = labels[j+1];
                    labels[j+1] = tmps;

                    if (overlay != "") {
                        tmps        = labels_ol[j];
                        labels_ol[j]   = labels_ol[j+1];
                        labels_ol[j+1] = tmps;
                    }
                }
            }
        }
    }

    // make the 1 sigma boxes
    vector<double> boxup;
    vector<double> boxdown;
    vector<double> cenup;
    vector<double> cendown;

    vector<double> boxup_ol;
    vector<double> boxdown_ol;
    vector<double> cenup_ol;
    vector<double> cendown_ol;

    for (int i = 0; i < nrNuis; i++) {
        boxup.push_back(1.*scale_theta);
        boxdown.push_back(1.*scale_theta);
        double height = 0.5;
        if (overlay != "") height = 0.25;
        cenup.push_back(height);
        cendown.push_back(height);
    }

    for (int i = 0; i < nrNuis_ol; i++) {
        boxup_ol.push_back(1.*scale_theta);
        boxdown_ol.push_back(1.*scale_theta);
        cenup_ol.push_back(0.25);
        cendown_ol.push_back(0.25);
    }

    // make the 1 sigma boxes
    double* statboxup   = new double[nrNuis];
    double* statboxdown = new double[nrNuis];
    double* systboxup   = new double[nrNuis];
    double* systboxdown = new double[nrNuis];

    for (int i = 0; i < nrNuis; i++) {
        statboxup[i]   = sigma_stat_hi * scale_poi / max_poi;
        statboxdown[i] = sigma_stat_lo * scale_poi / max_poi;

        systboxup[i]   = sigma_syst_hi * scale_poi / max_poi;
        systboxdown[i] = sigma_syst_lo * scale_poi / max_poi;
    }
    // TODO: same for overlay, not really needed though

    // find boundaries for NF box
    double max = 1.;
    double min = 1.;
    for (int i = 0; i < nrNFs; ++i) {
        if (nf_val[i] - nf_down[i] < min) min = nf_val[i] - nf_down[i];
        if (nf_val[i] + nf_up[i] > max) max = nf_val[i] + nf_up[i];
    }
    // TODO: same for overlay, not really needed though

    // make the final arrays for plotting, in particular remove parameters
    int nrNuis2remove = 0;
    for (int i = 0; i < nrNuis; i++) {
        cout << "DEBUG::Checking " << labels[i]  << " up " << fabs(poi_up[i]-poi_hat[i]) << " down " <<  fabs(poi_down[i]-poi_hat[i]) << endl;

        if ((fabs(poi_down[i]) + fabs(poi_up[i])) / (sigma_tot_lo + sigma_tot_hi) < showHighImpact) {
            cout << "WARNING::Removing " << labels[i] << ". Below threshold." << endl;
            nrNuis2remove++;
        }        
    }

    if (showTopParameters != -1) nrNuis2remove = nrNuis - showTopParameters;

    labels.erase(labels.begin(), labels.begin() + nrNuis2remove);
    points_nuis.erase(points_nuis.end() - nrNuis2remove, points_nuis.end());

    if (overlay != "") {
        labels_ol.erase(labels_ol.begin(), labels_ol.begin() + nrNuis2remove);
        points_nuis_ol.erase(points_nuis_ol.end() - nrNuis2remove, points_nuis_ol.end());
    }

    val.erase(val.begin(), val.begin() + nrNuis2remove);
    down.erase(down.begin(), down.begin() + nrNuis2remove);
    up.erase(up.begin(), up.begin() + nrNuis2remove);

    if (overlay != "") {
        val_ol.erase(val_ol.begin(), val_ol.begin() + nrNuis2remove);
        down_ol.erase(down_ol.begin(), down_ol.begin() + nrNuis2remove);
        up_ol.erase(up_ol.begin(), up_ol.begin() + nrNuis2remove);
    }

    poi_hat.erase(poi_hat.begin(), poi_hat.begin() + nrNuis2remove);
    poi_down.erase(poi_down.begin(), poi_down.begin() + nrNuis2remove);
    poi_up.erase(poi_up.begin(), poi_up.begin() + nrNuis2remove);

    if (overlay != "") {
        poi_hat_ol.erase(poi_hat_ol.begin(), poi_hat_ol.begin() + nrNuis2remove);
        poi_down_ol.erase(poi_down_ol.begin(), poi_down_ol.begin() + nrNuis2remove);
        poi_up_ol.erase(poi_up_ol.begin(), poi_up_ol.begin() + nrNuis2remove);
    }

    poi_nom_down.erase(poi_nom_down.begin(), poi_nom_down.begin() + nrNuis2remove);
    poi_nom_up.erase(poi_nom_up.begin(), poi_nom_up.begin() + nrNuis2remove);

    if (overlay != "") {
        poi_nom_down_ol.erase(poi_nom_down_ol.begin(), poi_nom_down_ol.begin() + nrNuis2remove);
        poi_nom_up_ol.erase(poi_nom_up_ol.begin(), poi_nom_up_ol.begin() + nrNuis2remove);        
    }

    boxdown.erase(boxdown.begin(), boxdown.begin() + nrNuis2remove);
    boxup.erase(boxup.begin(), boxup.begin() + nrNuis2remove);
    cendown.erase(cendown.begin(), cendown.begin() + nrNuis2remove);
    cenup.erase(cenup.begin(), cenup.begin() + nrNuis2remove);

    if (overlay != "") {
        boxdown_ol.erase(boxdown_ol.begin(), boxdown_ol.begin() + nrNuis2remove);
        boxup_ol.erase(boxup_ol.begin(), boxup_ol.begin() + nrNuis2remove);
        cendown_ol.erase(cendown_ol.begin(), cendown_ol.begin() + nrNuis2remove);
        cenup_ol.erase(cenup_ol.begin(), cenup_ol.begin() + nrNuis2remove);        
    }

    nrNuis -= nrNuis2remove;
    nrNuis_ol -= nrNuis2remove;
    cout << "INFO::" << nrNuis << " " << nrNuis_ol << " nuisance paramters remaining." << endl;

    int offset = ceil(2 * nrNuis / 10); // used for space to plot the labels and legend

    for (int i = 0; i < nrNuis; i++) {
        poi_up[i] = fabs(poi_up[i]) * scale_poi / max_poi;
        poi_down[i] = fabs(poi_down[i]) * scale_poi / max_poi;

        if (overlay != "") {
            poi_up_ol[i] = fabs(poi_up_ol[i]) * scale_poi / max_poi;
            poi_down_ol[i] = fabs(poi_down_ol[i]) * scale_poi / max_poi;
        }

        poi_nom_up[i] = fabs(poi_nom_up[i]) * scale_poi / max_poi;
        poi_nom_down[i] = fabs(poi_nom_down[i]) * scale_poi / max_poi;

        if (overlay != "") {
            poi_nom_up_ol[i] = fabs(poi_nom_up_ol[i]) * scale_poi / max_poi;
            poi_nom_down_ol[i] = fabs(poi_nom_down_ol[i]) * scale_poi / max_poi;
        }

	/*
        if (useRelativeImpact) {
	  poi_up[i] /= (sigma_tot_hi / max_poi);
	  poi_down[i] /= (sigma_tot_lo /  max_poi);

            if (overlay != "") {
	      poi_up_ol[i] /= (sigma_tot_ol_hi / max_poi);
	      poi_down_ol[i] /= (sigma_tot_ol_lo / max_poi);
            }

            poi_nom_up[i] /= (sigma_tot_hi / max_poi);
            poi_nom_down[i] /= (sigma_tot_lo / max_poi);

            if (overlay != "") {
	      poi_nom_up_ol[i] /= (sigma_tot_ol_hi / max_poi);
	      poi_nom_down_ol[i] /= (sigma_tot_ol_lo / max_poi);
            }
        }
	*/
	/*
        if (useRelativeImpact) {
            poi_up[i] /= sigma_tot_hi;
            poi_down[i] /= sigma_tot_lo;

            if (overlay != "") {
                poi_up_ol[i] /= sigma_tot_ol_hi;
                poi_down_ol[i] /= sigma_tot_ol_lo;
            }

            poi_nom_up[i] /= sigma_tot_hi;
            poi_nom_down[i] /= sigma_tot_lo;

            if (overlay != "") {
                poi_nom_up_ol[i] /= sigma_tot_ol_hi;
                poi_nom_down_ol[i] /= sigma_tot_ol_lo;
            }
        }
	*/

	cout << labels[i] << " : poi_up = " << poi_up[i] << " poi_down = " << poi_down[i] << " poi_nom_up = " << poi_nom_up[i] << " poi_nom_down = " << poi_nom_down[i] << " sigma_tot_hi = " << sigma_tot_hi << " sigma_tot_lo = " << sigma_tot_lo << " max_poi = " << max_poi << endl;



        up[i] = fabs(up[i]) * scale_theta;
        down[i] = fabs(down[i]) * scale_theta;

        if (overlay != "") {
            up_ol[i] = fabs(up_ol[i]) * scale_theta;
            down_ol[i] = fabs(down_ol[i]) * scale_theta;
        }
    }

    // change to the right pad
    pad1->cd();

    // make plot of pulls for nuisance paramters
    markerSize = 1;
    TGraphAsymmErrors* gr = makeGraphErr("", nrNuis, getAry(val), getAry(points_nuis), getAry(down), getAry(up), NULL, NULL);
    gr->SetLineColor(1);
    gr->SetMarkerColor(1);
    gr->SetMarkerStyle(20);
    gr->SetLineStyle(1);
    gr->SetLineWidth(2);
    gr->SetMarkerSize(markerSize);
    gr->GetXaxis()->SetTitleOffset(1.2);

    TGraphAsymmErrors* gr_ol;
    if (overlay != "") {
        gr_ol = makeGraphErr("", nrNuis_ol, getAry(val_ol), getAry(points_nuis_ol), getAry(down_ol), getAry(up_ol), NULL, NULL);
        gr_ol->SetLineColor(1);
        gr_ol->SetMarkerColor(1);
        gr_ol->SetMarkerStyle(24);
        gr_ol->SetLineStyle(1);
        gr_ol->SetLineWidth(2);
        gr_ol->SetMarkerSize(markerSize);
        gr_ol->GetXaxis()->SetTitleOffset(1.2);
    }

    // make plot of 1 sigma boxes
    TGraphAsymmErrors* gr1s = makeGraphErr("", nrNuis, getAry(val), getAry(points_nuis), getAry(boxdown), getAry(boxup), getAry(cendown), getAry(cenup));
    gr1s->SetFillColor(color_standardband);
    gr1s->SetMarkerSize(0);
    gr1s->GetXaxis()->SetTitleOffset(1.2);

    TGraphAsymmErrors* gr1s_ol;
    if (overlay != "") {
        gr1s_ol = makeGraphErr("", nrNuis_ol, getAry(val_ol), getAry(points_nuis_ol), getAry(boxdown_ol), getAry(boxup_ol), getAry(cendown_ol), getAry(cenup_ol));
        gr1s_ol->SetFillColor(color_standardband_ol);
        gr1s_ol->SetMarkerSize(0);
        gr1s_ol->GetXaxis()->SetTitleOffset(1.2);
    }

    // make plot of normalization parameters
    TGraphAsymmErrors* gr_nf = makeGraphErr("", nrNFs, getAry(nf_val), getAry(points_nf), getAry(nf_down), getAry(nf_up), NULL, NULL);
    gr_nf->SetLineColor(1);
    gr_nf->SetMarkerColor(1);
    gr_nf->SetMarkerStyle(20);
    gr_nf->SetLineStyle(1);
    gr_nf->SetLineWidth(1);
    gr_nf->SetMarkerSize(markerSize);
    gr_nf->GetXaxis()->SetTitleOffset(1.2);

    if (overlay != "") {
        // TODO: not needed at the moment
    }

    // make plot for the POI change for postfit uncertainties
    TGraphAsymmErrors* gr_poi = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), getAry(poi_down), getAry(poi_up), getAry(cenup), getAry(cendown));
    gr_poi->SetLineColor(color_postfit);
    gr_poi->SetFillColor(color_postfit);
    gr_poi->SetFillStyle(style_postfit);
    gr_poi->SetLineWidth(1);
    gr_poi->SetMarkerSize(0);
    
    TGraphAsymmErrors* gr_poi_ol;
    if (overlay != "") {
        gr_poi_ol = makeGraphErr("", nrNuis_ol, getAry(poi_hat_ol), getAry(points_nuis_ol), getAry(poi_down_ol), getAry(poi_up_ol), getAry(cenup_ol), getAry(cendown_ol));
        gr_poi_ol->SetLineColor(color_postfit_ol);
        gr_poi_ol->SetFillColor(color_postfit_ol);
        gr_poi_ol->SetFillStyle(style_postfit_ol);
        gr_poi_ol->SetLineWidth(1);
        gr_poi_ol->SetMarkerSize(0);
    }

    // make plot for the POI change for prefit uncertainties
    TGraphAsymmErrors* gr_poi_nom = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), getAry(poi_nom_down), getAry(poi_nom_up), getAry(cenup), getAry(cendown));
    gr_poi_nom->SetLineColor(color_prefit);
    gr_poi_nom->SetFillColor(color_prefit);
    gr_poi_nom->SetFillStyle(style_prefit);
    gr_poi_nom->SetLineWidth(1);
    gr_poi_nom->SetMarkerSize(0);

    TGraphAsymmErrors* gr_poi_nom_ol;
    if (overlay != "") {
        gr_poi_nom_ol = makeGraphErr("", nrNuis_ol, getAry(poi_hat_ol), getAry(points_nuis_ol), getAry(poi_nom_down_ol), getAry(poi_nom_up_ol), getAry(cenup_ol), getAry(cendown_ol));
        gr_poi_nom_ol->SetLineColor(color_prefit_ol);
        gr_poi_nom_ol->SetFillColor(color_prefit_ol);
        gr_poi_nom_ol->SetFillStyle(style_prefit_ol);
        gr_poi_nom_ol->SetLineWidth(1);
        gr_poi_nom_ol->SetMarkerSize(0);
    }

    // make hatched area for stat error
    TGraphAsymmErrors* grl_stat = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), statboxdown, statboxup, getAry(cendown), getAry(cenup));
    grl_stat->SetLineColor(color_staterror);
    grl_stat->SetFillColor(color_staterror);
    grl_stat->SetFillStyle(3335);
    grl_stat->SetLineWidth(1);
    grl_stat->SetMarkerSize(0);
    
    if (overlay != "") {
        // TODO: not needed at the moment
    }

    // make hatched area for syst error
    TGraphAsymmErrors* grl_syst = makeGraphErr("", nrNuis, getAry(poi_hat), getAry(points_nuis), systboxdown, systboxup, getAry(cendown), getAry(cenup));
    grl_syst->SetLineColor(color_systerror);
    grl_syst->SetFillColor(color_systerror);
    grl_syst->SetFillStyle(3353);
    grl_syst->SetLineWidth(1);
    grl_syst->SetMarkerSize(0);
    
    if (overlay != "") {
        // TODO: not needed at the moment
    }

    double border_lo = -sigma_tot_lo / max_poi;
    double border_hi = sigma_tot_hi / max_poi;
    cout << "sigma_tot_lo = " << sigma_tot_lo << " sigma_tot_hi = " << sigma_tot_hi << " max_poi = " << max_poi << " border_lo = " << border_lo << " border_hi = " << border_hi << endl;
    // histogram to get the nuisance parameter labels correct
    TH2F *h = new TH2F("h", "", 1, border_lo, border_hi, nrNuis+offset, -offset, nrNuis);
    for (int i = offset; i < nrNuis+offset; i++) {
      TString tmpstr=labels[i-offset].c_str();tmpstr.ReplaceAll("alpha_","");tmpstr.ReplaceAll("ATLAS_","");tmpstr.ReplaceAll("gamma_","");h->GetYaxis()->SetBinLabel(i+1, drawParamNames?tmpstr:"");
    }
    h->LabelsOption("h");
    double labelSize = 1./nrNuis;
    h->SetLabelSize(labelSize>0.02?0.02:labelSize,"Y");
    h->SetLabelSize(0.025,"Y");
    h->GetXaxis()->SetLabelColor(kWhite);
    h->GetXaxis()->SetAxisColor(kWhite);
    h->GetYaxis()->SetLabelColor(kBlack);
    h->GetYaxis()->SetAxisColor(kBlack);
    h->GetYaxis()->SetTickLength(0.);
    h->SetStats(0);
    // h->LabelsDeflate();
    gPad->SetGrid(0,1);
    h->Draw("h");
    // TODO: order should be the same for overlay, so just do it once


    // histogram to get the normalization parameters labels correct
    //    cout << "min = " << min << " max = " << max << endl;
    TH2F *h2 = new TH2F("h2", "", 1, min-0.05, max+0.05, nrNFs, 0, nrNFs);
    for (int i = 0; i < nrNFs; i++) {
      TString tmpstr=nf_labels[i].c_str(); tmpstr.ReplaceAll("alpha_",""); tmpstr.ReplaceAll("ATLAS_",""); tmpstr.ReplaceAll("gamma_","");h2->GetYaxis()->SetBinLabel(i+1, drawParamNames?tmpstr:"");
    }
    h2->SetStats(0);
    h2->SetLabelSize(0.1, "X");
    h2->SetLabelSize(0.1, "Y");
    // TODO: not needed at the moment

    // axis for the POI correlation
    TGaxis *axis_poi = 0;
    if (useRelativeImpact) {
      //      axis_poi = new TGaxis(border_lo, nrNuis, border_hi, nrNuis, -1.1 /(max_poi * scale_poi), 1.1 / (max_poi * scale_poi), 510, "+LS");
      if ( fabs(border_lo/border_hi) < 1 ) {
	axis_poi = new TGaxis(border_lo, nrNuis, border_hi, nrNuis, -1 / scale_poi, fabs(border_hi/border_lo) / scale_poi, 510, "+LS");
      } else {
	axis_poi = new TGaxis(border_lo, nrNuis, border_hi, nrNuis, -fabs(border_lo/border_hi) / scale_poi, 1 / scale_poi, 510, "+LS");
      }
      //	axis_poi = new TGaxis(border_lo, nrNuis, border_hi, nrNuis, -1 / scale_poi, 1 / scale_poi, 510, "+LS");
    } else {
      axis_poi = new TGaxis(border_lo, nrNuis, border_hi, nrNuis, -sigma_tot_lo / scale_poi, sigma_tot_hi / scale_poi, 510, "+LS");
    }
    axis_poi->ImportAxisAttributes(h->GetXaxis());
    axis_poi->SetName("axis_poi");
    if (useRelativeImpact) axis_poi->SetTitle("#Delta#hat{#mu}/#sigma_{tot}");
    else axis_poi->SetTitle("#Delta#hat{#mu}");
    axis_poi->SetNoExponent(kTRUE);
    axis_poi->SetTitleOffset(-1.1);
    axis_poi->SetLabelOffset(-.05);
    axis_poi->SetTickSize(.02);
    axis_poi->SetLineColor(kBlack);
    axis_poi->SetLabelColor(kBlack);
    axis_poi->SetTitleColor(kBlack);
    axis_poi->SetLabelSize(0.035);
    axis_poi->SetTitleSize(0.035);

    // axis for the nuisance parameter pull
    TGaxis *axis_theta = new TGaxis(border_lo, -offset, border_hi, -offset, (-sigma_tot_lo / max_poi) / scale_theta, (sigma_tot_hi / max_poi) / scale_theta, 510, "+R");
    axis_theta->ImportAxisAttributes(h->GetXaxis());
    axis_theta->SetName("axis_theta");
    axis_theta->SetTitle("(#hat{#theta} - #theta_{0})/#Delta#theta");
    axis_theta->SetTitleOffset(1.1);
    axis_theta->SetLineColor(kBlack);
    axis_theta->SetLabelColor(kBlack);
    axis_theta->SetTitleColor(kBlack);
    axis_theta->SetLabelSize(0.035);
    axis_theta->SetTitleSize(0.035);

    // axis for the nuisance parameter labels
    TGaxis *axis_label = new TGaxis(border_lo, 0, border_lo, nrNuis, 0, nrNuis, 510, "-R");
    axis_label->SetLineColor(kBlack);
    axis_label->SetTitleColor(kWhite);
    axis_label->SetLabelSize(0);
    axis_label->SetNdivisions(nrNuis-1);

    // some line definitions
    TLine l;
    l.SetLineWidth(2);
    l.SetLineColor(color_pulls);
    l.SetLineStyle(2);
  
    TLine l_stat;
    l_stat.SetLineWidth(2);
    l_stat.SetLineColor(grl_stat->GetLineColor());
    l_stat.SetLineStyle(2);

    TLine l_syst;
    l_syst.SetLineWidth(2);
    l_syst.SetLineColor(grl_syst->GetLineColor());
    l_syst.SetLineStyle(2);

    TLine l_tot;
    l_tot.SetLineWidth(2);
    l_tot.SetLineColor(color_totalerror);
    l_tot.SetLineStyle(2);

    // draw the nuisance parameter pulls including error bands and impact on poi
    if (drawHatchedBands) {
        grl_stat->Draw("p2");
        grl_syst->Draw("p2");
    }

    gr1s->Draw("p2");
    if (overlay != "") gr1s_ol->Draw("p2");
    
    if (drawPrefitImpactBand) {
        gr_poi_nom->Draw("p2");
        if (overlay != "") gr_poi_nom_ol->Draw("p2");
    }
    if (drawPostfitImpactBand) {
        gr_poi->Draw("p2");
        if (overlay != "") gr_poi_ol->Draw("p2");
    }
    
    // draw axes
    if (drawPrefitImpactBand || drawPostfitImpactBand || drawErrorBars || drawHatchedBands) axis_poi->Draw();
    axis_theta->Draw();
    axis_label->Draw();

    // draw +-1 and 0 sigma lines for pulls
    l.DrawLine( 0.              , 0.,  0.              , nrNuis);
    l.DrawLine( 1. * scale_theta, 0.,  1. * scale_theta, nrNuis);
    l.DrawLine(-1. * scale_theta, 0., -1. * scale_theta, nrNuis);

    // draw syst and stat errors
    if (drawHatchedBands) {
        l_stat.DrawLine( sigma_stat_hi * scale_poi / max_poi, 0.,  sigma_stat_hi * scale_poi / max_poi, nrNuis);
        l_stat.DrawLine(-sigma_stat_lo * scale_poi / max_poi, 0., -sigma_stat_lo * scale_poi / max_poi, nrNuis);

        l_syst.DrawLine( sigma_syst_hi * scale_poi / max_poi, 0.,  sigma_syst_hi * scale_poi / max_poi, nrNuis);
        l_syst.DrawLine(-sigma_syst_lo * scale_poi / max_poi, 0., -sigma_syst_lo * scale_poi / max_poi, nrNuis);
    }
    
    if (drawErrorBars) {
        l_stat.SetLineStyle(1);
        l_syst.SetLineStyle(1);
        l_tot.SetLineStyle(1);

        l_stat.DrawLine(-sigma_stat_lo * scale_poi / max_poi, 1.07*nrNuis,  sigma_stat_hi * scale_poi / max_poi, 1.07*nrNuis);
        l_stat.DrawLine( sigma_stat_hi * scale_poi / max_poi, 1.06*nrNuis,  sigma_stat_hi * scale_poi / max_poi, 1.08*nrNuis);
        l_stat.DrawLine(-sigma_stat_lo * scale_poi / max_poi, 1.06*nrNuis, -sigma_stat_lo * scale_poi / max_poi, 1.08*nrNuis);

        l_syst.DrawLine(-sigma_syst_lo * scale_poi / max_poi, 1.10*nrNuis,  sigma_syst_hi * scale_poi / max_poi, 1.10*nrNuis);
        l_syst.DrawLine( sigma_syst_hi * scale_poi / max_poi, 1.09*nrNuis,  sigma_syst_hi * scale_poi / max_poi, 1.11*nrNuis);
        l_syst.DrawLine(-sigma_syst_lo * scale_poi / max_poi, 1.09*nrNuis, -sigma_syst_lo * scale_poi / max_poi, 1.11*nrNuis);
    
        l_tot.DrawLine(-sigma_tot_lo * scale_poi / max_poi, 1.13*nrNuis,  sigma_tot_hi * scale_poi / max_poi, 1.13*nrNuis);
        l_tot.DrawLine( sigma_tot_hi * scale_poi / max_poi, 1.12*nrNuis,  sigma_tot_hi * scale_poi / max_poi, 1.14*nrNuis);
        l_tot.DrawLine(-sigma_tot_lo * scale_poi / max_poi, 1.12*nrNuis, -sigma_tot_lo * scale_poi / max_poi, 1.14*nrNuis);

        TLatex t_stat;
        TLatex t_syst;
        TLatex t_tot;
    
        t_stat.SetTextSize(0.03);
        t_stat.SetTextAlign(32);
        t_stat.SetTextColor(color_staterror);
        t_stat.DrawLatex((-sigma_stat_lo-0.025) * scale_poi / max_poi, 1.07*nrNuis, "statistics");
    
        t_syst.SetTextSize(0.03);
        t_syst.SetTextAlign(32);
        t_syst.SetTextColor(color_systerror);
        t_syst.DrawLatex((-sigma_syst_lo-0.025) * scale_poi / max_poi, 1.10*nrNuis, "systematics");
    
        t_tot.SetTextSize(0.03);
        t_tot.SetTextAlign(32);
        t_tot.SetTextColor(color_totalerror);
        t_tot.DrawLatex((-sigma_tot_lo-0.025) * scale_poi / max_poi, 1.13*nrNuis, "total");
    
        t_stat.Draw();
        t_syst.Draw();
        t_tot.Draw();
    }

    gr->Draw("p");
    if (overlay != "") gr_ol->Draw("p");

    pad1->SetTicks(0, 0);

    c1->SetLogy(0);

    TLegend* leg = makeLeg();
    leg->SetFillStyle(1000);
    leg->SetX1(channelPosX+0.33);
    leg->SetY1(channelPosY-0.09);
    leg->SetX2(channelPosX+0.66);
    leg->SetY2(channelPosY+0.02);
    leg->SetTextSize(0.0225);

    leg->AddEntry(gr1s, "1 standard deviation","f");
    if (overlay != "") leg->AddEntry(gr1s_ol, "Alt 1 standard deviation","f");
    if (drawPrefitImpactBand) {
        leg->AddEntry(gr_poi_nom, "-1#sigma Prefit Impact on #hat{#mu}","f");
        if (overlay != "") leg->AddEntry(gr_poi_nom_ol, "Alt -1#sigma Prefit Impact on #hat{#mu}","f");
    }
    if (drawPostfitImpactBand) {
        leg->AddEntry(gr_poi, "+1#sigma Postfit Impact on #hat{#mu}","f");
        if (overlay != "") leg->AddEntry(gr_poi_ol, "Alt +1#sigma Postfit Impact on #hat{#mu}","f");
    }
    if (drawHatchedBands) {
        leg->AddEntry(grl_stat, "statistics","f");
        leg->AddEntry(grl_syst, "systematics","f");
    }

    leg->Draw();

    // draw the normalizations
    if (drawInset) {
        pad2->cd();

        TLine l2;
        l2.SetLineWidth(2);
        l2.SetLineColor(13);
        l2.SetLineStyle(2);

        h2->Draw();
        l2.DrawLine(1., 0., 1., nrNFs);
        gr_nf->Draw("p");
    }
}

// ____________________________________________________________________________|__________
// dump ROOT file to ascii table
void ROOT2Ascii(string folder) {
    system("mkdir -vp ascii");

    enum ConvertMode {pulls, breakdown};

    ConvertMode mode;
    if (folder.find("pulls") != string::npos) {
        mode = pulls;
    } else if (folder.find("breakdown") != string::npos) {
        mode = pulls;
    } else {
        cout << "ERROR::Something went wrong." << endl;
        exit(1);
    }

    bool ignoreInfNan = 1;

    std::vector<TString> list;

    TSystemDirectory  dire(("root-files/"+folder).c_str(), ("root-files/"+folder).c_str());

    TList *files = dire.GetListOfFiles();
    TIter next(files);
    TSystemFile *file;
    TString fname;
    while((file = (TSystemFile*)next())) {
        fname = file->GetName();
        if(file->IsDirectory()) continue;

        if (removeHbb && fname.Contains("Hbb")) continue;
        if (removeHtt && (fname.Contains("Htt") || fname.Contains("ATLAS_TAU"))) continue;

        list.push_back(fname.Data());
    }
    
    stringstream outFileName;
    outFileName << "ascii/" << folder;
    
    ofstream outFile((outFileName.str()+".txt").c_str());
    ofstream outFile_id((outFileName.str()+"_id.txt").c_str());
    ofstream outFile_nf((outFileName.str()+"_nf.txt").c_str());
    ofstream outFile_nf_id((outFileName.str()+"_nf_id.txt").c_str());

    int nrNPs = 0;
    int nrNFs = 0;

    for(vector<TString>::iterator it = list.begin(); it != list.end(); it++) {
        stringstream fileName;
        fileName << "root-files/" << folder << "/" << *it;

        bool isNorm = false;
        bool success = true;
        bool isInfNan = false;
    
        TFile* f = NULL;
        f = new TFile(fileName.str().c_str());
        if (f && !f->IsOpen()) success = false;

        if (success) cout << "O";
        else cout << "X";
        cout << " File: " << fileName.str() << endl;
        if (!success) continue;

        TString histoName = it->ReplaceAll(".root", "");
	/*
	if (TString(folder).BeginsWith("vis")) {
	histoName.ReplaceAll("_SR_2jetvbf_","_SRM_2jetvbf_");
	histoName.ReplaceAll("_TCR_2jetvbf_","_TCRM_2jetvbf_");
	cout << *it << " " << histoName << endl;
	}
	*/
        // if (histoName.Contains("atlas_nbkg_")) isNorm = true;
        // if (histoName.Contains("slope_")) isNorm = true;
        // if (histoName.Contains("p0_")) isNorm = true;
        // if (histoName.Contains("p1_")) isNorm = true;
        // if (histoName.Contains("p2_")) isNorm = true;
        // if (histoName.Contains("p3_")) isNorm = true;
        if (histoName.Contains("ATLAS_norm") /*&& histoName.Contains("lvlv")*/) isNorm = true;
        // if (histoName.Contains("ATLAS_Hbb_norm_")) isNorm = true;
        // if (histoName.Contains("ATLAS_PM_EFF_")) isNorm = true;
        // if (histoName.Contains("scale_norm")) isNorm = true;
        if (histoName.Contains("scale_") && !histoName.Contains("QCDscale_")) isNorm = true;
        if (histoName.Contains("mu_BR")) isNorm = true;
	if (histoName.Contains("gamma_stat_")) isNorm = true;
	if (histoName.Contains("ATLAS_shape_")) isNorm = true;
        TH1D* hist = (TH1D*)f->Get(histoName);

        int nrBins = hist->GetNbinsX();

        for (int bin = 1; bin <= nrBins; bin++) {
            double number = hist->GetBinContent(bin);
            
            // check inf
            if (number > 10e9) isInfNan = true;

            // check nan
            if (number != number) isInfNan = true;
        }
    
        if (ignoreInfNan && isInfNan) {
            cout << "WARNING::Skipping " << *it << " because of inf/nan" << endl;
            continue;
        }

        (isNorm?outFile_nf:outFile) << (isNorm?nrNPs++:nrNFs++) << " ";

        for (int bin = 1; bin <= nrBins; bin++) {
            double number = hist->GetBinContent(bin);
            (isNorm?outFile_nf:outFile) << number << ((bin < nrBins)?" ":"\n");
        }

        if (isNorm) outFile_nf_id << * it << "\n";
        else outFile_id << * it << "\n";

        f->Close();
    }

    cout << "Writing to file: " << outFileName.str() << "*.txt" << endl;

    outFile.close();
    outFile_id.close();
    
    if (mode == pulls) {
        outFile_nf.close();
        outFile_nf_id.close();
    }
}

// ____________________________________________________________________________|__________
// initialize file
void loadFile(const char* fileName, int cols, fileHolder file) {
    ifstream testFile(fileName);
    if (testFile.fail()) {
        cout << "ERROR::file " << fileName << "does not exist.";
        exit(1);
    }
    drawPlot(fileName, cols, file);
}

// ____________________________________________________________________________|__________
// Return vector of strings from textfile
vector<string> getLabel(const char* fileName, int nrPars) {
    vector<string> tmp_labels;
    Int_t nlines = 0;
    ifstream idFile(fileName);
    while (1) {
        if (!idFile.good() || nlines > nrPars) break;
        string label;
        idFile >> label;
        tmp_labels.push_back(label);
        nlines++;
    }
    return tmp_labels;
}
