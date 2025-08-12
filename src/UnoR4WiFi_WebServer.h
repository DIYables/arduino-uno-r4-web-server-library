#ifndef UNOR4_WIFI_WEBSERVER_H
#define UNOR4_WIFI_WEBSERVER_H

#include <WiFiS3.h>

// Forward declare WebSocket class
class UnoR4WiFi_WebSocket;

#define MAX_ROUTES 10
#define MAX_PATH_LENGTH 32
#define MAX_HTML_SIZE 1024
#define MAX_QUERY_PARAMS 5
#define MAX_PARAM_KEY_LENGTH 16
#define MAX_PARAM_VALUE_LENGTH 16

// Structure to hold query parameters
struct QueryParams {
  struct Param {
    char key[MAX_PARAM_KEY_LENGTH];
    char value[MAX_PARAM_VALUE_LENGTH];
  };
  Param params[MAX_QUERY_PARAMS];
  int count;
};

// Handler function type
typedef void (*RouteHandler)(WiFiClient& client, const String& method, const String& request, const QueryParams& params, const String& jsonData);

class UnoR4WiFi_WebServer {
public:
  UnoR4WiFi_WebServer(int port = 80);
  void begin();  // Start server assuming WiFi is already connected
  void begin(const char* ssid, const char* pass);  // Connect to WiFi and start server
  void addRoute(const char* path, RouteHandler handler);
  void setNotFoundHandler(RouteHandler handler);
  void handleClient();
  void sendResponse(WiFiClient& client, const char* content, const char* contentType = "text/html");
  void send404(WiFiClient& client);
  void printWifiStatus();
  
  // WebSocket functionality
  UnoR4WiFi_WebSocket* enableWebSocket(uint16_t wsPort = 81);
  UnoR4WiFi_WebSocket* getWebSocket();
  void handleWebSocket();

private:
  WiFiServer server;
  UnoR4WiFi_WebSocket* webSocket;
  struct Route {
    char path[MAX_PATH_LENGTH];
    RouteHandler handler;
  };
  Route routes[MAX_ROUTES];
  int routeCount;
  RouteHandler notFoundHandler;
  void parseQueryString(const String& path, QueryParams& params);
  void processRequest(WiFiClient& client, const String& method, const String& path, const QueryParams& params, const String& jsonData);
};

// Include WebSocket functionality (automatically available but only compiled if used)
#include "UnoR4WiFi_WebSocket.h"

// Beginner-friendly type aliases - no namespace needed!
using WebSocketServer = net::WebSocketServer;
using WebSocket = net::WebSocket;

#endif