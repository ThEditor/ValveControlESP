#include <WiFi.h>
#include <WebServer.h>
#include <EEPROM.h>

const char *ssid = "Help";
const char *password = "justconnect";

const char *apSSID = "ESP32-Setup";
const char *apPassword = "esp32password";

const int relayPin = 5;

WebServer server(80);

struct Credentials {
  char ssid[32];
  char password[64];
};

bool relayState = false;

void setupWiFiStationMode() {
  Serial.println("Connecting to WiFi");

  Credentials storedCredentials;
  EEPROM.begin(sizeof(Credentials));
  EEPROM.get(0, storedCredentials);
  EEPROM.end();

  if (strlen(storedCredentials.ssid) > 0 && strlen(storedCredentials.password) > 0) {
    // Use stored credentials if available
    WiFi.begin(storedCredentials.ssid, storedCredentials.password);
  } else {
    // Use default credentials
    WiFi.begin(ssid, password);
  }

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(1000);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("\nFailed to connect to WiFi. Starting AP mode.");
    setupWiFiAPMode();
  } else {
    Serial.println("\nConnected to WiFi");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
    server.on("/", HTTP_GET, handleRootClient);
    server.on("/check", HTTP_GET, handleCheckClient);
    server.on("/relay", HTTP_GET, handleRelayClient);
    server.begin();
  }
}

void handleRoot() {
  String html = "<html><body><h1>ESP32 WiFi Setup</h1>";
  html += "<form action='/save' method='post'>";
  html += "SSID: <input type='text' name='ssid'><br>";
  html += "Password: <input type='password' name='password'><br>";
  html += "<input type='submit' value='Save'>";
  html += "</form>";

  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleRootClient() {
  String html = "<html><body><h1>Control Valve</h1>";
  // Relay control buttons
  html += "<h2>Relay Control</h2>";
  html += "<form action='/relay' method='get'>";
  html += "<button type='submit' name='state' value='on'>Turn On</button>";
  html += "<button type='submit' name='state' value='off'>Turn Off</button>";
  html += "</form>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}

void handleSave() {
  String newSSID = server.arg("ssid");
  String newPassword = server.arg("password");

  Serial.println("Saving new credentials");
  Serial.print("New SSID: ");
  Serial.println(newSSID);
  Serial.print("New Password: ");
  Serial.println(newPassword);

  Credentials newCredentials;
  strncpy(newCredentials.ssid, newSSID.c_str(), sizeof(newCredentials.ssid));
  strncpy(newCredentials.password, newPassword.c_str(), sizeof(newCredentials.password));

  EEPROM.begin(sizeof(Credentials));
  EEPROM.put(0, newCredentials);
  EEPROM.commit();
  EEPROM.end();

  server.send(200, "text/html", "Credentials saved, restart for changes to take effect.");
}

void handleRelayClient() {
  String state = server.arg("state");
  if (state == "on") {
    digitalWrite(relayPin, HIGH);
  } else if (state == "off") {
    digitalWrite(relayPin, LOW);
  }

  server.send(200, "text/html", "Relay state: " + String(relayState ? "On" : "Off"));
  // You can add code here to control your relay module based on the relayState
}

void handleCheckClient() {
  String jsonResponse = "{\"is_valve\":true}";
  server.send(200, "application/json", jsonResponse);
}

void setupWiFiAPMode() {
  Serial.println("Starting AP mode");
  WiFi.softAP(apSSID, apPassword);

  server.on("/", HTTP_GET, handleRoot);
  server.on("/save", HTTP_POST, handleSave);

  server.begin();
}

void setup() {
  Serial.begin(921600);  // Set the serial monitor speed
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);

  setupWiFiStationMode();
}

void loop() {
  server.handleClient();
  // Your main code goes here
}
