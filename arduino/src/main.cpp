#include <avr/sleep.h>
#include <Arduino.h>
#include <SoftwareSerial.h>
#include "config.h"
#include "states.h"
#include "helpers.h"

ApplicationState appState;
SoftwareSerial ESPSerial(WIFI_RX_PIN, WIFI_TX_PIN);

void setupWifi() {
  Serial.println("Setting up WiFi...");
  ESPSerial.begin(9600);
  ESPSerial.println("AT+CWSAP="+WIFI_SSID+","+WIFI_PASSWORD);
  /*
  ESPSerial.println("AT+CWMODE=1");
  ESPSerial.println("AT+CWJAP=\""+WIFI_NAME+"\",\""+WIFI_PASSWORD+"\"");
  */
}

void changeState(ApplicationState state) {
  Serial.println("State from "+ getStateName(appState) + " to " + getStateName(state));
  appState = state;
}

void setup() {
  Serial.begin(9600);
  Serial.println("Starting up...");
  setupWifi();

  pinMode(LATCH_PIN, OUTPUT);
  pinMode(DATA_PIN, OUTPUT);
  pinMode(CLOCK_PIN, OUTPUT);
  for(int i = 0; i < NUM_GAUGES; i++) {
    pinMode(GAUGE_PINS[i], OUTPUT);
    digitalWrite(GAUGE_PINS[i], LOW);
  }
  pinMode(INDICATOR_LED_PIN, OUTPUT);

  changeState(RUNNING);
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

int sequenceTimeCounter = 0;
void startupSequence() {
  uint32_t startTime = millis();
  iterateGauges();
  const int deltaTime = millis() - startTime;
  sequenceTimeCounter += deltaTime;

  if(sequenceTimeCounter >= STARTUP_SEQUENCE_TIME) {
    disableGauges();
    changeState(RUNNING);
  }
}

void runningSequence() {
  if(Serial.available()) {
    byte data = Serial.read();
    Serial.println("New data: " + (char)data);
    ESPSerial.write(data);
  }
  if(ESPSerial.available()) {
    Serial.write(ESPSerial.read());
  }
}

void pauseSequence() {
  pulseIndicatorLED();
}

void sleepSequence() {
  Serial.println("Shutting down...");
  delay(500);
  set_sleep_mode(SLEEP_MODE_PWR_DOWN);
  sleep_enable();
  sleep_mode();
}

void unknownSequence() {
  changeState(RUNNING);
  Serial.println("Unhandled sequence");
}

void loop() {
  switch(appState) {
    case STARTUP:
      startupSequence();
      break;
    case RUNNING:
      runningSequence();
      break;
    case PAUSING:
      pauseSequence();
      break;
    case SHUTDOWN:
      sleepSequence();
      break;
    default:
      unknownSequence();
      break;
  }
}
