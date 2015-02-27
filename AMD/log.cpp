#include <AMD/log.hpp>

namespace blg=boost::log;

BOOST_LOG_GLOBAL_LOGGER_INIT(ALD_logger,
    blg::sources::severity_logger_mt<blg::trivial:severity_level>) {
  blg::core::get()->set_filter(blg::trivial::severity >= blg::trivial::error);
  return blg::sources::severity_logger_mt<blg::trivial::severity_level>();
}

namespace AMD {

void setLogLevel(LogLevel::severity threshold)
{
    blg::trivial::severity_level bl_threshold;
 
    switch (threshold) {
        case LogLevel::TRACE:   bl_threshold = blg::trivial::trace; break;
        case LogLevel::DEBUG:   bl_threshold = blg::trivial::debug; break;
        case LogLevel::INFO:    bl_threshold = blg::trivial::info; break;
        case LogLevel::WARNING: bl_threshold = blg::trivial::warning; break;
        case LogLevel::ERROR:   bl_threshold = blg::trivial::error; break;
        case LogLevel::FATAL:   bl_threshold = blg::trivial::fatal; break;
    }
 
    blg::core::get()->set_filter(blg::trivial::severity > blg::trivial::trace);
    LOG_TRACE << "";
    blg::core::get()->set_filter(blg::trivial::severity >= bl_threshold);
}

void setLogLevel(int threshold)
{
    switch(threshold) {
      case 0 : setLogLevel(LogLevel::FATAL  ); break;
      case 1 : setLogLevel(LogLevel::ERROR  ); break;
      case 2 : setLogLevel(LogLevel::WARNING); break;
      case 3 : setLogLevel(LogLevel::INFO   ); break;
      case 4 : setLogLevel(LogLevel::DEBUG  ); break;
      default: setLogLevel(LogLevel::TRACE  ); break;
    }
}

} // namespace AMD
