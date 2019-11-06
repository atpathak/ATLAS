#include "ErrorBreakdown/ErrorBreakdown.h"

ErrorBreakdown::~ErrorBreakdown(){

  if( m_ofile.is_open() )
    m_ofile.close();

  if(m_ofilename.size())
    Info("ErrorBreakdown::~ErrorBreakdown()", "Results are stored in '%s' ...", m_ofilename.c_str());

  Info("ErrorBreakdown::calculate()", "Error breakdown is over!");
}

bool ErrorBreakdown::load(){

  //filename
  if(m_ws_file_name.empty()){
    Error("ErrorBreakdown::load()", "Empty WS file name...");
    return false;
  }

  //input ws file
  m_ws_file = CxxUtils::make_unique<TFile>( m_ws_file_name.c_str(), "open" );
  if(!m_ws_file){
    Error("ErrorBreakdown::load()", "Cannot retrieve input file '%s' ...", m_ws_file_name.c_str());
    return false;
  }
  
  //check input
  Long_t id,size,flags,mt;
  gSystem->GetPathInfo(m_ws_file_name.c_str(), &id, &size, &flags, &mt);
  if( size < 10000 ){
    Error("ErrorBreakdown::load()", "Tried to open file '%s' with too few bytes %lui", m_ws_file_name.c_str(), size);
    return false;
  }

  if( m_ws_file->TestBit(TFile::kRecovered) ){
    Error("ErrorBreakdown::load()", "Tried to open an unclosed file '%s' ", m_ws_file_name.c_str() );
    return false;
  }

  if( m_ws_file->IsZombie() ){
    Error("ErrorBreakdown::load()", "File '%s' is zombie!", m_ws_file_name.c_str() );
    return false;
  }

  //workspace
  m_ws = (RooWorkspace*) m_ws_file->Get(m_ws_name.c_str());
  if(!m_ws){
    Error("ErrorBreakdown::load()", "Cannot retrieve workspace '%s' from input file '%s' ...", m_ws_name.c_str(), m_ws_file_name.c_str());
    return false;
  }

  //model config 
  m_mc = (ModelConfig*) m_ws->obj(m_mc_name.c_str());
  if(!m_mc){
    Error("ErrorBreakdown::load()", "Cannot retrieve model config '%s' from input file '%s' ...", m_mc_name.c_str(), m_ws_file_name.c_str());
    return false;
  }

  //data
  m_data = m_ws->data(m_data_name.c_str());
  if(!m_data){
    Error("ErrorBreakdown::load()", "Cannot retrieve data '%s' from input file '%s' ...", m_data_name.c_str(), m_ws_file_name.c_str());
    return false;
  }

  //PDF
  m_pdf = (RooSimultaneous*) m_ws->pdf(m_pdf_name.c_str());
  if(!m_pdf){
    Error("ErrorBreakdown::load()", "Cannot retrieve PDF '%s' from input file '%s' ...", m_pdf_name.c_str(), m_ws_file_name.c_str());
    return false;
  }

  //parameters
  m_params = (RooArgSet*) m_pdf->getParameters(*m_data) ;
  if(!m_params){
    Error("ErrorBreakdown::load()", "Cannot retrieve parameters contained in PDF '%s' from input file '%s' ...", m_pdf_name.c_str(), m_ws_file_name.c_str());
    return false;
  }

  //fit function
  m_fit_function = CxxUtils::make_unique<FitPDF>(m_print_fit_pdf_model,
                                                 m_print_fit_pdf_init_params,
                                                 m_RooMinimizer_print_lvl,
                                                 m_minimizer,
						 m_fit_strategy,
						 m_minos,
						 m_minim_errorcalc_coupled,
						 m_max_fit_retries,
						 m_n_cpus );

  if(!m_fit_function){
    Error("ErrorBreakdown::load()", "Couldn't create the fit PDF class object");
    return false;
  }


  //output dir
  if( gSystem->Exec( ("mkdir -p " + m_output_directory ).c_str() ) ){ //true = bad
    Error("ErrorBreakdown::load()", "Couldn't create directory '%s'", m_output_directory.c_str() );
    return false;
  }

  if( ! gSystem->OpenDirectory( m_output_directory.c_str() ) ){
    Error("ErrorBreakdown::load()", "Couldn't open directory '%s'", m_output_directory.c_str() );
    return false;
  }else{
    Info("ErrorBreakdown::load()", "Directory '%s' is successfully created", m_output_directory.c_str() );
  }

  //output file
  if(!ends_with(m_output_directory, "/"))
    m_output_directory.append( "/" );

  m_ofilename = m_output_directory+ "error_breakdown.dat";
  m_ofile.open(m_ofilename.c_str());

  //load input param files
  if(!this->read_in_files()){
    Error("ErrorBreakdown::load()", "Unable to load input files...");
    return false;
  }

  
  return true;
}

