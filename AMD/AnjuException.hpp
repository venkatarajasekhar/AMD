#ifndef AMD_EXCEPTION_HPP
#define AMD_EXCEPTION_HPP
/**
 * \file exception.hpp
 * \brief Implementation of exceptions
 */ 

#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <cstdlib>

#include <AMD/config.h>

namespace {
/* Declaring some enumarations */
enum { AMD_ERROR = -0xFFFF, /**< An error has occured */
       AMD_MISMATCHED_DIMENSIONS, /**< Matrices don't match up */
       AMD_INTERNAL_NODE, /**< Internal node when leaf node expected */
       AMD_CONSTANT_FN, /** Node is a constant function */
       AMD_VARIABLE_FN, /** Node is a constant function */
       AMD_NULL_PTR, /**< Null pointer exception */
       AMD_INVALID_OPERATION, /**< Invalid operation */
       AMD_NOMEM, /**< Malloc failed */
       AMD_INVALID_ARGUMENTS, /**< The arguments are mismatched */
       /** ADD OTHER ERROR CODES HERE */
       AMD_SUCCESS = 0, /**< The function succeeded */
      AMD_INVALID_SHARED_PTR,
      AMD_NOT_NULL_PTR,
};
} /** Global namespace */

namespace AMD  {


typedef int error_code_type;  /**< Type of the error code */

/**
 * Helper function to covert line number to string
 */ 
#define LINE_TO_STR_HELP(line) #line

/**
 * Helper function to covert line number to string
 */ 
#define LINE_TO_STR(line) LINE_TO_STR_HELP(line)

/**
 * Returns a string that represents the file name and the line number
 */ 
#define FILE_AND_LINE() __FILE__ ":" LINE_TO_STR(__LINE__)

/**
 * Generates a generic catch block that take in the structure and function 
 * name and rethrows the error. Depends on the variable being called "exception"
 * Catch block for the constructor and destructor. Note that we know where 
 * the errors are coming from. Hence, we know that there is nothing that is 
 * deriving from exception_generic_impl. Else, we will have to use  
 * clone() and rethrow() methods to ensure that the information is preserved. 
 *
 * WARNING! Assumes you have a pointer called "except" of type "exception" 
 * defined in our class! Better define it!
 */ 
#define AMD_CATCH_AND_RETHROW(STRUCT_NAME,FUNC_NAME) \
  catch (const exception& error) { \
    /* Append append the information and throw again */ \
    const_cast<exception&>(error).add_to_trace \
      (": from " #STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE()); \
    error.rethrow (); \
  } catch (const std::exception& error) { \
    /* Wrap it up in a exception_generic_impl */ \
    std::string prev_trace (typeid (error).name()); \
    prev_trace += ": from " #STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE(); \
    throw exception_generic_impl::convert \
                 (prev_trace.c_str(), error.what(), \
                  static_cast<error_code_type>(AMD_ERROR)); \
  } catch (...) { \
    throw exception_generic_impl  \
     (#STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE(), \
      "Unknown error", \
      static_cast<error_code_type>(AMD_ERROR)); \
  }

/**
 * Used in the top-level functions to return the error from the library.
 */
#define AMD_CXX_CATCH_AND_RETHROW() \
 catch (const exception& error) { error.rethrow(); }

/**
 * Used to start of the try block
 */
#define AMD_START_TRY_BLOCK() try {

/**
 * Used to put try's in the constructor
 */
#define AMD_CONSTRUCTOR_TRY_BLOCK() try 

/**
 * Used to end the try block
 */
#define AMD_END_TRY_BLOCK() }

/**
 * Primary base class used to report errors up the call stack to the
 * invocation site
 */
struct exception : public std::exception {
  /** Name of the the object that threw the error */
  virtual const char* trace () const throw() = 0;

  /** To append stuff to the name or change it */
  virtual void add_to_trace (const char*) throw() = 0;

  /** Returns the description of the error */
  virtual const char* what () const throw() = 0;

  /** Returns the errno or GetLastError */
  virtual error_code_type code () const throw() = 0; 

  /** Clone yourself and return the pointer */
  virtual exception* clone () const throw()  = 0;

  /** Throw urself */
  virtual void rethrow () const = 0;

  /** Destroy -- turns around and calls the destructor */
  virtual void destroy () throw() = 0;
};

/**
 * Generic implementation that just stores the name and the error
 */ 
struct exception_generic_impl : virtual exception {
  private:
  std::string error_trace; /**< Trace of the object that threw the error */
  std::string error_string; /**< Description of the error */
  error_code_type error_code; /**< Error code -- either errno or GetLastError() */

  public:
  /**
   * Constructor
   *
   * \param [in] error_trace Name of the error -- used for upcasts.
   * \param [in] error_string Description of the error.
   * \param [in] error_code If a system error number was received.
   */  
  exception_generic_impl (const char* error_trace, 
                          const char* error_string,
                          const error_code_type& error_code=0) : 
                          error_trace (error_trace),
                          error_string (error_string),
                          error_code (error_code) {}

  /**
   * Copy constructor.
   *
   * \param other Object from which to copy construct
   */ 
  exception_generic_impl (const exception_generic_impl& other) : 
                                  exception (),
                                  error_trace (other.trace()), 
                                  error_string (other.what()),
                                  error_code (other.code()) {}

  /** Virtual destructor */
  virtual ~exception_generic_impl () throw() {} 

  /**
   * \return trace of the error stored in this exception object.
   */ 
  const char* trace (void) const throw() { return error_trace.c_str(); }

  /**
   * Add to the existing trace.
   *
   * \param new_trace The string to append to the trace.
   */
  void add_to_trace (const char* new_trace) throw() { 
    error_trace += new_trace; 
  }

  /**
   * \return Description string
   */ 
  const char* what (void) const throw () { return error_string.c_str(); }

  /** 
   * \return Error code -- errno or GetLastError()
   */
  error_code_type code (void) const throw() { return error_code; }

  /**
   * \return Allocates memory on the heap using "malloc" and psuedo
   * copy-constructs it by hand from its own values. Returns a pointer to
   * the cloned object 
   */ 
  exception* clone () const throw() {
    exception* clone = 
      reinterpret_cast<exception*> (malloc (sizeof (exception_generic_impl)));
    if (NULL != clone) new (clone) exception_generic_impl(*this);
    return clone;
  }

  /**
   * Throws itself. This is more useful that using a "throw" since this
   * carries with it information about the derived type rather than the
   * base type
   */
  void rethrow () const { throw *this; }

  /**
   * \param _error_trace Trace of the error -- used for upcasts.
   * \param _error_string Description of the error.
   * \param _error_code The error code to be returned.
   *
   * When we receive an error which is not derived from exception,
   * but instead from std::exception, we need to convert it into
   * exception so that we can clone it up the nesting levels and
   * deliver the value ultimately to the right function.
   */ 
  static exception* convert (const char* _error_trace, 
                             const char* _error_string,
                             const error_code_type& _error_code = 0) throw() {
    exception* clone = 
      reinterpret_cast<exception*> (malloc (sizeof (exception_generic_impl)));
    if (NULL != clone) 
      new (clone) exception_generic_impl(_error_trace, 
                                         _error_string,
                                         _error_code);
 
    return clone;
  }

  /**
   * Since clone and convert allocate memory using "malloc", this function
   * is used as a trampoline to call into the destuctor
   */ 
  void destroy () throw() {
    this->exception_generic_impl::~exception_generic_impl();
    free (this);
  }
};

} /* namespace AMD */

#endif // AMD_EXCEPTION_HPP
