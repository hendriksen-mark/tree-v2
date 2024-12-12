const bool apMode = false;

// AP mode password
const char WiFiAPPSK[] = "";

// Wi-Fi network to connect to (if not in AP mode)
const char* ssid = "";
const char* password = "";

#define LED_TYPE      WS2811
// #define LED_TYPE      WS2812 //for WS2812 strips
#define COLOR_ORDER   RGB
// #define COLOR_ORDER   GRB //for WS2812 strips
#define NUM_LEDS      300

#define MILLI_AMPS         15000     // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define FRAMES_PER_SECOND  60 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.

#if defined(ESP8266)
  #define DATA_PIN 13 // pin for led strip
  #define RECV_PIN 2  // pin for ir rec
#elif defined(ESP32)
  #define DATA_PIN 18 //  pin for led strip
  #define RECV_PIN 5  // pin for ir rec
#endif

// uncomment to enable WebSocketsServer
#define WebSocketsServer_enable

// uncomment to enable IRremote
#define IR_enable

// ten seconds per color palette makes a good demo
// 20-120 is better for deployment
uint8_t secondsPerPalette = 10;

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100
uint8_t cooling = 49;

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
uint8_t sparking = 60;

uint8_t speed = 30;
