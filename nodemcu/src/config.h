// WiFi configuration
const char WIFI_SSID[]                = "SSID";
const char WIFI_PASSWORD[]            = "PASSWORD";

// Gauge configuration
const int NUM_GAUGES                  = 4;
const int GAUGE_PINS[NUM_GAUGES]      = {D1, D2, D3, D5};
const int CLOCK_PIN                   = D6;
const int LATCH_PIN                   = D7;
const int DATA_PIN                    = D8;
const int GAUGE_SWITCHING_DELAY       = 2; // ms
const int GAUGE_PATTERN_VIEWTIME      = 140; // ms
const byte GAUGE_EMPTY_DATA           = 0x00;

// Indicator LED configuration
const int INDICATOR_LED_PIN           = D0;
const int INDICATOR_LED_MIN_VALUE     = 10;
const int INDICATOR_LED_MAX_VALUE     = 255*0.6;
const int INDICATOR_LED_PULSE_SPEED   = 10; // ms

// Startup Sequence
const int STARTUP_SEQUENCE_TIME             = 10000; // ms
const int STARTUP_SEQUENCE_GAUGE_DATA_SIZE  = 4;
byte STARTUP_SEQUENCE_GAUGE_DATA[STARTUP_SEQUENCE_GAUGE_DATA_SIZE][NUM_GAUGES] = {
  {0x11, 0x11, 0x11, 0x11},
  {0x33, 0x33, 0x33, 0x33},
  {0x77, 0x77, 0x77, 0x77},
  {0xFF, 0xFF, 0xFF, 0xFF}
};

// Data retrieval
const char DATA_HTTP_REQUEST[]        = "http://SOME_ADDRESS";
const int DATA_REFRESH_RATE           = 5000; // ms
