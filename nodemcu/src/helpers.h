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

byte gaugeDataFromRequests[NUM_GAUGES] = {0};
void parseData(String *data, RequestType *type) {
  log("Parsing data...");
  DynamicJsonBuffer  jsonBuffer(200);
  JsonObject& root = jsonBuffer.parseObject(*data);
  if (!root.success()) {
    log("Failed to parse data");
    return;
  }
  int values[NUM_GAUGE_SUBSECTIONS] = {0};
  for (int i = 0; i < 2; i++) {
    values[i] = root["data"]["result"][i]["value"][1];
  }
  int offset = mapRequestTypeToGaugeDataOffset(type);
  int valueOne = convertValueToGaugeValue(&values[0], type);
  int valueTwo = convertValueToGaugeValue(&values[1], type);
  log("Parsed data successfully: " + String(valueOne) + "/" + String(valueTwo) + ", offset: " + String(offset));
  gaugeDataFromRequests[offset] = convertDataToGaugeValue(&valueOne, &valueTwo);

  String output = "";
  for (int i = 0; i < NUM_GAUGES; i++) {
    output += String(gaugeDataFromRequests[i]) + ", ";
  }
  log("Data: " + output);
}

void sendRequest(String *url, RequestType *type) {
  log("Sending request...");
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(*url);
    int httpCode = http.GET();

    if (httpCode > 0) {
      String payload = http.getString();
      log("Received request data for: " + *url);
      parseData(&payload, type);
    }else {
      log("Error sending request.");
    }
    http.end();
  }
}
