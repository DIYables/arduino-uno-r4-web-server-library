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
 
// Server configuration
WiFiServer httpServer(80);
constexpr uint16_t wsPort = 81;
WebSocketServer wss{wsPort};

void setup() {
  Serial.begin(9600);
  delay(1000);
  
  // Initialize built-in LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  Serial.println("Arduino Uno R4 WiFi - WebSocket Server");
  
  // Connect to WiFi
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println(" connected!");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  // Start HTTP server
  httpServer.begin();
  Serial.println("HTTP server started on port 80");
  
  // Configure WebSocket server
  wss.onConnection([](WebSocket &ws) {
    Serial.println("New WebSocket connection");
     
    // Set up message handler
    ws.onMessage([](WebSocket &ws, const WebSocket::DataType dataType, const char *message, uint16_t length) {
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

      Serial.print("[WebSocket] sent (");
      Serial.print(response.length());
      Serial.print(" bytes): ");
      Serial.println(response);
      
      // Send response
      ws.send(WebSocket::DataType::TEXT, response.c_str(), response.length());
    });
    
    ws.onClose([](WebSocket &ws, const WebSocket::CloseCode code, const char *reason, uint16_t length) {
      Serial.println("WebSocket client disconnected");
    });
    
    // Send welcome message
    const char welcome[] = "Connected to Arduino Uno R4 WiFi!";
    ws.send(WebSocket::DataType::TEXT, welcome, strlen(welcome));
  });
  
  // Start WebSocket server
  wss.begin();
  Serial.println("WebSocket server started on port 81");
  
  Serial.println("\n=== WebSocket Server Ready! ===");
  Serial.print("Web page: http://");
  Serial.println(WiFi.localIP());
  Serial.print("WebSocket: ws://");
  Serial.print(WiFi.localIP());
  Serial.println(":81");
  Serial.println("Commands: ping, hello, time, led on, led off");
} 
 
// HTTP request handler
void handleHTTPClient(WiFiClient client) {
  String request = "";
  
  // Read HTTP request
  while (client.connected() && client.available()) {
    String line = client.readStringUntil('\n');
    if (line == "\r") break;
    if (request.length() == 0) request = line;
  }
  
  // Serve web page or 404
  if (request.indexOf("GET / HTTP") >= 0) {
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(htmlPage);
  } else {
    client.println("HTTP/1.1 404 Not Found");
    client.println("Connection: close");
    client.println();
  }
  
  client.stop();
} 

void loop() {
  // Handle HTTP requests
  WiFiClient httpClient = httpServer.available();
  if (httpClient) {
    handleHTTPClient(httpClient);
  }
  
  // Handle WebSocket connections
  wss.listen();
  
  delay(10);
}
