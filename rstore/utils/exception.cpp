#include <boost/stacktrace.hpp>
#include <rstore/utils/exception.h>
#include <sstream>

using namespace rstore::utils::exceptions;

Exception::Exception() {
  init_msg("");
}
Exception::Exception(const char *&message) {
  init_msg(message);
}
Exception::Exception(const std::string &message) {
  init_msg(message);
}

void Exception::init_msg(const std::string &msg_) {
  std::stringstream ss;
  ss << msg_
     << " Stacktrace:" << boost::stacktrace::to_string(boost::stacktrace::stacktrace());
  _msg = ss.str();
}
