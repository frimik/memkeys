#include <stdexcept>
#include <iostream>

#include "logging/logger.h"

namespace mctop {

using namespace std;

/**
 * Interal to this logging implementations.
 */
static Loggers loggers;
static string ROOT_LOGGER_NAME = "";

/**
 * Static methods. This is how you get a handle on a logger.
 */
LoggerPtr Logger::getLogger(const string &name)
{
  Loggers::iterator it = loggers.find(name);
  if (it != loggers.end()) {
    return it->second;
  } else {
    LoggerPtr logger = new Logger(name);
    if (name != ROOT_LOGGER_NAME) {
      LoggerPtr root = Logger::getRootLogger();
      logger->setParent(root);
      logger->setUseParent(true);
      logger->setLevel(root->getLevel());
    }
    loggers.insert(it, Loggers::value_type(name, logger));
    return logger;
  }
}
LoggerPtr Logger::getRootLogger()
{
  static LoggerPtr rootLogger = NULL;
  if (rootLogger == NULL) {
    rootLogger = Logger::getLogger(ROOT_LOGGER_NAME);
    rootLogger->setParent(NULL);
    rootLogger->setUseParent(false);
  }
  return rootLogger;
}

Logger::~Logger() {}

/**
 * Manage the logging.Level associated with this logger.
 */
Level Logger::getLevel() const
{
  return _level;
}
void Logger::setLevel(const Level &level)
{
  _level = level;
}

/**
 * The name associated with this logger.
 */
string Logger::getName() const
{
  return _name;
}

/**
 * Manage the parent of this logger.
 */
void Logger::setParent(const LoggerPtr &logger)
{
  _parent = logger;
}
LoggerPtr Logger::getParent() const
{
  return _parent;
}

/**
 * Whether or not to use the parent logger for logging.
 */
void Logger::setUseParent(const bool use_parent)
{
  _useParent = use_parent;
}
bool Logger::getUseParent() const
{
  return _useParent;
}

/**
 * Logging for various levels.
 */
void Logger::log(const Level &level, const string &msg)
{
  if (level.getValue() >= getLevel().getValue()) {
    Record rec = Record();
    rec.setLevel(level);
    rec.setLoggerName(getName());
    rec.setMessage(msg);
    log(level, rec);
  }
}
void Logger::log(const Level &level, const Record &record)
{
  if (level.getValue() >= getLevel().getValue()) {
    cout << format(record) << endl;
    LoggerPtr logger = getParent();
    if (logger != NULL && logger->getUseParent()) {
      logger->log(level, record);
    }
  }
}

void Logger::trace(const string &msg)
{
  log(Level::TRACE, msg);
}
void Logger::trace(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::TRACE, record, fmt, this);
}

void Logger::debug(const string &msg)
{
  log(Level::DEBUG, msg);
}
void Logger::debug(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::DEBUG, record, fmt, this);
}

void Logger::info(const string &msg)
{
  log(Level::INFO, msg);
}
void Logger::info(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::INFO, record, fmt, this);
}

void Logger::warning(const string &msg)
{
  log(Level::WARNING, msg);
}
void Logger::warning(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::WARNING, record, fmt, this);
}

void Logger::error(const string &msg)
{
  log(Level::ERROR, msg);
}
void Logger::error(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::ERROR, record, fmt, this);
}

void Logger::fatal(const string &msg)
{
  log(Level::FATAL, msg);
}
void Logger::fatal(Record record, const string &fmt, ...)
{
  LOG_WITH_VARARGS(Level::FATAL, record, fmt, this);
}

// protected
Logger::Logger(const string &name) : _name(name), _level(Level::WARNING)
{}

string Logger::format(const Record &rec)
{
  ostringstream out;
  out << rec.getLevel().getName() << " ";
  out << "[" << rec.getTimestamp() << "] ";
  if (!rec.getMethodName().empty()) {
    out << "[" << rec.getFileName() << ":" << rec.getLineNumber() << "][";
    out << rec.getMethodName() << "] ";
  }
  out << rec.getLoggerName() << ": ";
  out << rec.getMessage();
  return out.str();
}

} // end namespace mctop
