#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <config.h>
#include <state.h>
#include <requestParser.h>
#include <helpers.h>

ApplicationState appState = STARTUP;
ApplicationState nextState = STARTUP;

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
}

int gaugeIterationTimeCounter = 0;
int gaugeDataIndex = 0;
int gaugeCountDir = 1;
byte emptyGauge = 0;

void iterateGaugesRequestData() {
  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
    byte singleGauge = gaugeDataFromRequests[i];
    setGauge(&singleGauge);
    digitalWrite(GAUGE_PINS[i], HIGH);
    delay(GAUGE_SWITCHING_DELAY);
    digitalWrite(GAUGE_PINS[i], LOW);
    setGauge(&emptyGauge);
  }
}

void iterateStartupData() {
  uint32_t startTime = millis();

  for (int i = 0; i < NUM_GAUGES; i++) {
    digitalWrite(GAUGE_PINS[i], LOW);
    byte singleGauge = STARTUP_SEQUENCE_GAUGE_DATA[gaugeDataIndex][i];
    setGauge(&singleGauge);
    digitalWrite(GAUGE_PINS[i], HIGH);
    delay(GAUGE_SWITCHING_DELAY);
    digitalWrite(GAUGE_PINS[i], LOW);
    setGauge(&emptyGauge);
  }

  const int deltaTime = millis() - startTime;
  gaugeIterationTimeCounter += deltaTime;


  if (gaugeIterationTimeCounter >= GAUGE_PATTERN_VIEWTIME) {
    gaugeIterationTimeCounter = 0;
    gaugeDataIndex += gaugeCountDir;

    if (gaugeDataIndex == 0 || gaugeDataIndex == STARTUP_SEQUENCE_GAUGE_DATA_SIZE-1) {
      gaugeCountDir *= -1;
    }
  }
}

/**
 * ###############
 * ## SEQUENCES ##
 * ###############
 */

bool shouldSendRequests = true;
bool shouldResetSequenceRunTime = false;
int numSuccessConnected = 0;
int numConnectionAttempts = 0;

void connectingSequence() {
 if (numSuccessConnected == 0) {
   log("Setting up WiFi...");
   numSuccessConnected++;
   WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 }
 pulseIndicatorLED();
}

void afterConnectingSequence(int passedTime) {
   if (isWiFiConnected()) {
     log("Connected with IP: " + WiFi.localIP().toString());
     disableIndicatorLED();
   } else if (didWiFiConnectionFail()) {
     log("Failed to connect...");
     numConnectionAttempts++;
   }

   if (isWiFiConnected() && passedTime >= SEQUENCE_CONNECTING_MIN_TIME) {
     nextState = RUNNING;
   }

   if (numConnectionAttempts >= MAX_CONNECTION_ATTEMPTS) {
     disableIndicatorLED();
     nextState = PAUSING;
   }
}

void startupSequence() {
  iterateStartupData();
}

void afterStartUpSequence(int passedTime) {
  if(passedTime >= STARTUP_SEQUENCE_TIME) {
    disableGauges();
    nextState = CONNECTING;
  }
}

void runningSequence() {
  if (!isWiFiConnected()) {
    log("Connection lost.");
    nextState = PAUSING;
    return;
  }
  if (shouldSendRequests) {
    sendAllRequests();
  }
  iterateGaugesRequestData();
}

void afterRunningSequence(int passedTime) {
  shouldSendRequests = passedTime >= DATA_REFRESH_RATE;
  if (shouldSendRequests) {
    shouldResetSequenceRunTime = true;
  }
}

void pauseSequence() {
  pulseIndicatorLED();
}


int sequenceTimeCounter = 0;
void (*afterSequenceRoutine)(int) = NULL;
void loop() {
  uint32_t startTime = millis();
  afterSequenceRoutine = NULL;

  switch(appState) {
    case STARTUP:
      startupSequence();
      afterSequenceRoutine = &afterStartUpSequence;
      break;
    case RUNNING:
      runningSequence();
      afterSequenceRoutine = &afterRunningSequence;
      break;
    case CONNECTING:
      connectingSequence();
      afterSequenceRoutine = &afterConnectingSequence;
    case PAUSING:
    default:
      pauseSequence();
      break;
  }

  const int deltaTime = millis() - startTime;
  sequenceTimeCounter += deltaTime;
  if (afterSequenceRoutine != NULL) {
    afterSequenceRoutine(sequenceTimeCounter);
  }
  if (shouldResetSequenceRunTime) {
    sequenceTimeCounter = 0;
    shouldResetSequenceRunTime = false;
  }

  if (appState != nextState) {
    sequenceTimeCounter = 0;
    log("State transition: " + getStateName(appState) + " to " + getStateName(nextState));
  }
  appState = nextState;
}
