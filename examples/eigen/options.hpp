#ifndef OPTIONS_HPP
#define OPTIONS_HPP

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <algorithm>
#include <functional>
#include <utility>
#include <exception>

#include <boost/program_options.hpp>
#include <boost/tokenizer.hpp>

#include <AMD/log.hpp>

namespace po = boost::program_options;

/**
 * Additional command line parser which interprets '@something' as a
 * option "config-file" with the value "something".
 */ 
std::pair<std::string, std::string> at_option_parser(std::string const& s) {
  if ('@' == s[0]) {
    return std::make_pair (std::string("params"), s.substr(1));
  } else { 
    return std::make_pair (std::string(), std::string()); 
  }
}

/**
 * A structure that is used to pass options to the DYCK solver. This structure
 * has default values embedded. No accessor functions are being written.
 */
struct options_t {
  /** Integer options */
  int64_t n; /**< matrix dimension */
  int64_t nnz; /**< number of non-zeros in the matrix */
  int64_t m; /**< number of terms in Taylor expansion */
  int64_t t; /**< number of iterations for power method for eigen-value est */
  int64_t p; /**< number of iterations for trace estimation using Avron-Toledo*/
  int64_t num_threads; /**< number of threads to use for the experiment */
  int64_t verbosity; /**< debugging level to use for this experiment */
  int64_t maxM;    /**< run benchmarks with 1...maxM */
  int64_t maxIter; /**< run benchmarks 1..maxIter so we can be mean/variance*/

  /** Boolean options */
  bool use_random; /**< should we generate a random matrix */
  bool exact_logdet;  /**< should we compute the exact logdet? */
  bool exact_trace;   /**< should we compute the exact trace? */
  bool exact_l2_norm; /**< should we compute the exact spectral norm? */
  bool diag_dominant; /**< should we generate diagonally dominant? */
  bool benchmark;     /**< run the benchmarking with maxM and maxIter? */
  bool gen_and_dump;  /**< generate the random matrix and dump it? */

  /** Floating point options */
  double alpha;   /**< this should be between 1.0 and 2.0 --- default 1.5 */
  double epsilon; /**< error that we can tolerate */
  double delta;   /**< probability of failure */

  /** String options */
  std::string input_filename; /**< file name that you want to use */
  std::string input_fileformat; /**< file format that you want to use */
  std::string separator; /**< character separator for CSV files */

  bool exit_on_return; /**< should we just print the help message and exit */

