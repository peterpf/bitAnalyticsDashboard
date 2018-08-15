#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#include <config.h>
#include <states.h>
#include <helpers.h>

ApplicationState appState;
void changeState(ApplicationState state) {
    log("Changed state: " + getStateName(appState) + " to " + getStateName(state));
    appState = state;
}

void setupUpGauges() {
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  for(int i = 0; i < NUM_GAUGES; i++) {
    pinMode(GAUGE_PINS[i], OUTPUT);
    digitalWrite(GAUGE_PINS[i], LOW);
  }
}

void setupIndicatorLED() {
  pinMode(INDICATOR_LED_PIN, OUTPUT);
  digitalWrite(INDICATOR_LED_PIN, LOW);
}

void setup() {
  Serial.begin(9600);
  delay(30);
  log("Starting up...");

  setupUpGauges();
  setupIndicatorLED();

  changeState(CONNECTING);
}

int gaugeIterationTimeCounter = 0;
int gaugeDataIndex = 0;
int gaugeCountDir = 1;

void iterateGauges() {
  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
    setGauge(&(STARTUP_SEQUENCE_GAUGE_DATA[gaugeDataIndex][i]));
    digitalWrite(GAUGE_PINS[i], HIGH);
    delay(GAUGE_SWITCHING_DELAY);
  }
  gaugeIterationTimeCounter += GAUGE_SWITCHING_DELAY * NUM_GAUGES;

  if (gaugeIterationTimeCounter >= GAUGE_PATTERN_VIEWTIME) {
    gaugeIterationTimeCounter = 0;
    gaugeDataIndex += gaugeCountDir;

    if (gaugeDataIndex == 0 || gaugeDataIndex == STARTUP_SEQUENCE_GAUGE_DATA_SIZE-1) {
      gaugeCountDir *= -1;
    }
  }
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

/**
 * ###############
 * ## SEQUENCES ##
 * ###############
 */

int sequenceTimeCounter = 0;
int numSuccessConnected = 0;

void connectingSequence() {
 if (numSuccessConnected == 0) {
   log("Setting up WiFi...");
   numSuccessConnected++;
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 }

 pulseIndicatorLED();

 if (WiFi.status() == WL_CONNECTED) {
   log("Connected!");
   disableIndicatorLED();
   changeState(RUNNING);
 } else if (WiFi.status() == WL_CONNECT_FAILED) {
   log("Failed to connect.");
   changeState(PAUSING);
 }
}

void startupSequence() {
  uint32_t startTime = millis();

  iterateGauges();

  const int deltaTime = millis() - startTime;
  sequenceTimeCounter += deltaTime;
  if(sequenceTimeCounter >= STARTUP_SEQUENCE_TIME) {
    sequenceTimeCounter = 0;
    disableGauges();
    changeState(CONNECTING);
  }
}

void runningSequence() {
  uint32_t startTime = millis();

  iterateGauges();

  const int deltaTime = millis() - startTime;
  sequenceTimeCounter += deltaTime;
  if(sequenceTimeCounter >= DATA_REFRESH_RATE) {
    sequenceTimeCounter = 0;
    sendRequest();
  }
}

void pauseSequence() {
  pulseIndicatorLED();
}

void loop() {
  switch(appState) {
    case STARTUP:
      startupSequence();
      break;
    case RUNNING:
      runningSequence();
      break;
    case CONNECTING:
      connectingSequence();
      break;
    default:
      pauseSequence();
  }
}
