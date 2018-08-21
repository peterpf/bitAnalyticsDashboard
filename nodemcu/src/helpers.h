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

  double values[NUM_GAUGE_SUBSECTIONS]  = {0};
  int numResultValues                   = getNumResponseValuesForRequestType(type);
  for (int i = 0; i < NUM_GAUGE_SUBSECTIONS; i++) {
    if (i < numResultValues) {
      values[i] = root["data"]["result"][i]["value"][1];
    } else {
      values[i] = values[i-1];
    }
  }

  int dataIndex = *type;
  int valueOne  = convertValueToGaugeValue(&values[0], type);
  int valueTwo  = convertValueToGaugeValue(&values[1], type);
  gaugeDataFromRequests[dataIndex] = convertDataToGaugeValue(&valueOne, &valueTwo);
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
  for (int i = 0; i < NUM_GAUGES-1; i++) {
    output += "0x" + String(gaugeDataFromRequests[i], HEX) + ", ";
  }
  output += "0x" + String(gaugeDataFromRequests[NUM_GAUGES-1], HEX);
  log("Data: " + output);
}

void sendAllRequests() {
  RequestType type  = Instances;
  String url        = URL_INSTANCES;
  sendRequest(&url, &type);
  type  = Requests;
  url   = URL_REQUESTS_PER_MINUTE;
  sendRequest(&url, &type);
  type  = CPU_EU;
  url   = URL_CPU_USAGE_EU;
  sendRequest(&url, &type);
  type  = CPU_US;
  url   = URL_CPU_USAGE_US;
  sendRequest(&url, &type);

  printGaugeDataFromRequests();
}
