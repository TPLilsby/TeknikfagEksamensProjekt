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

const char* supabaseKey = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im55a2F0dHpqdmNrb2Jkb3lybmxuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzE5NTM0NTQsImV4cCI6MjA4NzUyOTQ1NH0.d_2tKReBu45WNdZqzILr66biS4oXK0s5Fsdc2Fwaizg";

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

  String json = "{\"value\":" + String(value, 1) + "}";

  client.println("POST /rest/v1/temperature HTTP/1.1");
  client.println("Host: nykattzjvckobdoyrnln.supabase.co");
  client.println("Content-Type: application/json");

  client.println("apikey: eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im55a2F0dHpqdmNrb2Jkb3lybmxuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzE5NTM0NTQsImV4cCI6MjA4NzUyOTQ1NH0.d_2tKReBu45WNdZqzILr66biS4oXK0s5Fsdc2Fwaizg");
  client.println("Authorization: Bearer eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJzdXBhYmFzZSIsInJlZiI6Im55a2F0dHpqdmNrb2Jkb3lybmxuIiwicm9sZSI6ImFub24iLCJpYXQiOjE3NzE5NTM0NTQsImV4cCI6MjA4NzUyOTQ1NH0.d_2tKReBu45WNdZqzILr66biS4oXK0s5Fsdc2Fwaizg");


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