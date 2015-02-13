#ifndef AMD_EXCEPTION_HPP
#define AMD_EXCEPTION_HPP

/// @file Exception.hpp
/// @author Anju Kambadur
///
///
/// @brief This file defines the exception classes for AMD, 
/// each containing a different exception to be thrown on the 
/// normal use of the library. They are all specialisations of 
/// std::exception, contained in <stdexcept>.

#include <iostream>
#include <exception>
#include <typeinfo>
#include <string>
#include <cstdlib>

namespace {
  ///< Enumeration that lists all the error code that we supply. The
  ///  error codes that are negative are errors, and those that are 
  ///  0 or positive are not.
  enum {
    AMD_ERROR = -0xFFFF, /**< An error has occured */
    AMD_MISMATCHED_DIMENSIONS, /**< Matrices don't match up */
    AMD_INTERNAL_NODE, /**< Internal node when leaf node expected */
    AMD_CONSTANT_FN, /** Node is a constant function */
    AMD_VARIABLE_FN, /** Node is a constant function */
    AMD_NULL_PTR, /**< Null pointer exception */
    AMD_INVALID_OPERATION, /**< Invalid operation */
    AMD_NOMEM, /**< Malloc failed */
    AMD_INVALID_ARGUMENTS, /**< The arguments are mismatched */
    AMD_INVALID_SHARED_PTR, /**< Shared pointer is not valid anymore */
    AMD_NOT_NULL_PTR, /**< Not null pointer exception */
    AMD_NON_SQUARE, /**< Non square matrix (no. of rows != no. of columns) */
    AMD_INVALID_DIMENSIONS, /**< Invalid matrix dimesions ( row < 0 or col < 0 ) */
    AMD_INVALID_EXPRESSION,
    /** ADD OTHER ERROR CODES HERE */
    AMD_SUCCESS = 0 /**< The function succeeded */
  };
} /** Global namespace */

namespace AMD  {

  typedef int error_code_type;  ///< Type of the error code 

   
///< Helper function to covert line number to string
#define LINE_TO_STR_HELP(line) #line

///< Helper function to covert line number to string
#define LINE_TO_STR(line) LINE_TO_STR_HELP(line)

///< Returns a string that represents the file name and the line number
#define FILE_AND_LINE() __FILE__ ":" LINE_TO_STR(__LINE__)

///< Append file and line information to string
#define APPEND_LOCATION(str) str" at "FILE_AND_LINE()

///< Generates a generic catch block that take in the structure and function
/// name and rethrows the error. Conversion from other exceptions done as well.
#define AMD_CATCH_AND_RETHROW(STRUCT_NAME,FUNC_NAME) \
  catch (const AMD::Exception& error) { \
    /* Append append the information and throw again */ \
    const_cast<AMD::Exception&>(error).appendTrace \
      (": from " #STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE()); \
    error.rethrow (); \
  } catch (const std::exception& error) { \
    /* Wrap it up in a exception_generic_impl */ \
    std::string prevTrace (typeid (error).name()); \
    prevTrace += ": from " #STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE();\
    throw ::AMD::ExceptionImpl::convert \
                 (prevTrace.c_str(), error.what(), \
                  static_cast<AMD::error_code_type>(AMD_ERROR)); \
  } catch (...) { \
    throw ::AMD::ExceptionImpl  \
     (#STRUCT_NAME"::"#FUNC_NAME " at " FILE_AND_LINE(), \
      "Unknown error", \
      static_cast<AMD::error_code_type>(AMD_ERROR)); \
  }

///< Used in user code to report the errors
#define AMD_CATCH_AND_PRINT() \
   catch (const ::AMD::Exception& error) { \
      std::cout << "error.what() : " << error.what() << "\n"\
                << "error.trace(): " << error.trace() << "\n"  \
                << "error.code() : " << error.code() << "\n"; \
   }

///< Used to start the try block
#define AMD_START_TRY_BLOCK() try {

///< Used to start the try block in the constructor
#define AMD_CONSTRUCTOR_TRY_BLOCK() try 

///< Used to end the try block
#define AMD_END_TRY_BLOCK() }

///< Primary base class used to report errors up the call stack to the
///  invocation site
struct Exception : public std::exception 
{
    /** Name of the the object that threw the error */
    virtual const char* trace() const throw() = 0;
 
    /** To append stuff to the name or change it */
    virtual void appendTrace(const char*) throw() = 0;
 
    /** Returns the description of the error */
    virtual const char* what() const throw() = 0;
 
    /** Returns the errno or GetLastError */
    virtual error_code_type code() const throw() = 0;
 
    /** Clone yourself and return the pointer */
    virtual exception* clone() const throw() = 0;
 
    /** Throw urself */
    virtual void rethrow() const = 0;
 
    /** Destroy -- turns around and calls the destructor */
    virtual void destroy() throw() = 0;
};

///< Implementation that just stores the name and the error code
struct ExceptionImpl : virtual ::AMD::Exception
{
    private:
    std::string d_errorTrace; ///< Trace of the object that threw error
    std::string d_errorString; ///< Description of the error
    error_code_type d_errorCode; ///< Error code or GetLastError()

    public:
    ExceptionImpl(const char* errorTrace,
                  const char* errorString,
                  const error_code_type& errorCode = 0);
    /// Constructor
    /// @param [in] d_errorTrace Name of the error -- used for upcasts.
    /// @param [in] d_errorString Description of the error.
    /// @param [in] d_errorCode If a system error number was received.

    
    ExceptionImpl(const Exception& other);
    ///< Copy constructor.
    ///  @param other Object from which to copy construct

    virtual ~ExceptionImpl() throw() {}
    ///< Virtual destructor

    const char* trace(void) const throw();
    //< @return trace of the error stored in this exception object.

    void appendTrace(const char* trace) throw();
    ///< Add to the existing trace.
    /// @param new_trace The string to append to the trace.
    

    const char* what(void) const throw ();
    ///< \return Description string

    error_code_type code(void) const throw();
    ///< \return Error code -- errno or GetLastError()

    Exception* clone() const throw();
    ///< @return Allocates memory on the heap using "malloc" and psuedo
    ///          copy-constructs it by hand from its own values. Returns 
    ///          a pointer to the cloned object
    
    void rethrow() const;
    /// Throws itself. This is more useful that using a "throw" since this
    /// carries with it information about the derived type rather than the
    /// base type
    /// 
    /// @return Throws itself

    static Exception* convert(const char* errorTrace,
                              const char* errorString,
                              const error_code_type& errorCode = 0) throw();

    /// @param errorTrace  Trace of the error -- used for upcasts.
    /// @param errorString Description of the error.
    /// @param errorCode   The error code to be returned.
    /// 
    /// When we receive an error which is not derived from exception,
    /// but instead from std::exception, we need to convert it into
    /// exception so that we can clone it up the nesting levels and
    /// deliver the value ultimately to the right function.

    void destroy() throw();
    ///< Since clone and convert allocate memory using "malloc", this function
    ///  is used as a trampoline to call into the destuctor
};

} /* namespace AMD */

#endif // AMD_EXCEPTION_HPP
