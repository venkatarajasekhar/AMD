#ifndef AMD_UTILITY_HPP
#define AMD_UTILITY_HPP

/**
 * @file utility.hpp
 *
 * @author Peder Olsen, Anju Kambadur
 *
 * @brief This file defines utility functions for SymbolicScalarMatlab class and 
 * SymbolicMatrixMatlab class.
 */

#include <string>
#include <fstream>
#include <iostream>

namespace AMD { namespace detail {
/**
 * Copy the pointer of T.
 * @param[in] x T object.
 * @return The T * pointer to x. 
 */
template <typename T> T* pointerCopy(T x) {
  T* tmp = new T;
  (*tmp) = x;
  return(tmp);
}
/**
 * Remove right and left parenthesis from string.
 * @param[in] str String with the parenthesis.
 * @return String without parenthesis.
 */
std::string removeParenthesis(std::string str) {
  std::string retVal = str;

  if ('(' == str[0] && 
      ')' == str[str.length()-1]) {
    retVal=str.substr(1,str.length()-2);
  }
  
  return retVal;
}

/**
 * The one annoying problem is that OutputIterator's do not define
 * a value type, so it may not always work! Hrmp!
 */ 
static int generate_seed () {
  int r_int;
  std::ifstream rf("/dev/random", std::ios::binary);
  if(rf.is_open())rf.read(reinterpret_cast<char*>(&r_int),sizeof(r_int));
  else r_int = std::time(NULL);
  return r_int;
}

} } /** namespace AMD::detail */

#endif /** AMD_UTILITY_HPP */
