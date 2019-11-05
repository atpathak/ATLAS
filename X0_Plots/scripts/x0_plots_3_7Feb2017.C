//#include <TString.h>
#include <string.h>
#include <TROOT.h>
//#include <TChain.h>
#include <TFile.h>
#include <TObject.h>
//#include <TLorentzVector.h>
//#include <iostream>
#include <vector>
//#include <TVector3.h>
#include <TH1.h>
#include <TProfile.h>
#include <TProfile2D.h>
//#include <atlasstyle-00-03-05/AtlasStyle.h>
//#include <atlasstyle-00-03-05/AtlasUtils.h>
//using namespace std;  

int x0_plots_3(string fileNameIn = "InclBrl4", string phi = "", string slice = "all"){

  
  //
  // Quick script written by Ben Smart (Ben.Smart@cern.ch)
  // It was never meant to be shared...
  //

  int BeamPipeCol             = kGreen+3;
  int ActiveSensorsCol        = kCyan+1;
  int PixelChipsCol           = kOrange+2;
  int SupportStructureCol     = kGray+2;
  int TitaniumCoolingPipesCol = kRed+1;
  int ElectricalCablingCol    = kViolet-6;
  int ModeratorCol            = kGreen+1;
  int PP1Col                  = kAzure+1;
  int DryNitrogenCol          = 1;

  int BeamPipeHash             = 3013;//3544//3590
  int ActiveSensorsHash        = 3007;//3215
  int PixelChipsHash           = 3006;//3251
  int SupportStructureHash     = 3002;//3544
  int TitaniumCoolingPipesHash = 3425;
  int ElectricalCablingHash    = 3452;
  int ModeratorHash            = 3005;
  int PP1Hash                  = 3004;
  int DryNitrogenHash          = 1001;
  
  //gROOT->LoadMacro("atlasstyle-00-03-05/AtlasStyle.C");
  //SetAtlasStyle();
  
  gStyle->SetOptStat(0);
  TH1::SetDefaultSumw2();
  gStyle->SetPadRightMargin(0.075);

  if(fileNameIn==""){
    cout << "Please enter an input file name identifier as the first argument." << endl;
    return 1;
  }

  if(phi!="" && phi!="Z" && phi!="R" && phi!="Phi"){
    cout << "Please enter either \"\", \"Z\", \"R\" or \"Phi\" as a second argument." << endl;
    return 1;
  }
  //  string fullfileNameIn = "rad_intLength."+fileNameIn+"_histo.root";
  string fullfileNameIn = "../Simulate/" + fileNameIn + "/Geantino_" + slice + "/rad_intLength.histo.root";
  cout << "Opening fullfileNameIn = " << fullfileNameIn << endl;
  TFile* inFile = TFile::Open(fullfileNameIn.c_str());

  string dir = "radLen/";
  //string dir = "intLen/";
  gDirectory->cd(dir.c_str());

  //TList lslist = inFile->ls();

  string versionNumber = fileNameIn;
  if(versionNumber.find("_") != string::npos){
    while(versionNumber.find("_") != string::npos){
      std::string::size_type npos = versionNumber.find("_");
      versionNumber = versionNumber.substr(npos+1,versionNumber.size());
    }
  }else{
    versionNumber = "Default";
  }

  //double plotlim = 200;
  double plotlim = 2;
  //plotlim = 0.04;
  if(fileNameIn.find("ID") != string::npos) plotlim = 6;
  if(phi=="Phi") plotlim = 1.2;

  bool poseta = true;
  //bool poseta = false;
  string posetaname = "";
  if(poseta) posetaname = "_Pos";

  string stacktitle = "";
  
  //string name2DPlot = "RZRadLen";
  //TProfile2D* h_RZRadLen = NULL;
  //inFile->GetObject(name2DPlot.c_str(),h_RZRadLen);

  string partnameTotal_X0_RL = "Total_X0"+phi;
  partnameTotal_X0_RL = partnameTotal_X0_RL+"_RL";
  string nameTotal_X0_RL = dir+partnameTotal_X0_RL;
  TProfile* h_Total_X0_RL = NULL;
  inFile->GetObject(nameTotal_X0_RL.c_str(),h_Total_X0_RL);
  
  TCanvas *c_total = new TCanvas(partnameTotal_X0_RL.c_str(),partnameTotal_X0_RL.c_str(),800,600);
  c_total->SetFillColor(0);
  h_Total_X0_RL->GetYaxis()->SetRangeUser(0,plotlim);
  h_Total_X0_RL->SetLineColor(1);
  h_Total_X0_RL->SetMarkerColor(1);
  TH1D* h_t2 = h_Total_X0_RL->ProjectionX();
  h_t2->SetFillColor(2);
  h_t2->SetLineColor(2);
  h_t2->SetMarkerColor(2);
  h_t2->GetYaxis()->SetRangeUser(0,plotlim);
  h_t2->GetYaxis()->SetTitle("%X0");
  if(phi==""){
    h_t2->GetXaxis()->SetTitle("#eta");
  }else if(phi=="Phi"){
    h_t2->GetXaxis()->SetTitle("#phi");
  }else if(phi=="Z"){
    h_t2->GetXaxis()->SetTitle("Z [mm]");
  }else if(phi=="R"){
    h_t2->GetXaxis()->SetTitle("R [mm]");
  }
  h_t2->Draw("hist");
  h_Total_X0_RL->Draw("same");

  TCanvas *c_stack = new TCanvas(fileNameIn.c_str(),fileNameIn.c_str(),800,600);
  c_stack->SetFillColor(0);

  gPad->SetTicks(1,1);
  //gPad->SetGridy();
  
  TString title = "";
  if(phi==""){
    title = fileNameIn+";#eta;%X0";
  }else if(phi=="Phi"){
    title = fileNameIn+";#phi;%X0";
  }else if(phi=="Z"){
    title = fileNameIn+";Z [mm];%X0";
  }else if(phi=="R"){
    title = fileNameIn+";R [mm];%X0";
  }
  THStack *h_stack = new THStack(fileNameIn.c_str(),title);

  //TLegend *leg_stack = new TLegend(0.70, 0.62, 0.87, 0.92);
  TLegend *leg_stack = new TLegend(gPad->GetLeftMargin()+0.025, 0.48, 0.485,1.-gPad->GetTopMargin()-0.031);
  leg_stack->SetFillStyle(0);
  leg_stack->SetBorderSize(0);
  leg_stack->SetShadowColor(0);
  leg_stack->SetTextSize(0.038);
  
  TH1D* h_services = NULL;
  TH1D* h_carbon = NULL;
  TH1D* h_steel = NULL;
  TH1D* h_moderator = NULL;
  TH1D* h_pixelchips = NULL;
  ////
  TH1D* h_PP0 = NULL;
  TH1D* h_PP1 = NULL;
  TH1D* h_cooling = NULL;
  TH1D* h_radiator = NULL;
  TH1D* h_activeSensors = NULL;
  TH1D* h_aluminium = NULL;
  TH1D* h_ibl = NULL;
  TH1D* h_OtherPixel = NULL;
  TH1D* h_OtherSCT = NULL;
  TH1D* h_beamPipe = NULL;
  TH1D* h_air = NULL;
  
  int n_colour = 1;
  string searchstring = "";
  string searchNOTstring = "BENNOT";
  //string searchNOTstring = "_PixelL";
  string searchstringAddition = "";
  string elsearchstring = "";
  string elsearchstringTi = "";
  TH1D* h_elsearch = NULL;
  TH1D* h_pixel_elsearch = NULL;
  bool switchTi = false;
  TKey *key;
  TObject *obj;
  TIter next(gDirectory->GetListOfKeys());
  while( (key = (TKey*)next()) ){
    string name = key->GetName();
    //cout << "Plot name1 = " << key->GetName() << endl;
    // Use this line to select certain types of plots
    // "M_" will get you all the material plots
    // "D_" will get you the detector volume plots (but use x0_plots_4.C for that)
    // "DM_Pixel_" will get you the material plots for just the Pixel volume


    
    searchstringAddition = "";
    //searchstringAddition = "_PixelEC123_";
    //searchstringAddition = "_PixelL0";
    //plotlim = 20;
    //searchNOTstring = "_PixelL";
    
    //BENsearchstring = "M_"+searchstringAddition;
    //searchstring = "M_";
    //searchstring = "M__L0_";
    //searchstring = "M__PixelL0_";
    ////searchstring = "M__Pixel";
    //searchstring = "M__PixelEC123_";
    //searchstring = "DM_Pixel_";
    //searchstring = "DM_Pixel_";
    //searchstring = "DM_SCT_";
    searchstring = "DM_";
    //searchstring = searchstring+searchstringAddition;

    //searchstring = "DM_";

    //searchstring = "M__StripsOuterSupport";
    //searchstring = "M__StripsL0";
    //searchstring = "M_pix::PST";
    /*
    if(name.find("PixelL2") != string::npos ) continue;
    if(name.find("PixelL3") != string::npos ) continue;
    if(name.find("PixelL4") != string::npos ) continue;
    if(name.find("PixelL3") != string::npos ) continue;
    if(name.find("Pixel") == string::npos ) continue;
    */

    //if(name.find("PST") == string::npos ) continue;
    if(name.find("DM_Atlas_Air") != string::npos ) continue;
    
    elsearchstring = "E_"+searchstringAddition;
    elsearchstringTi = "Titanium";
    //E__PixelL0_Titanium_RL
    switchTi = true;//false;
    //if(switchTi && name.find(elsearchstring) != string::npos && name.find(elsearchstringTi) != string::npos){
    if(switchTi && name.find(elsearchstring) == 0 && name.find(elsearchstringTi) != string::npos && name.find(searchNOTstring) == string::npos){
      if(phi==""){
	if(name.find("Phi_RL") == (name.size()-6) || name.find("R_RL") == (name.size()-4) || name.find("Z_RL") == (name.size()-4)) continue;  
      }else if(phi=="Phi"){
	if(name.find("Phi_RL") != (name.size()-6)) continue;
      }else if(phi=="Z"){
	if(name.find("Z_RL") != (name.size()-4)) continue;
      }else if(phi=="R"){
	if(name.find("R_RL") != (name.size()-4)) continue;
      }
      cout << "El Ti search: found " << name << endl;
      obj = key->ReadObj();
      TProfile* elplot = (TProfile*)obj;
      //h_elsearch = elplot->ProjectionX();
      if(!h_elsearch){
	h_elsearch = elplot->ProjectionX();
	//int ticolour = kBlue+1;
	int ticolour = TitaniumCoolingPipesCol;
	h_elsearch->SetFillColor(ticolour);
	h_elsearch->SetLineColor(ticolour);
	h_elsearch->SetMarkerColor(ticolour);
	h_elsearch->SetFillStyle(TitaniumCoolingPipesHash);

	/*
	if( name.find("Pixel") != string::npos ){
	  h_pixel_elsearch = elplot->ProjectionX();
	  h_pixel_elsearch->SetFillColor(ticolour);
	  h_pixel_elsearch->SetLineColor(ticolour);
	  h_pixel_elsearch->SetMarkerColor(ticolour);
	}
	*/
	
      }else{

	//if( name.find("Pixel") != string::npos ) h_pixel_elsearch->Add(elplot->ProjectionX(),1.0);
	
	h_elsearch->Add(elplot->ProjectionX());
      }
      /*
      if(!h_ibl){
	h_ibl = (TH1D*)h_plot->Clone();
	h_ibl->SetTitle("OtherIBL");
      }else{
	h_ibl->Add(h_plot);
      }
      */
    }
    

    
    if(name.find(searchstring) != 0 || name.find(searchNOTstring) != string::npos) continue;
    if(phi==""){
      if(name.find("Phi_RL") == (name.size()-6) || name.find("R_RL") == (name.size()-4) || name.find("Z_RL") == (name.size()-4)) continue;  
    }else if(phi=="Phi"){
      if(name.find("Phi_RL") != (name.size()-6)) continue;
      name = name.substr(0,name.find("Phi_RL"));
    }else if(phi=="Z"){
      if(name.find("Z_RL") != (name.size()-4)) continue;
      name = name.substr(0,name.find("Z_RL"));
    }else if(phi=="R"){
      if(name.find("R_RL") != (name.size()-4)) continue;
      name = name.substr(0,name.find("R_RL"));
    }
    name = name.substr(searchstring.size(),name.size());
    name = name.substr(0,name.find("_RL"));

    if(fileNameIn.find("ID") != string::npos){
      //if(name.find("Chip") != string::npos) name = "PixelChips";
      if(name.find("Cooling") != string::npos) name = "Cooling";
      if(name.find("Cool") != string::npos) name = "Cooling";
      if(name.find("CO2") != string::npos) name = "Cooling";
      if(name.find("Cryo") != string::npos) name = "Cooling";
      if(name.find("Pipe") != string::npos) name = "Cooling";
      if(name.find("Flange") != string::npos) name = "SupportStructure";
      if(name.find("Fixation") != string::npos) name = "SupportStructure";
      if(name.find("Clamp") != string::npos) name = "SupportStructure";
      if(name.find("Cable") != string::npos) name = "Services";
      if(name.find("Connector") != string::npos) name = "Services";
      if(name.find("Srv") != string::npos) name = "Services";
      if(name.find("Flex") != string::npos) name = "Services";
      if(name.find("Stave") != string::npos) name = "SupportStructure";
      if(name.find("Wire") != string::npos) name = "Services";
      if(name.find("Tape") != string::npos) name = "Services";
      if(name.find("PPF0") != string::npos) name = "PP0";
      if(name.find("PPB1") != string::npos) name = "PP1";
      if(name.find("IBL") != string::npos) name = "OtherIBL";
      if(name.find("BL") != string::npos) name = "OtherIBL";
      if(name.find("PST") != string::npos) name = "PST";
      if(name.find("IST") != string::npos) name = "IST";
      if(name.find("Disk") != string::npos) name = "SupportStructure";
      //if(name.find("L0") != string::npos) name = "SupportStructure";
      //if(name.find("L1") != string::npos) name = "SupportStructure";
      //if(name.find("L2") != string::npos) name = "SupportStructure";
      if(name.find("Panel") != string::npos) name = "Services";
      if(name.find("Radiator") != string::npos) name = "Radiators";
      //if(name.find("Spine") != string::npos) name = "ThermalSpine";
      if(name.find("Spine") != string::npos) name = "Cooling";
      if(name.find("Fibre") != string::npos) name = "Services";
      if(name.find("Chip") != string::npos) name = "PixelChips";
      if(name.find("Shell") != string::npos) name = "SupportStructure";
      if(name.find("Heat") != string::npos) name = "Cooling";
      if(name.find("Dogleg") != string::npos) name = "Cooling";
      if(name.find("PigTail") != string::npos) name = "Cooling";
      if(name.find("Pigtail") != string::npos) name = "Cooling";
      if(name.find("Aerogel") != string::npos) name = "Aerogel";
      if(name.find("Faraday") != string::npos) name = "FaradayCage";
      if(name.find("Prepreg") != string::npos) name = "SupportStructure";
      if(name.find("Harness") != string::npos) name = "SupportStructure";
      if(name.find("Spider") != string::npos) name = "SupportStructure";
      if(name.find("Finger") != string::npos) name = "SupportStructure";
      if(name.find("Serv") != string::npos) name = "Services";
      if(name.find("Bulkhead") != string::npos) name = "SupportStructure";
      if(name.find("EndPlate") != string::npos) name = "SupportStructure";
      if(name.find("Tray") != string::npos) name = "SupportStructure";
      if(name.find("Trolley") != string::npos) name = "SupportStructure";
      if(name.find("Cap") != string::npos) name = "SupportStructure";
      if(name.find("K13C_16") != string::npos) name = "SupportStructure";
      if(name.find("AlNitride") != string::npos) name = "Cooling";
      if(name.find("Copper") != string::npos) name = "Services";
      if(name.find("G10") != string::npos) name = "Services";
      if(name.find("DBMRod") != string::npos) name = "Services";
      if(name.find("Twister") != string::npos) name = "SupportStructure";
      if(name.find("Wolfram") != string::npos) name = "SupportStructure";
      if(name.find("Aluminium") != string::npos) name = "SupportStructure";
    }else{
      if(name.find("CO2") != string::npos) name = "Cooling";
      //if(name.find("BoronNitrideEpoxy") != string::npos) name = "Cooling";
      if(name.find("BoronNitrideEpoxy") != string::npos) name = "SupportStructure";
      if(name.find("FwdSpineOutBase") != string::npos) name = "SupportStructure";
      if(name.find("Rohacell") != string::npos) name = "SupportStructure";
      if(name.find("Honeycomb") != string::npos) name = "SupportStructure";
      if(name.find("matSV") != string::npos) name = "Services";
      if(name.find("matEOS") != string::npos) name = "Services";
      if(name.find("matDCDC") != string::npos) name = "Services";
      if(name.find("PCB") != string::npos) name = "Services";

      if(name.find("N2") != string::npos) name = "DryNitrogen";
      if(name.find("TiMetal") != string::npos) name = "Services";
      if(name.find("CuMetal") != string::npos) name = "Services";
      if(name.find("Aluminium") != string::npos) name = "BeamPipe";
    }
    // Do you want to split cooling and services where possible?
    if(name.find("Cooling") != string::npos) name = "Services";

    if(name.find("Inconel") != string::npos) name = "BeamPipe";
    if(name.find("Aerogel") != string::npos) name = "BeamPipe";
    if(name.find("Beryllium") != string::npos) name = "BeamPipe";
    if(name.find("Getter") != string::npos) name = "BeamPipe";
    if(name.find("Vacuum") != string::npos) name = "BeamPipe";
    if(searchstring=="M_"){
      if(name.find("Kapton") != string::npos) name = "BeamPipe";
    }else{
      if(name.find("Kapton") != string::npos) name = "Services";
    }
    //if(name.find("Iron") != string::npos) name = "BeamPipe";
    if(name.find("Iron") != string::npos) name = "SupportStructure";
    if(name.find("Peek") != string::npos) name = "SupportStructure";
    if(name.find("CFRP") != string::npos) name = "SupportStructure";
    if(name.find("CFoam") != string::npos) name = "SupportStructure";
    if(name.find("K13D2U") != string::npos) name = "SupportStructure";
    if(name.find("BoratedPolyethylene") != string::npos) name = "Moderator";


    if(name.find("TiMetal") != string::npos) name = "Titanium";
    if(name.find("CuMetal") != string::npos) name = "Copper";
      
    if(name.find("Alpine") != string::npos) name = "SupportStructure";
    
    if(name.find("Support") != string::npos) name = "SupportStructure";
    if(name.find("Carbon") != string::npos) name = "SupportStructure";
    if(name.find("Default") != string::npos) name = "SupportStructure";
    if(name.find("Moderator") != string::npos) name = "Moderator";
    if(name.find("Steel") != string::npos) name = "Steel";
    if(name.find("BarrelStrip") != string::npos) name = "Services";
    if(name.find("Brl") != string::npos) name = "Services";
    if(name.find("Svc") != string::npos) name = "Services";
    if(name.find("InnerIST") != string::npos) name = "Services";
    if(name.find("InnerPST") != string::npos) name = "Services";
    if(name.find("BarrelPixel") != string::npos) name = "Services";
    if(name.find("EndcapPixel") != string::npos) name = "Services";
    if(name.find("InnerPixel") != string::npos) name = "Services";
    if(name.find("OuterPixel") != string::npos) name = "Services";
    if(name.find("pix::Chip") != string::npos) name = "PixelChips";
    if(name.find("pix::Hybrid") != string::npos) name = "PixelChips";
    if(name.find("PP0") != string::npos) name = "PP0";
    if(name.find("PP1") != string::npos) name = "PP1";

    if(fileNameIn.find("ID") != string::npos){
      if(name.find("pix::") != string::npos) name = "OtherPixel";
      //if(name.find("Titanium") != string::npos) name = "OtherPixel";
      if(name.find("sct::") != string::npos) name = "OtherSCT";
      if(name.find("Glass") != string::npos) name = "OtherSCT";

      if(name.find("Other") != string::npos) name = "SupportStructure";
      if(name.find("PP0") != string::npos) name = "PP1";
      if(name.find("FaradayCage") != string::npos) name = "SupportStructure";
      if(name.find("Radiators") != string::npos) name = "SupportStructure";

      if(name.find("Titanium") != string::npos) name = "Services";
      
    }else{
      if(name.find("PP0") != string::npos) name = "PP1";
    }
      
    if(name.find("PST") != string::npos) name = "SupportStructure";
    if(name.find("IST") != string::npos) name = "SupportStructure";
    if(name.find("Silicon") != string::npos) name = "ActiveSensors";
    if(name.find("Straw") != string::npos) name = "ActiveSensors";
    if(name.find("Diamond") != string::npos) name = "ActiveSensors";
    if(name.find("SiMetal") != string::npos) name = "ActiveSensors";
    if(name.find("Air") != string::npos) name = "Air";
    // hack while we fix the air->nitrogen issue
    if(name.find("DryNitrogen") != string::npos) name = "Air";
    
    cout << "Plot name = " << key->GetName() << " = " << name << endl;
    //if(key->GetName() == searchstring+name+"_RL") cout << "Material name = " << name << endl;
    obj = key->ReadObj();
    TProfile* plot = (TProfile*)obj;

    //////////////////////////////////////////////////////////////////////////
    // Now we have the plot, so we need to get the count and fill the zeros...
    //
    // This is needed in order to deal with the plots in a statistically correct way
    // It's because in RadLengthIntegrator each plot (for each material, say) is only filled if a Geantion hits it, not if it's not hit in an event
    // If we don't make this correction here, then the THStack total will not equal the Total_X0_RL TProfile plot
    //
    
    // int nbins = plot->GetNbinsX();
    // for(int i=0 ; i<=nbins ; i++){

    //   double x_value = plot->GetBinCenter(i);
    //   int n_enties_plot = plot->GetBinEntries(i);
    //   int n_enties_ref = h_Total_X0_RL->GetBinEntries(i);
    //   int n_zeros_to_be_added = n_enties_ref - n_enties_plot;
      
    //   for(int j=0 ; j<n_zeros_to_be_added ; j++){

    // 	plot->Fill(x_value,0.0);

    //   }
      
    // }
    
    //////////////////////////////////////////////////////////////////////////

    //double fraction = plot->GetBinContent(plot->FindBin(0)) / h_Total_X0_RL->GetBinContent(h_Total_X0_RL->FindBin(0));
    //cout << "Material name = " << name << " Fraction = " << fraction << endl;
    //cout << "Material name = " << name << " , Origional name = " << key->GetName() << " , %X0 = " << plot->GetBinContent(plot->FindBin(0)) << endl;
    //    cout << "Material name = " << name << " , Origional name = " << key->GetName() << endl;

    int nbins = plot->GetNbinsX();
    string x_value_name = phi;
    if(phi=="") x_value_name = "Eta";
    for(int i=0 ; i<=nbins ; i++){

      double x_value = plot->GetBinCenter(i);

      if( x_value > 5.2 && name == "Services"){
	//cout << "Material name = " << name << " , Origional name = " << key->GetName() << endl;
	double yplus  = plot->GetBinContent(plot->FindBin(x_value));
	double yminus = plot->GetBinContent(plot->FindBin(-x_value));
	double x_minus = plot->GetBinCenter(plot->FindBin(-x_value));
	double ydiff = yplus - yminus;
	//if(ydiff != 0) cout << name << " " << key->GetName() << " +x=" << x_value << " \t-x=" << x_minus << " \t(x,y_diff) = " << x_value << ", \t" << ydiff/100.0 << endl;
      }

      //      if(plot->GetBinContent(plot->FindBin(x_value)) != 0.0) cout << "  " << key->GetName() << " \t" << x_value_name << " = " << x_value << " \t %X0 = " << plot->GetBinContent(plot->FindBin(x_value)) << endl;
      
    }  
    
    //////////////////////////////////////////////////////////////////////////

    TH1D* h_plot = plot->ProjectionX();
    //h_plot->Scale(0.01); // for versions before v43
    //h_plot->GetYaxis()->SetRangeUser(0,100);
    if(name == "SupportStructure"){
      h_plot->SetFillColor(SupportStructureCol);
      h_plot->SetLineColor(SupportStructureCol);
      h_plot->SetMarkerColor(SupportStructureCol);
      h_plot->SetFillStyle(SupportStructureHash);
    }else if(name == "Services"){
      h_plot->SetFillColor(ElectricalCablingCol);
      h_plot->SetLineColor(ElectricalCablingCol);
      h_plot->SetMarkerColor(ElectricalCablingCol);
      h_plot->SetFillStyle(ElectricalCablingHash);
    }else if(name == "Steel"){
      h_plot->SetFillColor(17);
      h_plot->SetLineColor(17);
      h_plot->SetMarkerColor(17);
    }else if(name == "FaradayCage"){
      h_plot->SetFillColor(15);
      h_plot->SetLineColor(15);
      h_plot->SetMarkerColor(15);
    }else if(name == "BeamPipe"){
      h_plot->SetFillColor(BeamPipeCol);
      h_plot->SetLineColor(BeamPipeCol);
      h_plot->SetMarkerColor(BeamPipeCol);
      h_plot->SetFillStyle(BeamPipeHash);
    }else if(name == "OtherSCT"){
      h_plot->SetFillColor(6);
      h_plot->SetLineColor(6);
      h_plot->SetMarkerColor(6);
    }else if(name == "OtherPixel"){
      h_plot->SetFillColor(13);
      h_plot->SetLineColor(13);
      h_plot->SetMarkerColor(13);
    }else if(name == "Cooling"){
      h_plot->SetFillColor(7);
      h_plot->SetLineColor(7);
      h_plot->SetMarkerColor(7);
    }else if(name == "Silicon"){
      h_plot->SetFillColor(9);
      h_plot->SetLineColor(9);
      h_plot->SetMarkerColor(9);
    }else if(name == "ActiveSensors"){
      h_plot->SetFillColor(ActiveSensorsCol);
      h_plot->SetLineColor(ActiveSensorsCol);
      h_plot->SetMarkerColor(ActiveSensorsCol);
      h_plot->SetFillStyle(ActiveSensorsHash);
    // }else if(name == "pix::IST"){
    //   h_plot->SetFillColor(12);
    //   h_plot->SetLineColor(12);
    //   h_plot->SetMarkerColor(12);
    // }else if(name == "pix::PST"){
    //   h_plot->SetFillColor(13);
    //   h_plot->SetLineColor(13);
    //   h_plot->SetMarkerColor(13);
    }else if(name == "Moderator"){
      h_plot->SetFillColor(ModeratorCol);
      h_plot->SetLineColor(ModeratorCol);
      h_plot->SetMarkerColor(ModeratorCol);
      h_plot->SetFillStyle(ModeratorHash);
    }else if(name == "PP0"){
      h_plot->SetFillColor(26);
      h_plot->SetLineColor(26);
      h_plot->SetMarkerColor(26);
    }else if(name == "PP1"){
      h_plot->SetFillColor(PP1Col);//23
      h_plot->SetLineColor(PP1Col);
      h_plot->SetMarkerColor(PP1Col);
      h_plot->SetFillStyle(PP1Hash);
    }else if(name == "Air"){
      h_plot->SetFillColor(DryNitrogenCol);
      h_plot->SetLineColor(DryNitrogenCol);
      h_plot->SetMarkerColor(DryNitrogenCol);
      h_plot->SetFillStyle(DryNitrogenHash);
      h_plot->SetLineWidth(3);
    }else if(name == "PixelChips"){
      h_plot->SetFillColor(PixelChipsCol);
      h_plot->SetLineColor(PixelChipsCol);
      h_plot->SetMarkerColor(PixelChipsCol);
      h_plot->SetFillStyle(PixelChipsHash);
    // }else if(name == "pix::Chip"){
    //   h_plot->SetFillColor(28);
    //   h_plot->SetLineColor(28);
    //   h_plot->SetMarkerColor(28);
    // }else if(name == "pix::Chip_SimpleChip"){
    //   h_plot->SetFillColor(29);
    //   h_plot->SetLineColor(29);
    //   h_plot->SetMarkerColor(29);
    // }else if(name == "pix::Chip_DoubleChip"){
    //   h_plot->SetFillColor(30);
    //   h_plot->SetLineColor(30);
    //   h_plot->SetMarkerColor(30);
    // }else if(name == "pix::Chip_FourChip"){
    //   h_plot->SetFillColor(31);
    //   h_plot->SetLineColor(31);
    //   h_plot->SetMarkerColor(31);
    }else{
      n_colour++;
      if(n_colour==100) n_colour=1;
      if(n_colour==3) n_colour++;
      if(n_colour==5) n_colour++;
      if(n_colour==6) n_colour++;
      if(n_colour==9) n_colour++;
      if(n_colour==10) n_colour++;
      /////if(n_colour==12) n_colour++;
      /////if(n_colour==13) n_colour++;
      if(n_colour==14) n_colour++;
      if(n_colour==15) n_colour++;
      if(n_colour==17) n_colour++;
      if(n_colour==23) n_colour++;
      if(n_colour==26) n_colour++;
      if(n_colour==28) n_colour++;
      //if(n_colour==29) n_colour++;
      //if(n_colour==30) n_colour++;
      //if(n_colour==31) n_colour++;
      if(n_colour==46) n_colour++;
      h_plot->SetFillColor(n_colour);
      h_plot->SetLineColor(n_colour);
      h_plot->SetMarkerColor(n_colour);
    }
    //cout << "Plot name = " << name << " n_colour = " << n_colour << endl;
    //h_plot->SetFillStyle(1001);
    if(name == "Services"){
      if(!h_services){
    	h_services = (TH1D*)h_plot->Clone();
	h_services->SetTitle("Services");
      }else{
    	h_services->Add(h_plot);
      }
    }else if(name == "SupportStructure"){
      if(!h_carbon){
    	h_carbon = (TH1D*)h_plot->Clone();
	h_carbon->SetTitle("SupportStructure");
      }else{
    	h_carbon->Add(h_plot);
      }
    }else if(name == "PixelChips"){
      if(!h_pixelchips){
    	h_pixelchips = (TH1D*)h_plot->Clone();
	h_pixelchips->SetTitle("PixelChips");
      }else{
    	h_pixelchips->Add(h_plot);
      }
    }else if(name == "Steel"){
      if(!h_steel){
    	h_steel = (TH1D*)h_plot->Clone();
	h_steel->SetTitle("Steel");
      }else{
    	h_steel->Add(h_plot);
      }
    }else if(name == "Moderator"){
      if(!h_moderator){
    	h_moderator = (TH1D*)h_plot->Clone();
	h_moderator->SetTitle("Moderator");
      }else{
    	h_moderator->Add(h_plot);
      }
    }else if(name == "PP0"){
      if(!h_PP0){
    	h_PP0 = (TH1D*)h_plot->Clone();
	h_PP0->SetTitle("PP0");
      }else{
    	h_PP0->Add(h_plot);
      }
    }else if(name == "PP1"){
      if(!h_PP1){
    	h_PP1 = (TH1D*)h_plot->Clone();
	h_PP1->SetTitle("PP1");
      }else{
    	h_PP1->Add(h_plot);
      }
    }else if(name == "Cooling"){
      if(!h_cooling){
    	h_cooling = (TH1D*)h_plot->Clone();
	h_cooling->SetTitle("Cooling");
      }else{
    	h_cooling->Add(h_plot);
      }
    }else if(name == "Radiators"){
      if(!h_radiator){
    	h_radiator = (TH1D*)h_plot->Clone();
	h_radiator->SetTitle("Radiators");
      }else{
    	h_radiator->Add(h_plot);
      }
    }else if(name == "ActiveSensors"){
      if(!h_activeSensors){
    	h_activeSensors = (TH1D*)h_plot->Clone();
	h_activeSensors->SetTitle("ActiveSensors");
      }else{
    	h_activeSensors->Add(h_plot);
      }
    }else if(name == "Aluminium"){
      if(!h_aluminium){
    	h_aluminium = (TH1D*)h_plot->Clone();
	h_aluminium->SetTitle("Aluminium");
      }else{
    	h_aluminium->Add(h_plot);
      }
    }else if(name == "OtherIBL"){
      if(!h_ibl){
    	h_ibl = (TH1D*)h_plot->Clone();
	h_ibl->SetTitle("OtherIBL");
      }else{
    	h_ibl->Add(h_plot);
      }
    }else if(name == "BeamPipe"){
      if(!h_beamPipe){
    	h_beamPipe = (TH1D*)h_plot->Clone();
	h_beamPipe->SetTitle("BeamPipe");
      }else{
    	h_beamPipe->Add(h_plot);
      }
    }else if(name == "OtherPixel"){
      if(!h_OtherPixel){
    	h_OtherPixel = (TH1D*)h_plot->Clone();
	h_OtherPixel->SetTitle("OtherPixel");
      }else{
    	h_OtherPixel->Add(h_plot);
      }
    }else if(name == "OtherSCT"){
      if(!h_OtherSCT){
    	h_OtherSCT = (TH1D*)h_plot->Clone();
	h_OtherSCT->SetTitle("OtherSCT");
      }else{
    	h_OtherSCT->Add(h_plot);
      }
    }else if(name == "Air"){
      if(!h_air){
	h_air = (TH1D*)h_plot->Clone();
	h_air->SetTitle("Air");
      }else{
    	h_air->Add(h_plot);
      }
    }else{
      h_stack->Add(h_plot);
    }
    if(name != "Services" &&
       name != "SupportStructure" &&
       name != "PixelChips" &&
       name != "Steel" &&
       name != "Moderator" &&
       name != "PP0" &&
       name != "PP1" &&
       name != "Radiators" &&
       name != "Cooling" &&
       name != "ActiveSensors" &&
       name != "Aluminium" &&
       name != "OtherPixel" &&
       name != "OtherSCT" &&
       name != "BeamPipe" &&
       name != "Air" &&
       name != "OtherIBL"){
      if(name == "FwdSpineOutBase"){
	leg_stack->AddEntry( h_plot, "SCT_EC_ThermalSpine" );
      }else{
	leg_stack->AddEntry( h_plot, name.c_str() );
      }
    }
    //h_stack->Add(h_plot);
    //leg_stack->AddEntry( h_plot, name.c_str() );
  }

  
  //////////////////////////////////

  // subtract the Titanium (cooling pipes) from the services, and add it to the support structure
  if(h_elsearch){
    //h_carbon->Add(h_elsearch);
    h_elsearch->Scale(0.01);
    h_services->Add(h_elsearch,-1.0);
  }else if(switchTi){
    cout << "DANGER WILL ROBINSON!!! switchTi = true but h_elsearch = NULL" << endl;
  }

  if(h_pixelchips && h_carbon){
    h_carbon->Add(h_pixelchips,0.613);
    h_pixelchips->Scale(0.387);
  }else{
    cout << "DANGER WILL ROBINSON!!! No chips and/or support structure" << endl;
  }
  
  //////////////////////////////////
  

  if(h_beamPipe){
    h_stack->Add(h_beamPipe);
    //leg_stack->AddEntry( h_beamPipe, "Beam Pipe" );
  }
  if(h_ibl){
    h_stack->Add(h_ibl);
    leg_stack->AddEntry( h_ibl, "OtherIBL" );
  }
  /*
  if(h_carbon){
    h_stack->Add(h_carbon);
    leg_stack->AddEntry( h_carbon, "SupportStructure" );
  }
  */
  if(h_activeSensors){
    h_stack->Add(h_activeSensors);
    //leg_stack->AddEntry( h_activeSensors, "Active Sensors" );
  }
  if(h_pixelchips){
    h_stack->Add(h_pixelchips);
    //leg_stack->AddEntry( h_pixelchips, "Pixel Chips" );
  }
  if(h_carbon){
    h_stack->Add(h_carbon);
    //leg_stack->AddEntry( h_carbon, "Support Structure" );
  }
  if(h_elsearch){
    h_stack->Add(h_elsearch);
    //h_stack->Add(h_pixel_elsearch); // For quick pipe doubleing
    //leg_stack->AddEntry( h_elsearch, "Titanium Cooling Pipes" );
  }
  if(h_services){
    h_stack->Add(h_services);
    //leg_stack->AddEntry( h_services, "Services" );
    //leg_stack->AddEntry( h_services, "Electrical Cabling" );
  }
  if(h_cooling){
    h_stack->Add(h_cooling);
    leg_stack->AddEntry( h_cooling, "Cooling" );
  }
  if(h_radiator){
    h_stack->Add(h_radiator);
    leg_stack->AddEntry( h_radiator, "Radiators" );
  }
  if(h_steel){
    h_stack->Add(h_steel);
    leg_stack->AddEntry( h_steel, "Steel" );
  }
  if(h_moderator){
    h_stack->Add(h_moderator);
    //leg_stack->AddEntry( h_moderator, "Moderator" );
  }
  if(h_OtherPixel){
    h_stack->Add(h_OtherPixel);
    leg_stack->AddEntry( h_OtherPixel, "OtherPixel" );
  }
  if(h_OtherSCT){
    h_stack->Add(h_OtherSCT);
    leg_stack->AddEntry( h_OtherSCT, "OtherSCT" );
  }
  if(h_PP0){
    h_stack->Add(h_PP0);
    leg_stack->AddEntry( h_PP0, "Patch Panel 0" );
  }
  if(h_PP1){
    h_stack->Add(h_PP1);
    //leg_stack->AddEntry( h_PP1, "Patch Panel 1" );
  }
  if(h_aluminium){
    h_stack->Add(h_aluminium);
    leg_stack->AddEntry( h_aluminium, "Aluminium" );
  }
  if(h_air){
    h_stack->Add(h_air);
    //leg_stack->AddEntry( h_air, "Air" );
    //
    //leg_stack->AddEntry( h_air, "Dry Nitrogen" );
  }

  ////////////////////

  if(h_air){
    //leg_stack->AddEntry( h_air, "Dry Nitrogen", "L" );
    leg_stack->AddEntry( h_air, "Dry Nitrogen" );
  }
  if(h_PP1){
    if(fileNameIn.find("ID") != string::npos){
      leg_stack->AddEntry( h_PP1, "Patch Panels 0 + 1" );
    }else{
      leg_stack->AddEntry( h_PP1, "Patch Panel 1" );
    }
  }
  if(h_moderator){
    leg_stack->AddEntry( h_moderator, "Moderator" );
  }
  if(h_services){
    leg_stack->AddEntry( h_services, "Electrical Cabling" );
  }
  if(h_elsearch){
    leg_stack->AddEntry( h_elsearch, "Titanium Cooling Pipes" );
  }
  if(h_carbon){
    leg_stack->AddEntry( h_carbon, "Support Structure" );
  }
  if(h_pixelchips){
    leg_stack->AddEntry( h_pixelchips, "Pixel Chips" );
  }
  if(h_activeSensors){
    leg_stack->AddEntry( h_activeSensors, "Active Sensors" );
  }
  if(h_beamPipe){
    leg_stack->AddEntry( h_beamPipe, "Beam Pipe" );
  }
  
  ////////////////////
  
  h_stack->Draw("hist");
  TH1D* h_temp_first = (TH1D*)h_stack->GetStack()->Last();
  h_temp_first->GetYaxis()->SetRangeUser(0,plotlim);
  //h_temp_first->GetYaxis()->SetTitle("Radiation Lengths Passed Through [#font[52]{X}_{0}]");
  h_temp_first->GetYaxis()->SetTitle("Radiation Lengths [X_{0}]");
  h_temp_first->GetXaxis()->SetLabelSize(0.042);
  h_temp_first->GetXaxis()->SetTitleSize(0.045);
  h_temp_first->GetYaxis()->SetLabelSize(0.042);
  h_temp_first->GetYaxis()->SetTitleSize(0.045);
  if(phi=="Z"){
    h_temp_first->GetXaxis()->SetRangeUser(0,3512);
  }
  if(poseta){
    h_temp_first->GetXaxis()->SetRangeUser(0,6);
    stacktitle = h_stack->GetTitle();
    cout << "stacktitle: " << stacktitle << endl;
    std::string::size_type npos = stacktitle.find("_");
    stacktitle = stacktitle.substr(0,npos)+" Step 1.2";
    if(stacktitle.find("InclBrl4") != string::npos){
      stacktitle = "ITk Inclined";
    }else if(stacktitle.find("IExtBrl4") != string::npos){
      stacktitle = "ITk Inclined";
    }else if(stacktitle.find("ExtBrl32") != string::npos){
      stacktitle = "ITk Extended";
    }else if(stacktitle.find("ExtBrl4") != string::npos){
      stacktitle = "ITk Extended";
    }else if(stacktitle.find("ID") != string::npos){
      stacktitle = "ID Run 2";
    }
    //h_temp_first->SetTitle(stacktitle.c_str());
    h_temp_first->SetTitle("");
  }else{
    h_temp_first->SetTitle(h_stack->GetTitle());
  }
  h_temp_first->Draw("hist");
  h_stack->Draw("hist same");
  //
  //h_t2->Draw("same");
  //leg_stack->AddEntry( h_t2, "Total %X0" );
  //
  h_t2->Add(h_t2,-1.0);
  h_t2->SetFillColor(1);
  h_t2->SetLineColor(1);
  h_t2->SetMarkerColor(1);
  h_t2->Draw("hist same");
  //
  gPad->RedrawAxis();
  //
  leg_stack->Draw();
  gPad->Update();
  leg_stack->Print();
  leg_stack->SetY1NDC(leg_stack->GetY2NDC()-leg_stack->GetNRows()*leg_stack->GetTextSize()*1.25);
  c_stack->Update();
  
  cout << endl;
  TIter snext(h_stack->GetHists());
  double xsum = 0;
  double fsum = 0;
  //string x_value_name = phi;
  //if(phi=="") x_value_name = "Eta";
  while (TObject *obj = snext()){

    double x_value = 3.6;
    
    TH1D* splot = (TH1D*)obj;
    double fraction = splot->GetBinContent(splot->FindBin(x_value)) / h_Total_X0_RL->GetBinContent(h_Total_X0_RL->FindBin(x_value));
    fsum+=fraction;
    xsum+=splot->GetBinContent(splot->FindBin(x_value));
    cout << splot->GetTitle() << " %X0 = " << splot->GetBinContent(splot->FindBin(x_value)) << " Fraction of total = " << fraction << endl;
    //    cout << splot->GetTitle() << " %X0 = " << splot->GetBinContent(splot->FindBin(x_value)) << endl;

    // int nbins = splot->GetNbinsX();
    // for(int i=0 ; i<=nbins ; i++){

    //   x_value = splot->GetBinCenter(i);
    //   cout << "  " << x_value_name << " = " << x_value << " \t" << splot->GetBinContent(splot->FindBin(x_value)) << endl;
      
    // }  

  }
  cout << "%X0 sum = " << xsum << " Fraction sum = " << fsum << endl;
  cout << endl;

  cout << "Nevents = " << h_Total_X0_RL->GetEntries() << endl;
  cout << endl;

  
  double x1 = -4;
  double x2 = -4;
  double x3 = 0;
  double x4 = 0;
  double y1 = 95;
  double y2 = 90;
  double y3 = 0;
  double y4 = 0;
  if(poseta){
    /*
    x1 = 0.25;
    x2 = 0.25;
    y1 = 190;
    y2 = 182.5;
    */
    /*
    x1 = 0.5;
    x2 = 0.5;
    y1 = 180;
    y2 = 165;
    */
    x1 = 4.25;//4.45;
    x2 = 4.25;
    x3 = 4.25;
    x4 = 4.25;
    y1 = 1.765*plotlim/2.0;
    y2 = y1 - 0.12*plotlim/2.0;
    y3 = y2 - 0.12*plotlim/2.0;
    y4 = y3 - 0.18*plotlim/2.0;
    if(fileNameIn.find("ID") != string::npos){
      x1 = 3;
      x2 = 3;
      x3 = 3;
      x4 = 3;
      y1 = 1.765*plotlim/2.0;
      y2 = y1 - 0.12*plotlim/2.0;
      y3 = y2 - 0.12*plotlim/2.0;
      y4 = y3 - 0.18*plotlim/2.0;
    }
  }
  if(phi=="Phi"){
    x1 = -2.5;
    x2 = -2.5;
    y1 = 117.5;
    y2 = 112.5;
  }else if (phi=="R"){
    x1 = 280;
    x2 = 280;
    y1 = 120;
    y2 = 110;
  }else if (phi=="Z"){
    x1 = 400;
    x2 = 400;
    y1 = 170;
    y2 = 160;
  }
  TText *t1 = new TText(x1,y1,"For xml debugging only!");
  TText *t2 = new TText(x2,y2,"Not to be used as an indication of the final detector layouts!");
  //
  TLatex l1; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  //l1.SetNDC();
  l1.SetTextFont(72);
  l1.SetTextColor(1);
  l1.SetTextSize(0.048);
  l1.DrawLatex(x1,y1,"ATLAS");
  //
  TLatex l2; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  //l2.SetNDC();
  l2.SetTextFont(42);
  l2.SetTextColor(1);
  l2.SetTextSize(0.048);
  l2.DrawLatex(x2,y2,"Simulation");
  TLatex l3; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  //l3.SetNDC();
  l3.SetTextFont(42);
  l3.SetTextColor(1);
  l3.SetTextSize(0.048);
  l3.DrawLatex(x3,y3,"Internal");
  //l3.DrawLatex(x3,y3,"Preliminary");
  //
  TLatex l4; //l.SetTextAlign(12); l.SetTextSize(tsize); 
  //l4.SetNDC();
  l4.SetTextFont(42);
  l4.SetTextColor(1);
  l4.SetTextSize(0.048);
  l4.DrawLatex(x4,y4,stacktitle.c_str());
  //l2.DrawLatex(x2,y2,"Simulation Preliminary");
  //
  //t->SetTextAlign(22);
  //t->SetTextColor(kRed+2);
  //t->SetTextFont(43);
  t1->SetTextSize(0.025);
  t2->SetTextSize(0.025);
  //t->SetTextAngle(45);
  if(fileNameIn.find("ID") == string::npos){
    //t1->Draw();
    //t2->Draw();
  }
  
   bool saveandexit = true;
  // un-comment this line when testing new plot-making. Then your plots will stay on screen.
  //saveandexit = false;
  if(saveandexit){
    if(phi==""){
      c_stack->SaveAs((versionNumber+"/x0_plots_3/"+fileNameIn+"/"+"Geantino_"+slice+"/"+fileNameIn+"_"+searchstring+posetaname+".png").c_str());
      //c_total->SaveAs((fileNameIn+"_"+searchstring+"_Total.eps").c_str());
    }else{
      //c_stack->SaveAs((versionNumber+"/x0_plots_3/"+fileNameIn+"/"+fileNameIn+"_"+searchstring+"_Phi.eps").c_str());
      c_stack->SaveAs((versionNumber+"/x0_plots_3/"+fileNameIn+"/"+"Geantino_"+slice+"/"+fileNameIn+"_"+searchstring+"_"+phi+posetaname+".eps").c_str());
      //c_total->SaveAs((fileNameIn+"_"+searchstring+"_Total_Phi.eps").c_str());
    }
    gROOT->ProcessLineSync(".q");
  }
  return 0;
  
}
