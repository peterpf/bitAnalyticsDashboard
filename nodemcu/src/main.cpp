#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ArduinoJson.h>

#include <config.h>
#include <state.h>
#include <hardwareInterface.h>
#include <requestTypes.h>
#include <requestHelper.h>
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

/**
 * ###############
 * ## SEQUENCES ##
 * ###############
 */

int sequenceTimeCounter = 0;
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

void afterConnectingSequence(int deltaTime) {
   if (isWiFiConnected()) {
     log("Connected with IP: " + WiFi.localIP().toString());
     disableIndicatorLED();
   } else if (didWiFiConnectionFail()) {
     log("Failed to connect...");
     numConnectionAttempts++;
   }

   if (isWiFiConnected() && sequenceTimeCounter >= SEQUENCE_CONNECTING_MIN_TIME) {
     nextState = RUNNING;
   }

   if (numConnectionAttempts >= MAX_CONNECTION_ATTEMPTS) {
     disableIndicatorLED();
     nextState = PAUSING;
   }
}

int startupSequenceTimeCounter = 0;
int startupSequenceGaugeDataIndex = 0;
int startupSequenceDataIndexCountDirection = 1;

void startupSequence() {
  iterateGauges(SEQUENCE_STARTUP_GAUGE_DATA[startupSequenceGaugeDataIndex]);
}

void afterStartUpSequence(int deltaTime) {
  if(sequenceTimeCounter >= SEQUENCE_STARTUP_TIME) {
    disableGauges();
    nextState = CONNECTING;
    return;
  }

  startupSequenceTimeCounter += deltaTime;

  if (startupSequenceTimeCounter >= GAUGE_PATTERN_VIEWTIME) {
    startupSequenceTimeCounter = 0;
    startupSequenceGaugeDataIndex += startupSequenceDataIndexCountDirection;

    if (startupSequenceGaugeDataIndex == 0 || startupSequenceGaugeDataIndex == SEQUENCE_STARTUP_GAUGE_DATA_SIZE-1) {
      startupSequenceDataIndexCountDirection *= -1;
    }
  }
}

void runningSequence() {
  if (!isWiFiConnected()) {
    log("Connection lost.");
    shouldSendRequests = false;
    nextState = PAUSING;
    return;
  }
  if (shouldSendRequests) {
    sendAllRequests();
  }
  iterateGauges(gaugeDataFromRequests);
}

void afterRunningSequence(int deltaTime) {
  shouldSendRequests = sequenceTimeCounter >= DATA_REFRESH_RATE;
  if (shouldSendRequests) {
    shouldResetSequenceRunTime = true;
  }
}

void pauseSequence() {
  pulseIndicatorLED();
}


/**
 * ###############
 * ## MAIN LOOP ##
 * ###############
 */

void (*postSequenceFunction)(int) = NULL;
void loop() {
  uint32_t startTime = millis();
  postSequenceFunction = NULL;

  switch(appState) {
    case STARTUP:
      startupSequence();
      postSequenceFunction = &afterStartUpSequence;
      break;
    case RUNNING:
      runningSequence();
      postSequenceFunction = &afterRunningSequence;
      break;
    case CONNECTING:
      connectingSequence();
      postSequenceFunction = &afterConnectingSequence;
    case PAUSING:
    default:
      pauseSequence();
      break;
  }

  const int deltaTime = millis() - startTime;
  sequenceTimeCounter += deltaTime;
  if (postSequenceFunction != NULL) {
    postSequenceFunction(deltaTime);
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
