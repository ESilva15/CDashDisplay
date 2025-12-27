#ifndef __LOGGER__
#define __LOGGER__

#include <Arduino.h>
#include <cstdint>

#define LOG_LEVEL_ERROR 0
#define LOG_LEVEL_WARN  1
#define LOG_LEVEL_INFO  2
#define LOG_LEVEL_DEBUG 3
#define LOG_LEVEL_TRACE 4

#define LOG_ENABLED(lvl) ((lvl) <= LOG_LEVEL)

#if LOG_ENABLED(LOG_LEVEL_ERROR)
  #define LOG_ERROR(fmt, ...) \
    do { \
      Logger::WritePrefix(Logger::Error); \
      Logger::Printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
  #define LOG_ERROR(...) do {} while (0)
#endif

#if LOG_ENABLED(LOG_LEVEL_WARN)
  #define LOG_WARN(fmt, ...) \
    do { \
      Logger::WritePrefix(Logger::Warn); \
      Logger::Printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
  #define LOG_WARN(...) do {} while (0)
#endif

#if LOG_ENABLED(LOG_LEVEL_INFO)
  #define LOG_INFO(fmt, ...) \
    do { \
      Logger::WritePrefix(Logger::Info); \
      Logger::Printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
  #define LOG_INFO(...) do {} while (0)
#endif

#if LOG_ENABLED(LOG_LEVEL_DEBUG)
  #define LOG_DEBUG(fmt, ...) \
    do { \
      Logger::WritePrefix(Logger::Debug); \
      Logger::Printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
  #define LOG_DEBUG(...) do {} while (0)
#endif

#if LOG_ENABLED(LOG_LEVEL_TRACE)
  #define LOG_TRACE(fmt, ...) \
    do { \
      Logger::WritePrefix(Logger::Trace); \
      Logger::Printf(fmt, ##__VA_ARGS__); \
    } while (0)
#else
  #define LOG_TRACE(...) do {} while (0)
#endif

namespace Logger {
  enum LogLevel : uint8_t {
    Error = LOG_LEVEL_ERROR,
    Warn  = LOG_LEVEL_WARN,
    Info  = LOG_LEVEL_INFO,
    Debug = LOG_LEVEL_DEBUG,
    Trace = LOG_LEVEL_TRACE
  };

  void Initialize(int8_t rx, int8_t tx);

  void WritePrefix(LogLevel lvl);
  void VPrintf(const char* fmt, va_list args);
  void Printf(const char* fmt, ...);
  void Printf(const __FlashStringHelper* fmt, ...);
};

#endif
