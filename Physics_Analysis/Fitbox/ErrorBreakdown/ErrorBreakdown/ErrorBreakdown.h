#ifndef _ERRORBREAKDOWN_
#define _ERRORBREAKDOWN_

#include "FitPDF/FitPDF.h"
#include "FitDefinitions/FitDefinitions.h"
#include "FitIncludes/FitIncludes.h"

using namespace RooFit;
using namespace RooStats;


/**
 * @class ErrorSet
 */
class ErrorSet {

 public:
  explicit ErrorSet( float center, float up, float down) : m_center(center), m_up(up), m_down(down) {}

  explicit ErrorSet( float up, float down) : ErrorSet(0.f, up, down) {}
  
  ~ErrorSet() = default;

 public:
  inline void set_up(float x){ m_up = x;}
  inline void set_down(float x){m_down = x;}  
  inline void set_center(float x){m_center = x;}

  inline float center() const { return m_center;}
  inline float up() const { return m_up;}
  inline float down() const { return m_down;}
  inline float up2() const { return m_up * m_up;}
  inline float down2() const { return m_down * m_down;}

  inline float average(){ return (std::fabs(m_up) + std::fabs(m_down))/2.f; }
 private:
  float m_center;
  float m_up;
  float m_down;

};

template <typename T>
std::ostream& operator<< (std::ostream& out, const std::vector<T>& v) {
  if ( !v.empty() ) {
    out << '[';
    std::copy (v.begin(), v.end(), std::ostream_iterator<T>(out, ", "));
    out << "\b\b]";
  }
  return out;
}

/**
 * @class ErrorBreakdown
 */
class ErrorBreakdown  {

 public:
  
  /**
   * Custom ctor */
  ErrorBreakdown(){};

  /**
   * Custom dtor */
  ~ErrorBreakdown();

  /**
   * Callable methods to set up the fitting procedure's attributes
   * @{
   */
  inline void set_workspace_file(const std::string &wsf){ m_ws_file_name = wsf; }
  inline void set_output_directory(const std::string & d){ m_output_directory = d;}
  inline void set_minuit2(){ m_minimizer = "Minuit2"; }
  inline void set_poi_file(const std::string &s){ m_poi_input_file_name = s;}
  inline void set_norm_file(const std::string &s){ m_norm_params_input_file_name = s;}
  inline void set_theo_file(const std::string &s){ m_theo_params_input_file_name = s;}
  inline void enable_minos(){ m_minos = true;}
  inline void set_strategy_order(const unsigned int strategy) {m_fit_strategy = strategy;}
  inline void couple_minimization_and_error_calculation() {m_minim_errorcalc_coupled = true;}

  //main function
  bool calculate();
  /**
   *@}
   */


 private:
  
  /**
   * Private memebr functions
   */
  bool load();
  void clear();
  bool string_has_substring(const std::string &str, const std::string &sub);
  bool create_params_iter();
  auto next_variable() -> RooRealVar*;
  bool is_normalization(const std::string &str);
  bool is_theory(const std::string &str);
  void subtract_error(const ErrorSet &e1, const ErrorSet &e2, float &up, float &down);
  std::string adjust_up(float x);
  std::string adjust_down(float x);
  bool fill_in_content(std::ifstream &f, std::vector<std::string> &v);
  bool read_in_file(std::ifstream &f, const std::string &name);
  bool read_in_files();
  bool is_file_ok(const std::ifstream &f);
  bool ends_with(std::string const & value, std::string const & ending);
  /**
   *Private member attributes
   */
  //can be set
  std::string	m_ws_file_name = "";
  std::string	m_output_directory = "./";
  std::string	m_ws_name = "combined";
  std::string	m_mc_name = "ModelConfig";
  std::string	m_data_name = "obsData";
  std::string	m_pdf_name = "simPdf";
  bool		m_print_fit_pdf_model = false;
  bool		m_print_fit_pdf_init_params = false;
  bool		m_RooMinimizer_print_lvl = false;
  std::string	m_minimizer = "Minuit";
  unsigned int	m_fit_strategy = 102;
  bool		m_minos = false;
  bool		m_minim_errorcalc_coupled = false;
  unsigned int	m_max_fit_retries = 5;
  bool		m_n_cpus = false;
  std::string	m_poi_input_file_name;
  std::string	m_norm_params_input_file_name;
  std::string	m_theo_params_input_file_name;

  //model
  std::unique_ptr<TFile> 	m_ws_file = nullptr;
  RooWorkspace *		m_ws = NULL;
  ModelConfig *			m_mc  = NULL;
  RooAbsData *			m_data = NULL;
  RooSimultaneous *		m_pdf = NULL;
  RooArgSet *			m_params = NULL;
  RooArgSet *			m_constrained_params = NULL;
  RooArgSet*			m_minos_params = NULL;
  TIterator*			m_params_iter = NULL;

  //parsed attributes
  std::vector<std::string> m_poi_names;
  std::vector<std::string> m_norm_params;
  std::vector<std::string> m_theory_params;

  //Error sets
  std::vector<ErrorSet> m_error_set_poi;
  std::vector<ErrorSet> m_error_set_fixed_all_sys;
  std::vector<ErrorSet> m_error_set_fixed_sys;
  std::vector<ErrorSet> m_error_set_fixed_theo;

  /**
   * @short Aggregation to @a FitPDF
   */
  std::unique_ptr<FitPDF> m_fit_function;

  /**
   * @short Fit result
   */
  RooFitResult  *m_fit_result = NULL;

  /**
   * @short output file
   */
  std::string m_ofilename;
  std::ofstream m_ofile;

  /**
   * Input files
   */

  std::ifstream	m_poi_input_file;
  std::ifstream	m_norm_params_input_file;
  std::ifstream	m_theo_params_input_file;

};
#endif
