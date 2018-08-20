// WiFi configuration
const char WIFI_SSID[]                      = "SSID";
const char WIFI_PASSWORD[]                  = "PASSWORD";
const int MAX_CONNECTION_ATTEMPTS           = 3;

// Gauge configuration
const int NUM_GAUGES                        = 4;
const int NUM_GAUGE_SUBSECTIONS             = 2;
const int GAUGE_PINS[NUM_GAUGES]            = {D0, D1, D2, D5}; // active-high
const int CLOCK_PIN                         = D6;
const int LATCH_PIN                         = D7;
const int DATA_PIN                          = D8;
const int GAUGE_SWITCHING_DELAY             = 2; // ms
const int GAUGE_PATTERN_VIEWTIME            = 140; // ms
const byte GAUGE_EMPTY_DATA                 = 0x00;

// Indicator LED configuration
const int INDICATOR_LED_PIN                 = D4; // active-high
const int INDICATOR_LED_MIN_VALUE           = 10;
const int INDICATOR_LED_MAX_VALUE           = 255*0.6;
const int INDICATOR_LED_PULSE_SPEED         = 20; // ms

// Startup Sequence
const int STARTUP_SEQUENCE_TIME             = 5000; // ms
const int STARTUP_SEQUENCE_GAUGE_DATA_SIZE  = 4;
byte STARTUP_SEQUENCE_GAUGE_DATA[STARTUP_SEQUENCE_GAUGE_DATA_SIZE][NUM_GAUGES] = {
  {0x11, 0x11, 0x11, 0x11},
  {0x33, 0x33, 0x33, 0x33},
  {0x77, 0x77, 0x77, 0x77},
  {0xFF, 0xFF, 0xFF, 0xFF}
};

// Data retrieval
const int URL_INSTANCES_NUM_VALUES = 2;
const String BASE_URL                       = "SOME_ADDRESS";
const String URL_INSTANCES                  = BASE_URL+"/query?query=count(up{job=\"analytics\"} == 1) by (continent)";
const String URL_REQUESTS_PER_MINUTE        = BASE_URL+"/query=sum(rate(requests_total[1m])) by (continent)";
const String URL_CPU_USAGE_EU               = BASE_URL+"/query?query=avg(node_load1{name=~\".*ingress.*\", continent=\"europe\"})";
const String URL_CPU_USAGE_US               = BASE_URL+"/query?query=avg(node_load1{name=~\".*ingress.*\", continent=\"us\"})";
const int DATA_REFRESH_RATE                 = 10000; // ms
