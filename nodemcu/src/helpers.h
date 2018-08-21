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
  if (LOGGING_ENABLED) {
    Serial.println(message);
  }
}

byte gaugeDataFromRequests[NUM_GAUGES] = {0};
void parseData(String *data, RequestType *type) {
  DynamicJsonBuffer  jsonBuffer(200);
  JsonObject& root = jsonBuffer.parseObject(*data);
  if (!root.success()) {
    log("Failed to parse data: " + String(*data));
    return;
  }

  int offset = mapRequestTypeToGaugeDataOffset(type);

  int numResultValues = 1;
  if (*type == Instances || *type == Requests) {
    numResultValues = 2;
  }

  double values[NUM_GAUGE_SUBSECTIONS] = {0};
  for (int i = 0; i < NUM_GAUGE_SUBSECTIONS; i++) {
    double result = root["data"]["result"][i]["value"][1];
    if (i >= numResultValues) {
      result = values[i-1];
    }
    values[i] = result;
  }

  int valueOne = convertValueToGaugeValue(&values[0], type);
  int valueTwo = convertValueToGaugeValue(&values[1], type);
  gaugeDataFromRequests[offset] = convertDataToGaugeValue(&valueOne, &valueTwo);
}

void sendRequest(String *url, RequestType *type) {
  log("Sending request: " + *url);
  HTTPClient http;
  http.begin(*url);

  int httpCode = http.GET();
  if (httpCode > 0) {
    String payload = http.getString();
    parseData(&payload, type);
  }else {
    log(printf("HTTP GET failed, error: %s\n", http.errorToString(httpCode).c_str()));
  }
  http.end();
}

void printGaugeDataFromRequests() {
  if (!LOGGING_ENABLED) {
    return;
  }
  String output = "";
  for (int i = 0; i < NUM_GAUGES; i++) {
    output += "0x"+String(gaugeDataFromRequests[i], HEX) + ", ";
  }
  log("Data: " + output);
}

void sendAllRequests() {
  RequestType type = Instances;
  String url = URL_INSTANCES;
  sendRequest(&url, &type);
  type = Requests;
  url = URL_REQUESTS_PER_MINUTE;
  sendRequest(&url, &type);
  type = CPU_EU;
  url = URL_CPU_USAGE_EU;
  sendRequest(&url, &type);
  type = CPU_US;
  url = URL_CPU_USAGE_US;
  sendRequest(&url, &type);

  printGaugeDataFromRequests();
}
