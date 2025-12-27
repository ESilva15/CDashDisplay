#include "logger.h"
#include "HardwareSerial.h"

namespace Logger {
  inline HardwareSerial& out() {
    return Serial2;
  }

  void WritePrefix(LogLevel lvl) {
    switch (lvl) {
      case Error: out().print(F("\r[ERR] ")); break;
      case Warn:  out().print(F("\r[WRN] ")); break;
      case Info:  out().print(F("\r[INF] ")); break;
      case Debug: out().print(F("\r[DBG] ")); break;
      case Trace: out().print(F("\r[TRC] ")); break;
    }
  }

  void VPrintf(const char* fmt, va_list args) {
    char buffer[128];
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    out().print(buffer);
  }

  void Printf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    VPrintf(fmt, args);
    va_end(args);
  }

  void Printf(const __FlashStringHelper* fmt, ...) {
    char buffer[128];
  
    va_list args;
    va_start(args, fmt);
    vsnprintf_P(buffer, sizeof(buffer),
                reinterpret_cast<const char*>(fmt), args);
    va_end(args);
  
    out().print(buffer);
  }

  void Initialize(int8_t rx, int8_t tx) {
    Serial2.begin(115200, SERIAL_8N1, rx, tx);
    Serial2.flush();
  }
};
