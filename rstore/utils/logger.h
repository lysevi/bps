#pragma once

#include <rstore/exports.h>
#include <rstore/utils/strings.h>

#include <fstream>
#include <memory>
#include <mutex>
#include <utility>

namespace rstore::utils::logging {

enum class MESSAGE_KIND { message, info, warn, fatal };

class AbstractLogger {
public:
  virtual void message(MESSAGE_KIND kind, const std::string &msg) noexcept = 0;
  virtual ~AbstractLogger() {}

  template <typename... T>
  void variadic_message(MESSAGE_KIND kind, T &&... args) noexcept {
    auto str_message = utils::strings::to_string(args...);
    this->message(kind, str_message);
  }

  template <typename... T>
  void dbg(T &&... args) noexcept {
    variadic_message(utils::logging::MESSAGE_KIND::message, args...);
  }

  template <typename... T>
  void info(T &&... args) noexcept {
    variadic_message(utils::logging::MESSAGE_KIND::info, args...);
  }

  template <typename... T>
  void warn(T &&... args) noexcept {
    variadic_message(utils::logging::MESSAGE_KIND::warn, args...);
  }

  template <typename... T>
  void fatal(T &&... args) noexcept {
    variadic_message(utils::logging::MESSAGE_KIND::fatal, args...);
  }
};

using AbstractLoggerPtr = std::shared_ptr<AbstractLogger>;
using AbstractLoggerUptr = std::unique_ptr<AbstractLogger>;

class PrefixLogger final : public AbstractLogger {
public:
  PrefixLogger(AbstractLoggerPtr target, const std::string &prefix)
      : _prefix(prefix)
      , _shared_target(target) {}
  ~PrefixLogger() override { _shared_target = nullptr; }
  void message(MESSAGE_KIND kind, const std::string &msg) noexcept override {
    _shared_target->message(kind, utils::strings::to_string(_prefix, msg));
  }

private:
  const std::string _prefix;
  AbstractLoggerPtr _shared_target;
};

class ConsoleLogger final : public AbstractLogger {
public:
  EXPORT void message(MESSAGE_KIND kind, const std::string &msg) noexcept override;
};

class QuietLogger final : public AbstractLogger {
public:
  EXPORT void message(MESSAGE_KIND kind, const std::string &msg) noexcept override;
};

class FileLogger final : public AbstractLogger {
public:
  EXPORT FileLogger(std::string fname, bool _verbose = false);
  EXPORT void message(MESSAGE_KIND kind, const std::string &msg) noexcept override;

private:
  bool _verbose;
  std::unique_ptr<std::ofstream> _output;
  std::mutex _locker;
};

enum class VERBOSE_KIND { verbose, debug, quiet };

class LoggerManager {
public:
  EXPORT static VERBOSE_KIND verbose;
  LoggerManager(AbstractLoggerPtr &logger);
  [[nodiscard]] EXPORT AbstractLogger *get_logger() noexcept;
  [[nodiscard]] EXPORT AbstractLoggerPtr get_shared_logger() noexcept;

  EXPORT static void start(AbstractLoggerPtr &logger);
  EXPORT static void stop();
  [[nodiscard]] EXPORT static LoggerManager *instance() noexcept;

  template <typename... T>
  void variadic_message(MESSAGE_KIND kind, T &&... args) noexcept {
    std::lock_guard lg(_msg_locker);
    _logger->variadic_message(kind, std::forward<T>(args)...);
  }

private:
  static std::shared_ptr<LoggerManager> _instance;
  static std::mutex _locker;
  std::mutex _msg_locker;
  AbstractLoggerPtr _logger;
};

template <typename... T>
void logger(T &&... args) noexcept {
  utils::logging::LoggerManager::instance()->get_logger()->dbg(args...);
}

template <typename... T>
void logger_info(T &&... args) noexcept {
  utils::logging::LoggerManager::instance()->get_logger()->info(args...);
}

template <typename... T>
void logger_warn(T &&... args) noexcept {
  utils::logging::LoggerManager::instance()->get_logger()->warn(args...);
}

template <typename... T>
void logger_fatal(T &&... args) noexcept {
  utils::logging::LoggerManager::instance()->get_logger()->fatal(args...);
}
} // namespace rstore::utils::logging
