/*
 * Arduino Uno R4 WiFi - WebSocket Server Example
 * 
 * This example demonstrates how to create a WebSocket server with:
 * - Real-time bidirectional communication
 * - Message parsing and command handling
 * - LED control via WebSocket commands
 * - Web page serving for WebSocket testing
 * 
 * Hardware: Arduino Uno R4 WiFi or DIYables STEM V4 IoT
 * Library: UnoR4WiFi_WebServer (with WebSocket support)
 */

#include <UnoR4WiFi_WebServer.h>
#include "websocket_html.h"

// WiFi credentials
const char WIFI_SSID[] = "YOUR_WIFI_SSID";
const char WIFI_PASSWORD[] = "YOUR_WIFI_PASSWORD";

// Create web server instance
UnoR4WiFi_WebServer server;
UnoR4WiFi_WebSocket* webSocket;

// Page handlers
void handleHome(WiFiClient& client, const String& method, const String& request, const QueryParams& params, const String& jsonData) {
  server.sendResponse(client, htmlPage);
}

// WebSocket event handlers
void onWebSocketOpen(net::WebSocket& ws) {
  Serial.println("New WebSocket connection");
  // Send welcome message
  const char welcome[] = "Connected to Arduino Uno R4 WiFi!";
  // Note: Individual client messaging will be handled differently in the integrated version
}

void onWebSocketMessage(net::WebSocket& ws, const net::WebSocket::DataType dataType, const char* message, uint16_t length) {
  Serial.print("[WebSocket] Received (");
  Serial.print(length);
  Serial.print(" bytes): ");
  Serial.println(message);
  
  String msgStr = String(message);
  String response = "";
  
  // Command processing
  if (msgStr.equalsIgnoreCase("ping")) {
    response = "pong";
  }
  else if (msgStr.equalsIgnoreCase("hello")) {
    response = "Hello from Arduino Uno R4 WiFi!";
  }
  else if (msgStr.equalsIgnoreCase("time")) {
    response = "Uptime: " + String(millis()/1000) + " seconds";
  }
  else if (msgStr.equalsIgnoreCase("led on")) {
    digitalWrite(LED_BUILTIN, HIGH);
    response = "LED ON";
  }
  else if (msgStr.equalsIgnoreCase("led off")) {
    digitalWrite(LED_BUILTIN, LOW);
    response = "LED OFF";
  }
  else {
    response = "Echo: " + msgStr;
  }

  Serial.print("[WebSocket] Sending response (");
  Serial.print(response.length());
  Serial.print(" bytes): ");
  Serial.println(response);
  
  // Broadcast response to all connected clients using the library
  if (webSocket != nullptr) {
    webSocket->broadcastTXT(response);
  }
}

void onWebSocketClose(net::WebSocket& ws, const net::WebSocket::CloseCode code, const char* reason, uint16_t length) {
  Serial.println("WebSocket client disconnected");
}

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("Arduino Uno R4 WiFi - WebSocket Server");
  
  // Configure web server routes
  server.addRoute("/", handleHome);
  
  // Start web server with WiFi connection
  server.begin(WIFI_SSID, WIFI_PASSWORD);
  
  // Enable WebSocket functionality
  webSocket = server.enableWebSocket(81);
  
  if (webSocket != nullptr) {
    // Set up WebSocket event handlers
    webSocket->onOpen(onWebSocketOpen);
    webSocket->onMessage(onWebSocketMessage);
    webSocket->onClose(onWebSocketClose);
    
    Serial.println("WebSocket server started on port 81");
  } else {
    Serial.println("Failed to start WebSocket server");
  }
  
  Serial.println("\n=== WebSocket Server Ready! ===");
  Serial.print("Web page: http://");
  Serial.println(WiFi.localIP());
  Serial.print("WebSocket: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");
  Serial.println("Commands: ping, hello, time, led on, led off");
} 
 
void loop() {
  // Handle HTTP requests and WebSocket connections using the library
  server.handleClient();
  server.handleWebSocket();
  
  delay(10);
}
