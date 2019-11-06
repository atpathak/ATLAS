#include <WorkspaceBuilder/StatAnalysis.h>
#include <WorkspaceBuilder/InputSample.h>
#include <WorkspaceBuilder/InputChannel.h>
#include "TDOMParser.h"
#include "TXMLAttr.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TLine.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <set>
#include <sys/stat.h> //to provide mkdir()
#include <sstream>
#include <RooWorkspace.h>
#include <RooStats/HistFactory/Measurement.h>
#include <RooStats/HistFactory/HistoToWorkspaceFactoryFast.h>
#include "RooStats/ModelConfig.h"

namespace WorkspaceBuilder{

  //-------------------------------------------------------------------------------

  bool StatAnalysis::getXMLStringAttribute(TXMLNode* node, std::string attributename, std::string &value){

    TListIter attribIt = node->GetAttributes();
    TXMLAttr* curAttr = 0;

    while( ( curAttr = dynamic_cast< TXMLAttr* >( attribIt() ) ) != 0 ) {

      // Get the Name, Val of this node
      TString attrName    = curAttr->GetName();
      std::string attrVal = curAttr->GetValue();

      if( attrName == TString( "" ) ) {
        std::cout << " Error: Attribute for '" << node->GetNodeName() << "' with no name found" << std::endl;
        return false;
      }
      else if( attrName == TString( attributename ) ) {
        value=attrVal;
        return true;
        break;
      }
    }//loop over attributes

    return false;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readInformationFromXML(std::vector<std::string> xmlpaths, UserTools* usertools, bool mcstat){

    std::vector<TXMLNode*> rootNodes;
    std::vector<TDOMParser*> parser;

    try {
      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){


        // Open the Driver XML File
        //TDOMParser xmlparser;
        parser.push_back(new TDOMParser());
        Int_t parseError = parser[ixmlfile]->ParseFile( xmlpaths[ixmlfile].c_str() );
        if( parseError ) {
          std::cerr << "Loading of xml document \"" << xmlpaths[ixmlfile]
                    << "\" failed" << std::endl;
          return false;
        }

        // Read the Driver XML File
        TXMLDocument* xmldoc = parser[ixmlfile]->GetXMLDocument();
        TXMLNode* rootNode = xmldoc->GetRootNode();

        // Check that it is the proper DOCTYPE
        if( rootNode->GetNodeName() != TString( "WorkspaceBuilder" ) ){
          std::cout << "Error: Driver DOCTYPE not equal to 'WorkspaceBuilder'" << std::endl;
          return false;
        }
        rootNodes.push_back(rootNode);
      }

      std::cout << "INFO: looking for files to include" << std::endl;
      std::vector<TString> allincludefiles;
      unsigned int nfiles=xmlpaths.size();
      for(unsigned ixmlfile=0; ixmlfile<nfiles; ixmlfile++){
        readIncludeFiles(rootNodes[ixmlfile],xmlpaths[ixmlfile], xmlpaths, rootNodes, parser);
      }

      std::cout << "INFO: reading StatAnalysis now" << std::endl;

      bool ret=false;
      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        ret|=readStatAnalysisInformation(rootNodes[ixmlfile]);
      }
      if(!ret){
        std::cout << "ERROR: StatAnalysis with name \"" << m_statanalysisname << "\" was not found in xml definition." << std::endl;
        return false;
      }

      std::cout << "INFO: reading channels now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readChannelInformation(rootNodes[ixmlfile]);
        if(!ret)
          return false;
      }

      checkIfAllChannelsWereUsed();

