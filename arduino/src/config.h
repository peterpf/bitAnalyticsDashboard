// WiFi configuration
const int WIFI_RX_PIN             = 11;
const int WIFI_TX_PIN             = 10;
const String WIFI_SSID            = "bitAnalyticsDashboard";
const String WIFI_PASSWORD        = "4321";

// Gauge configuration
const int CLOCK_PIN               = 6;
const int LATCH_PIN               = 7;
const int DATA_PIN                = 8;
const int NUM_GAUGES              = 4;
const int GAUGE_SWITCHING_DELAY   = 2; // ms
const int GAUGE_PATTERN_VIEWTIME  = 140; // ms
const int GAUGE_PINS[NUM_GAUGES]  = {2, 3, 5, 4};
const byte GAUGE_EMPTY_DATA       = 0x00;

// Indicator LED configuration
const int INDICATOR_LED_PIN           = 9;
const int INDICATOR_LED_MIN_VALUE     = 10;
const int INDICATOR_LED_MAX_VALUE     = 255*0.6;
const int INDICATOR_LED_PULSE_SPEED   = 10; // ms

// Startup Sequence
const int STARTUP_SEQUENCE_TIME = 5000; // ms
const int STARTUP_SEQUENCE_GAUGE_DATA_SIZE = 4;
byte STARTUP_SEQUENCE_GAUGE_DATA[STARTUP_SEQUENCE_GAUGE_DATA_SIZE][NUM_GAUGES] = {
  {0x11, 0x11, 0x11, 0x11},
  {0x33, 0x33, 0x33, 0x33},
  {0x77, 0x77, 0x77, 0x77},
  {0xFF, 0xFF, 0xFF, 0xFF}
};
