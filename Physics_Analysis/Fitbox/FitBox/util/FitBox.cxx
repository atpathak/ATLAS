/** 
 * @file utils/FitBox.cxx
 *
 * @author Z.Zinonos, A. De Maria
 *
 * Sep 2017
 */

#include <iostream>
#include <sys/stat.h>

#include "boost/program_options.hpp"
#include "boost/program_options/options_description.hpp"

#include "NLLProfileCreator/NLLProfileCreator.h"
#include "NLLProfileScanner/NLLProfileScanner.h"
#include "NLLProfileNPRanking/NLLProfileNPRanking.h"
#include "NLLProfileSignificance/NLLProfileSignificance.h"
#include "NLLProfileLimits/NLLProfileLimits.h"
#include "NLLCreateVarPlots/NLLCreateVarPlots.h"
#include "Toys/Toys.h"
#include "ErrorBreakdown/ErrorBreakdown.h"

#define DEFAULT_STRATEGY_ORDER 102

/**
 * Compilation printout
 */
__attribute__((constructor)) static void initializer(void) {
  printf("\n" "\x1b[32m"); //ANSI_COLOR_GREEN
  printf("FitBox (c) 2016-2018\n");
  printf("\nA non-black-box software that fits your model just out of the box!\n");
  printf("\t GitLab:      https://gitlab.cern.ch/ATauLeptonAnalysiS/FitBox\n");
  printf("\t Mattermost:  https://mattermost.web.cern.ch/fitbox/channels/town-square\n");
  printf("\x1b[0m" "\n"); //color reset
}

/**
 * declarations
 * @{
 */
void LaunchErrorBreakdown(const std::string &rws, 
			  const std::string &outputdir,
			  const std::string &poi_file,
			  const std::string &norm_file,
			  const std::string &theo_file,
			  const bool minos,
			  const unsigned int strategy,
			  const bool minim_errorcalc_coupled
			  );

void LaunchNLLProfileCreator( const std::string &NPvar,
                              const std::string &rws,
                              const std::string &outputdir,
                              const bool asimov,
                              const bool printNP,
                              const bool printSubChannels,
                              const std::string &tag,
                              const double poi_value,
                              const bool conditional_fit,
                              const bool dry,
                              const bool do_nll,
                              const bool do_pulls,
                              const bool do_pulls_channels,
                              const bool do_plot,
                              const bool do_contour,
                              const bool do_morphing,
                              const bool do_envelope,
                              const bool multiple_pois,
                              const bool minos,
			      const unsigned int strategy,
			      const bool minim_errorcalc_coupled,
                              const bool gamma,
                              const std::string &xvar,
                              const std::string &yvar
                              );

void LaunchNLLProfileScanner( const std::string &NPvar,
                              const std::string &rws,
                              const std::string &outputdir,
                              const bool asimov,
                              const bool minos,
			      const unsigned int strategy,
			      const bool minim_errorcalc_coupled,
                              const std::string &fit_category,
                              const std::string &tag,
                              const uint n_segments,
                              const bool dry);


void LaunchNLLProfileNPRanking(
                               const std::string &NPvar,
                               const std::string &rws,
                               const std::string &outputdir,
                               const bool asimov,
                               const bool minos,
                               const unsigned int strategy,
			       const bool minim_errorcalc_coupled,
                               const std::string &fit_category,
                               const std::string &tag,
                               const bool recalc_sigma,
                               const bool prefit_impact,
                               const bool dry,
                               const bool multiple_pois,
                               const bool gamma,
                               const std::string &NPimp);

void LaunchNLLProfileSignificance(
                                  const std::string &rws,
                                  const std::string &outputdir,
                                  const bool asimov,
                                  const bool nosyst
                                  );


void LaunchNLLProfileLimits(
                            const std::string &rws,
                            const std::string &outputdir,
                            const bool asimov,
                            const unsigned int strategy,
                            const bool minos,
			    const bool minim_errorcalc_coupled,
			    const bool nosyst,
                            const bool onesided,
                            const double conflv,
                            const bool autoscan,
                            const int fragments,
                            const double poimin,
                            const double poimax,
                            const bool usetoys,
                            const int ntoys
                            );

void LaunchNLLCreateVarPlots(
                             const std::string &rws_input,
                             const std::string &rws_helper,
                             const std::string &outputdir
                             );

unsigned int NumberCPUs();

void LaunchToys(const std::string &rws_input,
		const std::string &outputdir,
                const std::string &outputfile,
		const bool conditional, 
		const double POI,
		const uint ntoys,
		const uint seed,
                const bool saveyields,
                const bool calcsign);
/**
 * @}
 */

//returns
namespace
{
  const size_t ERROR_IN_COMMAND_LINE = 1;
  const size_t SUCCESS = 0;
  const size_t ERROR_UNHANDLED_EXCEPTION = 2;

} // namespace

