// Configuration: debugging
const bool LOGGING_ENABLED                  = false;

// Configuration: WiFi
const char WIFI_SSID[]                      = "SSID";
const char WIFI_PASSWORD[]                  = "PASSWORD";
const int MAX_CONNECTION_ATTEMPTS           = 3;

// Configuration: gauges
const int NUM_GAUGES                        = 4;
const int NUM_GAUGE_SUBSECTIONS             = 2;
const int GAUGE_PINS[NUM_GAUGES]            = {D0, D2, D1, D5}; // active-high
const int CLOCK_PIN                         = D6;
const int LATCH_PIN                         = D7;
const int DATA_PIN                          = D8;
const int GAUGE_SWITCHING_DELAY             = 2; // ms
const int GAUGE_PATTERN_VIEWTIME            = 140; // ms
const byte GAUGE_EMPTY_DATA                 = 0x00;

// Configuration: Indicator LED
const int INDICATOR_LED_PIN                 = D4; // active-high
const int INDICATOR_LED_MIN_VALUE           = 10;
const int INDICATOR_LED_MAX_VALUE           = 255*0.6;
const int INDICATOR_LED_PULSE_SPEED         = 20; // ms

// Sequence: startup
const int SEQUENCE_STARTUP_TIME             = 10000; // ms
const int SEQUENCE_STARTUP_GAUGE_DATA_SIZE  = 4;
byte SEQUENCE_STARTUP_GAUGE_DATA[SEQUENCE_STARTUP_GAUGE_DATA_SIZE][NUM_GAUGES] = {
  {0x11, 0x11, 0xFF, 0xFF},
  {0x33, 0x33, 0x77, 0x77},
  {0x77, 0x77, 0x33, 0x33},
  {0xFF, 0xFF, 0x11, 0x11}
};

// Sequence: connecting
const int SEQUENCE_CONNECTING_MIN_TIME      = 10000; // ms

//Configuration: data retrieval
const int URL_INSTANCES_NUM_VALUES          = 2;
const String BASE_URL                       = "http://SOME_ADDRESS/query?query=";
const String URL_INSTANCES                  = BASE_URL+"count(up{job=\"analytics\"}%20==%201)%20by%20(continent)";
const String URL_REQUESTS_PER_MINUTE        = BASE_URL+"sum(rate(requests_total[1m]))%20by%20(continent)";
const String URL_CPU_USAGE_EU               = BASE_URL+"avg(node_load1{name=~\".*ingress.*\",%20continent=\"europe\"})";
const String URL_CPU_USAGE_US               = BASE_URL+"avg(node_load1{name=~\".*ingress.*\",%20continent=\"us\"})";
const int DATA_REFRESH_RATE                 = 10000; // ms

const int NUM_MAX_INSTANCES                 = 8;
const int NUM_MAX_REQUESTS                  = 200; // per minute
const int NUM_MAX_CPU_USAGE                 = 100;
