#ifndef LOAD_HPP
#define LOAD_HPP

#include <sys/stat.h>
#include <boost/shared_ptr.hpp>
#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

#include <AMD/log.hpp>

#include <Eigen/Dense>
#include <Eigen/Sparse>

#include <fstream>

namespace {
/* check if something is a directory */
static inline bool is_directory (const char* candidate) {
  struct stat s;
  return (0 == stat (candidate, &s) && (s.st_mode&S_IFDIR));
}

/* check if something is a file */
static inline bool is_file (const char* candidate) {
  struct stat s;
  return (0 == stat (candidate, &s) && (s.st_mode&S_IFREG));
}

/**
 * Read a sparse format (i,j,v)
 */
template <typename IndexOutputIterator,
          typename ValueOutputIterator>
void read_ascii_ijv (const std::string& input_filename,
                     IndexOutputIterator row_indices,
                     IndexOutputIterator col_indices,
                     ValueOutputIterator values) {
  typedef typename std::iterator_traits<IndexOutputIterator>::value_type 
                                                                index_type;
  typedef typename std::iterator_traits<ValueOutputIterator>::value_type 
                                                                value_type;
  /** open the file */
  std::ifstream in (input_filename.c_str());
  if (false==in.is_open()) { 
    LOG_TRACE << "Could not open " <<input_filename;
    return; 
  }

  /** create a token separator */
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_type;
  boost::char_separator<char> separators(" ");

  /** iterate over all the lines and populate the buffer */
  std::string current_line;
  for (int64_t i=0; getline (in, current_line); ++i) {
    tokenizer_type tokens(current_line, separators);
    int64_t token_index = 0;
    for (tokenizer_type::iterator token_iter = tokens.begin();
         token_iter != tokens.end(); 
         ++token_iter, ++token_index) {
      switch (token_index) {
        case 0: 
        *row_indices++ = boost::lexical_cast<index_type>(token_iter->c_str());
        break;

        case 1: 
        *col_indices++ = boost::lexical_cast<index_type>(token_iter->c_str());
        break;

        case 2: 
        *values++ = boost::lexical_cast<value_type>(token_iter->c_str());
        break;
      };
    }
  }

  /* close the file stream */
  in.close();
}

/**
 * [values in double precision in column-major order]
 */
template <typename OutputIterator>
void read_ascii_dense (const std::string& input_filename,
                       OutputIterator result) { 
  typedef typename std::iterator_traits<OutputIterator>::value_type value_type;

  /** open the file */
  std::ifstream in (input_filename.c_str());
  if (false==in.is_open()) { 
    LOG_ERROR << "Could not open " << input_filename;
    return; 
  }

  /** create a token separator */
  typedef boost::tokenizer<boost::char_separator<char> > tokenizer_type;
  boost::char_separator<char> separators(" ");

  /** iterate over all the lines and populate the buffer */
  std::string current_line;
  for (int64_t i=0; getline (in, current_line); ++i) {
    tokenizer_type tokens(current_line, separators);
    for (tokenizer_type::iterator token_iter = tokens.begin();
         token_iter != tokens.end(); 
         ++token_iter) {
      *result++=boost::lexical_cast<value_type>(token_iter->c_str());
    }
  }

  /* close the file stream */
  in.close();
}

/**
 * [values in double precision in column-major order]
 */
template <typename RandomAccessIterator>
void read_binary_dense (const std::string& input_filename,
                        int64_t n,
                        RandomAccessIterator values) { 
  typedef typename std::iterator_traits<RandomAccessIterator>::value_type 
                                                               value_type;

  /* open the file */
  std::ifstream in (input_filename.c_str(), std::ios::binary);

  /* check for any obvious errors */
  if (false==in.is_open()) { 
    LOG_ERROR << "Could not open " << input_filename;
    return;
  }

  /* Now, read the requested number of bytes */
  in.read (reinterpret_cast<char*>(&(values[0])), 
           (sizeof(value_type)*(n*n)));

  /* close the file stream */
  in.close();
}

} // anon namespace

namespace AMD {

/**
 * This function exists to load a dense matrix into memory. Make sure that the
 * file that is passed actually exists and contains the matrix in the following
 * format: 
 * 
 * At the end of the operation, the matrix is loaded in a 
 * Eigen::Matrix<double, Eigen::Dynamic, Eigen::Dynamic>
 *
 * @param[in] file_name The file that we want to load
 * @param[in] n The dimension of the matrix
 * @return A shared pointer that has the required matrix
 */
static inline boost::shared_ptr<Eigen::Matrix<double, 
                                Eigen::Dynamic, 
                                Eigen::Dynamic> > 
load_dense (const std::string& input_filename,
            std::string& input_fileformat,
            int64_t n) { 
  typedef Eigen::Matrix<double, 
                        Eigen::Dynamic, 
                        Eigen::Dynamic> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;

  /** 0. Declare the return value */
  MatrixPtrType A;

  /** 1. Check for validity of arguments */
  if (false == is_file (input_filename.c_str())) {
    LOG_ERROR << input_filename << " is not a valid file";
    return A;
  }
 
  /** 3. File is valid, simply read in the matrix */
  A = MatrixPtrType (new MatrixType(n,n));
  if ("DENSE_TXT" == input_fileformat) {
    read_ascii_dense (input_filename, A->data());
  } else if ("DENSE_BIN" == input_fileformat) {
    read_binary_dense (input_filename, n, A->data());
  }

  return A;
}

/**
 * This function exists to load a sparse matrix into memory. Make sure that the
 * file that is passed actually exists and contains the matrix in the following
 * format: 
 * 
 * At the end of the operation, the matrix is loaded in a 
 * Eigen::SparseMatrix<double>
 *
 * @param[in] file_name The file that we want to load
 * @param[in] n The dimension of the matrix
 * @return A shared pointer that has the required matrix
 */
static inline boost::shared_ptr<Eigen::SparseMatrix<double> >
load_sparse (const std::string& input_filename,
             std::string& input_fileformat,
             int64_t n,
             int64_t nnz) { 
  typedef Eigen::SparseMatrix<double> MatrixType;
  typedef boost::shared_ptr<MatrixType> MatrixPtrType;

  /** 0. Declare the return value */
  MatrixPtrType A;

  /** 1. Check for validity of arguments */
  if (false == is_file (input_filename.c_str())) {
    LOG_ERROR << input_filename << " is not a valid file";
    return A;
  }
  
  std::vector<Eigen::Triplet<double> > triplets(nnz);
  A = MatrixPtrType (new MatrixType(n,n));
 
  /** 3. File is valid, simply read in the matrix */
  if ("SPARSE_IJV" == input_fileformat) {
    std::vector<int64_t> row_indices(nnz);
    std::vector<int64_t> col_indices(nnz);
    std::vector<double> values(nnz);
    read_ascii_ijv (input_filename, 
                    row_indices.begin(), 
                    col_indices.begin(), 
                    values.begin());
    for (int64_t i=0; i<nnz; ++i) {
      triplets.at(i) = Eigen::Triplet<double>(row_indices[i], 
                                              col_indices[i], 
                                              values[i]);
    }
  } 

  A->setFromTriplets(triplets.begin(), triplets.end());
  A->makeCompressed();

  return A;
}

} // namespace AMD

#endif /** LOAD_HPP */
