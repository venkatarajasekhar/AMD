#ifndef ALD_LOG_HPP
#define ALD_LOG_HPP

#include <iostream>

#include <boost/log/sources/global_logger_storage.hpp>
#include <boost/log/sources/severity_feature.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

namespace blg = boost::log;

BOOST_LOG_GLOBAL_LOGGER(
  ALD_logger,
  blg::sources::severity_logger_mt<blg::trivial::severity_level>)

#define LOG_TRACE BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::trace)
#define LOG_DEBUG BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::debug)
#define LOG_INFO BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::info)
#define LOG_WARNING BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::warning)
#define LOG_ERROR BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::error)
#define LOG_FATAL BOOST_LOG_SEV(ALD_logger::get(),blg::trivial::fatal)

struct LogLevel {
  enum severity { TRACE, 
                  DEBUG, 
                  INFO, 
                  WARNING, 
                  ERROR, 
                  FATAL 
                };
  ///< Enumeration of the various logging levels, in ascending order of        
  ///  severity. 
};

namespace AMD {

void setLogLevel(LogLevel::severity threshold);
///< Set the minimum global logging level.                                  
/// Only log messages with LogLevel::severity at or above `threshold`       
/// will be printed.                                                        
///                                                                         
/// @param[in] threshold The minimum logging level at which to print logs  

void setLogLevel(int threshold);
///< Set the minimum global logging level. The integer equivalent version of
///  the above. It turns around and calls the above method.
///
///  Only log messages with LogLevel::severity at or above `threshold`       
///  will be printed.                                                        
///                                                                         
/// @param[in] threshold The minimum logging level at which to print logs  

} // namespace AMD

#endif // ALD_LOG_HPP