void ErrorBreakdown::clear(){

  m_error_set_poi.clear();
  m_error_set_fixed_sys.clear();
  m_error_set_fixed_all_sys.clear();
  m_error_set_fixed_theo.clear();

}

bool ErrorBreakdown::ends_with(std::string const & value, std::string const & ending){
  if (ending.size() > value.size()) return false;
  return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

bool ErrorBreakdown::is_file_ok(const std::ifstream &f){
  return f.good();
}

bool ErrorBreakdown::fill_in_content(std::ifstream &f, std::vector<std::string> &v){

  std::string line;
  while(std::getline(f, line)) {
    if(line.size())
      v.push_back(line);
  }
  
  return true;
}

bool ErrorBreakdown::read_in_file(std::ifstream &f, const std::string &name){

  f.open(name.c_str(), std::ifstream::in);

  if(! f.is_open() ){
    Error("ErrorBreakdown::read_in_file", "Couldn't open input file '%s'", name.c_str() );
    return false;
  }
 
  if(!is_file_ok(f)){
    Error("ErrorBreakdown::read_in_file", "Bad input file '%s'", name.c_str() );
    return false;
  }
  
  return true;
}

bool ErrorBreakdown::read_in_files(){

  if(! read_in_file(m_poi_input_file, m_poi_input_file_name) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in an input file...");
    return false;
  }

  if(! read_in_file(m_norm_params_input_file, m_norm_params_input_file_name) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in an input file...");
    return false;
  }

  if(! read_in_file(m_theo_params_input_file, m_theo_params_input_file_name) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in an input file...");
    return false;
  }

  if(! fill_in_content(m_poi_input_file, m_poi_names) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in content...");
    return false;
  }

  if(! fill_in_content(m_norm_params_input_file, m_norm_params) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in content...");
    return false;
  }

  if(! fill_in_content(m_theo_params_input_file, m_theory_params) ){
    Error("ErrorBreakdown::read_in_files", "Failure in read in content...");
    return false;
  }

  //important for POI
  if(m_poi_names.empty()){
    Error("ErrorBreakdown::load()", "No POI was provided...");
    return false;
  }
  
  std::cout<<"POI(s): "<<m_poi_names<<std::endl;
  std::cout<<"Norms: "<<m_norm_params<<std::endl;
  std::cout<<"Theory: "<<m_theory_params<<std::endl;

  return true;
}

bool ErrorBreakdown::string_has_substring(const std::string &str, const std::string &sub){

  return str.find(sub) != std::string::npos;
}

bool ErrorBreakdown::is_normalization(const std::string &str){

  return find( begin(m_norm_params), end(m_norm_params), str) != m_norm_params.end();
}

bool ErrorBreakdown::is_theory(const std::string &str){

  return find( begin(m_theory_params), end(m_theory_params), str) != m_theory_params.end();
}

bool ErrorBreakdown::create_params_iter(){
  m_params_iter = m_params->createIterator();
  return m_params_iter != NULL;
}

auto ErrorBreakdown::next_variable() -> RooRealVar* {
  return (RooRealVar*)m_params_iter->Next();
}

void ErrorBreakdown::subtract_error(const ErrorSet &e1, const ErrorSet &e2, float &up, float &down){
  
  up = std::sqrt( e1.up2() - e2.up2() );
  down = std::sqrt( e1.down2() - e2.down2() );
}

std::string ErrorBreakdown::adjust_up(float x){

  return "+"+std::to_string(std::fabs(x));
}

std::string ErrorBreakdown::adjust_down(float x){

  return "-"+std::to_string(std::fabs(x));
}

bool ErrorBreakdown::calculate(){

  //load setup
  if(!this->load()){
    Error("ErrorBreakdown::calculate()", "Unable to correctly load the model setup...");
    return false;
  }
  
  //clean up
  clear();

  //set POI to be constant
  RooRealVar * poi = (RooRealVar*) m_mc->GetParametersOfInterest()->first();
  poi->setVal(1);
  poi->setRange(-10., 10.);
  poi->setConstant(1);
  
  
  //variable
  RooRealVar* variable = NULL;
  //RooArgSet* m_minos_params = new RooArgSet(); //TMP

  //make POI non-const
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (free POIs)...");
    return false;
  }   
  while( (variable = this->next_variable()) ){
    std::string variable_name =  variable->GetName();
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	variable->setConstant(0);
	variable->setRange(-10., 10.);
	//m_minos_params->add(*variable); //TMP
      }
    }
  }
  
  //paramsIn->Print("v");
  
  //store initial params
  m_ws->saveSnapshot("snapshot_initial_parameters", *m_params);
  
  //fit output
  int minuit_status(-1); int error_status(-1); double edm(1.);

  //general fit
  auto fit_result = m_fit_function->FitResult(m_mc, m_pdf, m_data, minuit_status, error_status, edm);
  
  if(!fit_result){
    Error("ErrorBreakdown::calculate()", "Unable to perform the first fit of the  model...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "First fit has been performed...");
  }

  //store snapshot
  m_ws->saveSnapshot("snapshot_unconditional", *m_params);

  //read in POI and error
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (read POIs)...");
    return false;
  }
  while( (variable =  this->next_variable()) ){
    std::string variable_name =  variable->GetName();
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	m_error_set_poi.emplace_back( ErrorSet(variable->getVal(), variable->getErrorHi(), variable->getErrorLo()) );
      }
    }
  }

  // Perform fi by fixing all systematics
  if(!m_ws->loadSnapshot("snapshot_unconditional") ){
    Error("ErrorBreakdown::calculate()", "Unable to load the unconditional snapshot...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Performing a new fit with all systematics except normalization factors...");
  }

  //make all systs constant apart the norms
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (fix systs)...");
    return false;
  }
  while( (variable = this->next_variable()) ){
    std::string variable_name =  variable->GetName();    

    //set all to constant
    if( ! is_normalization(variable_name) ){
      variable->setConstant(1);
      Info("ErrorBreakdown::calculate()", "Performing a new fit with all systematics - Setting variable %s to constant...", variable_name.c_str());
    }	

    //revert POIs
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	variable->setConstant(0);	
      }
    }

  }

  //fit result
  fit_result = m_fit_function->FitResult(m_mc, m_pdf, m_data, minuit_status, error_status, edm);
  
  if(!fit_result){
    Error("ErrorBreakdown::calculate()", "Unable to perform the second fit of the  model (all systs are fixed, except norm)...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Second fit has been performed...");
  }

  //read new errors
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (read errors for fixed systs)...");
    return false;
  }
  while( (variable =  this->next_variable()) ){
    std::string variable_name =  variable->GetName();
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	m_error_set_fixed_all_sys.emplace_back( ErrorSet(variable->getVal(), variable->getErrorHi(), variable->getErrorLo()) );
      }
    }
  }

  // Perform fit by fixing all systs apart from theory, cross section etc and Norm
  if(!m_ws->loadSnapshot("snapshot_unconditional") ){
    Error("ErrorBreakdown::calculate()", "Unable to load the unconditional snapshot...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Performing a new fit with all systematics fixed except theory + norm...");
  }

  //make all systs const except theory and Norm (let them float)
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (fix systs except theory + norm)...");
    return false;
  }
  while( (variable = this->next_variable()) ){
    std::string variable_name =  variable->GetName();    

    //set all to constant
    if( ! (is_theory(variable_name) || is_normalization(variable_name)) ){
      variable->setConstant(1);
      Info("ErrorBreakdown::calculate()", "Performing a new fit with all systematics except theory+norm- Setting variable %s to constant...", variable_name.c_str());
    }	

    //revert POIs
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	variable->setConstant(0);	
      }
    }

  }

  //fit result
  fit_result = m_fit_function->FitResult(m_mc, m_pdf, m_data, minuit_status, error_status, edm);
  
  if(!fit_result){
    Error("ErrorBreakdown::calculate()", "Unable to perform the second fit of the  model (all systs are fixed, except theory+norm)...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Third fit has been performed...");
  }

  //read new errors
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (read errors for fixed systs, except theory+norm)...");
    return false;
  }
  while( (variable =  this->next_variable()) ){
    std::string variable_name =  variable->GetName();
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	m_error_set_fixed_sys.emplace_back( ErrorSet(variable->getVal(), variable->getErrorHi(), variable->getErrorLo()) );
      }
    }
  }

  // Perform fit by fixing all  theory, cross section etc 
  //QUESTION: Do we need to include NORm as well?
  if(!m_ws->loadSnapshot("snapshot_unconditional") ){
    Error("ErrorBreakdown::calculate()", "Unable to load the unconditional snapshot...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Performing a new fit with all theory fixed...");
  }

  //make all theory systs const (let them float)
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (fix theory)...");
    return false;
  }
  while( (variable = this->next_variable()) ){
    std::string variable_name =  variable->GetName();    

    //set all to constant
    if( is_theory(variable_name) ){
      variable->setConstant(1);
      Info("ErrorBreakdown::calculate()", "Performing a new fit with all theory systematics - Setting variable %s to constant...", variable_name.c_str());
    }	

    //revert POIs
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	variable->setConstant(0);	
      }
    }

  }

  //fit result
  fit_result = m_fit_function->FitResult(m_mc, m_pdf, m_data, minuit_status, error_status, edm);
  
  if(!fit_result){
    Error("ErrorBreakdown::calculate()", "Unable to perform the second fit of the  model (all theory systs are fixed)...");
    return false;
  }else{
    Info("ErrorBreakdown::calculate()", "Fourth fit has been performed...");
  }

  //read new errors
  if(! this->create_params_iter() ){
    Error("ErrorBreakdown::calculate()", "Unable to create iterator for parameters (read errors for fixed theory systs)...");
    return false;
  }
  while( (variable =  this->next_variable()) ){
    std::string variable_name =  variable->GetName();
    for(auto POI : m_poi_names){
      if( string_has_substring(variable_name, POI) ){
	m_error_set_fixed_theo.emplace_back( ErrorSet(variable->getVal(), variable->getErrorHi(), variable->getErrorLo()) );
      }
    }
  }
  


  //final estimate
  for(unsigned int ipoi=0; ipoi < m_poi_names.size(); ipoi++){
    m_ofile<<"Results for "<< m_poi_names.at(ipoi)<<std::endl;

    /*
    float aver = m_error_set_poi.at(ipoi).average();
    float aver_fixed_all_sys = m_error_set_fixed_all_sys.at(ipoi).average();
    float aver_fixed_sys = m_error_set_fixed_sys.at(ipoi).average();
    float aver_fixed_theo = m_error_set_fixed_theo.at(ipoi).average();
    */

    float error_theo_up(0); float error_theo_down(0);
    subtract_error(m_error_set_poi.at(ipoi), m_error_set_fixed_theo.at(ipoi), error_theo_up, error_theo_down);

    float error_sys_up(0); float error_sys_down(0);
    subtract_error(m_error_set_poi.at(ipoi), m_error_set_fixed_sys.at(ipoi), error_sys_up, error_sys_down);

    float error_all_sys_up(0); float error_all_sys_down(0);
    subtract_error(m_error_set_poi.at(ipoi), m_error_set_fixed_all_sys.at(ipoi), error_all_sys_up, error_all_sys_down);

    m_ofile<<"POI: "<<m_error_set_poi.at(ipoi).center()<<std::endl;
    m_ofile<<"Total error     : "<< adjust_up(m_error_set_poi.at(ipoi).up())		<<" "<<adjust_down(m_error_set_poi.at(ipoi).down())<<std::endl;
    m_ofile<<"Statistical     : "<< adjust_up(m_error_set_fixed_all_sys.at(ipoi).up())	<<" "<<adjust_down(m_error_set_fixed_all_sys.at(ipoi).down())<<std::endl;
    m_ofile<<"Systematic      : "<< adjust_up(error_all_sys_up)				<<" "<<adjust_down(error_all_sys_down)<<std::endl;
    m_ofile<<"Exp Systematic  : "<< adjust_up(error_sys_up)				<<" "<<adjust_down(error_sys_down)<<std::endl;
    m_ofile<<"Theo Systematic : "<< adjust_up(error_theo_up)			        <<" "<<adjust_down(error_theo_down)<<std::endl;
    
    m_ofile<<std::endl;
    m_ofile<<"\\begin{equation}"<<std::endl;
    m_ofile<<"\\mu = "<<m_error_set_poi.at(ipoi).center()
	     <<"\\oplus {}^{"<< adjust_up(m_error_set_poi.at(ipoi).up())		<<"}_{"<<adjust_down(m_error_set_poi.at(ipoi).down())<<"} (\\mathrm{total})"
	     <<"\\oplus {}^{"<< adjust_up(m_error_set_fixed_all_sys.at(ipoi).up())	<<"}_{"<<adjust_down(m_error_set_fixed_all_sys.at(ipoi).down())<<"} (\\mathrm{stat.})"
	     <<"\\oplus {}^{"<< adjust_up(error_all_sys_up)				<<"}_{"<<adjust_down(error_all_sys_down)<<"} (\\mathrm{syst.})"
	     <<"\\oplus {}^{"<< adjust_up(error_sys_up)					<<"}_{"<<adjust_down(error_sys_down)<<"} (\\mathrm{exp.})"
	     <<"\\oplus {}^{"<< adjust_up(error_theo_up)				<<"}_{"<<adjust_down(error_theo_down)<<"} (\\mathrm{theo.})"
	     <<std::endl;
    m_ofile<<"\\end{equation}"<<std::endl<<std::endl;

    //float error
  }

  return true;
}