      std::cout << "INFO: reading data now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readDataInformation(rootNodes[ixmlfile]);
        if(!ret)
          return false;
      }

      std::cout << "INFO: reading samples now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readSampleInformation(rootNodes[ixmlfile],mcstat);
        if(!ret)
          return false;
      }

      std::cout << "INFO: reading systematics now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readSystematicInformation(rootNodes[ixmlfile], usertools);
        if(!ret)
          return false;
      }

      std::cout << "INFO: reading normfactors now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readNormFactorInformation(rootNodes[ixmlfile]);
        if(!ret)
          return false;
      }

      std::cout << "INFO: reading shapefactors now" << std::endl;

      for(unsigned ixmlfile=0; ixmlfile<xmlpaths.size(); ixmlfile++){
        bool ret=readShapeFactorInformation(rootNodes[ixmlfile]);
        if(!ret)
          return false;
      }

      for(unsigned int i=0; i<parser.size(); i++)
        if(parser[i]) { delete parser[i]; parser[i] = NULL; }
      parser.clear();
    }
    catch(std::exception& e){
      std::cout << e.what() << std::endl;
      return false;
    }

    return true;
  }// end of void StatAnalysis::readInformationFromXML(std::vector<std::string> xmlpaths, std::string statanalysisname)

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readIncludeFiles(TXMLNode* rootNode, TString xmlinputpath, std::vector<std::string> &xmlpaths, std::vector<TXMLNode*> &rootNodes, std::vector<TDOMParser*> &parser){
    std::vector<TString> includefiles;
    TXMLNode* node = NULL;
    xmlinputpath.Resize(xmlinputpath.Last('/'));

    // Find the right StatAnalysis node
    std::vector< std::string > xml_channel_files;
    node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "IncludeFile" ) ) {
        //get attribute
        if( node->GetText() == NULL ) {
          std::cout << "Error: node: " << node->GetName()
                    << " has no text." << std::endl;
          return false;
        }
        TString xmlpath=xmlinputpath+"/"+node->GetText();
        xmlpaths.push_back(xmlpath.Data());
        parser.push_back(new TDOMParser());
        Int_t parseError = parser.back()->ParseFile( xmlpath.Data() );
        if( parseError ) {
          std::cerr << "Loading of xml document \"" << xmlpath.Data()
                    << "\" failed" << std::endl;
          return false;
        }

        // Read the Driver XML File
        TXMLDocument* xmldoc = parser.back()->GetXMLDocument();
        TXMLNode* rootNode = xmldoc->GetRootNode();

        // Check that it is the proper DOCTYPE
        if( rootNode->GetNodeName() != TString( "WorkspaceBuilder" ) ){
          std::cout << "Error: Driver DOCTYPE not equal to 'WorkspaceBuilder'" << std::endl;
          return false;
        }
        rootNodes.push_back(rootNode);
        std::cout << "INFO: Found includefile " << xmlpath <<  std::endl;
        //now look for includefiles in this file (recursively)
        readIncludeFiles(rootNode, xmlpath, xmlpaths,rootNodes, parser);

      } // if node IncludeFile
      node = node->GetNextNode();
    } // loop over nodes
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readStatAnalysisInformation(TXMLNode* rootNode){
    TXMLNode* node = NULL;

    // Find the right StatAnalysis node
    std::vector< std::string > xml_channel_files;
    node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "StatAnalysis" ) ) {
        //get attribute
        std::string name="";
        getXMLStringAttribute(node,"Name",name);
        if(name==m_statanalysisname){
          TXMLNode* childnode = node->GetChildren();
          while( childnode != 0 ) {
            if( childnode->GetNodeName() == TString( "UseChannel" ) ) {
              if( childnode->GetText() == NULL ) {
                std::cout << "Error: node: " << childnode->GetName()
                          << " has no text." << std::endl;
                return false;
              }
              m_channelstouse.push_back(childnode->GetText());
            }//found a UseChannel subtag
            childnode = childnode->GetNextNode();
          }//loop over attributes
          return true;
        } // if this is the correct StatAnalysis
      } // if node StatAnalysis
      node = node->GetNextNode();
    } // loop over nodes

    return false;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readChannelInformation(TXMLNode* rootNode){

    //first look at inputchannels
    TXMLNode* node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "InputChannel" ) ) {
        std::string inputchannelname="";
        if(!getXMLStringAttribute(node, "Name", inputchannelname)){
          std::cout << " Error: Name attribute for 'InputChannel' not found." << std::endl;
          return false;
        }

        std::string s_considerlumi="True";
        bool b_considerlumi=true;
        if(!getXMLStringAttribute(node, "ConsiderLumi", s_considerlumi)){
          s_considerlumi="True";
        }
        if( s_considerlumi=="False" || s_considerlumi=="false" || s_considerlumi=="FALSE"){
          b_considerlumi=false;
        }
        else if( s_considerlumi=="True"  || s_considerlumi=="true"  || s_considerlumi=="TRUE" ){
          b_considerlumi=true;
        }
        else
          std::cout << "Error: For InputChannel '"<<inputchannelname << "' attribute ConsiderLumi was no set to True or False, but '" << s_considerlumi <<"'." << std::endl;

        InputChannel* inputchannel=new InputChannel(inputchannelname, b_considerlumi);

        //add tags and names in file
        TXMLNode* childnode = node->GetChildren();
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "ChannelNameInFile" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            inputchannel->setNameInFile(childnode->GetText());
          }
          if( childnode->GetNodeName() == TString( "Tag" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            inputchannel->addTag(childnode->GetText());
          }
          childnode=childnode->GetNextNode();
        }//loop over child nodes

        m_inputchannels.insert(std::pair<std::string,InputChannel*>(inputchannelname,inputchannel));
      }
      node = node->GetNextNode();
    }

    //now look at channels
    node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "Channel" ) ) {
        std::string channelname="";
        if(!getXMLStringAttribute(node, "Name", channelname)){
          std::cout << " Error: Name attribute for 'Channel' not found." << std::endl;
          return false;
        }

        //check if the defined channel is indeed used in the measurement
        bool channelisused=false;
        for(unsigned int ichannel=0; ichannel<m_channelstouse.size(); ichannel++)
          if(channelname==m_channelstouse[ichannel])
            channelisused=true;

        if(channelisused){
          Channel* channel=new Channel(channelname);

          //get the input channels
          TXMLNode* childnode = node->GetChildren();
          while( childnode != 0 ) {
            if( childnode->GetNodeName() == TString( "UseInputChannel" ) ) {
              if( childnode->GetText() == NULL ) {
                std::cout << "Error: node: " << childnode->GetName()
                          << " has no text." << std::endl;
                return false;
              }
              std::string inputchannelname=childnode->GetText();
              std::map<std::string,InputChannel*>::iterator it=m_inputchannels.find(inputchannelname);
              if(it!=m_inputchannels.end())
                channel->addInputChannel(it->second);
              else{
                std::cout << "ERROR: For channel \""
                          << channelname << "\" the input channel with name \""
                          << inputchannelname << "\" was requested, but not defined." << std::endl;
                return false;
              }
            }
            if( childnode->GetNodeName() == TString( "Tag" ) ) {
              if( childnode->GetText() == NULL ) {
                std::cout << "Error: node: " << childnode->GetName()
                          << " has no text." << std::endl;
                return false;
              }
              channel->addTag(childnode->GetText());
            }
            childnode=childnode->GetNextNode();
          }
          std::cout << "INFO: Using channel with name " << channelname << std::endl;
          m_channels.push_back(channel);
        }
      }//if node name == Channel

      node = node->GetNextNode();
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readDataInformation(TXMLNode* rootNode){
    TXMLNode* node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "Data" ) ) {
        //get the name

        std::string dataname="";
        if(!getXMLStringAttribute(node, "Name", dataname)){
          std::cout << " Error: Name attribute for 'Data' not found." << std::endl;
          return false;
        }

        //get the input channels
        TXMLNode* childnode = node->GetChildren();
        Data* data=new Data(dataname);
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "InputData" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "InputChannel", channel);

            //get the real channel
            data->addInputData(childnode->GetText(), channel);
          }
          childnode=childnode->GetNextNode();
        }

        for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
          Data *newdata=new Data(*data);
          newdata->setChannel(m_channels[ichannel]->name(), m_channels[ichannel]);
          m_channels[ichannel]->setData(newdata);
        }

        if(data) { delete data; data = NULL;}
      }
      node = node->GetNextNode();
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readSampleInformation(TXMLNode* rootNode, bool mcstat){

    //first look at inputsamples
    TXMLNode* node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "InputSample" ) ) {
        std::string inputsamplename="";
        if(!getXMLStringAttribute(node, "Name", inputsamplename)){
          std::cout << " Error: Name attribute for 'InputSample' not found." << std::endl;
          return false;
        }

        InputSample* inputsample = new InputSample(inputsamplename);

        //add tags and names in file
        TXMLNode* childnode = node->GetChildren();
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "SampleNameInFile" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }

            std::string inputchannelname="All";
            getXMLStringAttribute(childnode,"InputChannel",inputchannelname);
            inputsample->addNameInFile(childnode->GetText(), inputchannelname);
          }

          if( childnode->GetNodeName() == TString( "Tag" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            inputsample->addTag(childnode->GetText());
          }

          if( childnode->GetNodeName() == TString( "ApplyToInputChannel" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            inputsample->addApplyToInputChannel(childnode->GetText());
          }

          childnode=childnode->GetNextNode();
        }//loop over child nodes

        m_inputsamples.insert(std::pair<std::string,InputSample*>(inputsamplename,inputsample));
      }
      node = node->GetNextNode();
    }

    node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "Sample" ) ) {
        //get the name

        std::string samplename="";
        if(!getXMLStringAttribute(node, "Name", samplename)){
          std::cout << " Error: Name attribute for 'Sample' not found." << std::endl;
          return false;
        }

        std::string s_normalizebytheory="False";
        bool normalizebytheory=false;
        getXMLStringAttribute(node, "NormalizeByTheory",s_normalizebytheory);
        if( s_normalizebytheory=="False" || s_normalizebytheory=="false" || s_normalizebytheory=="FALSE"){
          normalizebytheory=false;
        }
        else if( s_normalizebytheory=="True"  || s_normalizebytheory=="true"  || s_normalizebytheory=="TRUE" ){
          normalizebytheory=true;
        }
        else
          std::cout << "Error: For Sample '"<<samplename << "' attribute NormalizeByTheory was no set to True or False, but '" << s_normalizebytheory<<"'." << std::endl;

        std::string s_staterror="False";
        bool staterror=false;
        if(mcstat){
          getXMLStringAttribute(node, "StatError",s_staterror);
          if     ( s_staterror=="False" || s_staterror=="false" || s_staterror=="FALSE"){
            staterror=false;
          }
          else if( s_staterror=="True"  || s_staterror=="true"  || s_staterror=="TRUE" ){
            staterror=true;
          }
          else
            std::cout << "Error: For Sample '"<<samplename << "' attribute StatError was no set to True or False, but '" << s_staterror<<"'." << std::endl;
        }
        //else do not use MC stat error

        std::string s_signal="False";
        bool signal=false;
        getXMLStringAttribute(node, "Signal",s_signal);
        if     ( s_signal=="False" || s_signal=="false" || s_signal=="FALSE"){
          signal=false;
        }
        else if( s_signal=="True"  || s_signal=="true"  || s_signal=="TRUE" ){
          signal=true;
        }
        else
          std::cout << "Error: For Sample '"<<samplename << "' attribute Signal was no set to True or False, but '" << s_signal<<"'." << std::endl;

        //get the input channels
        TXMLNode* childnode = node->GetChildren();
        Sample sample=Sample(samplename, normalizebytheory, staterror, signal);
        std::vector<std::string> applytochannel;
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "Tag" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            sample.addTag(childnode->GetText());
          }
          if( childnode->GetNodeName() == TString( "UseInputSample" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }

            std::string inputsamplename=childnode->GetText();
            std::map<std::string,InputSample*>::iterator it=m_inputsamples.find(inputsamplename);
            if(it!=m_inputsamples.end())
              sample.addInputSample(it->second);
            else{
              std::cout << "ERROR: For sample \"" << samplename << "\" the input sample with name \"" << inputsamplename << "\" was requested, but not defined." << std::endl;
              return false;
            }
          }
          if( childnode->GetNodeName() == TString( "ApplyToChannel" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }

            applytochannel.push_back(childnode->GetText());
          }

          childnode=childnode->GetNextNode();
        }

        for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
          bool applysampletothischannel=false;
          //first look if there is any input channel
          for(unsigned int iinputchannel=0; iinputchannel<m_channels[ichannel]->inputChannels().size(); iinputchannel++){
            InputChannel* inputchannel=m_channels[ichannel]->inputChannels()[iinputchannel];
            for(unsigned int iinputsample=0; iinputsample<sample.inputSamples().size(); iinputsample++){
              if(sample.inputSamples()[iinputsample]->applyToInputChannel(inputchannel)){
                applysampletothischannel=true;
              }
            }
          }

          //now check if it is explicitly applied
          if(applytochannel.size()>0){//if no such tag was given, we assume that it is applied to all samples
            bool apply=false;
            for(unsigned int i=0; i<applytochannel.size(); i++){
              if(applytochannel[i]=="All" || applytochannel[i]==m_channels[ichannel]->name() || m_channels[ichannel]->hasTag(applytochannel[i])) // tags are not considered
                apply=true;
            }
            applysampletothischannel=applysampletothischannel && apply;
          }

          if(applysampletothischannel){
            Sample *newsample=new Sample(sample);
            newsample->setChannel(m_channels[ichannel]->name(), m_channels[ichannel]);
            m_channels[ichannel]->addSample(newsample);
          }
        }
      }
      node = node->GetNextNode();
    }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readSystematicInformation(TXMLNode* rootNode, UserTools* usertools){
    TXMLNode* node = rootNode->GetChildren();
    std::vector<Systematic*> allsystematics;

    while( node != 0 ) {
      if( node->GetNodeName() == TString( "Systematic" ) ) {
        //get the name

        std::string systematicname="";
        if(!getXMLStringAttribute(node, "Name", systematicname)){
          std::cout << " Error: Name attribute for 'Systematic' not found." << std::endl;
          return false;
        }

        std::string forceoverallstring="";
        if(!getXMLStringAttribute(node, "ForceOverall",forceoverallstring)){
          forceoverallstring="False";
        }

        std::string onesidedstring="";
        if(!getXMLStringAttribute(node, "OneSided", onesidedstring)){
          onesidedstring="False";
        }

        std::string shapeonlystring="";
        if(!getXMLStringAttribute(node, "ShapeOnly", shapeonlystring)){
          shapeonlystring="False";
        }

        std::string symmetrizestring="";
        if(!getXMLStringAttribute(node, "Symmetrize", symmetrizestring)){
          symmetrizestring="False";
        }

        std::string npattributesstring="";
        if(!getXMLStringAttribute(node, "NPAttributes", npattributesstring)){
          npattributesstring="exp";
        }

        std::stringstream ss(npattributesstring);
        std::string item;
        std::vector<std::string> npattributes;
        while (getline(ss, item, ',')) {
          npattributes.push_back(item);
        }

        //get the input channels
        TXMLNode* childnode = node->GetChildren();
        TString newsystematicname=usertools->correlatedSystematicName(systematicname);
        Systematic* systematic=0;
        //check if this systematic already exists
        unsigned int isyst=0;
        bool systematicalreadyused=false;
        while(systematic==0 && isyst<allsystematics.size()){
          if(allsystematics[isyst]->name()==newsystematicname){
            systematic=allsystematics[isyst];
            systematicalreadyused=true;
          }
          isyst++;
        }
        if(systematic==0)
          systematic=new Systematic(newsystematicname.Data());
        for(unsigned int iattribute=0; iattribute<npattributes.size(); iattribute++)
          systematic->addAttribute(npattributes[iattribute]);

        if(forceoverallstring=="True" || forceoverallstring=="TRUE" || forceoverallstring=="true" )
          systematic->setForceOverall(true);
        else
          systematic->setForceOverall(false);

        if(onesidedstring=="True" || onesidedstring=="TRUE" || onesidedstring=="true" )
          systematic->setOneSided(true);
        else
          systematic->setOneSided(false);

        if(shapeonlystring=="True" || shapeonlystring=="TRUE" || shapeonlystring=="true" )
          systematic->setShapeOnly(true);
        else
          systematic->setShapeOnly(false);

        if(symmetrizestring=="True" || symmetrizestring=="TRUE" || symmetrizestring=="true" )
          systematic->setSymmetrize(true);
        else
          systematic->setSymmetrize(false);

        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "SystematicNameInFile" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "InputChannel", channel);
            std::string sample="All";
            getXMLStringAttribute(childnode, "InputSample", sample);

            //get the real channel
            systematic->addSystematicNameInFile(childnode->GetText(), sample, channel);
          }
          if( childnode->GetNodeName() == TString( "ApplyToInputSample" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << node->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "InputChannel", channel);
            systematic->addApplyToSample(childnode->GetText(),channel);
          }
          if( childnode->GetNodeName() == TString( "ApplyToInputSampleExcept" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << node->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "InputChannel", channel);
            systematic->addApplyToSampleExcept(childnode->GetText(),channel);
          }
          if( childnode->GetNodeName() == TString( "Tag" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << node->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            systematic->addTag(childnode->GetText());
          }
          childnode=childnode->GetNextNode();
        }
        if(!systematicalreadyused)
          allsystematics.push_back(systematic);
      }
      node=node->GetNextNode();
    }

    std::set<TString> addedsystematics;
    for(unsigned int isystematic=0; isystematic<allsystematics.size(); isystematic++){
      Systematic* systematic=allsystematics.at(isystematic);
      for(unsigned int iapply=0; iapply<systematic->applyToInputChannels()->size(); iapply++){
        std::string applytoinputchannel=systematic->applyToInputChannels()->at(iapply);
        std::string applytoinputsample=systematic->applyToInputSamples()->at(iapply);

        //bool samplefound=false;
        for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){

          for(unsigned int iinputchannel=0; iinputchannel<m_channels[ichannel]->inputChannels().size(); iinputchannel++){
            //first check if a tag matches
            InputChannel* inputchannel=m_channels[ichannel]->inputChannels()[iinputchannel];
            bool inputchanneltagfound=inputchannel->hasTag(applytoinputchannel);

            if(inputchanneltagfound || m_channels[ichannel]->inputChannels()[iinputchannel]->name()==applytoinputchannel|| applytoinputchannel=="All"){
              for(unsigned int isample=0; isample<m_channels.at(ichannel)->samples()->size(); isample++){

                for(unsigned int iinputsample=0; iinputsample<m_channels.at(ichannel)->samples()->at(isample)->inputSamples().size(); iinputsample++){
                  InputSample* inputsample=m_channels.at(ichannel)->samples()->at(isample)->inputSamples()[iinputsample];
                  bool inputsampletagfound=inputsample->hasTag(applytoinputsample);
                  if( inputsampletagfound || m_channels[ichannel]->samples()->at(isample)->inputSamples()[iinputsample]->name()==applytoinputsample|| applytoinputsample=="All"){
                    //look for exception
                    bool exception=false;
                    for(unsigned int iexceptapply=0; iexceptapply<systematic->applyToInputChannelsExcept()->size(); iexceptapply++){
                      std::string applytoinputchannelexcept=systematic->applyToInputChannelsExcept()->at(iexceptapply);
                      std::string applytoinputsampleexcept=systematic->applyToInputSamplesExcept()->at(iexceptapply);
                      bool inputsampleexcepttagfound=inputsample->hasTag(applytoinputsampleexcept);
                      bool inputchannelexcepttagfound=inputchannel->hasTag(applytoinputchannelexcept);
                      if( (inputsampleexcepttagfound || m_channels[ichannel]->samples()->at(isample)->inputSamples()[iinputsample]->name()==applytoinputsampleexcept || applytoinputsampleexcept=="All" ) &&
                          (inputchannelexcepttagfound || m_channels[ichannel]->inputChannels()[iinputchannel]->name()==applytoinputchannelexcept || applytoinputchannelexcept=="All") )
                        exception=true;
                    }
                    if(!exception){
                      TString systsample=m_channels[ichannel]->name()+"_"+m_channels[ichannel]->samples()->at(isample)->name()+"_"+systematic->name();
                      if(addedsystematics.find(systsample)==addedsystematics.end()){
                        //a systematic is added if it applies to any input sample in any input channel
                        Systematic *newsystematic=new Systematic(*systematic);
                        newsystematic->setChannel(m_channels[ichannel]->name(),m_channels[ichannel]);
                        newsystematic->setSample(m_channels[ichannel]->samples()->at(isample)->name(),
                                                 m_channels[ichannel]->samples()->at(isample));
                        m_channels[ichannel]->samples()->at(isample)->addSystematic(newsystematic);
                        addedsystematics.insert(systsample);
                      }//if
                    }//if not exception
                  } //if inputsampletagfound
                } //for loop over input sample
              }//for loop over sample
            }//inputchanneltagfound
          }//for inputchannel
        } //for channel
      }//loop over all apply statement
      if(systematic) { delete systematic; systematic = NULL;}
    } //loop over all systematics
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readNormFactorInformation(TXMLNode* rootNode){
    TXMLNode* node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "NormFactor" ) ) {
        //get the name

        std::string normfactorname="";
        if(!getXMLStringAttribute(node, "Name", normfactorname)){
          std::cout << " Error: Name attribute for 'NormFactor' not found." << std::endl;
          return false;
        }

        std::string s_const="false";
        bool b_const=false;
        getXMLStringAttribute(node, "Const", s_const);
        std::cout << "Info: NormFactor " << normfactorname << " const=" << s_const << std::endl;
        if( s_const=="False" || s_const=="false" || s_const=="FALSE"){
          b_const=false;
        }
        else if( s_const=="True"  || s_const=="true"  || s_const=="TRUE" ){
          b_const=true;
        }
        else
          std::cout << "Error: For NormFactor '"<<normfactorname << "' attribute Const was no set to True or False, but '" << s_const <<"'." << std::endl;

        std::string s_poi="false";
        bool b_poi=false;
        getXMLStringAttribute(node, "POI", s_poi);
        if( s_poi=="False" || s_poi=="false" || s_poi=="FALSE"){
          b_poi=false;
        }
        else if( s_poi=="True"  || s_poi=="true"  || s_poi=="TRUE" ){
          b_poi=true;
        }
        else
          std::cout << "Error: For NormFactor '"<<normfactorname << "' attribute POI was no set to True or False, but '" << s_poi <<"'." << std::endl;


        //get the input channels
        TXMLNode* childnode = node->GetChildren();
        NormFactor* normfactor=new NormFactor(normfactorname, b_const, b_poi);
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "NormFactorNameInFile" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << childnode->GetName()
                        << " has no text." << std::endl;
              return false;
            }

            //get the real channel
            normfactor->setNameInFile(childnode->GetText());
          }
          if( childnode->GetNodeName() == TString( "ApplyToSample" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << node->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "Channel", channel);
            normfactor->addApplyToSample(childnode->GetText(),channel);
          }
          childnode=childnode->GetNextNode();
        }

        for(unsigned int iapply=0; iapply<normfactor->applyToChannels()->size(); iapply++){
          std::string applytochannel=normfactor->applyToChannels()->at(iapply);
          std::string applytosample=normfactor->applyToSamples()->at(iapply);

          //bool samplefound=false;
          for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
            if(m_channels[ichannel]->name()==applytochannel|| m_channels[ichannel]->hasTag(applytochannel) || applytochannel=="All"){
              for(unsigned int isample=0; isample<m_channels.at(ichannel)->samples()->size(); isample++){
                if(m_channels[ichannel]->samples()->at(isample)->name()==applytosample || m_channels[ichannel]->samples()->at(isample)->hasTag(applytosample) || applytosample=="All"){
                  NormFactor *newnormfactor=new NormFactor(*normfactor);
                  m_channels[ichannel]->samples()->at(isample)->addNormFactor(newnormfactor);
                }
              }
            }
          }
        }
        if(normfactor) {delete normfactor; normfactor = NULL;}
      }
      node=node->GetNextNode();
    }
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readShapeFactorInformation(TXMLNode* rootNode){
    TXMLNode* node = rootNode->GetChildren();
    while( node != 0 ) {
      if( node->GetNodeName() == TString( "ShapeFactor" ) ) {
        //get the name

        std::string shapefactorname="";
        if(!getXMLStringAttribute(node, "Name", shapefactorname)){
          std::cout << " Error: Name attribute for 'ShapeFactor' not found." << std::endl;
          return false;
        }

        std::string s_const="false";
        bool b_const=false;
        getXMLStringAttribute(node, "Const", s_const);
        //std::cout << "Normfactor " << normfactorname << " const=" << s_const << std::endl;
        if( s_const=="False" || s_const=="false" || s_const=="FALSE"){
          b_const=false;
        }
        else if( s_const=="True"  || s_const=="true"  || s_const=="TRUE" ){
          b_const=true;
        }
        else
          std::cout << "Error: For ShapeFactor '"
                    <<shapefactorname
                    << "' attribute Const was not set to True or False, but '"
                    << s_const
                    <<"'."
                    << std::endl;

        //get the input channels
        TXMLNode* childnode = node->GetChildren();
        ShapeFactor* shapefactor=new ShapeFactor(shapefactorname, b_const);
        while( childnode != 0 ) {
          if( childnode->GetNodeName() == TString( "ApplyToSample" ) ) {
            if( childnode->GetText() == NULL ) {
              std::cout << "Error: node: " << node->GetName()
                        << " has no text." << std::endl;
              return false;
            }
            std::string channel="All";
            getXMLStringAttribute(childnode, "Channel", channel);
            shapefactor->addApplyToSample(childnode->GetText(),channel);
          }
          childnode=childnode->GetNextNode();
        }

        for(unsigned int iapply=0; iapply<shapefactor->applyToChannels()->size(); iapply++){
          std::string applytochannel=shapefactor->applyToChannels()->at(iapply);
          std::string applytosample=shapefactor->applyToSamples()->at(iapply);

          //bool samplefound=false;
          for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
            if(m_channels[ichannel]->name()==applytochannel|| m_channels[ichannel]->hasTag(applytochannel) || applytochannel=="All"){
              for(unsigned int isample=0; isample<m_channels.at(ichannel)->samples()->size(); isample++){
                if(m_channels[ichannel]->samples()->at(isample)->name()==applytosample || m_channels[ichannel]->samples()->at(isample)->hasTag(applytosample) || applytosample=="All"){
                  ShapeFactor *newshapefactor=new ShapeFactor(*shapefactor);
                  m_channels[ichannel]->samples()->at(isample)->addShapeFactor(newshapefactor);
                }
              }
            }
          }
        }
        if(shapefactor) {delete shapefactor; shapefactor = NULL;}
      }
      node=node->GetNextNode();
    }
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::readHistogramsFromRootFile(UserTools* usertools){
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++)
      if(!m_channels.at(ichannel)->readHistogramsFromRootFile(usertools))
        return false;

    //Check if all input histogram has same number of bins and x-axis range
    bool hasInconsistentAxis = false;
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      for(unsigned int isample=0; isample<m_channels[ichannel]->samples()->size(); isample++){
        TH1F* inputHist = m_channels[ichannel]->samples()->at(isample)->inputhist();
        for(unsigned int isyst=0; isyst<m_channels[ichannel]->samples()->at(isample)->systematics()->size(); isyst++){
          TH1F* systup_inputHist = m_channels[ichannel]->samples()->at(isample)->systematics()->at(isyst)->getUpInputHistogram();
          TH1F* systdown_inputHist = m_channels[ichannel]->samples()->at(isample)->systematics()->at(isyst)->getDownInputHistogram();
          Int_t nbins[3] = {0};
          Double_t lowedge[3] = {0.};
          Double_t upedge [3] = {0.};
          nbins[0] = inputHist         ->GetNbinsX();
          nbins[1] = systup_inputHist  ->GetNbinsX();
          nbins[2] = systdown_inputHist->GetNbinsX();
          lowedge[0] = inputHist         ->GetBinLowEdge(1);
          lowedge[1] = systup_inputHist  ->GetBinLowEdge(1);
          lowedge[2] = systdown_inputHist->GetBinLowEdge(1);
          upedge[0] = inputHist         ->GetBinLowEdge(inputHist         ->GetNbinsX()+1);
          upedge[1] = systup_inputHist  ->GetBinLowEdge(systup_inputHist  ->GetNbinsX()+1);
          upedge[2] = systdown_inputHist->GetBinLowEdge(systdown_inputHist->GetNbinsX()+1);
          if(!(nbins[0]==nbins[1] && nbins[0]==nbins[2])){
            std::cout<<"Error: #Bins in nomianl and variation histograms don't match!!"<<std::endl;
            std::cout<<m_channels[ichannel]->name()<<", "<<m_channels[ichannel]->samples()->at(isample)->name()<<", "
                     <<m_channels[ichannel]->samples()->at(isample)->systematics()->at(isyst)->name()<<std::endl;
            std::cout<<"Nominal="<<nbins[0]<<", Up="<<nbins[1]<<", Down="<<nbins[2]<<std::endl;
            hasInconsistentAxis = true;
          }
          if(!(lowedge[0]==lowedge[1] && lowedge[0]==lowedge[2])){
            std::cout<<"Error: Xaxis range in nomianl and variation histograms don't match!!"<<std::endl;
            std::cout<<m_channels[ichannel]->name()<<", "<<m_channels[ichannel]->samples()->at(isample)->name()<<", "
                     <<m_channels[ichannel]->samples()->at(isample)->systematics()->at(isyst)->name()<<std::endl;
            std::cout<<"Nominal=("<<lowedge[0]<<","<<upedge[0]<<"), Up=("<<lowedge[1]<<","<<upedge[1]<<"), Down=("<<lowedge[2]<<","<<upedge[2]<<")"<<std::endl;
            hasInconsistentAxis = true;
          }
        }
      }
    }
    if(hasInconsistentAxis) return false;

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::deriveHistograms(UserTools* usertools, float lumi){

    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++)
      if(!m_channels.at(ichannel)->deriveHistograms(usertools, lumi)){
        std::cerr<<"ERROR! StatAnalysis::deriveHistograms() - Unable to derive histograms for channel "
                 <<ichannel<<" '"<<m_channels.at(ichannel)->name()<<"'"<<std::endl;
        return false;
      }

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::pruneSystematics(UserTools* usertools){
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++)
      if(!m_channels.at(ichannel)->pruneSystematics(usertools))
        return false;
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::deriveFinalHistograms(UserTools* usertools){
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++)
      if(!m_channels.at(ichannel)->deriveFinalHistograms(usertools))
        return false;

    return true;
  }


  //-------------------------------------------------------------------------------

  bool StatAnalysis::writeHistogramsToRootFile(){
    //first make sure the directory exists
    TString command="mkdir -p ";
    command+=m_outputpath;
    command+=m_version;
    command+="/xml";
    system(command.Data());

    //now write ther root file
    TString rootfilename=m_outputpath;
    rootfilename+=m_version;
    rootfilename+="/xml/";
    rootfilename+=m_statanalysisname;
    rootfilename+="_histofile.root";
    TFile* file=new TFile(rootfilename.Data(),"RECREATE");
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++)
      if(!m_channels.at(ichannel)->writeHistogramsToRootFile(file))
        return false;
    file->Write();
    file->Close();
    if(file) {delete file; file = NULL;}
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::createModelConfig(UserTools* usertools){

    m_histfactory_measurement=RooStats::HistFactory::Measurement(m_statanalysisname.c_str(),m_statanalysisname.c_str());
    std::set<std::string> pois;
    std::set<std::string> constparams;
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      for(unsigned int isample=0; isample<m_channels[ichannel]->samples()->size(); isample++){
        for(unsigned int inormfactor=0; inormfactor<m_channels[ichannel]->samples()->at(isample)->normfactors()->size(); inormfactor++){
          if(m_channels[ichannel]->samples()->at(isample)->normfactors()->at(inormfactor)->isPOI())
            pois.insert(m_channels[ichannel]->samples()->at(isample)->normfactors()->at(inormfactor)->name());
          if(m_channels[ichannel]->samples()->at(isample)->normfactors()->at(inormfactor)->isConst() || usertools->isNuisanceParameterConstant(m_channels[ichannel]->samples()->at(isample)->normfactors()->at(inormfactor)->name()) )
            constparams.insert(m_channels[ichannel]->samples()->at(isample)->normfactors()->at(inormfactor)->name());
        }
        for(unsigned int isystematic=0; isystematic<m_channels[ichannel]->samples()->at(isample)->systematics()->size(); isystematic++){
          if(!m_channels[ichannel]->samples()->at(isample)->systematics()->at(isystematic)->isPrunedAway() && usertools->isNuisanceParameterConstant(m_channels[ichannel]->samples()->at(isample)->systematics()->at(isystematic)->name()) )
            constparams.insert("alpha_"+m_channels[ichannel]->samples()->at(isample)->systematics()->at(isystematic)->name());
        }
      }
    }

    std::set<std::string>::iterator it;
    for (it = pois.begin(); it != pois.end(); ++it){
      m_histfactory_measurement.AddPOI((*it).c_str());
    }

    //set parameters constant
    for (it = constparams.begin(); it != constparams.end(); ++it){
      m_histfactory_measurement.AddConstantParam((*it).c_str());
    }
    m_histfactory_measurement.AddConstantParam("Lumi");

    m_histfactory_measurement.SetExportOnly(true);
    m_histfactory_measurement.SetLumi(1.); //lumi uncertainty is added as extra systematic so that different sqrt(s) can be combined
    m_histfactory_measurement.SetLumiRelErr(1.);
    m_histfactory_measurement.SetOutputFilePrefix(m_statanalysisname);

    //add channels
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      m_histfactory_measurement.AddChannel(m_channels[ichannel]->getHistFactoryChannel(m_outputpath, m_version, m_statanalysisname));
    }

    if(!m_verbose){
      std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
      std::ofstream out("/dev/null");
      std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
      m_histfactory_measurement.CollectHistograms();
      std::cout.rdbuf(coutbuf); //reset to standard output again
    }
    else
      m_histfactory_measurement.CollectHistograms();

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::writeMeasurement(){
    TString dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+=m_version;
    dircmd+="/monitoring/";
    system(dircmd.Data());

    TString filename=m_outputpath;
    filename+=m_version;
    filename+="/monitoring/measurement.root";

    TFile *outfile=new TFile(filename, "RECREATE");
    m_histfactory_measurement.writeToFile(outfile);

    /*
      RooStats::HistFactory::Measurement measurement=m_histfactory_measurement;
      measurement.SetDirectory(outfile);*/

    outfile->Write();
    outfile->Close();
    if(outfile) {delete outfile; outfile = NULL;}

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::writeWorkspace(UserTools* usertools){

    RooStats::HistFactory::HistoToWorkspaceFactoryFast hist2workspace;// = RooStats::HistFactory::HistoToWorkspaceFactory::HistoToWorkspaceFactory(m_histfactory_measurement);
    RooWorkspace* workspace=0;
    //w->Print("t") ;
    TString dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+=m_version;
    dircmd+="/";
    dircmd+=m_statanalysisname;
    dircmd+="/combined/";
    system(dircmd.Data());

    TString filename=m_outputpath;
    filename+=m_version;
    filename+="/";
    filename+=m_statanalysisname;
    filename+="/combined/";
    filename+=usertools->getMainParameter();
    filename+=".root";

    if(!m_verbose){
      std::streambuf *coutbuf = std::cout.rdbuf(); //save old buf
      std::ofstream out("/dev/null");
      std::cout.rdbuf(out.rdbuf()); //redirect std::cout to out.txt!
      workspace=hist2workspace.MakeCombinedModel(m_histfactory_measurement);
      std::cout.rdbuf(coutbuf); //reset to standard output again
    }
    else
      workspace=hist2workspace.MakeCombinedModel(m_histfactory_measurement);


    // Loop over NPs and remove alpha_ prefix, as requested by HComb
    // Also add the requested attributes
    RooStats::ModelConfig* mc = (RooStats::ModelConfig*)workspace->obj("ModelConfig");
    RooArgSet* NPs = (RooArgSet*)mc->GetNuisanceParameters();
    RooArgSet* POIs = (RooArgSet*)mc->GetParametersOfInterest();

    // loop over NPs to conform to the HComb specifications for workspaces.
    if(NPs ){ //protection over NPs

      TIterator* nit = NPs->createIterator();
      if(!nit){
        Error("StatAnalysis::writeWorkspace", "Null NPs iterator!");
        return false;
      }

      Info("StatAnalysis::writeWorkspace", "Looping over %u NPs for the HComb specifications", NPs->getSize ());

      RooAbsArg* n;
      while((n = (RooAbsArg*)nit->Next() )) { // double parenthesis to make compiler happy
        //      std::cout<<n->GetName();
        TString name = n->GetName();
        name.ReplaceAll("alpha_","");
        n->SetName(name);
        n->setAttribute("NP"); // incredibly, it's SetName but setAttribute...

        std::set<std::string> attributes;

        for(unsigned int ichannel=0; ichannel<m_channels.size() && attributes.size()==0; ichannel++)
          for(unsigned int isample=0; isample<m_channels[ichannel]->samples()->size() && attributes.size()==0; isample++)
            for(unsigned int isystematic=0; isystematic<m_channels[ichannel]->samples()->at(isample)->systematics()->size() && attributes.size()==0; isystematic++){
              if(m_channels[ichannel]->samples()->at(isample)->systematics()->at(isystematic)->name()==name)
                attributes=m_channels[ichannel]->samples()->at(isample)->systematics()->at(isystematic)->attributes();
            }

        if(m_verbose ) std::cout << " -> " << n->GetName() << ":";
        std::set<std::string>::iterator it;
        for (it=attributes.begin(); it!=attributes.end(); ++it){
          n->setAttribute((*it).c_str());
          if(m_verbose) std::cout << ' ' << *it;
        }

        if(m_verbose) std::cout << std::endl;
      }
      if(nit) {delete nit; nit = NULL;}

      std::cout<<"INFO: Have removed alpha_ prefix from NP names and set attributes."<<std::endl;

    }else{
      Warning("StatAnalysis::writeWorkspace", "Null NPs RooArgSet pointer - won't proceed to the HComb part.");
    }


    // loop over POIs
    TIterator* pit = POIs->createIterator();
    RooAbsArg* p;
    while((p = (RooAbsArg*)pit->Next() )) {
      p->setAttribute("POI");
    }
    if(pit) {delete pit; pit = NULL;}

    if(POIs->getSize() == 0)
      Warning("StatAnalysis::writeWorkspace", "Have found no POIs!");
    else
      Info("StatAnalysis::writeWorkspace", "Have found %u POIs", POIs->getSize());


    workspace->writeToFile(filename.Data());

    //write histograms with binning into this file
    TFile* wsfile=new TFile(filename, "UPDATE");
    wsfile->mkdir("binning");
    TDirectory* bindir=(TDirectory*)wsfile->Get("binning");
    bindir->cd();
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      m_channels[ichannel]->data()->binningHistogram()->SetDirectory(bindir);
      m_channels[ichannel]->data()->binningHistogram()->Write();
      m_channels[ichannel]->data()->binningHistogram()->SetDirectory(0);
    }
    wsfile->Close();

    dircmd="mkdir -p ";
    dircmd+=m_outputpath;
    dircmd+=m_version;
    dircmd+="/combined/";
    system(dircmd.Data());

    dircmd="ln -f -s ../";
    dircmd+=m_statanalysisname;
    dircmd+="/combined/125.root ";
    dircmd+=m_outputpath;
    dircmd+=m_version;
    dircmd+="/combined/";
    system(dircmd.Data());



    //std::cout << "TODO: StatAnalysis::writeWorkspace()" << std::endl;
    //MakeModelAndMeasurementFast(m_histfactory_measurement);
    //void FitModel(RooWorkspace *, std::string data_name="obsData");

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::writeXML(){

    TString prefix=m_outputpath;
    prefix+=m_version;
    prefix+="/xml/";
    prefix+=m_statanalysisname;
    TString path=m_outputpath;
    path+=m_version;
    path+="/xml";
    m_histfactory_measurement.PrintXML(path.Data(),prefix.Data());

    //write histograms with binning into this file
    TFile* wsfile=new TFile(path+"/"+m_statanalysisname+"_binning.root", "RECREATE");
    wsfile->mkdir("binning");
    TDirectory* bindir=(TDirectory*)wsfile->Get("binning");
    bindir->cd();
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      m_channels[ichannel]->data()->binningHistogram()->SetDirectory(bindir);
      m_channels[ichannel]->data()->binningHistogram()->Write();
      m_channels[ichannel]->data()->binningHistogram()->SetDirectory(0);
    }
    wsfile->Close();

    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::analyze(UserTools* usertools){
    usertools->analyze(m_channels);
    return true;
  }

  //-------------------------------------------------------------------------------

  bool StatAnalysis::makeEnvelopePlots(UserTools* usertools){

    std::cout<<"INFO: makeEnvelopePlots() - Making envelope plots for both before and after rebinning/smoothing/Kyle-fixing."<<std::endl;

    std::string outputformat = "pdf"; // png, pdf, ...

    Double_t ksval_threshold = 1.9; //Essentially no threshold for KS prob. for now.

    //first make sure the directory exists
    TString command="mkdir -p ";
    command+=m_outputpath;
    command+=m_version;
    command+="/envelope; rm -f ";
    command+=m_outputpath;
    command+=m_version;
    command+="/envelope/*";
    system(command.Data());

    //Processes and Systematics which are in the vector below will be considered.
    std::vector<TString> keyWordsSyst; keyWordsSyst.clear();
    for(std::map<std::string,std::string>::iterator itr=(usertools->getParameters())->begin(); itr!=(usertools->getParameters())->end(); itr++){
      if(TString(itr->first).Contains("EnvelopePlot",TString::kIgnoreCase)){
          std::string inputPara=itr->second;
          std::istringstream splitParameterList(inputPara);
          std::string parameter;
          
          while(std::getline(splitParameterList,parameter,',')){
            if (parameter.size()!=0)
              keyWordsSyst.push_back(parameter);
            }
        }
    }
    for(UInt_t icnt=0; icnt<keyWordsSyst.size(); icnt++){
      std::cout<<"Will make plots for "<<keyWordsSyst.at(icnt).Data()<<std::endl;
    }
    //Loop over all channels, samples, systematics
    std::vector<std::string> systname_list; systname_list.clear();
    gStyle->SetLegendBorderSize(0);
    gStyle->SetLegendFillColor(0);
    Int_t nHists = 0;
    Int_t nAggrSymmHists = 0;
    for(unsigned int ichannel=0; ichannel<m_channels.size(); ichannel++){
      std::cout<<"Info: makeEnvelopePlots() - "<<m_channels[ichannel]->name()<<" will be processed... "<<std::endl;
      //      std::cout<<"#samples="<<m_channels[ichannel]->samples()->size()<<std::endl;
      unsigned int nSamp = m_channels[ichannel]->samples()->size();
      TString strChanName = m_channels[ichannel]->name();
      //Loop for samples
      for(unsigned int isamp=0; isamp<nSamp; isamp++){
        Sample* tmpSamp = m_channels[ichannel]->samples()->at(isamp);
        TString strSampName = tmpSamp->name();
        unsigned int nSyst = tmpSamp->systematics()->size();
        //Nominal histograms before/after treatment
        TH1F* inputHist = (TH1F*)(tmpSamp->inputhist()->Clone("inputHist"));
        TH1F* finalHist = (TH1F*)(tmpSamp->finalhist()->Clone("finalHist"));
        if(inputHist->GetSumOfWeights()==0.){
          std::cout<<"Info: makeEnvelopePlots() - "
                   <<m_channels[ichannel]->name()
                   <<": "<<strSampName.Data()<<" histogram was skipped because of SumOfWeight=0."<<std::endl;
          inputHist->Delete();
          finalHist->Delete();
          continue;
        }

        //Loop for systematics
        for(unsigned int isyst=0; isyst<nSyst; isyst++){
          Systematic* tmpSyst = tmpSamp->systematics()->at(isyst);
          TString strSystName = tmpSyst->name();
          nHists++;
          if(tmpSyst->isAggressiveSymm() && tmpSyst->isHistoSys()) nAggrSymmHists++;

          TH1F* systup_inputHist = (TH1F*)(tmpSyst->getUpInputHistogram()->Clone("systup_inputHist"));
          TH1F* systup_finalHist = (TH1F*)(tmpSyst->getUpHistogram()->Clone("systup_finalHist"));
          TH1F* systdown_inputHist = (TH1F*)(tmpSyst->getDownInputHistogram()->Clone("systdown_inputHist"));
          TH1F* systdown_finalHist = (TH1F*)(tmpSyst->getDownHistogram()->Clone("systdown_finalHist"));
          systup_finalHist->Scale(tmpSyst->getUpNormalisation());
          systdown_finalHist->Scale(tmpSyst->getDownNormalisation());

          TString HistOutName = Form("%s%s/envelope/envelope_%s_%s_%s_accepted.%s",
                                     m_outputpath.c_str(),
                                     m_version.c_str(),
                                     tmpSyst->name().c_str(),
                                     m_channels[ichannel]->name().c_str(),
                                     tmpSamp->name().c_str(),
                                     outputformat.c_str());
          systname_list.push_back(tmpSyst->name());

          //Skipping if it's not specified NPs
          Bool_t skipped = keyWordsSyst.size()==0 ? kFALSE : kTRUE;
          for(UInt_t ikeysys=0; ikeysys<keyWordsSyst.size(); ikeysys++){
            if(HistOutName.Contains(keyWordsSyst.at(ikeysys).Data())) skipped = kFALSE;
          }
          if(skipped) continue;

          //KS probability computation
          Double_t KS_systup_input = inputHist->KolmogorovTest(systup_inputHist,"N");
          Double_t KS_systup_final = finalHist->KolmogorovTest(systup_finalHist,"N");
          Double_t KS_systdown_input = inputHist->KolmogorovTest(systdown_inputHist,"N");
          Double_t KS_systdown_final = finalHist->KolmogorovTest(systdown_finalHist,"N");

          if(KS_systup_input<ksval_threshold || KS_systup_final<ksval_threshold || KS_systdown_input<ksval_threshold || KS_systdown_final<ksval_threshold){
            TString tmpChanName = m_channels[ichannel]->name();

            TH1F* finalHist_forDrawingOnInput         =NULL;
            TH1F* systdown_finalHist_forDrawingOnInput=NULL;
            TH1F* systup_finalHist_forDrawingOnInput  =NULL;


            finalHist_forDrawingOnInput          = (TH1F*)inputHist->Clone("finalHist_forDrawingOnInput");
            systdown_finalHist_forDrawingOnInput = (TH1F*)inputHist->Clone("systdown_finalHist_forDrawingOnInput");
            systup_finalHist_forDrawingOnInput   = (TH1F*)inputHist->Clone("systup_finalHist_forDrawingOnInput");
            finalHist_forDrawingOnInput         ->Reset();
            systdown_finalHist_forDrawingOnInput->Reset();
            systup_finalHist_forDrawingOnInput  ->Reset();
            const Int_t nbins = inputHist->GetNbinsX();
            for(Int_t ibin=1; ibin<=nbins; ibin++){
              finalHist_forDrawingOnInput         ->SetBinContent(ibin,finalHist         ->GetBinContent(ibin));
              systdown_finalHist_forDrawingOnInput->SetBinContent(ibin,systdown_finalHist->GetBinContent(ibin));
              systup_finalHist_forDrawingOnInput  ->SetBinContent(ibin,systup_finalHist  ->GetBinContent(ibin));
              finalHist_forDrawingOnInput         ->SetBinError(ibin,finalHist         ->GetBinError(ibin));
              systdown_finalHist_forDrawingOnInput->SetBinError(ibin,systdown_finalHist->GetBinError(ibin));
              systup_finalHist_forDrawingOnInput  ->SetBinError(ibin,systup_finalHist  ->GetBinError(ibin));
            }

            //Setting histogram style
            setHistStyle(inputHist,kBlack,2,1,1,1);
            setHistStyle(finalHist,kBlack,2,2,1,1);
            setHistStyle(systup_inputHist  ,kRed ,1,1,1,1);
            setHistStyle(systdown_inputHist,kBlue,1,1,1,1);
            setHistStyle(finalHist_forDrawingOnInput         ,kBlack,2,2,1,1);
            setHistStyle(systup_finalHist_forDrawingOnInput  ,kRed  ,2,2,1,1);
            setHistStyle(systdown_finalHist_forDrawingOnInput,kBlue ,2,2,1,1);

            //Preparing ratio plots
            TH1F* ratio_error     = (TH1F*)finalHist_forDrawingOnInput         ->Clone("ratio_error");
            TH1F* ratio_upinput   = (TH1F*)systup_inputHist                    ->Clone("ratio_upinput");
            TH1F* ratio_downinput = (TH1F*)systdown_inputHist                  ->Clone("ratio_downinput");
            TH1F* ratio_upfinal   = (TH1F*)systup_finalHist_forDrawingOnInput  ->Clone("ratio_upfinal");
            TH1F* ratio_downfinal = (TH1F*)systdown_finalHist_forDrawingOnInput->Clone("ratio_downfinal");
            ratio_error    ->Reset();
            ratio_upinput  ->Reset();
            ratio_downinput->Reset();
            ratio_upfinal  ->Reset();
            ratio_downfinal->Reset();
            ratio_upinput  ->Divide(systup_inputHist  ,inputHist);
            ratio_downinput->Divide(systdown_inputHist,inputHist);
            ratio_upfinal  ->Divide(systup_finalHist_forDrawingOnInput  ,finalHist_forDrawingOnInput);
            ratio_downfinal->Divide(systdown_finalHist_forDrawingOnInput,finalHist_forDrawingOnInput);
            for(Int_t ibin=0; ibin<ratio_error->GetNbinsX()+2; ibin++){
              Double_t bincont = finalHist_forDrawingOnInput->GetBinContent(ibin);
              Double_t binerr  = finalHist_forDrawingOnInput->GetBinError  (ibin);
              Double_t binrelerr = bincont==0. ? 0. : binerr/bincont;
              ratio_error->SetBinContent(ibin,1.);
              ratio_error->SetBinError  (ibin,binrelerr);
            }
            setHistStyle(ratio_error,kBlack,1.,1,1,1);
            ratio_error->SetFillStyle(1001);
            ratio_error->SetFillColor(kGray);
            ratio_error->GetYaxis()->SetRangeUser(0.66,1.34);
            ratio_error->GetXaxis()->SetLabelSize(0.08);
            ratio_error->GetXaxis()->SetLabelOffset(0.005);
            ratio_error->GetYaxis()->SetLabelSize(0.08);

            //Checking if there is an one-sided bin
            Bool_t hasOneSided = kFALSE;
            for(Int_t ibin=1; ibin<=ratio_upfinal->GetNbinsX(); ibin++){//check only visible bins
              Double_t upFinalCont   = ratio_upfinal  ->GetBinContent(ibin);
              Double_t downFinalCont = ratio_downfinal->GetBinContent(ibin);
              if((upFinalCont < 1. && downFinalCont < 1.) || (upFinalCont > 1. && downFinalCont > 1.)){
                if(tmpSyst->isHistoSys()){
                  // std::cout<<"INFO: One-sided bin detected: bin="<<ibin<<", upRatio="<<upFinalCont<<", downRatio="<<downFinalCont<<std::endl;
                  hasOneSided = kTRUE;
                }
              }
            }

            //Preparation for plots
            TLegend leg(0.72,0.5, 0.82, 0.82);
            leg.AddEntry(inputHist,"Input");
            leg.AddEntry(systup_inputHist,"Input (Up)");
            leg.AddEntry(systdown_inputHist,"Input (Down)");
            leg.AddEntry(finalHist_forDrawingOnInput,"Final");
            leg.AddEntry(systup_finalHist_forDrawingOnInput,"Final (Up)");
            leg.AddEntry(ratio_error, "Stat. Error");

            Double_t inputMax = inputHist->GetBinContent(inputHist->GetMaximumBin())+inputHist->GetBinError(inputHist->GetMaximumBin());
            inputHist->GetYaxis()->SetRangeUser(0,1.5*inputMax);
            inputHist->GetXaxis()->SetLabelOffset(1.5);
            inputHist->GetYaxis()->SetRangeUser(0,1.5*inputMax);
            inputHist->GetXaxis()->SetLabelOffset(1.5);
            finalHist_forDrawingOnInput->GetYaxis()->SetRangeUser(0,1.5*inputMax);
            finalHist_forDrawingOnInput->GetXaxis()->SetLabelOffset(1.5);
            finalHist_forDrawingOnInput->SetStats(0);
            //Preparing canvas for the actual distribution and ratio between variations and nominal
            TCanvas c("c","",800,600);
            TPad pad1("pad1","",0.,0.3,1.0,1.0,0,0.,0);
            TPad pad2("pad2","",0.,0.0,1.0,0.3,0,0.,0);
            pad1.SetBottomMargin(0.02);
            pad2.SetTopMargin(0.00);
            pad2.SetBottomMargin(0.20);
            pad1.Draw();
            pad1.SetNumber(1);
            pad2.Draw();
            pad2.SetNumber(2);
            //Drawing histograms
            //upper pad
            c.cd(1);
            inputHist         ->Draw("E");
            systup_inputHist  ->Draw("hist same ");
            systdown_inputHist->Draw("hist same ");
            finalHist_forDrawingOnInput         ->Draw("hist same");
            systdown_finalHist_forDrawingOnInput->Draw("hist same");
            systup_finalHist_forDrawingOnInput  ->Draw("hist same");
            //Showing miscellaneous information
            leg.Draw();
            myText(0.13,0.86,kBlack,Form("%s_%s_%s",m_channels[ichannel]->name().c_str(),tmpSamp->name().c_str(),tmpSyst->name().c_str()),0.03);
            myText(0.13,0.82,kRed ,Form("KS(before,up): %5.3f",KS_systup_input),0.03);
            myText(0.13,0.78,kBlue,Form("KS(before,down): %5.3f",KS_systdown_input),0.03);
            myText(0.13,0.74,kRed ,Form("KS(after,up): %5.3f",KS_systup_final),0.03);
            myText(0.13,0.70,kBlue,Form("KS(after,down): %5.3f",KS_systdown_final),0.03);
            myText(0.30,0.82,kRed ,Form("OverallSys(up): %5.3f",tmpSyst->getUpNormalisation()),0.03);
            myText(0.30,0.78,kBlue,Form("OverallSys(down): %5.3f",tmpSyst->getDownNormalisation()),0.03);
            if(tmpSyst->isAggressiveSymm()){
              myText(0.13,0.65,kRed,const_cast<char*>("Aggressive symmetrisation applied"),0.04);
            }
            if(tmpSyst->isOverallSys() && tmpSyst->isHistoSys()){
              myText(0.74,0.91,kBlack,const_cast<char*>("OverallSys+HistoSys"),0.04);
            }
            if(tmpSyst->isOverallSys() && not tmpSyst->isHistoSys()){
              myText(0.81,0.91,kBlack,const_cast<char*>("OverallSys"),0.04);
            }
            if(not tmpSyst->isOverallSys() && tmpSyst->isHistoSys()){
              myText(0.84,0.91,kBlack,const_cast<char*>("HistoSys"),0.04);
            }
            if(not tmpSyst->isOverallSys() && not tmpSyst->isHistoSys()){
              myText(0.79,0.91,kBlack,const_cast<char*>("Pruned away"),0.04);
              HistOutName.ReplaceAll("accepted", "pruned");
            }
            //lower pad
            //Drawing ratio plot
            c.cd(2);
            ratio_error    ->Draw("E2");
            ratio_upinput  ->Draw("histsame");
            ratio_downinput->Draw("histsame");
            ratio_upfinal  ->Draw("histsame");
            ratio_downfinal->Draw("histsame");
            //
            if(hasOneSided) myText(0.6, 0.05, kRed, const_cast<char*>("There is one-sided variation bin"),0.1);
            TLine line(ratio_error->GetXaxis()->GetBinLowEdge(1),1.,ratio_error->GetXaxis()->GetBinUpEdge(ratio_error->GetNbinsX()),1.);
            line.SetLineStyle(2);
            line.Draw();
            //Finalizing drawing loop
            // std::cout<<"INFO: KS<"<<ksval_threshold<<" for Sample \""<<tmpSamp->name()<<"\", Systematics "<<tmpSyst->name()<<"..."<<std::endl;
            c.Print(HistOutName.Data());
            ratio_error    ->Delete();
            ratio_upinput  ->Delete();
            ratio_downinput->Delete();
            ratio_upfinal  ->Delete();
            ratio_downfinal->Delete();
            finalHist_forDrawingOnInput->Delete();
            systdown_finalHist_forDrawingOnInput->Delete();
            systup_finalHist_forDrawingOnInput->Delete();
            //c.cd(1);
            //leg.AddEntry(ratio_error, "Rel. data error", "fs");
            //leg.Draw();
          }//
          systup_inputHist->Delete();
          systup_finalHist->Delete();
          systdown_inputHist->Delete();
          systdown_finalHist->Delete();
        }
        inputHist->Delete();
        finalHist->Delete();
      }

      //For envelope plots of sum of MC.
      std::sort(systname_list.begin(), systname_list.end());
      systname_list.erase(std::unique(systname_list.begin(), systname_list.end()), systname_list.end()); //Erasing all duplicated systematics
      std::map<std::string,double> nom_para; //Should be empty to retrieve nominal histogram
      TH1F* sumOfNominal=m_channels[ichannel]->getSumOfMCHistogram(nom_para);
      TH1F* sumOfStatErr=m_channels[ichannel]->getSumOfMCHistogram(nom_para);
      const Int_t nOriginalBins = m_channels[ichannel]->samples()->at(0)->inputhist()->GetNbinsX();  
      const Double_t* originalBins = m_channels[ichannel]->samples()->at(0)->inputhist()->GetXaxis()->GetXbins()->GetArray();
      if(originalBins!=NULL){
        sumOfNominal->SetBins(nOriginalBins,originalBins);
        sumOfStatErr->SetBins(nOriginalBins,originalBins);
      }
      for(Int_t ibin=0; ibin<sumOfNominal->GetNbinsX()+2; ibin++){
        //Setting error size to be all stat uncertainty (Asimov data+MC)
        sumOfStatErr->SetBinError(ibin, sqrt( sumOfStatErr->GetBinContent(ibin) +
                                              pow(sumOfStatErr->GetBinError(ibin),2.) ));
      }
      for(UInt_t isyst=0; isyst<systname_list.size(); isyst++){
        TString HistOutName = Form("%s%s/envelope/envelope_%s_%s_%s_accepted.%s",
                                   m_outputpath.c_str(),
                                   m_version.c_str(),
                                   systname_list.at(isyst).c_str(),
                                   m_channels[ichannel]->name().c_str(),
                                   "AllMC",
                                   outputformat.c_str());
        
        //Skipping if it's not specified NPs
        Bool_t skipped = keyWordsSyst.size()==0 ? kFALSE : kTRUE;
        for(UInt_t ikeysys=0; ikeysys<keyWordsSyst.size(); ikeysys++){
          if(HistOutName.Contains(keyWordsSyst.at(ikeysys).Data())) skipped = kFALSE;
        }
        if(skipped) continue;

        //Retrieving upward variation histogram
        std::map<std::string,double> up_para;
        up_para.insert(std::pair<std::string,double>("alpha_"+systname_list.at(isyst),1.));
        up_para.insert(std::pair<std::string,double>("ConsiderPrune",0.));
        TH1F* sumOfUp=m_channels[ichannel]->getSumOfMCHistogram(up_para);
        if(originalBins!=NULL){
          sumOfUp->SetBins(nOriginalBins,originalBins);
        }
        //Retrieving downward variation histogram
        std::map<std::string,double> down_para;
        down_para.insert(std::pair<std::string,double>("alpha_"+systname_list.at(isyst),-1.));
        down_para.insert(std::pair<std::string,double>("ConsiderPrune",0.));
        TH1F* sumOfDown=m_channels[ichannel]->getSumOfMCHistogram(down_para);
        if(originalBins!=NULL){
          sumOfDown->SetBins(nOriginalBins,originalBins);
        }        
        //Setting histogram style
        sumOfStatErr->UseCurrentStyle();
        sumOfStatErr->SetFillColor(kGray);
        sumOfStatErr->SetStats(0);
        sumOfStatErr->SetTitle("");
        setHistStyle(sumOfNominal,kBlack,2,2,1,1);
        setHistStyle(sumOfUp     ,kRed  ,2,2,1,1);
        setHistStyle(sumOfDown   ,kBlue ,2,2,1,1);
        
        //Preparing ratio plots
        TH1F* sum_ratio_error     = (TH1F*)sumOfNominal->Clone("sum_ratio_error");
        TH1F* sum_ratio_allerror  = (TH1F*)sumOfNominal->Clone("sum_ratio_allerror");
        TH1F* sum_ratio_upfinal   = (TH1F*)sumOfUp     ->Clone("sum_ratio_upfinal");
        TH1F* sum_ratio_downfinal = (TH1F*)sumOfDown   ->Clone("sum_ratio_downfinal");
        sum_ratio_error    ->Reset();
        sum_ratio_allerror ->Reset();
        sum_ratio_upfinal  ->Reset();
        sum_ratio_downfinal->Reset();
        sum_ratio_upfinal  ->Divide(sumOfUp  ,sumOfNominal);
        sum_ratio_downfinal->Divide(sumOfDown,sumOfNominal);
        for(Int_t ibin=0; ibin<sum_ratio_error->GetNbinsX()+2; ibin++){
          Double_t bincont = sumOfNominal->GetBinContent(ibin);
          Double_t binerr  = sumOfNominal->GetBinError  (ibin);
          Double_t binrelerr = bincont==0. ? 0. : binerr/bincont;
          Double_t asimoverr = sumOfStatErr->GetBinContent(ibin)==0. ? 0. : sumOfStatErr->GetBinError(ibin)/sumOfStatErr->GetBinContent(ibin);
          Double_t allstaterr = sqrt(pow(binrelerr,2)+pow(asimoverr,2));
          sum_ratio_error->SetBinContent(ibin,1.);
          sum_ratio_error->SetBinError(ibin,binrelerr);
          sum_ratio_allerror->SetBinContent(ibin,1.);
          sum_ratio_allerror->SetBinError(ibin,allstaterr);                
        }
        setHistStyle(sum_ratio_error,kBlack,2,2,1,1);
        sum_ratio_allerror->UseCurrentStyle();
        sum_ratio_allerror->SetFillColor(kGray);
        sum_ratio_allerror->SetStats(0);
        sum_ratio_allerror->SetTitle("");
        sum_ratio_allerror->GetYaxis()->SetRangeUser(0.66,1.34);
        sum_ratio_allerror->GetXaxis()->SetLabelSize(0.08);
        sum_ratio_allerror->GetXaxis()->SetLabelOffset(0.005);
        sum_ratio_allerror->GetYaxis()->SetLabelSize(0.08);
        //Checking data healthyness
        Bool_t hasOneSided = kFALSE;
        Bool_t hasVariationBeyondStatErr = kFALSE;
        for(Int_t ibin=1; ibin<=sum_ratio_upfinal->GetNbinsX(); ibin++){//check only visible bins
          Double_t upFinalCont   = sum_ratio_upfinal  ->GetBinContent(ibin);
          Double_t downFinalCont = sum_ratio_downfinal->GetBinContent(ibin);
          Double_t statErrSize   = sum_ratio_allerror->GetBinError(ibin);
          //check if there is an one-sided bin
          if( (upFinalCont < 1. && downFinalCont < 1.) ||
              (upFinalCont > 1. && downFinalCont > 1.) ) hasOneSided = kTRUE;
          //check if there is an impactful bin
          if( (upFinalCont   > 1. && upFinalCont   > 1.+statErrSize) ||
              (upFinalCont   < 1. && upFinalCont   < 1.-statErrSize) ||
              (downFinalCont > 1. && downFinalCont > 1.+statErrSize) ||
              (downFinalCont < 1. && downFinalCont < 1.-statErrSize) ) hasVariationBeyondStatErr = kTRUE;
        }
        //Preparation for plots
        TLegend leg(0.7,0.68, 0.86, 0.82);
        leg.AddEntry(sumOfNominal,"Final"       );
        leg.AddEntry(sumOfUp     ,"Final (Up)"  );
        leg.AddEntry(sumOfDown   ,"Final (Down)");
        leg.AddEntry(sumOfStatErr,"Stat. err. (Asimov+MC)","f");
        Double_t inputMax = sumOfStatErr->GetBinContent(sumOfStatErr->GetMaximumBin())+sumOfStatErr->GetBinError(sumOfStatErr->GetMaximumBin());
        sumOfStatErr->GetYaxis()->SetRangeUser(0,1.5*inputMax);
        sumOfStatErr->GetXaxis()->SetLabelOffset(1.5);
        sumOfStatErr->GetYaxis()->SetRangeUser(0,1.5*inputMax);
        sumOfStatErr->GetXaxis()->SetLabelOffset(1.5);
        //Preparing canvas for the actual distribution and ratio between variations and nominal
        TCanvas c("c","",800,600);
        TPad pad1("pad1","",0.,0.3,1.0,1.0,0,0.,0);
        TPad pad2("pad2","",0.,0.0,1.0,0.3,0,0.,0);
        pad1.SetBottomMargin(0.02);
        pad2.SetTopMargin(0.00);
        pad2.SetBottomMargin(0.20);
        pad1.Draw();
        pad1.SetNumber(1);
        pad2.Draw();
        pad2.SetNumber(2);
        //Drawing histograms
        c.cd(1);
        sumOfStatErr->Draw("E2");
        sumOfNominal->Draw("E same");
        sumOfUp     ->Draw("hist same");
        sumOfDown   ->Draw("hist same");
        //Showing miscellaneous information
        leg.Draw();
        myText(0.13,0.86,kBlack,Form("%s_%s_%s",m_channels[ichannel]->name().c_str(),"AllMC",systname_list.at(isyst).c_str()),0.03);
        Double_t upnorm   = sumOfUp  ->GetSumOfWeights()/sumOfNominal->GetSumOfWeights();
        Double_t downnorm = sumOfDown->GetSumOfWeights()/sumOfNominal->GetSumOfWeights();
        myText(0.13,0.82,kRed ,Form("Total yield change(up): %5.3f",upnorm),0.03);
        myText(0.13,0.78,kBlue,Form("Total yield change(down): %5.3f",downnorm),0.03);
        if(hasVariationBeyondStatErr){
          myText(0.74,0.91,kBlack,const_cast<char*>("SystErr. > StatErr."),0.04);
          HistOutName.ReplaceAll("accepted", "significant");
        }
        //Drawing ratio plot
        c.cd(2);
        sum_ratio_allerror ->Draw("E2");
        sum_ratio_error    ->Draw("E same");
        sum_ratio_upfinal  ->Draw("hist same");
        sum_ratio_downfinal->Draw("hist same");
        if(hasOneSided){
          myText(0.6, 0.05, kRed, const_cast<char*>("There is one-sided variation bin"),0.1);
          HistOutName.ReplaceAll("accepted", "accepted_onesided");
          HistOutName.ReplaceAll("significant", "significant_onesided");
        }
        TLine line(sum_ratio_error->GetXaxis()->GetBinLowEdge(1),1.,sum_ratio_error->GetXaxis()->GetBinUpEdge(sum_ratio_error->GetNbinsX()),1.);
        line.SetLineStyle(2);
        line.Draw();
        c.Print(HistOutName.Data());
        
        //Cleaning up things
        sum_ratio_error    ->Delete();
        sum_ratio_allerror ->Delete();
        sum_ratio_upfinal  ->Delete();
        sum_ratio_downfinal->Delete();
        delete sumOfUp;
        delete sumOfDown;
      }
      delete sumOfNominal;
    }

    return true;
  }

  //-------------------------------------------------------------------------------
  //Definition of non-class method
  //-------------------------------------------------------------------------------
  void myText(Double_t x,Double_t y,Color_t color,char *text,Double_t size,Font_t tfont){
    //Double_t tsize=0.05;
    TLatex l; //l.SetTextAlign(12);
    l.SetTextFont(tfont); //Default Helvetica (See "class TAttText")
    l.SetTextSize(size);
    l.SetNDC();
    l.SetTextColor(color);
    l.DrawLatex(x,y,text);
    return;
  }

  void setHistStyle(TH1F *h,Color_t color,Width_t lineWidth,Style_t lineStyle,Size_t markerSize,Style_t markerStyle){
    h->SetStats(0);
    h->SetTitle("");
    h->SetFillColor(0);
    h->SetLineColor(color);
    h->SetLineWidth(lineWidth);
    h->SetLineStyle(lineStyle);
    h->SetMarkerColor(color);
    h->SetMarkerSize(markerSize);
    h->SetMarkerStyle(markerStyle);
    h->GetXaxis()->SetTitle("");
    h->GetYaxis()->SetTitle("");
    return;
  }

} // namespace
