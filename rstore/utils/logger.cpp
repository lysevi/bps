#include <fstream>
#include <iostream>
#include <sstream>

#include <rstore/utils/exception.h>
#include <rstore/utils/logger.h>
#include <rstore/utils/utils.h>

using namespace rstore::utils::logging;

std::shared_ptr<LoggerManager> LoggerManager::_instance = nullptr;
std::mutex LoggerManager::_locker;

VERBOSE_KIND LoggerManager::verbose = VERBOSE_KIND::debug;

void LoggerManager::start(AbstractLoggerPtr &logger) {
  if (_instance == nullptr) {
    _instance = std::shared_ptr<LoggerManager>{new LoggerManager(logger)};
  }
}

void LoggerManager::stop() {
  _instance = nullptr;
}

LoggerManager *LoggerManager::instance() noexcept {
  auto tmp = _instance.get();
  if (tmp == nullptr) {
    std::lock_guard lock(_locker);
    tmp = _instance.get();
    if (tmp == nullptr) {
      AbstractLoggerPtr l = std::make_shared<ConsoleLogger>();
      _instance = std::make_shared<LoggerManager>(l);
      tmp = _instance.get();
    }
  }
  return tmp;
}

AbstractLogger *LoggerManager::get_logger() noexcept {
  return _logger.get();
}

AbstractLoggerPtr LoggerManager::get_shared_logger() noexcept {
  return _logger;
}

LoggerManager::LoggerManager(AbstractLoggerPtr &logger) {
  _logger = logger;
}

void ConsoleLogger::message(MESSAGE_KIND kind, const std::string &msg) noexcept {
  if (LoggerManager::verbose == VERBOSE_KIND::quiet) {
    return;
  }
  switch (kind) {
  case MESSAGE_KIND::fatal:
    std::cerr << "[err] " << msg << std::endl;
    break;
  case MESSAGE_KIND::warn:
    std::cout << "[wrn] " << msg << std::endl;
    break;
  case MESSAGE_KIND::info:
    std::cout << "[inf] " << msg << std::endl;
    break;
  case MESSAGE_KIND::message:
    if (LoggerManager::verbose == VERBOSE_KIND::debug) {
      std::cout << "[dbg] " << msg << std::endl;
    }
    break;
  }
}

void QuietLogger::message(MESSAGE_KIND kind, const std::string &msg) noexcept {
  UNUSED(kind);
  UNUSED(msg);
}

FileLogger::FileLogger(std::string fname, bool verbose) {
  _verbose = verbose;

  std::stringstream fname_ss;
  fname_ss << fname;
  fname_ss << ".log";

  auto logname = fname_ss.str();
  if (verbose) {
    std::cout << "See output in " << logname << std::endl;
  }
  _output = std::make_unique<std::ofstream>(logname, std::ofstream::app);
  (*_output) << "Start programm" << std::endl;
}

void FileLogger::message(MESSAGE_KIND kind, const std::string &msg) noexcept {
  std::lock_guard lg(_locker);

  std::stringstream ss;
  switch (kind) {
  case rstore::utils::logging::MESSAGE_KIND::fatal:
    ss << "[err] " << msg << std::endl;
    break;
  case rstore::utils::logging::MESSAGE_KIND::info:
    ss << "[inf] " << msg << std::endl;
    break;
  case rstore::utils::logging::MESSAGE_KIND::warn:
    ss << "[wrn] " << msg << std::endl;
    break;
  case rstore::utils::logging::MESSAGE_KIND::message:
    ss << "[dbg] " << msg << std::endl;
    break;
  }

  (*_output) << ss.str();
  _output->flush();
  _output->flush();

  if (_verbose || kind == rstore::utils::logging::MESSAGE_KIND::fatal) {
    std::cerr << ss.str();
  }
}