#include "UnoR4WiFi_WebServer.h"
#include "UnoR4WiFi_WebSocket.h"
#include "NotFound_Default.h"

UnoR4WiFi_WebServer::UnoR4WiFi_WebServer(int port) : server(port), routeCount(0), notFoundHandler(nullptr), webSocket(nullptr) {
}

void UnoR4WiFi_WebServer::begin() {
  String fv = WiFi.firmwareVersion();
  if (fv < WIFI_FIRMWARE_LATEST_VERSION)
    Serial.println("ERROR: Update WiFi module firmware. Follow: http://newbiely.com/uno-r4-update");

  // Assume WiFi is already connected, just start the server
  Serial.print("Starting web server on IP: ");
  Serial.println(WiFi.localIP());
  server.begin();
}

void UnoR4WiFi_WebServer::begin(const char* ssid, const char* pass) {
  Serial.begin(9600);

  // Check WiFi module
  if (WiFi.status() == WL_NO_MODULE) {
    Serial.println("WiFi module not found!");
    while (true);
  }

  // Connect to WiFi
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    Serial.println("Connecting to WiFi...");
    delay(10000);
  }

  Serial.print("Connected! IP Address: ");
  Serial.println(WiFi.localIP());
  printWifiStatus();

  server.begin();
}

void UnoR4WiFi_WebServer::addRoute(const char* path, RouteHandler handler) {
  if (routeCount < MAX_ROUTES) {
    strncpy(routes[routeCount].path, path, MAX_PATH_LENGTH - 1);
    routes[routeCount].path[MAX_PATH_LENGTH - 1] = '\0';
    routes[routeCount].handler = handler;
    routeCount++;
  } else {
    Serial.println("Max routes reached!");
  }
}

void UnoR4WiFi_WebServer::setNotFoundHandler(RouteHandler handler) {
  notFoundHandler = handler;
}

void UnoR4WiFi_WebServer::parseQueryString(const String& path, QueryParams& params) {
  params.count = 0;
  int queryStart = path.indexOf('?');
  if (queryStart == -1) return;

  String query = path.substring(queryStart + 1);
  int start = 0;
  while (start < query.length() && params.count < MAX_QUERY_PARAMS) {
    int end = query.indexOf('&', start);
    if (end == -1) end = query.length();

    String pair = query.substring(start, end);
    int equals = pair.indexOf('=');
    if (equals != -1) {
      String key = pair.substring(0, equals);
      String value = pair.substring(equals + 1);
      strncpy(params.params[params.count].key, key.c_str(), MAX_PARAM_KEY_LENGTH - 1);
      params.params[params.count].key[MAX_PARAM_KEY_LENGTH - 1] = '\0';
      strncpy(params.params[params.count].value, value.c_str(), MAX_PARAM_VALUE_LENGTH - 1);
      params.params[params.count].value[MAX_PARAM_VALUE_LENGTH - 1] = '\0';
      params.count++;
    }
    start = end + 1;
  }
}

