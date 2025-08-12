#ifndef UNOR4_WIFI_WEBSOCKET_H
#define UNOR4_WIFI_WEBSOCKET_H

#include <WiFiS3.h>
#include "WebSocketServer.h"

// Forward declare the WebSocket event handler function type
typedef void (*WebSocketEventHandler)(net::WebSocket& ws, const net::WebSocket::DataType dataType, const char* message, uint16_t length);
typedef void (*WebSocketOpenHandler)(net::WebSocket& ws);
typedef void (*WebSocketCloseHandler)(net::WebSocket& ws, const net::WebSocket::CloseCode code, const char* reason, uint16_t length);

class UnoR4WiFi_WebSocket {
public:
  UnoR4WiFi_WebSocket(uint16_t port = 81);
  ~UnoR4WiFi_WebSocket();
  
  // Initialize the WebSocket server
  bool begin();
  
  // Handle WebSocket connections and messages
  void loop();
  
  // Set event handlers
  void onOpen(WebSocketOpenHandler handler);
  void onMessage(WebSocketEventHandler handler);
  void onClose(WebSocketCloseHandler handler);
  
  // Send text message to all connected clients
  void broadcastTXT(const char* payload);
  void broadcastTXT(const String& payload);
  
  // Send binary data to all connected clients
  void broadcastBIN(const uint8_t* payload, size_t length);
  
  // Get the number of connected clients
  uint8_t connectedClients();
  
  // Check if server is listening
  bool isListening();

private:
  net::WebSocketServer* wsServer;
  uint16_t port;
  WebSocketOpenHandler openHandler;
  WebSocketEventHandler messageHandler;
  WebSocketCloseHandler closeHandler;
  bool initialized;
  uint8_t clientCount;
  
  // Static instance for callbacks
  static UnoR4WiFi_WebSocket* instance;
  
  // Static callback functions
  static void staticOnConnection(net::WebSocket &ws);
  static void staticOnMessage(net::WebSocket &ws, const net::WebSocket::DataType dataType, const char *message, uint16_t length);
  static void staticOnClose(net::WebSocket &ws, const net::WebSocket::CloseCode code, const char *reason, uint16_t length);
};

#endif
