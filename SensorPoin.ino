#include <WiFi.h>
#include <HTTPClient.h>

#define SENSOR_PIN 26

// const char* ssid = "HASYABA 2.4G";
// const char* password = "10nov1970";
const char* ssid = "KENZKOPI";
const char* password = "podokarowingi";
const String serverUrl = "http://192.168.18.149:8000/api/update-training-counter";
// const String serverUrl = "http://192.168.1.93:8000/api/update-training-counter";

volatile bool objectDetected = false;
volatile unsigned long lastInterruptTime = 0;
const unsigned long debounceDelay = 50; // ms

void IRAM_ATTR handleSensorInterrupt() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > debounceDelay) {
    objectDetected = true;
    lastInterruptTime = currentTime;
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(SENSOR_PIN, INPUT);
  attachInterrupt(digitalPinToInterrupt(SENSOR_PIN), handleSensorInterrupt, FALLING);

  // WiFi connect
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi");
}

void loop() {
  if (objectDetected) {
    objectDetected = false;
    sendStatusToServer("Object Detected");
  }
}

void sendStatusToServer(String statusText) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(serverUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonData = "{\"status\": \"" + statusText + "\"}";
    int httpResponseCode = http.POST(jsonData);

    if (httpResponseCode > 0) {
      String response = http.getString();
      Serial.println("Response: " + response);
    } else {
      Serial.print("Error sending POST: ");
      Serial.println(httpResponseCode);
    }

    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}