  /**
   * The constructor takes in all the command line parameters and parses them.
   */ 
  options_t (int64_t argc, 
             char** argv, 
             bool is_root) : exit_on_return(false) {
    /** Set up the options that we want */
    po::options_description desc ("Approximate Log Det (A) Program Options");
    desc.add_options()
  ("help", "produce a help message")
  ("n", po::value<int64_t>(&n)->default_value(-1), 
        "Dimension of the PSD matrix")
  ("nnz", po::value<int64_t>(&nnz)->default_value(-1), 
        "Number of non zeros in the PSD matrix")
  ("m", po::value<int64_t>(&m)->default_value(4), 
        "Number of terms in Taylor expansion of log")
  ("p", po::value<int64_t>(&p)->default_value(-1), 
        "Number of Gaussians for trace estimation using Avron-Toledo")
  ("t", po::value<int64_t>(&t)->default_value(-1), 
        "Number of iterations for power method for eigen-value estimation")
  ("num-threads", po::value<int64_t>(&num_threads)->default_value(4), 
                  "Number of threads to use for this experiment")
  ("verbosity", po::value<int64_t>(&verbosity)->default_value(0), 
                  "Debugging level to use for this experiment")
  ("maxM", po::value<int64_t>(&maxM)->default_value(10), 
                  "Run benchmarks from 1..maxM")
  ("maxIter", po::value<int64_t>(&maxIter)->default_value(10), 
                  "Run benchmarks from 1..maxIter")
  ("use-random", po::value<bool>(&use_random)->default_value(false),
            "Should we run a simulated experiment")
  ("exact-logdet", po::value<bool>(&exact_logdet)->default_value(false),
             "Should we compute exact values for trace and spectral norm")
  ("exact-trace",po::value<bool>(&exact_trace)->default_value(false),
            "Should we compute exact values for trace")
  ("exact-l2-norm",po::value<bool>(&exact_l2_norm)->default_value(false),
            "Should we compute exact values for spectral norm")
  ("diag-dominant",po::value<bool>(&diag_dominant)->default_value(true),
            "Should we generate diagonally dominant random matrices")
  ("benchmark",po::value<bool>(&benchmark)->default_value(false),
            "Should we run benchmarking with maxM and maxIter?")
  ("gen-and-dump",po::value<bool>(&gen_and_dump)->default_value(false),
            "Should we generate and dump the data?")
  ("alpha", po::value<double>(&alpha)->default_value(1.5),
            "lambda_1 < alpha < 2lambda_1 --- default (1.5)")
  ("epsilon", po::value<double>(&epsilon)->default_value(1e-4),
            "Error that we can tolerate")
  ("delta", po::value<double>(&delta)->default_value(0.10),
            "Failure rate that we can tolerate")
  ("input", po::value<std::string>(&input_filename)->default_value(""),
            "Input file name")
  ("format", po::value<std::string>(&input_fileformat)->default_value(""),
            "Input file format (DENSE_TXT, DENSE_BIN, SPARSE_IJV, SPARSE_MM")
  ("separator", po::value<std::string>(&separator)->default_value(" "),
            "Separator token for ASCII formats")
  ("params", po::value<std::string>(), 
                    "can be specified with '@name', too")
      ; /* end options */

    /** create a variable map to hold all these things */
    po::variables_map vm;
    po::store(po::command_line_parser(argc, argv).options(desc)
              .extra_parser(at_option_parser).run(), vm);
 
    /** Print help and return if needed */
    if (vm.count ("help")) { 
      if (is_root) std::cout << desc;
      exit_on_return = true;
      return;
    } 
    
    try {
      if (vm.count("params")) {
        /** Load the file and tokenize it */
        std::ifstream ifs (vm["params"].as<std::string>().c_str());
        if (false == ifs.is_open()) {
          std::cout << "Could not open params file" << std::endl;
          exit_on_return = true;
          return;
        }
      
        /** Read the whole file into a string */
        std::stringstream ss;
        ss << ifs.rdbuf ();
        boost::char_separator<char> sep(" \n\r");
        std::string sstr(ss.str());
        boost::tokenizer<boost::char_separator<char> > tok(sstr, sep);
        std::vector<std::string> args;
        std::copy (tok.begin(), tok.end(), std::back_inserter(args));
      
        /** Parse the file and store the options */
        po::store(po::command_line_parser(args).options(desc).run(), vm);
      }
      
      po::notify(vm);
    } catch (std::exception& e) {
      std::cout << e.what() << std::endl;
      exit_on_return = true;
    }

    /** Force the issue on use random */
    if (""==input_filename || gen_and_dump) use_random = true;

    /** Now, check for error conditions */
    if (0 > n && ("" == input_filename || "SPARSE_MM" != input_fileformat)) {
      LOG_ERROR << "Invalid matrix size";
      exit_on_return = true;
    } else if ((0 < n) && (0 < nnz) && ((n*n) < nnz)) {
      LOG_ERROR << "Incompatible n and nnz sizes";
      exit_on_return = true;
    } else if ((false==use_random) && ("" == input_filename)) {
      LOG_ERROR << "Input file name not given";
      exit_on_return = true;
    } else if ((false==use_random) && ("" == input_fileformat)) {
      LOG_ERROR << "Input file format not given";
      exit_on_return = true;
    } else if (1.0 > alpha || 2.0 < alpha) {
      LOG_ERROR << "alpha should be in (1,2)";
      exit_on_return = true;
    } else if (1000 < n && (exact_logdet || exact_trace || exact_l2_norm)) {
      LOG_WARNING << "dangerous to compute exact quantities for this n";
    } else if ("" != input_fileformat &&
               "DENSE_TXT"  != input_fileformat &&
               "DENSE_BIN"  != input_fileformat &&
               "SPARSE_IJV" != input_fileformat &&
               "SPARSE_MM"  != input_fileformat) {
      LOG_ERROR << "Invalid input file format";
      exit_on_return = true;
    }

    /** Set the values of t and p */
    if (0>t) t = log(4.0 * n);
    if (0>p) p = (20.0 * log(2.0/delta)) / (epsilon*epsilon);

  }

  void pretty_print (bool is_root) const {
    if (is_root) {
      std::cout << "n = " << n << std::endl;
      std::cout << "nnz = " << nnz << std::endl;
      std::cout << "p = " << p << std::endl;
      std::cout << "t = " << t << std::endl;
      std::cout << "m = " << m << std::endl;
      std::cout << "num-threads = " << num_threads << std::endl;
      std::cout << "verbosity = " << verbosity << std::endl;
      std::cout << "use-random = " << use_random << std::endl;
      std::cout << "exact-logdet = " << exact_logdet << std::endl;
      std::cout << "exact-trace = " << exact_trace << std::endl;
      std::cout << "exact-l2-norm = " << exact_l2_norm << std::endl;
      std::cout << "diag-dominant = " << diag_dominant << std::endl;
      std::cout << "alpha = " << alpha << std::endl;
      std::cout << "epsilon = " << epsilon << std::endl;
      std::cout << "delta = " << delta << std::endl;
      std::cout << "input = " << input_filename << std::endl;
      std::cout << "format = " << input_fileformat << std::endl;
      std::cout << "separator = \"" << separator << "\"" << std::endl;
    }
  }
};

#endif // OPTIONS_HPP
