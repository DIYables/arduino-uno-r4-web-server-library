#ifndef WEBSOCKET_HTML_H
#define WEBSOCKET_HTML_H

const char htmlPage[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>
<head>
<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
<title>Arduino WebSocket Demo</title>
<style>
body { 
  font-family: Arial, sans-serif; 
  margin: 20px; 
  background-color: #f5f5f5;
}
.container { 
  max-width: 600px; 
  margin: 0 auto; 
  background: white;
  padding: 20px;
  border-radius: 10px;
  box-shadow: 0 2px 10px rgba(0,0,0,0.1);
}
h1 {
  color: #333;
  text-align: center;
  margin-bottom: 20px;
}
.status { 
  padding: 15px; 
  margin: 15px 0; 
  border-radius: 8px; 
  text-align: center;
  font-weight: bold;
}
.connected { 
  background-color: #d4edda; 
  color: #155724; 
  border: 1px solid #c3e6cb;
}
.disconnected { 
  background-color: #f8d7da; 
  color: #721c24; 
  border: 1px solid #f5c6cb;
}
input, button { 
  padding: 10px 15px; 
  margin: 5px; 
  border-radius: 5px;
  border: 1px solid #ddd;
  font-size: 14px;
}
button {
  background-color: #007bff;
  color: white;
  border: none;
  cursor: pointer;
}
button:hover {
  background-color: #0056b3;
}
.btn-danger {
  background-color: #dc3545;
}
.btn-danger:hover {
  background-color: #c82333;
}
.btn-success {
  background-color: #28a745;
}
.btn-success:hover {
  background-color: #218838;
}
.messages { 
  border: 1px solid #ddd; 
  height: 300px; 
  overflow-y: scroll; 
  padding: 15px; 
  background: #f9f9f9; 
  border-radius: 5px;
  margin: 15px 0;
}
.message { 
  margin: 5px 0; 
  padding: 5px; 
  border-radius: 3px;
}
.sent { 
  background-color: #e3f2fd;
  color: #1976d2; 
}
.received { 
  background-color: #e8f5e8;
  color: #388e3c; 
}
.button-group {
  margin: 15px 0;
  text-align: center;
}
.input-section {
  display: flex;
  align-items: center;
  gap: 10px;
  margin: 15px 0;
}
.input-section input {
  flex: 1;
  margin: 0;
}
.input-section button {
  margin: 0;
}
.quick-buttons {
  display: flex;
  flex-wrap: wrap;
  gap: 8px;
  margin: 15px 0;
  justify-content: center;
}
.quick-buttons button {
  flex: 1;
  min-width: 90px;
  margin: 0;
}
</style>
</head>
<body>
<div class="container">
<h1>Arduino WebSocket Demo</h1>
<div id="status" class="status disconnected">Status: Disconnected</div>
<div class="button-group">
<button onclick="toggleConnection()" id="connBtn">Connect</button>
</div>
<div class="input-section">
<input type="text" id="msgInput" placeholder="Type message...">
<button onclick="sendMsg()">Send</button>
</div>
<div class="quick-buttons">
<button onclick="sendQuick('ping')">Ping</button>
<button onclick="sendQuick('hello')">Hello</button>
<button onclick="sendQuick('time')">Time</button>
<button onclick="sendQuick('led on')" class="btn-success">LED ON</button>
<button onclick="sendQuick('led off')" class="btn-danger">LED OFF</button>
</div>
<div id="messages" class="messages">
<div class="message">Ready. Click Connect to start.</div>
</div>
<div class="button-group">
<button onclick="clearMsg()">Clear</button>
</div>
<div style="text-align: center; margin-top: 20px; color: #666;">
This works with Arduino Uno R4 WiFi and <a href="https://diyables.io/products/diyables-stem-v4-iot-fully-compatible-with-arduino-uno-r4-wifi" target="_blank">DIYables STEM V4 IoT</a>
</div>
</div>
<script>
var ws = null;
var connected = false;
var wsUrl = 'ws://' + window.location.hostname + ':81';

function toggleConnection() {
  if (connected) {
    disconnect();
  } else {
    connect();
  }
}

function connect() {
  try {
    console.log('Attempting WebSocket connection to: ' + wsUrl);
    addMsg('Connecting to ' + wsUrl);
    ws = new WebSocket(wsUrl);
    
    ws.onopen = function() {
      console.log('WebSocket connection opened');
      connected = true;
      updateStatus('Connected', true);
      addMsg('Connected!');
    };
    
    ws.onmessage = function(event) {
      console.log('WebSocket message received:', event.data);
      addMsg('Arduino: ' + event.data, 'received');
    };
    
    ws.onclose = function(event) {
      console.log('WebSocket connection closed:', event);
      connected = false;
      updateStatus('Disconnected', false);
      addMsg('Disconnected - Code: ' + event.code + ', Reason: ' + (event.reason || 'No reason'));
    };
    
    ws.onerror = function(event) {
      console.error('WebSocket error:', event);
      connected = false;
      updateStatus('Error', false);
      addMsg('WebSocket error occurred - Check browser console for details');
    };
  } catch(e) {
    console.error('Error creating WebSocket:', e);
    addMsg('Error creating WebSocket: ' + e.message);
  }
}

function disconnect() {
  if (ws) {
    ws.close();
  }
}

function sendMsg() {
  var input = document.getElementById('msgInput');
  var msg = input.value.trim();
  if (msg && connected && ws) {
    ws.send(msg);
    addMsg('You: ' + msg, 'sent');
    input.value = '';
  } else if (!connected) {
    addMsg('Not connected!');
  }
}

function sendQuick(msg) {
  if (connected && ws) {
    ws.send(msg);
    addMsg('You: ' + msg, 'sent');
  } else {
    addMsg('Not connected!');
  }
}

function addMsg(text, type) {
  var div = document.createElement('div');
  div.className = 'message ' + (type || '');
  div.textContent = new Date().toLocaleTimeString() + ' - ' + text;
  document.getElementById('messages').appendChild(div);
  document.getElementById('messages').scrollTop = 999999;
}

function clearMsg() {
  document.getElementById('messages').innerHTML = '';
}

function updateStatus(text, isConnected) {
  var statusDiv = document.getElementById('status');
  var connBtn = document.getElementById('connBtn');
  statusDiv.textContent = 'Status: ' + text;
  statusDiv.className = 'status ' + (isConnected ? 'connected' : 'disconnected');
  connBtn.textContent = isConnected ? 'Disconnect' : 'Connect';
  connBtn.className = isConnected ? 'btn-danger' : '';
}

document.getElementById('msgInput').addEventListener('keypress', function(e) {
  if (e.key === 'Enter') sendMsg();
});

window.onload = function() {
  addMsg('Ready. Click Connect to start.');
};
</script>


</body></html>
)rawliteral";

#endif
