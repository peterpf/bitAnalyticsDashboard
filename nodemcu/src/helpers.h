int indicatorLEDValue = INDICATOR_LED_MIN_VALUE; // max: 255
int indicatorLEDCountDir = 1;

void pulseIndicatorLED() {
  indicatorLEDValue += indicatorLEDCountDir;
  if (indicatorLEDValue <= INDICATOR_LED_MIN_VALUE || indicatorLEDValue >= INDICATOR_LED_MAX_VALUE) {
  indicatorLEDCountDir = indicatorLEDCountDir * (-1);
  }
  analogWrite(INDICATOR_LED_PIN, indicatorLEDValue);
  delay(INDICATOR_LED_PULSE_SPEED);
}

void disableIndicatorLED() {
  digitalWrite(INDICATOR_LED_PIN, LOW);
}

void setGauge(byte *data) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, *data);
  digitalWrite(LATCH_PIN, HIGH);
}

void disableGauges() {
  byte emptyGauges = 0x00;
  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
  }
  setGauge(&emptyGauges);
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool didWiFiConnectionFail() {
  return WiFi.status() == WL_CONNECT_FAILED;
}

void log(String message) {
  Serial.println(message);
}

void sendRequest() {
  log("Sending request...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(DATA_HTTP_REQUEST);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      log("Received data:");
      log(payload);
    }else {
      log("Error sending request.");
    }
    http.end();
  }
}
