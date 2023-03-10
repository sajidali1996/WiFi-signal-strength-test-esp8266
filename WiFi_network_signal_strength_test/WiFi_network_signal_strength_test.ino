#include <ESP8266WiFi.h>
#include <WiFiManager.h>

const int BUTTON_PIN = 0; // D3 on NodeMCU 12E

void setup() {
  Serial.begin(9600);

  // Connect to Wi-Fi or start configuration portal if not connected
  WiFiManager wifiManager;
  if (!wifiManager.autoConnect("WiFiManagerAP")) {
    Serial.println("Failed to connect to Wi-Fi and timed out.");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  Serial.println("Connected to Wi-Fi.");
}

void loop() {
  static unsigned long buttonPressTime = 0;
  static bool buttonPressed = false;

  // Check if button is pressed and hold for 10 seconds
  if (digitalRead(BUTTON_PIN) == LOW) {
    if (!buttonPressed) {
      buttonPressTime = millis();
      buttonPressed = true;
    } else if (millis() - buttonPressTime >= 10000) {
      Serial.println("Triggering network change...");
      WiFiManager wifiManager;
      wifiManager.resetSettings();
      wifiManager.startConfigPortal("WiFiManagerAP");
      ESP.reset();
      delay(5000);
    }
  } else {
    buttonPressed = false;
  }

  // Measure RSSI and print to serial monitor
  int rssiSum = 0;
  int numSamples = 0;
  unsigned long startTime = millis();

  // Measure RSSI for 10 times
  for(int i=0;i<10;i++){
    int rssi = WiFi.RSSI();
    if (rssi != 0) { // Ignore invalid RSSI values
      rssiSum -= rssi;
      numSamples++;
    }
    delay(10); // Wait 10ms between measurements
  }

  // Calculate average RSSI and convert to percentage
  float avgRssi = (float)rssiSum / numSamples;
  float percentage = 0.0;

  // Convert RSSI to percentage using a logarithmic formula
  if (avgRssi <= -100) {
    percentage = 0.0;
  } else if (avgRssi >= -50) {
    percentage = 100.0;
  } else {
    percentage = pow(10, (2.0 * (avgRssi + 100.0) / 10.0));
  }
  Serial.print("RSSI (dBm)");
  Serial.print(",");
  Serial.print(-1*avgRssi);
  Serial.print(",");
  Serial.print("Signal Quality (%) ");
  Serial.print(",");
  Serial.println(percentage);

  delay(1000); // Wait 1s before next measurement
}