void UnoR4WiFi_WebServer::handleClient() {
  WiFiClient client = server.available();
  if (client) {
    String currentLine = "";
    String request = "";
    String method = "";
    bool currentLineIsBlank = true;
    bool isPost = false;
    bool headersComplete = false;
    int contentLength = 0;

    while (client.connected()) {
      if (client.available()) {
        char c = client.read();
        request += c;

        if (c == '\n' && currentLineIsBlank && !headersComplete) {
          headersComplete = true;
          
          // Parse first line of HTTP request
          String firstLine = request.substring(0, request.indexOf('\n'));
          int start = firstLine.indexOf(' ') + 1;
          int end = firstLine.indexOf(' ', start);
          String path = firstLine.substring(start, end);
          method = firstLine.substring(0, firstLine.indexOf(' '));

          // Check if it's a POST request
          isPost = (method == "POST");

          // Extract Content-Length for POST requests
          if (isPost) {
            int contentLengthIndex = request.indexOf("Content-Length: ");
            if (contentLengthIndex != -1) {
              int start = contentLengthIndex + 16;
              int end = request.indexOf('\r', start);
              String lengthStr = request.substring(start, end);
              contentLength = lengthStr.toInt();
              Serial.print("Content-Length: ");
              Serial.println(contentLength);
            }
          }

          // Parse query parameters
          QueryParams params;
          parseQueryString(path, params);

          // Extract path without query string for route matching
          if (path.indexOf('?') != -1) {
            path = path.substring(0, path.indexOf('?'));
          }

          // Debug: Print the requested path and method
          Serial.print("Method: ");
          Serial.println(method);
          Serial.print("Requested path: ");
          Serial.println(path);
          for (int i = 0; i < params.count; i++) {
            Serial.print("Query param: ");
            Serial.print(params.params[i].key);
            Serial.print("=");
            Serial.println(params.params[i].value);
          }

          // For POST requests, continue reading to get the body
          String jsonData = "";
          if (isPost && contentLength > 0) {
            // Continue reading the body
            continue;
          } else {
            // Process the request
            processRequest(client, method, path, params, jsonData);
            break;
          }
        } else if (headersComplete && isPost && contentLength > 0) {
          // We're reading the body now
          static String bodyData = "";
          bodyData += c;
          
          if (bodyData.length() >= contentLength) {
            String jsonData = bodyData;
            bodyData = ""; // Reset for next request
            
            // Parse first line of HTTP request again
            String firstLine = request.substring(0, request.indexOf('\n'));
            int start = firstLine.indexOf(' ') + 1;
            int end = firstLine.indexOf(' ', start);
            String path = firstLine.substring(start, end);
            
            // Parse query parameters
            QueryParams params;
            parseQueryString(path, params);
            
            // Extract path without query string for route matching
            if (path.indexOf('?') != -1) {
              path = path.substring(0, path.indexOf('?'));
            }
            
            Serial.print("JSON body: ");
            Serial.println(jsonData);
            
            processRequest(client, method, path, params, jsonData);
            break;
          }
        }

        if (c == '\n') {
          currentLineIsBlank = true;
        } else if (c != '\r') {
          currentLineIsBlank = false;
        }
      }
    }
    delay(1);
    client.stop();
    Serial.println("Client disconnected");
  }
}

void UnoR4WiFi_WebServer::processRequest(WiFiClient& client, const String& method, const String& path, const QueryParams& params, const String& jsonData) {
  // Find matching route
  bool routeFound = false;
  for (int i = 0; i < routeCount; i++) {
    if (path.equals(routes[i].path)) {
      routes[i].handler(client, method, String(""), params, jsonData);
      routeFound = true;
      break;
    }
  }

  if (!routeFound) {
    send404(client);
  }
}

void UnoR4WiFi_WebServer::sendResponse(WiFiClient& client, const char* content, const char* contentType) {
  client.println("HTTP/1.1 200 OK");
  client.print("Content-Type: ");
  client.println(contentType);
  client.println("Connection: close");
  client.println();
  client.print(content);
}

void UnoR4WiFi_WebServer::send404(WiFiClient& client) {
  QueryParams emptyParams;
  emptyParams.count = 0; // Explicitly initialize count
  String emptyJson = "";
  String emptyMethod = "";
  if (notFoundHandler != nullptr) {
    notFoundHandler(client, emptyMethod, String(""), emptyParams, emptyJson);
  } else {
	// send the default page
    client.println("HTTP/1.1 404 Not Found");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.print(NOT_FOUND_PAGE_DEFAULT);
  }
}

void UnoR4WiFi_WebServer::printWifiStatus() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("Signal strength (RSSI): ");
  Serial.print(rssi);
  Serial.println(" dBm");
}

// WebSocket functionality
UnoR4WiFi_WebSocket* UnoR4WiFi_WebServer::enableWebSocket(uint16_t wsPort) {
  if (webSocket == nullptr) {
    webSocket = new UnoR4WiFi_WebSocket(wsPort);
    if (webSocket->begin()) {
      Serial.println("WebSocket server enabled successfully");
      return webSocket;
    } else {
      delete webSocket;
      webSocket = nullptr;
      Serial.println("Failed to enable WebSocket server");
      return nullptr;
    }
  }
  return webSocket;
}

UnoR4WiFi_WebSocket* UnoR4WiFi_WebServer::getWebSocket() {
  return webSocket;
}

void UnoR4WiFi_WebServer::handleWebSocket() {
  if (webSocket != nullptr) {
    webSocket->loop();
  }
}

// WebSocket functionality temporarily disabled
// Will be re-enabled once properly implemented