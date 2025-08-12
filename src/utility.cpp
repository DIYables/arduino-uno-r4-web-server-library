/*
 * utility.cpp
 * 
 * Originally from mWebSockets library by skaarj1989
 * https://github.com/skaarj1989/mWebSockets
 * 
 * Modified for Arduino Uno R4 WiFi compatibility
 * - Removed multi-platform support 
 * - Simplified for Arduino Uno R4 only
 * - Uses standard Serial and vsnprintf
 * 
 * Original license: LGPL-2.1 License
 */

#include "utility.h"
#include <stdarg.h>

void printf(const __FlashStringHelper *fmt, ...) {
  char buffer[256]{};
  va_list args;
  va_start(args, fmt);

  // Arduino Uno R4 uses standard vsnprintf
  vsnprintf(buffer, sizeof(buffer), reinterpret_cast<const char *>(fmt), args);

  va_end(args);

  // Arduino Uno R4 uses standard Serial
  Serial.print(buffer);
}

namespace net {

IPAddress fetchRemoteIp(const NetClient &client) {
  // WiFiClient class is not "const correct"
  return const_cast<NetClient &>(client).remoteIP();
}

} // namespace net
