#ifndef Exception_H
#define Exception_H

/**
* @file Exception.hpp
*
* @author Gabriel Borges
*
* @brief This file defines the exception classes for AMD, each containing a
* different exception to be thrown on the normal use of the library.
* They are all specialisations of std::exception, contained in <stdexcept>.
**/

#include <stdexcept>

namespace AMD {

  /**
  * @brief The mismatched_dimension_exception_t class. This class inherits from
  * std::exception.
  */
  class mismatched_dimension_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Matrix dimensions do not match!";
    }
  } mismatched_dimension; //end mismatched_dimension_exception class definition

  /**
  * @brief The internal_node_exception_t class. This class inherits from
  * std::exception.
  */
  class internal_node_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Node is not a leaf node!";
    }
  } internal_node;

  /**
  * @brief The variable_function_exception_t class. This class inherits from
  * std::exception.
  */
  class variable_function_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Function is not constant!";
    }
  } variable_function;

  /**
  * @brief The constant_function_exception_t class. This class inherits from
  * std::exception.
  */
  class constant_function_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Function is not variable!";
    }
  } constant_function;

  /**
  * @brief The invalid_shared_ptr_exception_t class. This class inherits from
  * std::exception.
  */
  class invalid_shared_ptr_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "One or more shared pointers are not valid anymore!";
    }
  } invalid_shared_ptr;

  /**
  * @brief The wrong_operation_exception_t class. This class inherits from
  * std::exception.
  */
  class wrong_operation_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Mathematical operation does not match expectances!";
    }
  } wrong_operation;

  /**
  * @brief The null_node_exception_t class. This class inherits from
  * std::exception.
  */
  class null_node_exception_t : public std::exception {
  public:
    virtual const char* what() const throw() {
      return "Node does not exist!";
    }
  } null_node;

}

#endif /* Exception_H */