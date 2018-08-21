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

void setEmptyGauge() {
  byte data = GAUGE_EMPTY_DATA;
  setGauge(&data);
}

void disableGauges() {
  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
  }
  setEmptyGauge();
}

void setSingleGauge(int gaugePin, byte *data) {
  digitalWrite(gaugePin, LOW);
  setGauge(data);
  digitalWrite(gaugePin, HIGH);
}

void iterateGauges(byte data[]) {
  byte singleGaugeData = 0;
  for (int i = 0; i < NUM_GAUGES; i++) {
    singleGaugeData = data[i];
    setSingleGauge(GAUGE_PINS[i], &singleGaugeData);
    delay(GAUGE_SWITCHING_DELAY);
    digitalWrite(GAUGE_PINS[i], LOW);
    setEmptyGauge();
  }
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}

bool didWiFiConnectionFail() {
  return WiFi.status() == WL_CONNECT_FAILED;
}
