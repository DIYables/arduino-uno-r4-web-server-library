#include "UnoR4WiFi_WebSocket.h"

// Static instance pointer for callbacks
UnoR4WiFi_WebSocket* UnoR4WiFi_WebSocket::instance = nullptr;

UnoR4WiFi_WebSocket::UnoR4WiFi_WebSocket(uint16_t port) 
  : port(port), openHandler(nullptr), messageHandler(nullptr), closeHandler(nullptr), initialized(false), clientCount(0) {
  wsServer = new net::WebSocketServer(port);
  instance = this; // Set static instance for callbacks
}

UnoR4WiFi_WebSocket::~UnoR4WiFi_WebSocket() {
  if (wsServer) {
    delete wsServer;
    wsServer = nullptr;
  }
  if (instance == this) {
    instance = nullptr;
  }
}

// Static callback functions
void UnoR4WiFi_WebSocket::staticOnConnection(net::WebSocket &ws) {
  if (instance) {
    instance->clientCount++;
    Serial.print("WebSocket client connected from: ");
    Serial.println(ws.getRemoteIP());
    
    // Set up individual client handlers
    ws.onMessage(staticOnMessage);
    ws.onClose(staticOnClose);
    
    if (instance->openHandler) {
      instance->openHandler(ws);
    }
  }
}

void UnoR4WiFi_WebSocket::staticOnMessage(net::WebSocket &ws, const net::WebSocket::DataType dataType, const char *message, uint16_t length) {
  if (instance && instance->messageHandler) {
    instance->messageHandler(ws, dataType, message, length);
  }
}

void UnoR4WiFi_WebSocket::staticOnClose(net::WebSocket &ws, const net::WebSocket::CloseCode code, const char *reason, uint16_t length) {
  if (instance) {
    instance->clientCount--;
    Serial.println("WebSocket client disconnected");
    
    if (instance->closeHandler) {
      instance->closeHandler(ws, code, reason, length);
    }
  }
}

bool UnoR4WiFi_WebSocket::begin() {
  if (!wsServer) {
    Serial.println("WebSocket server not initialized!");
    return false;
  }
  
  // Check if WiFi is connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected! Cannot start WebSocket server.");
    return false;
  }
  
  // Set up connection handler
  wsServer->onConnection(staticOnConnection);
  
  // Start the WebSocket server
  wsServer->begin();
  initialized = true;
  
  Serial.print("WebSocket server started on port ");
  Serial.println(port);
  Serial.print("WebSocket URL: ws://");
  Serial.print(WiFi.localIP());
  Serial.print(":");
  Serial.println(port);
  
  return true;
}

void UnoR4WiFi_WebSocket::loop() {
  if (initialized && wsServer) {
    wsServer->listen();
  }
}

void UnoR4WiFi_WebSocket::onOpen(WebSocketOpenHandler handler) {
  openHandler = handler;
}

void UnoR4WiFi_WebSocket::onMessage(WebSocketEventHandler handler) {
  messageHandler = handler;
}

void UnoR4WiFi_WebSocket::onClose(WebSocketCloseHandler handler) {
  closeHandler = handler;
}

void UnoR4WiFi_WebSocket::broadcastTXT(const char* payload) {
  if (wsServer && initialized) {
    wsServer->broadcast(net::WebSocket::DataType::TEXT, payload, strlen(payload));
  }
}

void UnoR4WiFi_WebSocket::broadcastTXT(const String& payload) {
  if (wsServer && initialized) {
    wsServer->broadcast(net::WebSocket::DataType::TEXT, payload.c_str(), payload.length());
  }
}

void UnoR4WiFi_WebSocket::broadcastBIN(const uint8_t* payload, size_t length) {
  if (wsServer && initialized) {
    wsServer->broadcast(net::WebSocket::DataType::BINARY, reinterpret_cast<const char*>(payload), length);
  }
}

uint8_t UnoR4WiFi_WebSocket::connectedClients() {
  return clientCount;
}

bool UnoR4WiFi_WebSocket::isListening() {
  return initialized && wsServer;
}
