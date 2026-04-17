#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiS3.h>

// ---------------- WIFI ----------------
char ssid[] = "Lab-ZBC";
char pass[] = "Prestige#PuzzledCASH48!";

// ---------------- SENSOR ----------------
#define SENSOR_PIN 4

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

// ---------------- SUPABASE ----------------
const char* host = "nykattzjvckobdoyrnln.supabase.co";
const int httpsPort = 443;

const char* supabaseKey = "YOUR_SUPABASE_KEY";

// ---------------- CLIENT ----------------
WiFiSSLClient client;

// ---------------- SETUP ----------------
void setup() {
  Serial.begin(9600);
  delay(1000);

  DS18B20.begin();

  Serial.print("Connecting to WiFi");

  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nConnected!");
  Serial.print("IP: ");
  Serial.println(WiFi.localIP());
}

// ---------------- SEND FUNCTION ----------------
void sendTemperature(float value) {
  Serial.println("\nConnecting to server...");

  if (!client.connect(host, httpsPort)) {
    Serial.println("Connection failed");
    return;
  }

  String json = "{\"value\":" + String(value, 1) + "}";

  client.println("POST /rest/v1/temperature HTTP/1.1");
  client.println("Host: " + String(host));
  client.println("Content-Type: application/json");
  client.println("apikey: " + String(supabaseKey));
  client.println("Authorization: Bearer " + String(supabaseKey));
  client.println("Prefer: return=minimal");
  client.print("Content-Length: ");
  client.println(json.length());
  client.println();
  client.println(json);

  Serial.println("Data sent!");

  // Read response (debug)
  while (client.connected() || client.available()) {
    if (client.available()) {
      char c = client.read();
      Serial.print(c);
    }
  }

  client.stop();
}

// ---------------- LOOP ----------------
void loop() {
  DS18B20.requestTemperatures();

  float tempC = DS18B20.getTempCByIndex(0);

  if (tempC == DEVICE_DISCONNECTED_C) {
    Serial.println("Sensor error!");
    delay(2000);
    return;
  }

  Serial.print("Temperature: ");
  Serial.print(tempC);
  Serial.println(" °C");

  sendTemperature(tempC);

  delay(5000); // send every 5 seconds
}