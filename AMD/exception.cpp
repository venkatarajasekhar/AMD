
/// @file exception.cpp
/// @author Anju Kambadur
///

#include <exception>
#include <typeinfo>
#include <stdexcept>
#include <string>
#include <cstdlib>

#include <AMD/exception.hpp>

namespace AMD  {

ExceptionImpl::ExceptionImpl(const char* errorTrace,
                             const char* errorString,
                             const error_code_type& errorCode) :
      d_errorTrace (errorTrace),
      d_errorString(errorString),
      d_errorCode  (errorCode) 
{
}

ExceptionImpl::ExceptionImpl(const Exception& other) :
      d_errorTrace(other.trace()),
      d_errorString(other.what()),
      d_errorCode(other.code()) 
{
}

const char* ExceptionImpl::trace(void) const throw() 
{ 
    return d_errorTrace.c_str(); 
}

void ExceptionImpl::appendTrace(const char* trace) throw() 
{ 
    d_errorTrace += trace; 
}

const char* ExceptionImpl::what(void) const throw () 
{ 
    return d_errorString.c_str(); 
}

error_code_type ExceptionImpl::code(void) const throw() 
{ 
    return d_errorCode; 
}

Exception* ExceptionImpl::clone() const throw() 
{
    Exception* clone =
         reinterpret_cast<Exception*> (malloc(sizeof(ExceptionImpl)));
    if (NULL != clone) new (clone) ExceptionImpl(*this);
    return clone;
}

void ExceptionImpl::rethrow() const 
{ 
    throw *this; 
}

Exception* ExceptionImpl::convert(const char* errorTrace,
                                  const char* errorString,
                                  const error_code_type& errorCode) throw() 
{
    Exception* clone = 
        reinterpret_cast<Exception*> (malloc(sizeof(ExceptionImpl)));

    if (NULL != clone) {
        new (clone) ExceptionImpl(errorTrace, errorString, errorCode);
    }

    return clone;
}

void ExceptionImpl::destroy() throw() 
{
    this->Exception::~Exception();
    free(this);
}

} /* namespace AMD */
