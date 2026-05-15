#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFiS3.h>
#include "secrets.h"

// ---------------- WIFI ----------------
char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;

// ---------------- SENSOR ----------------
#define SENSOR_PIN 4

OneWire oneWire(SENSOR_PIN);
DallasTemperature DS18B20(&oneWire);

// ---------------- SUPABASE ----------------
const char* host = SECRET_SUPABASE_HOST;

const int httpsPort = 443;

const char* supabaseKey = SECRET_SUPABASE_KEY;

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

  if (!client.connect(host, 443)) {
    Serial.println("Connection failed");
    return;
  }

  String json = "{\"value\":" + String(value, 1) + ",\"sensor_id\":\"88dc5f9d-9cfc-470e-b09d-da1fbb09bf42\"}";
  client.println("POST /rest/v1/temperature HTTP/1.1");
  client.print("Host: ");
  client.println(SECRET_SUPABASE_HOST);
  client.println("Content-Type: application/json");

  client.print("apikey: ");
  client.println(SECRET_SUPABASE_KEY);
  client.print("Authorization: Bearer ");
  client.println(SECRET_SUPABASE_KEY);


  client.println("Prefer: return=minimal");

  client.print("Content-Length: ");
  client.println(json.length());

  client.println();   // IMPORTANT blank line
  client.println(json);

  Serial.println("Sent, waiting response...");

  while (client.connected() || client.available()) {
    if (client.available()) {
      Serial.write(client.read());
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
