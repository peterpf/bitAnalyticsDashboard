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

void setGauge(byte *data = 0x00) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(DATA_PIN, CLOCK_PIN, LSBFIRST, *data);
  digitalWrite(LATCH_PIN, HIGH);
}

void disableGauges() {
  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
    setGauge();
  }
}