//main
int main(int argc, char *argv[]){


  std::cout << "argc=" << argc << std::endl;
  for (int i = 0; i < argc; ++i) {
    std::cout << "argv["<<i<<"]=" << argv[i] << std::endl;
  }



  std::string NPvar("SigXsecOverSM");
  std::string NPimp("");
  std::string rws("./workspace.root");
  std::string rws_helper("./workspace.root");
  std::string outputdir("test");
  std::string outputfile("toys");
  std::string xvar("");
  std::string yvar("");
  std::string fit_category("combined");
  std::string tag("htautau");
  unsigned int n_fixed_np_segments(50);
  bool asimov(false);
  bool minos(false);
  bool minim_errorcalc_coupled(false);
  bool printNP(false);
  bool printSubChannels(false);
  bool dry(false);
  bool conditional_fit(false);
  //method
  bool error_breakdown(false);
  bool creator(false);
  bool scanner(false);
  bool npranking(false);
  bool significance(false);
  bool limits(false);
  bool varplots(false);
  bool toys(false);
  // creator methods
  bool run_nll(false);
  bool run_pulls(false);
  bool run_pulls_channels(false);
  bool run_plot(false);
  bool run_contour(false);
  bool run_morphing(false);
  bool run_envelope(false);
  bool run_multiple_pois(false);
  // ranking methods
  bool recalc_sigma(false);
  bool prefit_impact(false);
  bool gamma(false);
  // significance/limits method
  bool nosyst(false);
  bool onesided(false);
  bool autoscan(false);
  bool usetoys(false);

  //strategy
  unsigned int strategy(DEFAULT_STRATEGY_ORDER);

  //scanner
  double poi_value(1); //default
  double conflv(0.95); //default
  int fragments(10); // default
  double poimin(0); // default
  double poimax(10); //default
  int  ntoys(50); //default
  
  //toys
  uint toys_seed(0);
  bool saveyields(0);
  bool calcsign(0);
 
  //error breakdown
  std::string error_breakdown_poi_file("");
  std::string error_breakdown_norm_file("");
  std::string  error_breakdown_theo_file("");


  //retrieve options
  try
    {
      //NPvar = argv[1]; rws = argv[2]; outputdir = argv[3];
      namespace po = boost::program_options;
      po::options_description desc("Options");

      desc.add_options()
        // algorithm
        ("breakdown",     "Run ErrorBreakdown")
        ("creator",     "Run LaunchNLLProfileCreator")
        ("scanner",     "Run LaunchNLLProfileScanner")
        ("npranking",   "Run LaunchNLLProfileNPRanking")
        ("significance",  "Run LaunchNLLProfileSignificance")
        ("limits",      "Run LaunchNLLProfileLimits")
        ("varplots",   "Run LaunchNLLCreateVarPlots")
        ("toys",     "Run Toys")

        //common
        ("npname,n",    po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Common option: Name of the NP for NLL Scan. Default: SigXsecOverSM")
        ("workspace,w", po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Common option: Full path to workspace file.")
        ("output,o",    po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Common option: Name of output folder. Default: test")
        ("tag,t",   po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Common option: Project tag . Default: htautau")
        ("asimov,a",    "Common option: Enable Asimov Data instead of Observed Data")
        ("dry,d",       "Common option: Dry run")
        ("gamma,g",     "Add treatment of the gamma parametes")
        ("minos,m",     "Use Minos instead of Hesse for the error calculations")
	("minim_errorcalc_coupled", "Couple the error calculation to the minimization process")

        //creator only
        ("strategy",    po::value<unsigned int>(&strategy)->default_value(DEFAULT_STRATEGY_ORDER), 
	 ("LaunchNLLProfileCreator: Minimizer's strategy order. Default = "+std::to_string(DEFAULT_STRATEGY_ORDER)+" where 1=default, 0=speedy, 2=precision.").c_str() )
        ("printnp,p",       "LaunchNLLProfileCreator: Print a list of the RooWorkspace nuisance parameters on the screen and in a txt file.")
        ("printch,q",       "LaunchNLLProfileCreator: Print a list of the RooWorkspace subchannels on the screen and in a txt file.")
        ("conditional,k",       "LaunchNLLProfileCreator: Conditional fit (POI is fixed while NPs are floating in the fit).")
        ("nll,l",               "LaunchNLLProfileCreator: Run the NLL profile scans")
        ("pulls,u",             "LaunchNLLProfileCreator: Run the pulls for the combined model")
        ("pullsch,y",           "LaunchNLLProfileCreator: Run the pulls for all sub-channels")
        ("plot,p",              "LaunchNLLProfileCreator: Draw the plots")
        ("contour",             "LaunchNLLProfileCreator: Draw 2D contour plot")
        ("morphing,mor",    "LaunchNLLProfileCreator: Draw morphing plots")
        ("envelope,env",    "LaunchNLLProfileCreator: Draw np envelope plots")
        ("multiple_pois",   "LaunchNLLProfileCreator: Perform a fit using multiple POIs") 
        
        // contour xy variables
        ("xvar",   po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Contour : X variable of the Contour plot")
        ("yvar",   po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Contour : Y variable of the Contour plot")
        
	//creator & toys
        ("poi_value,P",  po::value<double>(&poi_value)->default_value(1),  
	 "LaunchNLLProfileCreator, Toys; Set the value of the POI to be used in a conditional fit")
        ("saveyields",  "Toys: save yields for each toy")
        ("calcsign",    "Toys: check the significance for each toy")
        ("outputfile",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Toys: output root file name")
	//significance & toys
        ("ntoys",       po::value<int>(&ntoys)->default_value(50), "LaunchNLLProfileLimits: set number of toys")

	//toys
        ("toys_seed",  po::value<unsigned int>(&toys_seed)->default_value(0), "Toys: seed for sampler.")

        //scanner only
        ("category,c",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "LaunchNLLProfileScanner: Name of category. Default: combined")
        ("segments,s",  po::value<unsigned int>(), "LaunchNLLProfileScanner: Number of segments to split the NP and perfom the NLL fit")

        // np ranking only
        ("rsigma,r",   "Run LaunchNLLProfileNPRanking: recalculate the sigma for the ranking calculation")
        ("prefit,i",   "Run LaunchNLLProfileNPRanking: add pre-fit impact calculation")
        ("npimp",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "Run LaunchNLLProfileNPRanking: check only the impact of the specified NP")

        // significance only
        ("nosyst",      "LaunchNLLProfileSignificance/Limits: perform the task without systematics")
        ("onesided",    "LaunchNLLProfileLimits: perform onesided test")
        ("conflv",      po::value<double>(&conflv)->default_value(0.95), "LaunchNLLProfileLimits: set confidence level")
        ("autoscan",    "LaunchNLLProfileLimits: automatic scan over the poi")
        ("fragments",   po::value<int>(&fragments)->default_value(10), "LaunchNLLProfileLimits: set number of fragments for manual scan")
        ("poimin",      po::value<double>(&poimin)->default_value(0),  "LaunchNLLProfileLimits: minimum poi value for manual scan")
        ("poimax",      po::value<double>(&poimax)->default_value(2),  "LaunchNLLProfileLimits: maximum poi value for manual scan")
        ("usetoys",     "LaunchNLLProfileLimits: use toys to rebuild upper limit distribution")

        // varplots only
        ("result,res",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "LaunchNLLCreateVarPlots: Full path to file with main fit results.")

	//error breakdown only
        ("poi_file",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "LaunchErrorBreakdown: File listing the POI names")
        ("norm_file",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "LaunchErrorBreakdown: File listing the normalization factor names")
        ("theo_file",  po::value<std::string>()->multitoken()->zero_tokens()->composing(), "LaunchErrorBreakdown: File listing the theory NP names")

        //help
        ("help,h", "Print help messages");

      po::positional_options_description positionalOptions;
      positionalOptions.add("scanner", 1);
      positionalOptions.add("creator", 1);
      //positionalOptions.add("output", -1);

      po::variables_map vm;
      try {
        po::store(po::command_line_parser(argc, argv).
                  options(desc).
                  positional(positionalOptions).
                  run(),
                  vm); //can throw excpetions

        //help
        if ( vm.count("help") ) {
          std::cout << "FitBox: Application that builds NLL profiles, NP pulls, etc." << std::endl
                    << desc << std::endl;
          //std::cout << positionalOptions << std::endl;
          return SUCCESS;
        }
        //
        po::notify(vm); // throws on error. Keep it after help in case there are any problems
        //
        if ( vm.count("breakdown") ){
	  error_breakdown=true;
        }
        //
        if ( vm.count("creator") ){
          creator=true;
        }
        //
        if ( vm.count("scanner") ){
          scanner=true;
        }
        //
        if ( vm.count("npranking") ){
          npranking=true;
        }
        //
        if ( vm.count("significance") ) {
          significance=true;
        }
        //
        if ( vm.count("limits") ) {
          limits=true;
        }
        //
        if ( vm.count("varplots") ) {
          varplots=true;
        }
        //
        if ( vm.count("toys") ){
          toys=true;
        }
        //
        if(!scanner && !creator && !npranking && !significance && !limits && !varplots && !toys && !error_breakdown){
          std::cout << "ERROR: No program has been specified!" << std::endl;
          return ERROR_IN_COMMAND_LINE;
        }
        //
        if ( vm.count("asimov") ){
          asimov=true;
        }
        if ( vm.count("minos") ){
          minos=true;
        }
	//
	if ( vm.count("minim_errorcalc_coupled") ){
          minim_errorcalc_coupled=true;
        }	 
        //
        if ( vm.count("printnp")  ){
          printNP=true;
        }
        //
        if ( vm.count("printch")  ){
          printSubChannels=true;
        }
        //
        if ( vm.count("conditional")  ){
          conditional_fit=true;
        }
        //
        if ( vm.count("nll")  ){
          run_nll=true;
        }
        //
        if ( vm.count("pulls")  ){
          run_pulls=true;
        }
        //
        if ( vm.count("pullsch")  ){
          run_pulls_channels=true;
        }
        //
        if ( vm.count("plot") ) {
          run_plot = true;
        }
        if ( vm.count("contour")) {
          run_contour = true;
        }
        //
        if( vm.count("morphing")){
          run_morphing = true;
        }
        //
        if( vm.count("envelope")){
          run_envelope = true;
        }
        //
        if( vm.count("multiple_pois")){
          run_multiple_pois = true;
        }
        //
        if( vm.count("rsigma")){
          recalc_sigma = true;
        }
        //
        if( vm.count("prefit")){
          prefit_impact = true;
        }
        if( vm.count("gamma")){
          gamma = true;
        }
        //
        if( vm.count("nosyst")){
          nosyst = true;
        }
        //
        if( vm.count("onesided")){
          onesided = true;
        }
        //
        if( vm.count("autoscan")){
          autoscan = true;
        }
        //
        if( vm.count("usetoys")){
          usetoys = true;
        }
        //
        if( vm.count("saveyields")){
          saveyields = true;
        }
        //
        if( vm.count("calcsign")){
          calcsign = true;
        }
        //
        if( vm.count("xvar")){
           xvar = vm["xvar"].as<std::string>();
           std::cout << "INFO : X var for the contour plot \"" << xvar << std::endl;
        }
        //
        if( vm.count("yvar")){
           yvar = vm["yvar"].as<std::string>();
           std::cout << "INFO : Y var for the contour plot \"" << yvar << std::endl;
        }
        //
        if (vm.count("outputfile")){
          outputfile=vm["outputfile"].as<std::string>();
          std::cout << "INFO: Output filename \"" << outputfile <<"\"." << std::endl;
        }
        else{
          std::cout << "WARNING: No output folder given! Setting default..." << std::endl;
        }
        //
        if ( vm.count("dry")  ){
          dry=true;
        }
        //
        if (vm.count("npname")){
          NPvar=vm["npname"].as<std::string>();
          std::cout << "INFO: Performing NLL scan of ( or NP ranking respect to )  \"" << NPvar <<"\"." << std::endl;
        }
        else{
          std::cout << "WARNING: No NP name was given! Setting default" << std::endl;
        }
        // 
        if (vm.count("npimp")){
          NPimp=vm["npimp"].as<std::string>();
          std::cout << "INFO: Checking the impact of " << NPimp << " for the NPRanking " << "\"," << std::endl; 
        }
        else{
          std::cout << " INFO: no NP has been specified. Ranking will be done considering all NPs " << std::endl;
        }
        //
        if (vm.count("workspace")){
          rws=vm["workspace"].as<std::string>();
          std::cout << "INFO: Taking input \"" << rws <<"\"." << std::endl;
        }
        else{
          std::cout << "ERROR: No workspace was given! Exiting..." << std::endl;
          return ERROR_IN_COMMAND_LINE;
        }
        //
        if (vm.count("output")){
          outputdir=vm["output"].as<std::string>();
          std::cout << "INFO: Output folder \"" << outputdir <<"\"." << std::endl;
        }
        else{
          std::cout << "WARNING: No output folder given! Setting default..." << std::endl;
        }
        //
        if (vm.count("category")){
          fit_category=vm["category"].as<std::string>();
          std::cout << "INFO: fit category \"" << fit_category <<"\"." << std::endl;
        }
        else{
          std::cout << "WARNING: No fit category given! Setting default" << std::endl;
        }
        //
        if (vm.count("tag")){
          tag=vm["tag"].as<std::string>();
          std::cout << "INFO: project tag \"" << tag <<"\"." << std::endl;
        }
        else{
          std::cout << "WARNING: No project tag given! Setting default" << std::endl;
        }
        //
        if (vm.count("segments")){
          n_fixed_np_segments=vm["segments"].as<unsigned int>();
          std::cout << "INFO: number of fixed NP segments \"" << n_fixed_np_segments  <<"\"." << std::endl;
        }
        else{
	  if(scanner)
	    std::cout << "WARNING: No number of segments is provided! Setting default" << std::endl;
        }
        //
        if (vm.count("result")){
          rws_helper=vm["result"].as<std::string>();
          std::cout << " INFO: taking fit result from \" " << rws_helper << "\"." << std::endl;
        }
	// Error breakdown
        if (vm.count("poi_file")){
          error_breakdown_poi_file=vm["poi_file"].as<std::string>();
          std::cout << "INFO: POI file name \"" << error_breakdown_poi_file <<"\"." << std::endl;
        }
        else{
	  if(error_breakdown){
	    std::cout << "ERROR: No POI file was provided! Bailing out..." << std::endl;
	    return ERROR_IN_COMMAND_LINE;
	  }
        }
	//
        if (vm.count("norm_file")){
          error_breakdown_norm_file=vm["norm_file"].as<std::string>();
          std::cout << "INFO: Norm file name \"" << error_breakdown_norm_file <<"\"." << std::endl;
        }
        else{
          std::cout << "Warning: No Norm file was provided! Anyway, can live without it..." << std::endl;
        }
	//
        if (vm.count("theo_file")){
          error_breakdown_theo_file=vm["theo_file"].as<std::string>();
          std::cout << "INFO: theo file name \"" << error_breakdown_theo_file <<"\"." << std::endl;
        }
        else{
          std::cout << "Warning: No Theo file was provided! Anyway, can live without it..." << std::endl;
        }
	//

      } catch(po::error& e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        std::cerr << desc << std::endl;
        return ERROR_IN_COMMAND_LINE;
      }

    }
  catch(std::exception& e)
    {
      std::cerr << "Unhandled Exception reached the top of main: "
                << e.what() << ", application will now exit" << std::endl;
      return ERROR_UNHANDLED_EXCEPTION;

    }

  // force output to go in the "results/" directory
  outputdir = "results/"+outputdir;

  /**
   *
   *
   */
  if(error_breakdown)
    LaunchErrorBreakdown(rws, 
			 outputdir,
			 error_breakdown_poi_file,
			 error_breakdown_norm_file,
			 error_breakdown_theo_file,
			 minos,
			 strategy,
			 minim_errorcalc_coupled
			 );
  
  /**
   * NLLProfileCreator
   *
   */
  if(creator)
    LaunchNLLProfileCreator( NPvar,
                             rws,
                             outputdir,
                             asimov,
                             printNP,
                             printSubChannels,
                             tag,
                             poi_value,
                             conditional_fit,
                             dry,
                             run_nll,
                             run_pulls,
                             run_pulls_channels,
                             run_plot,
                             run_contour,
                             run_morphing,
                             run_envelope,
                             run_multiple_pois,
                             minos,
			     strategy,
			     minim_errorcalc_coupled,
                             gamma,
                             xvar,
                             yvar
                             );

  /**
   * NLLProfileScanner
   *
   */
  if(scanner)
    LaunchNLLProfileScanner( NPvar,
                             rws,
                             outputdir,
                             asimov,
                             minos,
			     strategy,
			     minim_errorcalc_coupled,
                             fit_category,
                             tag,
                             n_fixed_np_segments,
                             dry);

  /**
   * NLLProfileNPRanking
   *
   */
  if(npranking)
    LaunchNLLProfileNPRanking(
                              NPvar,
                              rws,
                              outputdir,
                              asimov,
                              minos,
                              strategy,
			      minim_errorcalc_coupled,
                              fit_category,
                              tag,
                              recalc_sigma,
                              prefit_impact,
                              dry,
                              run_multiple_pois,
                              gamma,
                              NPimp);

  /**
   * NLLProfileSignificance
   *
   */
  if(significance)
    LaunchNLLProfileSignificance(
                                 rws,
                                 outputdir,
                                 asimov,
                                 nosyst
                                 );

  /**
   * NLLProfileLimits
   *
   *
   */
  if(limits)
    LaunchNLLProfileLimits(
                           rws,
                           outputdir,
                           asimov,
                           strategy,
                           minos,
			   minim_errorcalc_coupled,
                           nosyst,
                           onesided,
                           conflv,
                           autoscan,
                           fragments,
                           poimin,
                           poimax,
                           usetoys,
                           ntoys
                           );

  /**
   * NLLCreateVarPLots
   *
   *
   */
  if(varplots)
    LaunchNLLCreateVarPlots(
                            rws,
                            rws_helper,
                            outputdir
                            );


  /**
   * Toys
   *
   *
   */
  if(toys)
    LaunchToys(rws, 
	       outputdir,
               outputfile,
	       conditional_fit,
	       poi_value,
	       ntoys,
	       toys_seed,
               saveyields,
               calcsign
	       );

  return 0;
}
//=========================================================================================
void LaunchErrorBreakdown(const std::string &rws, 
			  const std::string &outputdir,
			  const std::string &poi_file,
			  const std::string &norm_file,
			  const std::string &theo_file,
			  const bool minos,
			  const unsigned int strategy,
			  const bool minim_errorcalc_coupled
			  ){

  std::cout<<"LaunchErrorBreakdown"<<std::endl;
  std::cout<<"\tWorkspace input file : '"<<rws<<"'"<<std::endl;
  std::cout<<"\tOutput directory     : '"<<outputdir<<"'"<<std::endl;
  std::cout<<"\tPOI file             : '"<<poi_file<<"'"<<std::endl;
  std::cout<<"\tNorm file            : '"<<norm_file<<"'"<<std::endl;
  std::cout<<"\tTheo file            : '"<<theo_file<<"'"<<std::endl;
  std::cout<<"\tMinos                : " << (minos?"yes":"no")<<std::endl;
  std::cout<<"\tMinimizer strategy order : "<<strategy<<(strategy == DEFAULT_STRATEGY_ORDER?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tMinimization process and error calculation are coupled in the minimization loop: " << (minim_errorcalc_coupled?"yes":"no")<<std::endl;

  ErrorBreakdown EB;
  EB.set_workspace_file(rws);
  EB.set_output_directory(outputdir);
  EB.set_poi_file(poi_file);
  EB.set_norm_file(norm_file);
  EB.set_theo_file(theo_file);
  if(minos) EB.enable_minos();
  EB.set_strategy_order(strategy);
  EB.couple_minimization_and_error_calculation();

  if( !EB.calculate() )
    std::cerr<<"ERROR: could not calculated any error breakdown..."<<std::endl;
  else
    std::cout<<"ErrorBreakdown: Done!"<<std::endl;
}
//=========================================================================================
void LaunchNLLProfileCreator( const std::string &NPvar,
                              const std::string &rws,
                              const std::string &outputdir,
                              const bool asimov,
                              const bool printNP,
                              const bool printSubChannels,
                              const std::string &tag,
                              const double poi_value,
                              const bool conditional_fit,
                              const bool dry,
                              const bool do_nll,
                              const bool do_pulls,
                              const bool do_pulls_channels,
                              const bool do_plot,
                              const bool do_contour,
                              const bool do_morphing,
                              const bool do_envelope,
                              const bool multiple_pois,
                              const bool minos,
			      const unsigned int strategy,
			      const bool minim_errorcalc_coupled,
                              const bool gamma,
                              const std::string &xvar,
                              const std::string &yvar
                              )
{

  std::string roofilename = NPvar.size() ? "nll_profiles_" + tag + "_" + NPvar : "test"; //add a JO here
  std::cout<<"LaunchNLLProfileCreator"<<std::endl;
  std::cout<<"\tNP variable: "<<NPvar<<std::endl;
  std::cout<<"\tProject Tag: "<<tag<<std::endl;
  std::cout<<"\tRoot filename: "<<roofilename<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;
  std::cout<<"\tAsimov data:"<<(asimov?"yes":"no")<<std::endl;
  std::cout<<"\tPrint NPs:"<<(printNP?"yes":"no")<<std::endl;
  std::cout<<"\tPrint SubChannels:"<<(printSubChannels?"yes":"no")<<std::endl;
  std::cout<<"\tPOI value for conditional fit: "<<poi_value<<std::endl;
  std::cout<<"\tConditional fit:"<<(conditional_fit?"yes":"no")<<std::endl;
  std::cout<<"\tDo NLL profiles:"<<(do_nll?"yes":"no")<<std::endl;
  std::cout<<"\tDo Pulls:"<<(do_pulls?"yes":"no")<<std::endl;
  std::cout<<"\tDo Pulls for channels:"<<(do_pulls?"yes":"no")<<std::endl;
  std::cout<<"\tDraw Plot: " << (do_plot?"yes":"no")<<std::endl;
  std::cout<<"\tDraw 2D Contour Plot : "  << (do_contour?"yes":"no") << std::endl;
  if(do_contour){ std::cout << "\tContour X var " << xvar << " Contour Y var " << yvar << std::endl;}
  std::cout<<"\tDraw Morphing Plot: " << (do_morphing?"yes":"no")<<std::endl;
  std::cout<<"\tDraw NP Envelope Plot: " << (do_envelope?"yes":"no")<<std::endl;
  std::cout<<"\tFit with multiple POIs: " << (multiple_pois?"yes":"no")<<std::endl;
  std::cout<<"\tMinos: " << (minos?"yes":"no")<<std::endl;
  std::cout<<"\tMinimizer strategy order: "<<strategy<<(strategy == DEFAULT_STRATEGY_ORDER?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tMinimization process and error calculation are coupled in the minimization loop: " << (minim_errorcalc_coupled?"yes":"no")<<std::endl;
  std::cout<<"\tAdd Gamma parameters in the ranking: "<<(gamma?"yes":"no")<<std::endl;


  NLLProfileCreator nllpc;
  nllpc.SetInputFileName(rws);
  nllpc.SetOutputDirectoryName(outputdir);
  nllpc.SetOutputFileName(roofilename);
  nllpc.SetProjectTag(tag);
  nllpc.SetWorkspaceName("combined");
  nllpc.SetModelConfigName("ModelConfig");
  nllpc.SetObsDataName(asimov ? "asimovData" : "obsData");
  //nllpc.SetNormalizationNPname("norm"); //other variations : ATLAS_norm, default in Run2 from HComb: norm
  nllpc.SetNuisanceParametersWithoutAlphaPrefix();
  nllpc.Print_Times();
  //nllpc.Print_ModelObservables();
  if(printNP) nllpc.Print_NuisanceParameters();
  if(printNP) nllpc.Print_NuisanceParametersList();
  if(printSubChannels) nllpc.Print_SubChannels();
  if(printSubChannels) nllpc.Print_SubChannelsList();
  //    nllpc.Print_FitPDF_InitialParams();
  //    nllpc.Print_FitPDF_Model();
  nllpc.FitSubChannels();
  nllpc.SetRooMinimizerPrintLvl(-1); //MINUIT internal printing level: printlevel : -1  quiet (also suppress all warnings), 0  normal, 1  verbose http://root.cern.ch/root/html/TMinuit.html#TMinuit:SetPrintLevel  and  http://root.cern.ch/root/html/TMinuit.html#TMinuit:SetPrintLevel
  nllpc.SetRooFitMsgLvl(RooFit::ERROR); //DEBUG=0, INFO=1, PROGRESS=2, WARNING=3, ERROR=4, FATAL=5
  nllpc.SetMinimizerFitStrategy(strategy);
  nllpc.SetPOI(poi_value); //valid only for conditional fits
  if(conditional_fit) nllpc.SetFitConditional();
  nllpc.Minuit2();
  nllpc.MaxFitRetries(3);
  //nllpc.ParallelProcessing(Ncpus);
  nllpc.SetLumiRelativeError(0.029);
  if(minos) nllpc.Minos();
  if(minim_errorcalc_coupled) nllpc.CoupleMinimErrorCalc();
  nllpc.ScanPOI();
  if(NPvar.size()) nllpc.ScanVariable(NPvar);
  //nllpc.ScanGreen();
  nllpc.StoreGlobalFitMinimizationResults();
  nllpc.StoreCategoryFitMinimizationResults();
  if(dry) nllpc.DryRun();
  if(do_nll) nllpc.DoNLLScans();
  if(do_pulls) nllpc.DoPulls();
  if(do_pulls_channels) nllpc.DoPullsCategories();
  if(do_plot) nllpc.DrawPlot();
  if(do_contour){
	nllpc.DrawContour();
	nllpc.SetXvar(xvar);
        nllpc.SetYvar(yvar);
  }
  if(do_morphing) nllpc.DrawMorphingPlot();
  if(do_envelope) nllpc.DrawEnvelopePlot();
  if(multiple_pois) nllpc.FitMultiplePOIs();
  if(gamma) nllpc.enable_gamma_parameters(gamma);

  nllpc.Calculate();

}
//=========================================================================================
void LaunchNLLProfileScanner( const std::string &NPvar,
                              const std::string &rws,
                              const std::string &outputdir,
                              const bool asimov,
                              const bool minos,
			      const unsigned int strategy,
			      const bool minim_errorcalc_coupled,
                              const std::string &fit_category,
                              const std::string &tag,
                              const uint n_segments,
                              const bool dry)
{

  std::cout<<"LaunchNLLProfileScanner"<<std::endl;
  std::cout<<"\tNP variable: "<<NPvar<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;
  std::cout<<"\tAsimov data:"<<(asimov?"yes":"no")<<std::endl;
  std::cout<<"\tMinos: "<<(minos?"yes":"no")<<std::endl;
  std::cout<<"\tMinimization process and error calculation are coupled in the minimization loop: " << (minim_errorcalc_coupled?"yes":"no")<<std::endl;
  std::cout<<"\tFit category:"<<fit_category<<std::endl;
  std::cout<<"\tProject tag:"<<tag<<std::endl;
  std::cout<<"\tFixed NP segments:"<<n_segments<<std::endl;
  std::cout<<"\tMinimizer strategy order: "<<strategy<<(strategy == DEFAULT_STRATEGY_ORDER?" (default)":" (modified)")<<std::endl;

  std::string tree_file_name = NPvar.size() ? "nll_profiles." + tag + "." +  fit_category + "."  + NPvar : "test";

  NLLProfileScanner nllsc;
  nllsc.set_input_ws_file_name(rws);
  nllsc.set_output_tree_path_name(outputdir);
  nllsc.set_output_tree_file_name(tree_file_name);
  if(asimov) nllsc.set_asimov_data();
  nllsc.set_minos(minos);
  nllsc.set_minimizer_fit_strategy(strategy);
  nllsc.set_minim_errorcalc_coupled(minim_errorcalc_coupled);
  nllsc.set_fixed_nuisance_parameter(NPvar);
  nllsc.set_fixed_nuisance_parameter_segments(n_segments);
  nllsc.set_workspace(fit_category); //default is "combined"
  if(!dry) nllsc.Calculate();
}
//=========================================================================================
void LaunchNLLProfileNPRanking(
                               const std::string &NPvar,
                               const std::string &rws,
                               const std::string &outputdir,
                               const bool asimov,
                               const bool minos,
                               const unsigned int strategy,
			       const bool minim_errorcalc_coupled,
                               const std::string &fit_category,
                               const std::string &tag,
                               const bool recalc_sigma,
                               const bool prefit_impact,
                               const bool dry,
                               const bool multiple_pois,
                               const bool gamma,
                               const std::string &NPimp )
{

  std::cout<<"LaunchNLLProfileNPRanking"<<std::endl;
  std::cout<<"\tNP variable: "<<NPvar<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;
  std::cout<<"\tAsimov data:"<<(asimov?"yes":"no")<<std::endl;
  std::cout<<"\tMinos: "<<(minos?"yes":"no")<<std::endl;
  std::cout<<"\tMinimization process and error calculation are coupled in the minimization loop: " << (minim_errorcalc_coupled?"yes":"no")<<std::endl;
  std::cout<<"\tFit category:"<<fit_category<<std::endl;
  std::cout<<"\tRecalc sigma:"<<(recalc_sigma?"yes":"no")<<std::endl;
  std::cout<<"\tAdd Prefit-impact:"<<(prefit_impact?"yes":"no")<<std::endl;
  std::cout<<"\tAdd Gamma parameters in the ranking: "<<(gamma?"yes":"no")<<std::endl;
  std::cout<<"\tProject tag:"<<tag<<std::endl;
  std::cout<<"\tFit with multiple POIs : " << (multiple_pois?"yes":"no")<<std::endl;
  std::cout<<"\tEvaluate impact of the NP : " << NPimp << std::endl;
  std::cout<<"\tMinimizer strategy order: "<<strategy<<(strategy == DEFAULT_STRATEGY_ORDER?" (default)":" (modified)")<<std::endl;

  NLLProfileNPRanking nllnp;
  nllnp.set_variable(NPvar);
  nllnp.set_input_ws_file_name(rws);
  nllnp.SetOutputDirectoryName(outputdir);
  if(asimov) nllnp.set_asimov_data();
  nllnp.set_minos(minos);
  nllnp.set_minimizer_fit_strategy(strategy);
  nllnp.set_minim_errorcalc_coupled(minim_errorcalc_coupled);
  nllnp.set_workspace(fit_category); //default is "combined"
  nllnp.set_recalc_sigma(recalc_sigma);
  nllnp.set_prefit_impact(prefit_impact);
  if(multiple_pois) nllnp.FitMultiplePOIs();
  if(gamma) nllnp.enable_gamma_parameters(gamma);
  nllnp.check_np_impact(NPimp);

  if(!dry) nllnp.Calculate();
}
//=========================================================================================
void LaunchNLLProfileSignificance(
                                  const std::string &rws,
                                  const std::string &outputdir,
                                  const bool asimov,
                                  const bool nosyst
                                  )
{

  std::cout<<"LaunchNLLProfileSignificance"<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;
  std::cout<<"\tAsimov data:"<<(asimov?"yes":"no")<<std::endl;
  std::cout<<"\tNoSyst: "<<(nosyst?"yes":"no")<<std::endl;

  NLLProfileSignificance nllsig;
  nllsig.set_input_ws_file_name(rws);
  nllsig.SetOutputDirectoryName(outputdir);
  if(asimov) nllsig.set_asimov_data();
  nllsig.set_nosyst(nosyst);
  nllsig.Calculate();
}


//=========================================================================================
void LaunchNLLProfileLimits(
                            const std::string &rws,
                            const std::string &outputdir,
                            const bool asimov,
                            const unsigned int strategy,
                            const bool minos,
			    const bool minim_errorcalc_coupled,
                            const bool nosyst,
                            const bool onesided,
                            const double conflv,
                            const bool autoscan,
                            const int fragments,
                            const double poimin,
                            const double poimax,
                            const bool usetoys,
                            const int ntoys
                            )
{

  std::cout<<"LaunchNLLProfileLimits"<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;
  std::cout<<"\tAsimov data:"<<(asimov?"yes":"no")<<std::endl;
  std::cout<<"\tMinimizer strategy order: "<<strategy<<(strategy == DEFAULT_STRATEGY_ORDER?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tMinos: "<<(minos?"yes":"no")<<std::endl;
  std::cout<<"\tMinimization process and error calculation are coupled in the minimization loop: " << (minim_errorcalc_coupled?"yes":"no")<<std::endl;
  std::cout<<"\tNoSyst: "<<(nosyst?"yes":"no")<<std::endl;
  std::cout<<"\tOneSided: " <<(onesided?"yes":"no")<<std::endl;
  std::cout<<"\tConfidence level: "<<conflv<<(conflv== 0.95?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tAutoscan : "<<(autoscan?"yes":"no")<<std::endl;
  std::cout<<"\tFragments : " <<fragments<<(fragments==10?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tPoimin : " <<poimin<<(poimin==0?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tPoimax : " <<poimax<<(poimax==2?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tUse Toys : " <<(usetoys?"yes":"no")<<std::endl;
  std::cout<<"\tNumber of Toys : " << ntoys<<(ntoys==50?" (default)":" (modified)")<<std::endl;


  NLLProfileLimits nlllim;
  nlllim.set_input_ws_file_name(rws);
  nlllim.SetOutputDirectoryName(outputdir);
  if(asimov) nlllim.set_asimov_data();
  nlllim.set_minos(minos);
  nlllim.set_minim_errorcalc_coupled(minim_errorcalc_coupled);
  nlllim.set_nosyst(nosyst);
  nlllim.set_onesided(onesided);
  if(conflv != 0.95) nlllim.SetConfidenceLevel(conflv);
  nlllim.set_autoscan(autoscan);
  if(fragments != 10) nlllim.SetNumFragments(fragments);
  if(poimin != 0) nlllim.SetPoiMin(poimin);
  if(poimax != 2) nlllim.SetPoiMax(poimax);
  nlllim.set_use_toys(usetoys);
  if(ntoys != 50) nlllim.set_num_toys(ntoys);
  nlllim.SetMinimizerFitStrategy(strategy);
  nlllim.Calculate();
}

//=========================================================================================
void LaunchNLLCreateVarPlots(
                             const std::string &rws_input,
                             const std::string &rws_helper,
                             const std::string &outputdir
                             )
{
  std::cout<<"LaunchNLLCreateVarPlots"<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws_input<<std::endl;
  std::cout<<"\tFitResult input file: "<<rws_helper<<std::endl;
  std::cout<<"\tOutput directory:"<<outputdir<<std::endl;

  NLLCreateVarPlots nllcvp;
  nllcvp.set_input_ws_file_name(rws_input);
  nllcvp.set_helper_ws_file_name(rws_helper);
  nllcvp.SetOutputDirectoryName(outputdir);
  nllcvp.Calculate();
}

//=========================================================================================
void LaunchToys(const std::string &rws_input,
		const std::string &outputdir,
                const std::string &outputfile,
		const bool conditional,
		const double POI,
		const uint ntoys,
		const uint toys_seed,
                const bool saveyields,
                const bool calcsign
		)
{
  std::cout<<"LaunchToys"<<std::endl;
  std::cout<<"\tWorkspace input file: "<<rws_input<<std::endl;
  std::cout<<"\tOutput directory: "<<outputdir<<std::endl;
  std::cout<<"\tOutput file name: "<<outputfile << std::endl;
  std::cout<<"\tConditional fit:"<<(conditional?"yes":"no")<<std::endl;
  std::cout<<"\tValue of POI: "<<POI<<std::endl;
  std::cout<<"\tNumber of Toys : " << ntoys<<(ntoys==50?" (default)":" (modified)")<<std::endl;
  std::cout<<"\tToys seed: "<<toys_seed<<std::endl;
  std::cout<<"\tSave yields: " << saveyields << std::endl;
  std::cout<<"\tCheck significance: " << calcsign << std::endl;

  Toys toys(rws_input, outputdir, outputfile, POI, conditional, ntoys, toys_seed, saveyields, calcsign);

  toys.Calculate();
}

//=========================================================================================
unsigned int NumberCPUs(){

  SysInfo_t* s = new SysInfo_t();
  gSystem->GetSysInfo(s);
  unsigned int NCPUs = (unsigned int)s->fCpus;

  delete s;

  return NCPUs;
}

//=========================================================================================
