#include <WiFi.h>
#include <WebServer.h>

// Set your network name and password
const char *ssid = "ognistyzuczek";
const char *password = "123456789";

// Create a WebServer object listening on standard HTTP port 80
WebServer server(80);

// The HTML content to display. 
// PROGMEM saves RAM by storing this string in flash memory.
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <title>ESP32-C6 Web Server</title>
  <style>
    body { font-family: Arial, sans-serif; text-align: center; margin-top: 50px; background-color: #f4f4f9; }
    h1 { color: #005bb5; }
    .container { padding: 20px; border-radius: 10px; background: white; display: inline-block; box-shadow: 0px 4px 8px rgba(0,0,0,0.1); }
  </style>
</head>
<body>
  <div class="container">
    <h1>Hello from FireBeetle 2 ESP32-C6!</h1>
    <p>This web page is hosted directly on your microcontroller.</p>
  </div>
</body>
</html>
)rawliteral";

// Function to handle requests to the root URL ("/")
void handleRoot() {
  server.send(200, "text/html", index_html);
}

// Function to handle requests for unknown pages
void handleNotFound() {
  server.send(404, "text/plain", "404: Page not found");
}

void setup() {
  Serial.begin(115200);
  delay(2000); 
  
  Serial.println("\n--- Starting Soft AP & Web Server ---");

  // 1. Start the Soft AP
  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  
  Serial.print("Connect to SSID: ");
  Serial.println(ssid);
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // 2. Configure Web Server Routes
  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);

  // 3. Start the Web Server
  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  // Listen for and handle incoming client requests
  server.handleClient();
}