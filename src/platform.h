/*
 * platform.h
 * 
 * Originally from mWebSockets library by skaarj1989
 * https://github.com/skaarj1989/mWebSockets
 * 
 * Modified for Arduino Uno R4 WiFi compatibility
 * - Removed multi-platform detection
 * - Hardcoded for Arduino Uno R4 WiFi only
 * - Uses WiFiS3 library specifically
 * 
 * Original license: LGPL-2.1 License
 */

#pragma once

/** @file */

#include <stdint.h>

//
// Arduino Uno R4 WiFi specific platform configuration
//

#define PLATFORM_ARCHITECTURE_UNO_R4 7
#define PLATFORM_ARCH PLATFORM_ARCHITECTURE_UNO_R4

#include "config.h"

// Uno R4 WiFi uses WiFiS3 library
#include <WiFiS3.h>
#include <WiFiClient.h>
#include <WiFiServer.h>

/** Specifies maximum number of clients connected to server. */
constexpr uint8_t kMaxConnections{8};

using NetClient = WiFiClient;
using NetServer = WiFiServer;
