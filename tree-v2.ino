/*
   Tree v2: https://github.com/evilgeniuslabs/tree-v2
   Copyright (C) 2016 Jason Coon

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#define FASTLED_ALLOW_INTERRUPTS 0
#define FASTLED_INTERRUPT_RETRY_COUNT 0
#include <FastLED.h>
FASTLED_USING_NAMESPACE

#if defined(ESP8266)
  extern "C" {
  #include "user_interface.h"
  }

  #include <ESP8266WiFi.h>
  #include <ESP8266mDNS.h>
  #include <ESP8266WebServer.h>
  #include <ESP8266HTTPUpdateServer.h>
  #define HOSTNAME "Tree-ESP8266-" ///< Hostname. The setup function adds the Chip ID at the end.
  ESP8266WebServer webServer(80);
  ESP8266HTTPUpdateServer httpUpdateServer;
  #define DATA_PIN 13 //led
  #define RECV_PIN 2  //ir
#elif defined(ESP32)
  #include <WiFi.h>
  #include <ESPmDNS.h>
  #include <WebServer.h>
  #include <HTTPUpdateServer.h>
  #define HOSTNAME "Tree-ESP32-" ///< Hostname. The setup function adds the Chip ID at the end.
  WebServer webServer(80);
  HTTPUpdateServer httpUpdateServer;
  uint32_t chipId = 0;
  #define DATA_PIN 18 //led
  #define RECV_PIN 5  //ir
#endif

String hostname(HOSTNAME);

#define WebSocketsServer_enable
#if defined(WebSocketsServer_enable)
#include <WebSocketsServer.h>
WebSocketsServer webSocketsServer = WebSocketsServer(81);
#endif

#include <LittleFS.h>
#include <EEPROM.h>
#include "GradientPalettes.h"
#include <DebugLog.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

#include "Field.h"

#define IR_enable
#if defined(IR_enable)
  #include <IRremoteESP8266.h>
  #include <IRrecv.h>
  #include <IRutils.h>
  IRrecv irReceiver(RECV_PIN);
  #include "Commands.h"
#endif

#include "wifi_info.h"

#include "FSBrowser.h"

#define LED_TYPE      WS2811
// #define LED_TYPE      WS2812 //for WS2812 strips
#define COLOR_ORDER   RGB
// #define COLOR_ORDER   GRB //for WS2812 strips
#define NUM_LEDS      300

#define MILLI_AMPS         15000     // IMPORTANT: set the max milli-Amps of your power supply (4A = 4000mA)
#define FRAMES_PER_SECOND  60 // here you can control the speed. With the Access Point / Web Server the animations run a bit slower.

#include "Map.h"

CRGB leds[NUM_LEDS];

uint8_t patternIndex = 0;

const uint8_t brightnessCount = 5;
uint8_t brightnessMap[brightnessCount] = { 16, 32, 64, 128, 255 };
uint8_t brightnessIndex = 0;

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

///////////////////////////////////////////////////////////////////////

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPaletteRef gGradientPalettes[];

uint8_t gCurrentPaletteNumber = 0;

CRGBPalette16 gCurrentPalette( CRGB::Black);
CRGBPalette16 gTargetPalette( gGradientPalettes[0] );

CRGBPalette16 IceColors_p = CRGBPalette16(CRGB::Black, CRGB::Blue, CRGB::Aqua, CRGB::White);

int currentPatternIndex = 0; // Index number of which pattern is current
uint8_t autoplay = 0;

uint8_t autoplayDuration = 10;
unsigned long autoPlayTimeout = 0;

uint8_t gHue = 0; // rotating "base color" used by many of the patterns

CRGB solidColor = CRGB::Blue;

// scale the brightness of all pixels down
void dimAll(byte value)
{
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].nscale8(value);
  }
}

typedef void (*Pattern)();
typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

#include "Twinkles.h"
#include "TwinkleFOX.h"
#include "Noise.h"
#include "Meteors.h"

void sendString(String value)
{
  webServer.send(200, "text/plain", value);
}

void sendInt(uint8_t value)
{
  sendString(String(value));
}

void broadcastInt(String name, uint8_t value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":" + String(value) + "}";
#if defined(WebSocketsServer_enable)
  webSocketsServer.broadcastTXT(json);
#endif
}

void broadcastString(String name, String value)
{
  String json = "{\"name\":\"" + name + "\",\"value\":\"" + String(value) + "\"}";
#if defined(WebSocketsServer_enable)
  webSocketsServer.broadcastTXT(json);
#endif
}

#if defined(WebSocketsServer_enable)
void hexDump(const void* mem, uint32_t len, uint8_t cols)
{
  #if defined(ESP8266)
    const char* src = (const char*)mem;
    os_printf("\n[HEXDUMP] Address: %p len: 0x%X (%d)", src, len, len);
    while (len > 0)
    {
        uint32_t linesize = cols > len ? len : cols;
        os_printf("\n[%p] 0x%04x: ", src, (int)(src - (const char*)mem));
        for (uint32_t i = 0; i < linesize; i++)
        {
            os_printf("%02x ", *(src + i));
        }
        os_printf("  ");
        for (uint32_t i = linesize; i < cols; i++)
        {
            os_printf("   ");
        }
        for (uint32_t i = 0; i < linesize; i++)
        {
            unsigned char c = *(src + i);
            os_putc(isprint(c) ? c : '.');
        }
        src += linesize;
        len -= linesize;
        optimistic_yield(10000);
    }
    os_printf("\n");
  #endif
}
void webSocketEvent(uint8_t num, WStype_t type, uint8_t * payload, size_t length) {

  switch (type) {
    case WStype_FRAGMENT_TEXT_START:
    case WStype_FRAGMENT_BIN_START:
    case WStype_FRAGMENT:
    case WStype_FRAGMENT_FIN:
    case WStype_PING:
    case WStype_PONG:
    case WStype_ERROR:
      LOG_ERROR((uint8_t)num, " Error!");
      break;
    
    case WStype_DISCONNECTED:
      LOG_DEBUG((uint8_t)num, " Disconnected!");
      break;

    case WStype_CONNECTED:
      {
        IPAddress ip = webSocketsServer.remoteIP(num);
        LOG_DEBUG((uint8_t)num, "Connected from", ip[0], ip[1], ip[2], ip[3], "url:", (uint8_t)*payload);

        // send message to client
        webSocketsServer.sendTXT(num, "Connected");
      }
      break;

    case WStype_TEXT:
      LOG_DEBUG((uint8_t)num, "get Text:", (uint8_t)*payload);

      // send message to client
      webSocketsServer.sendTXT(num, payload);

      // send data to all connected clients
      webSocketsServer.broadcastTXT(payload);
      break;

    case WStype_BIN:
      LOG_DEBUG((uint8_t)num, "get binary:", (uint8_t)*payload, "length:", (size_t)length);
      hexDump(payload, length, length);

      // send message to client
      webSocketsServer.sendBIN(num, payload, length);
      break;
  }
}
#endif

void setAutoplay(uint8_t value)
{
  autoplay = value == 0 ? 0 : 1;

  LOG_DEBUG(autoplay);

  EEPROM.write(6, autoplay);
  EEPROM.commit();

  broadcastInt("autoplay", autoplay);
}

void setAutoplayDuration(uint8_t value)
{
  autoplayDuration = value;

  LOG_DEBUG(autoplayDuration);

  EEPROM.write(7, autoplayDuration);
  EEPROM.commit();

  autoPlayTimeout = millis() + (autoplayDuration * 1000);

  broadcastInt("autoplayDuration", autoplayDuration);
}

void strandTest()
{
  static uint16_t i = 0;

  EVERY_N_SECONDS(1)
  {
    i++;
    if (i >= NUM_LEDS)
      i = 0;
  }

  fill_solid(leds, NUM_LEDS, CRGB::Black);

  leds[i] = solidColor;
}

void showSolidColor()
{
  fill_solid(leds, NUM_LEDS, solidColor);
}

// mapped rainbow patterns

void northwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(yCoords[i] - beat8(speed), 255, 255);
  }
}

void northeastwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV((xCoords[i] + yCoords[i]) - beat8(speed), 255, 255);
  }
}

void eastwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(xCoords[i] - beat8(speed), 255, 255);
  }
}

void southeastwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV((xCoords[i] - yCoords[i]) + beat8(speed), 255, 255);
  }
}

void southwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(yCoords[i] + beat8(speed), 255, 255);
  }
}

void southwestwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV((xCoords[i] + yCoords[i]) - beat8(speed), 255, 255);
  }
}

void westwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(xCoords[i] + beat8(speed), 255, 255);
  }
}

void northwestwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV((xCoords[i] + yCoords[i]) - beat8(speed), 255, 255);
  }
}

void inwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(radii[i] / 4 + beat8(speed), 255, 255);
  }
}

void outwardRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(radii[i] / 4 - beat8(speed), 255, 255);
  }
}

void rotatingRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(angles[i] + beat8(speed), 255, 255);
  }
}

void risingRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(zCoords[i] - beat8(speed), 255, 255);
  }
}

void fallingRainbow()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = CHSV(zCoords[i] + beat8(speed), 255, 255);
  }
}

// mapped palette patterns

void inwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(gCurrentPalette, radii[i] / 4 + beat8(speed));
  }
}

void outwardPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(gCurrentPalette, radii[i] / 4 - beat8(speed));
  }
}

void rotatingPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(gCurrentPalette, angles[i] + beat8(speed));
  }
}

void risingPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(gCurrentPalette, zCoords[i] / 4 - beat8(speed));
  }
}

void candyCane() {
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(RedWhite_p, zCoords[i] - beat8(speed));
  }
}

void fallingPalette()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++)
  {
    leds[i] = ColorFromPalette(gCurrentPalette, zCoords[i] / 4 + beat8(speed));
  }
}

// Patterns from FastLED example DemoReel100: https://github.com/FastLED/FastLED/blob/master/examples/DemoReel100/DemoReel100.ino

void rainbow()
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 255 / NUM_LEDS);
}

void addGlitter( uint8_t chanceOfGlitter)
{
  if ( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void rainbowWithGlitter()
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void rainbowSolid()
{
  fill_solid(leds, NUM_LEDS, CHSV(gHue, 255, 255));
}

void confetti()
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16(speed, 0, NUM_LEDS);
  static int prevpos = 0;
  if ( pos < prevpos ) {
    fill_solid( leds + pos, (prevpos - pos) + 1, CHSV(gHue, 220, 255));
  } else {
    fill_solid( leds + prevpos, (pos - prevpos) + 1, CHSV( gHue, 220, 255));
  }
  prevpos = pos;
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t beat = beatsin8( speed, 64, 255);
  for ( int i = 0; i < NUM_LEDS; i++) {
    leds[i] = ColorFromPalette(gCurrentPalette, gHue + (i * 2), beat - gHue + (i * 10));
  }
}

void juggle()
{
  static uint8_t    numdots =   4; // Number of dots in use.
  static uint8_t   faderate =   2; // How long should the trails be. Very low value = longer trails.
  static uint8_t     hueinc =  255 / numdots - 1; // Incremental change in hue between each dot.
  static uint8_t    thishue =   0; // Starting hue.
  static uint8_t     curhue =   0; // The current hue
  static uint8_t    thissat = 255; // Saturation of the colour.
  static uint8_t thisbright = 255; // How bright should the LED/display be.
  static uint8_t   basebeat =   5; // Higher = faster movement.

  static uint8_t lastSecond =  99;  // Static variable, means it's only defined once. This is our 'debounce' variable.
  uint8_t secondHand = (millis() / 1000) % 30; // IMPORTANT!!! Change '30' to a different value to change duration of the loop.

  if (lastSecond != secondHand) { // Debounce to make sure we're not repeating an assignment.
    lastSecond = secondHand;
    switch (secondHand) {
      case  0: numdots = 1; basebeat = 20; hueinc = 16; faderate = 2; thishue = 0; break; // You can change values here, one at a time , or altogether.
      case 10: numdots = 4; basebeat = 10; hueinc = 16; faderate = 8; thishue = 128; break;
      case 20: numdots = 8; basebeat =  3; hueinc =  0; faderate = 8; thishue = random8(); break; // Only gets called once, and not continuously for the next several seconds. Therefore, no rainbows.
      case 30: break;
    }
  }

  // Several colored dots, weaving in and out of sync with each other
  curhue = thishue; // Reset the hue values.
  fadeToBlackBy(leds, NUM_LEDS, faderate);
  for ( int i = 0; i < numdots; i++) {
    //beat16 is a FastLED 3.1 function
    leds[beatsin16(basebeat + i + numdots, 0, NUM_LEDS)] += CHSV(gHue + curhue, thissat, thisbright);
    curhue += hueinc;
  }
}

// Pride2015 by Mark Kriegsman: https://gist.github.com/kriegsman/964de772d64c502760e5
// This function draws rainbows with an ever-changing,
// widely-varying set of parameters.
void pride()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < NUM_LEDS; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (NUM_LEDS - 1) - pixelnumber;

    nblend( leds[pixelnumber], newcolor, 64);
  }
}

void pride2()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 1, 3000);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0; i < levelCount; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    CRGB newcolor = CHSV( hue8, sat8, bri8);

    for (uint16_t j = 0; j < NUM_LEDS; j++)
    {
      if (levels[j] == i) {
        nblend(leds[j], newcolor, 64);
      }
    }
  }
}

void radialPaletteShift()
{
  for (uint16_t i = 0; i < NUM_LEDS; i++) {
    // leds[i] = ColorFromPalette( gCurrentPalette, gHue + sin8(i*16), brightness);
    leds[i] = ColorFromPalette(gCurrentPalette, i + gHue, 255, LINEARBLEND);
  }
}

// based on FastLED example Fire2012WithPalette: https://github.com/FastLED/FastLED/blob/master/examples/Fire2012WithPalette/Fire2012WithPalette.ino
void heatMap(CRGBPalette16 palette, bool up)
{
  fill_solid(leds, NUM_LEDS, CRGB::Black);

  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(256));

  // Array of temperature readings at each simulation cell
  static uint16_t heat[NUM_LEDS];

  byte colorindex;

  // Step 1.  Cool down every cell a little
  for ( uint16_t i = 0; i < NUM_LEDS; i++) {
    heat[i] = qsub8( heat[i],  random8(0, ((cooling * 10) / NUM_LEDS) + 2));
  }

  // Step 2.  Heat from each cell drifts 'up' and diffuses a little
  for ( uint16_t k = NUM_LEDS - 1; k >= 2; k--) {
    heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
  }

  // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
  if ( random8() < sparking ) {
    int y = random8(7);
    heat[y] = qadd8( heat[y], random8(160, 255) );
  }

  // Step 4.  Map from heat cells to LED colors
  for ( uint16_t j = 0; j < NUM_LEDS; j++) {
    // Scale the heat value from 0-255 down to 0-240
    // for best results with color palettes.
    colorindex = scale8(heat[j], 190);

    CRGB color = ColorFromPalette(palette, colorindex);

    if (up) {
      leds[j] = color;
    }
    else {
      leds[(NUM_LEDS - 1) - j] = color;
    }
  }
}

void fire()
{
  heatMap(HeatColors_p, true);
}

void water()
{
  heatMap(IceColors_p, false);
}

///////////////////////////////////////////////////////////////////////

// Forward declarations of an array of cpt-city gradient palettes, and
// a count of how many there are.  The actual color palette definitions
// are at the bottom of this file.
extern const TProgmemRGBGradientPaletteRef gGradientPalettes[];

uint8_t beatsaw8( accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                  uint32_t timebase = 0, uint8_t phase_offset = 0)
{
  uint8_t beat = beat8( beats_per_minute, timebase);
  uint8_t beatsaw = beat + phase_offset;
  uint8_t rangewidth = highest - lowest;
  uint8_t scaledbeat = scale8( beatsaw, rangewidth);
  uint8_t result = lowest + scaledbeat;
  return result;
}

// ColorWavesWithPalettes by Mark Kriegsman: https://gist.github.com/kriegsman/8281905786e8b2632aeb
// This function draws color waves with an ever-changing,
// widely-varying set of parameters, using a color palette.
void colorwaves( CRGB* ledarray, uint16_t numleds, CRGBPalette16& palette)
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t i = 0 ; i < numleds; i++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( palette, index, bri8);

    uint16_t pixelnumber = i;
    pixelnumber = (numleds - 1) - pixelnumber;

    nblend( ledarray[pixelnumber], newcolor, 128);
  }
}

void colorWaves()
{
  colorwaves( leds, NUM_LEDS, gCurrentPalette);
}

void colorWaves2()
{
  static uint16_t sPseudotime = 0;
  static uint16_t sLastMillis = 0;
  static uint16_t sHue16 = 0;

  // uint8_t sat8 = beatsin88( 87, 220, 250);
  uint8_t brightdepth = beatsin88( 341, 96, 224);
  uint16_t brightnessthetainc16 = beatsin88( 203, (25 * 256), (40 * 256));
  uint8_t msmultiplier = beatsin88(147, 23, 60);

  uint16_t hue16 = sHue16;//gHue * 256;
  uint16_t hueinc16 = beatsin88(113, 300, 1500);

  uint16_t ms = millis();
  uint16_t deltams = ms - sLastMillis ;
  sLastMillis  = ms;
  sPseudotime += deltams * msmultiplier;
  sHue16 += deltams * beatsin88( 400, 5, 9);
  uint16_t brightnesstheta16 = sPseudotime;

  for ( uint16_t levelIndex = 0 ; levelIndex < levelCount; levelIndex++) {
    hue16 += hueinc16;
    uint8_t hue8 = hue16 / 256;
    uint16_t h16_128 = hue16 >> 7;
    if ( h16_128 & 0x100) {
      hue8 = 255 - (h16_128 >> 1);
    } else {
      hue8 = h16_128 >> 1;
    }

    brightnesstheta16  += brightnessthetainc16;
    uint16_t b16 = sin16( brightnesstheta16  ) + 32768;

    uint16_t bri16 = (uint32_t)((uint32_t)b16 * (uint32_t)b16) / 65536;
    uint8_t bri8 = (uint32_t)(((uint32_t)bri16) * brightdepth) / 65536;
    bri8 += (255 - brightdepth);

    uint8_t index = hue8;
    //index = triwave8( index);
    index = scale8( index, 240);

    CRGB newcolor = ColorFromPalette( gCurrentPalette, index, bri8);

    for (uint16_t i = 0; i < NUM_LEDS; i++) {
      if (levels[i] == levelIndex) {
        uint16_t pixelnumber = (NUM_LEDS - 1) - i;

        nblend( leds[pixelnumber], newcolor, 128);
      }
    }
  }
}

// Alternate rendering function just scrolls the current palette
// across the defined LED strip.
void palettetest( CRGB* ledarray, uint16_t numleds, const CRGBPalette16& gCurrentPalette)
{
  static uint8_t startindex = 0;
  startindex--;
  fill_palette( ledarray, numleds, startindex, (256 / NUM_LEDS) + 1, gCurrentPalette, 255, LINEARBLEND);
}

void paletteWaves() {
  fadeToBlackBy( leds, NUM_LEDS, 20);

  for (uint8_t levelIndex = 0; levelIndex < levelCount; levelIndex++) {
    for (uint16_t i = levelStart[levelIndex]; i < levelEnd[levelIndex]; i++) {
      leds[i] = ColorFromPalette(gCurrentPalette, gHue + levelIndex, beatsin8(speed + levelIndex));
    }
  }
}

// List of patterns to cycle through.  Each is defined as a separate function below.

PatternAndNameList patterns = {
  { whiteMeteors,           "White Meteors" },
  { rainbowMeteors,         "Rainbow Meteors" },
  { paletteMeteors,         "Palette Meteors" },

  { pride,                  "Pride" },
  { pride2,                 "Pride 2" },

  { colorWaves,             "Color Waves" },
  { colorWaves2,            "Color Waves 2" },

  { paletteWaves,           "Palette Waves" },

  { northwardRainbow,       "Northward Rainbow" },
  { northeastwardRainbow,   "Northeastward Rainbow" },
  { eastwardRainbow,        "Eastward Rainbow" },
  { southeastwardRainbow,   "Southeastward Rainbow" },
  { southwardRainbow,       "Southward Rainbow" },
  { southwestwardRainbow,   "Southwestward Rainbow" },
  { westwardRainbow,        "Westward Rainbow" },
  { northwestwardRainbow,   "Northwestward Rainbow" },

  { rotatingRainbow,        "Rotating Rainbow" },
  { outwardRainbow,         "Outward Rainbow" },
  { inwardRainbow,          "Inward Rainbow" },
  { fallingRainbow,         "Falling Rainbow" },
  { risingRainbow,          "Rising Rainbow" },

  { rotatingPalette,        "Rotating Palette" },
  { outwardPalette,         "Outward Palette" },
  { inwardPalette,          "Inward Palette" },
  { fallingPalette,         "Falling Palette" },
  { risingPalette,          "Rising Palette" },

  // twinkle patterns
  { rainbowTwinkles,        "Rainbow Twinkles" },
  { snowTwinkles,           "Snow Twinkles" },
  { cloudTwinkles,          "Cloud Twinkles" },
  { incandescentTwinkles,   "Incandescent Twinkles" },

  // TwinkleFOX patterns
  { retroC9Twinkles,        "Retro C9 Twinkles" },
  { redWhiteTwinkles,       "Red & White Twinkles" },
  { blueWhiteTwinkles,      "Blue & White Twinkles" },
  { redGreenWhiteTwinkles,  "Red, Green & White Twinkles" },
  { fairyLightTwinkles,     "Fairy Light Twinkles" },
  { snow2Twinkles,          "Snow 2 Twinkles" },
  { hollyTwinkles,          "Holly Twinkles" },
  { iceTwinkles,            "Ice Twinkles" },
  { partyTwinkles,          "Party Twinkles" },
  { forestTwinkles,         "Forest Twinkles" },
  { lavaTwinkles,           "Lava Twinkles" },
  { fireTwinkles,           "Fire Twinkles" },
  { cloud2Twinkles,         "Cloud 2 Twinkles" },
  { oceanTwinkles,          "Ocean Twinkles" },
  { gradientPaletteTwinkles,"Palette Twinkles" },

  { candyCane,              "Candy Cane" },

  // noise patterns
  { fireNoise, "Fire Noise" },
  { fireNoise2, "Fire Noise 2" },
  { lavaNoise, "Lava Noise" },
  { rainbowNoise, "Rainbow Noise" },
  { rainbowStripeNoise, "Rainbow Stripe Noise" },
  { partyNoise, "Party Noise" },
  { forestNoise, "Forest Noise" },
  { cloudNoise, "Cloud Noise" },
  { oceanNoise, "Ocean Noise" },
  { blackAndWhiteNoise, "Black & White Noise" },
  { blackAndBlueNoise, "Black & Blue Noise" },

  { rainbow,                "Rainbow" },
  { rainbowWithGlitter,     "Rainbow With Glitter" },
  { rainbowSolid,           "Solid Rainbow" },
  { confetti,               "Confetti" },
  { sinelon,                "Sinelon" },
  { bpm,                    "Beat" },
  { juggle,                 "Juggle" },
  { fire,                   "Fire" },
  { water,                  "Water" },

  //  { draw,                   "Draw" },
  { showSolidColor,         "Solid Color" }
};

const uint8_t patternCount = ARRAY_SIZE(patterns);
extern const uint16_t gGradientPaletteCount;

#include "Fields.h"

void setPower(uint8_t value)
{
  power = value == 0 ? 0 : 1;

  LOG_DEBUG(power);

  EEPROM.write(5, power);
  EEPROM.commit();

  broadcastInt("power", power);
}

void adjustBrightness(bool up)
{
  if (up && brightnessIndex < brightnessCount - 1)
    brightnessIndex++;
  else if (!up && brightnessIndex > 0)
    brightnessIndex--;

  brightness = brightnessMap[brightnessIndex];

  LOG_DEBUG(brightness);

  FastLED.setBrightness(brightness);

  EEPROM.write(0, brightness);
  EEPROM.commit();

  broadcastInt("brightness", brightness);
}

void setBrightness(uint8_t value)
{
  if (value > 255)
    value = 255;
  else if (value < 0) value = 0;

  brightness = value;
  LOG_DEBUG(brightness);

  FastLED.setBrightness(brightness);

  EEPROM.write(0, brightness);
  EEPROM.commit();

  broadcastInt("brightness", brightness);
}

void setPattern(uint8_t value)
{
  if (value >= patternCount)
    value = patternCount - 1;

  LOG_DEBUG(patterns[value].name);

  currentPatternIndex = value;

  if (autoplay == 0) {
    EEPROM.write(1, currentPatternIndex);
    EEPROM.commit();
  }

  broadcastInt("pattern", currentPatternIndex);
}

void setPatternName(String name)
{
  for (uint8_t i = 0; i < patternCount; i++) {
    if (patterns[i].name == name) {
      setPattern(i);
      break;
    }
  }
}

// increase or decrease the current pattern number, and wrap around at the ends
void adjustPattern(bool up)
{
  if (up)
    currentPatternIndex++;
  else
    currentPatternIndex--;

  // wrap around at the ends
  if (currentPatternIndex < 0)
    currentPatternIndex = patternCount - 1;
  if (currentPatternIndex >= patternCount)
    currentPatternIndex = 0;

  LOG_DEBUG(patterns[currentPatternIndex].name);

  if (autoplay == 0) {
    EEPROM.write(1, currentPatternIndex);
    EEPROM.commit();
  }

  broadcastInt("pattern", currentPatternIndex);
}

void adjustSpeed(bool up)
{
  if (up)
    speed++;
  else
    speed--;

  // wrap around at the ends
  if (speed < 0)
    speed = 0;
  if (speed >= 255)
    speed = 255;
  
  LOG_DEBUG(speed);

  EEPROM.write(12, speed);
  EEPROM.commit();

  broadcastInt("speed", speed);
}

void setSolidColor(uint8_t r, uint8_t g, uint8_t b)
{
  solidColor = CRGB(r, g, b);

  LOG_DEBUG("Red:", r, "Green:", g, "Blue:", b);

  EEPROM.write(2, r);
  EEPROM.write(3, g);
  EEPROM.write(4, b);
  EEPROM.commit();

  setPattern(patternCount - 1);

  broadcastString("color", String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
}

void setSolidColor(CRGB color)
{
  setSolidColor(color.r, color.g, color.b);
}

void loadSettings()
{
  brightness = EEPROM.read(0);

  currentPatternIndex = EEPROM.read(1);
  if (currentPatternIndex < 0)
    currentPatternIndex = 0;
  else if (currentPatternIndex >= patternCount)
    currentPatternIndex = patternCount - 1;

  byte r = EEPROM.read(2);
  byte g = EEPROM.read(3);
  byte b = EEPROM.read(4);

  if (r == 0 && g == 0 && b == 0)
  {
  }
  else
  {
    solidColor = CRGB(r, g, b);
  }

  power = EEPROM.read(5);

  autoplay = EEPROM.read(6);
  autoplayDuration = EEPROM.read(7);

//  currentPaletteIndex = EEPROM.read(8);
//  if (currentPaletteIndex < 0)
//    currentPaletteIndex = 0;
//  else if (currentPaletteIndex >= paletteCount)
//    currentPaletteIndex = paletteCount - 1;

  twinkleSpeed = EEPROM.read(9);
  twinkleDensity = EEPROM.read(10);

  coolLikeIncandescent = EEPROM.read(11);

  speed = EEPROM.read(12);
  cooling = EEPROM.read(13);
  sparking = EEPROM.read(14);
}


#if defined(IR_enable)
void handleIrInput()
{
  InputCommand command = readCommand(defaultHoldDelay);

  if (command != InputCommand::None) {
    LOG_DEBUG("command:", (int) command);
  }

  switch (command) {
    case InputCommand::None: {
        break;
      }
    case InputCommand::Palette: {
        setPattern(random(patternCount));
        break;
      }
    case InputCommand::Up: {
        adjustPattern(true);
        break;
      }
    case InputCommand::Down: {
        adjustPattern(false);
        break;
      }
    case InputCommand::Left: {
        adjustSpeed(false);
        break;
      }
    case InputCommand::Right: {
        adjustSpeed(true);
        break;
      }
    case InputCommand::Power: {
        setPower(power == 0 ? 1 : 0);
        break;
      }
    case InputCommand::PowerOn: {
        setPower(1);
        break;
      }
    case InputCommand::PowerOff: {
        setPower(0);
        break;
      }
    case InputCommand::BrightnessUp: {
        adjustBrightness(true);
        break;
      }
    case InputCommand::BrightnessDown: {
        adjustBrightness(false);
        break;
      }
    case InputCommand::Brightness: {
        adjustBrightness(true);
        break;
      }
    case InputCommand::Select: {
        setBrightness(random(255));
        break;
      }
    case InputCommand::PlayMode: {  //toggle pause/play
        setAutoplay(!autoplay);
        break;
      }

     //pattern buttons

    case InputCommand::Pattern1: {
        setPattern(0);
        break;
      }
    case InputCommand::Pattern2: {
        setPattern(1);
        break;
      }
    case InputCommand::Pattern3: {
        setPattern(2);
        break;
      }
    case InputCommand::Pattern4: {
        setPattern(3);
        break;
      }
    case InputCommand::Pattern5: {
        setPattern(4);
        break;
      }
    case InputCommand::Pattern6: {
        setPattern(5);
        break;
      }
    case InputCommand::Pattern7: {
        setPattern(6);
        break;
      }
    case InputCommand::Pattern8: {
        setPattern(7);
        break;
      }
    case InputCommand::Pattern9: {
        setPattern(8);
        break;
      }
    case InputCommand::Pattern10: {
        setPattern(9);
        break;
      }
    case InputCommand::Pattern11: {
        setPattern(10);
        break;
      }
    case InputCommand::Pattern12: {
        setPattern(11);
        break;
      }

    // custom color adjustment buttons

    case InputCommand::RedUp: {
        solidColor.red += 8;
        setSolidColor(solidColor);
        break;
      }
    case InputCommand::RedDown: {
        solidColor.red -= 8;
        setSolidColor(solidColor);
        break;
      }
    case InputCommand::GreenUp: {
        solidColor.green += 8;
        setSolidColor(solidColor);
        break;
      }
    case InputCommand::GreenDown: {
        solidColor.green -= 8;
        setSolidColor(solidColor);
        break;
      }
    case InputCommand::BlueUp: {
        solidColor.blue += 8;
        setSolidColor(solidColor);
        break;
      }
    case InputCommand::BlueDown: {
        solidColor.blue -= 8;
        setSolidColor(solidColor);
        break;
      }

    // color buttons

    case InputCommand::Red: {
        setSolidColor(CRGB::Red);
        break;
      }
    case InputCommand::RedOrange: {
        setSolidColor(CRGB::OrangeRed);
        break;
      }
    case InputCommand::Orange: {
        setSolidColor(CRGB::Orange);
        break;
      }
    case InputCommand::YellowOrange: {
        setSolidColor(CRGB::Goldenrod);
        break;
      }
    case InputCommand::Yellow: {
        setSolidColor(CRGB::Yellow);
        break;
      }

    case InputCommand::Green: {
        setSolidColor(CRGB::Green);
        break;
      }
    case InputCommand::Lime: {
        setSolidColor(CRGB::Lime);
        break;
      }
    case InputCommand::Aqua: {
        setSolidColor(CRGB::Aqua);
        break;
      }
    case InputCommand::Teal: {
        setSolidColor(CRGB::Teal);
        break;
      }
    case InputCommand::Navy: {
        setSolidColor(CRGB::Navy);
        break;
      }

    case InputCommand::Blue: {
        setSolidColor(CRGB::Blue);
        break;
      }
    case InputCommand::RoyalBlue: {
        setSolidColor(CRGB::RoyalBlue);
        break;
      }
    case InputCommand::Purple: {
        setSolidColor(CRGB::Purple);
        break;
      }
    case InputCommand::Indigo: {
        setSolidColor(CRGB::Indigo);
        break;
      }
    case InputCommand::Magenta: {
        setSolidColor(CRGB::Magenta);
        break;
      }

    case InputCommand::White: {
        setSolidColor(CRGB::White);
        break;
      }
    case InputCommand::Pink: {
        setSolidColor(CRGB::Pink);
        break;
      }
    case InputCommand::LightPink: {
        setSolidColor(CRGB::LightPink);
        break;
      }
    case InputCommand::BabyBlue: {
        setSolidColor(CRGB::CornflowerBlue);
        break;
      }
    case InputCommand::LightBlue: {
        setSolidColor(CRGB::LightBlue);
        break;
      }
  }
}
#endif

void setup() {
  //WiFi.setSleepMode(WIFI_NONE_SLEEP);
  LOG_SET_LEVEL(DebugLogLevel::LVL_TRACE);
  
  Serial.begin(115200);
  delay(100);
  Serial.setDebugOutput(true);

  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER>(leds, NUM_LEDS);         // for WS2812 (Neopixel)
  //FastLED.addLeds<LED_TYPE,DATA_PIN,CLK_PIN,COLOR_ORDER>(leds, NUM_LEDS); // for APA102 (Dotstar)
  FastLED.setDither(false);
  FastLED.setCorrection(Typical8mmPixel);
  FastLED.setBrightness(brightness);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, MILLI_AMPS);
  fill_solid(leds, NUM_LEDS, CRGB::Black);
  FastLED.show();

  EEPROM.begin(512);
  loadSettings();

  FastLED.setBrightness(brightness);
  FastLED.show();

#if defined(IR_enable)
  irReceiver.enableIRIn(); // Start the receiver
#endif

#if defined(ESP8266)
  LOG_DEBUG("Heap:",system_get_free_heap_size());
  LOG_DEBUG("Boot Vers:", system_get_boot_version());
  LOG_DEBUG("CPU:", system_get_cpu_freq());
  LOG_DEBUG("SDK:", system_get_sdk_version());
  LOG_DEBUG("Chip ID:", system_get_chip_id());
  LOG_DEBUG("Flash ID:", spi_flash_get_id());
  LOG_DEBUG("Flash Size:", ESP.getFlashChipRealSize());
  LOG_DEBUG("Vcc:", ESP.getVcc());

  hostname += String(ESP.getChipId(), HEX);
#elif defined(ESP32)
  for (int i = 0; i < 17; i = i + 8) {
    chipId |= ((ESP.getEfuseMac() >> (40 - i)) & 0xff) << i;
  }
  hostname += (String)chipId;
  LOG_DEBUG("Heap:", esp_get_free_heap_size());
  LOG_DEBUG("CPU:", ESP.getCpuFreqMHz());
  LOG_DEBUG("SDK:", ESP.getSdkVersion());
  LOG_DEBUG("Chip ID:", chipId);
  //LOG_DEBUG("Flash ID:", ESP_getFlashChipId());
  LOG_DEBUG("Flash Size:", ESP.getFlashChipSize());
  //LOG_DEBUG("Vcc:", ESP.getVcc());
#endif

  bool FS_ERROR = LittleFS.begin();
  if (!FS_ERROR) {
    LOG_ERROR("Failed to mount file system", FS_ERROR);
    LittleFS.format();
  } else {
    LOG_DEBUG("Mount file system", FS_ERROR);
#if defined(ESP8266)
    Dir dir = LittleFS.openDir("/");
    while (dir.next()) {
      String fileName = dir.fileName();
      size_t fileSize = dir.fileSize();
      LOG_DEBUG("FS File:", fileName.c_str(), "size:", String(fileSize).c_str());
    }
#elif defined(ESP32)
    LOG_DEBUG("list /");
    File dir = LittleFS.open("/");
    File file = dir.openNextFile();
    if (!file){
      LOG_DEBUG("root is empty");
    }
    while (file) {
      String fileName = file.name();
      size_t fileSize = file.size();
      LOG_DEBUG("FS File:", fileName.c_str(), "size:", String(fileSize).c_str());
      file = dir.openNextFile();
    }
#endif
    LOG_DEBUG("\n");
  }

  // Set Hostname.
  WiFi.hostname(hostname);

  // Print hostname.
  LOG_DEBUG("Hostname: ", hostname);

  if (apMode)
  {
    WiFi.mode(WIFI_AP);

    // Do a little work to get a unique-ish name. Append the
    // last two bytes of the MAC (HEX'd) to "Thing-":
    //uint8_t mac[WL_MAC_ADDR_LENGTH];
    //WiFi.softAPmacAddress(mac);
    //String macID = String(mac[WL_MAC_ADDR_LENGTH - 2], HEX) +
    //               String(mac[WL_MAC_ADDR_LENGTH - 1], HEX);
    //macID.toUpperCase();
    String AP_NameString = hostname;

    char AP_NameChar[AP_NameString.length() + 1];
    memset(AP_NameChar, 0, AP_NameString.length() + 1);

    for (unsigned int i = 0; i < AP_NameString.length(); i++)
      AP_NameChar[i] = AP_NameString.charAt(i);

    WiFi.softAP(AP_NameChar, WiFiAPPSK);

    LOG_DEBUG("Connect to Wi-Fi access point:", AP_NameString);
    LOG_DEBUG("and open http://192.168.4.1 in your browser");
  }
  else
  {
    WiFi.mode(WIFI_STA);
    LOG_DEBUG("Connecting to", ssid);
    if (String(WiFi.SSID()) != String(ssid)) {
      WiFi.begin(ssid, password);
    }
    LOG_INFO("IP:", WiFi.localIP().toString());
  }

  if (!MDNS.begin(hostname)) {
    LOG_ERROR("Error setting up MDNS responder!");
  }
  else{
    LOG_DEBUG("MDNS responder started");
    MDNS.addService("http", "tcp", 80);
    MDNS.addServiceTxt("http", "tcp", "addresses", WiFi.localIP().toString());
    MDNS.addServiceTxt("http", "tcp", "WebSocketsServerPort", "81");
  }

  httpUpdateServer.setup(&webServer);

  webServer.on("/all", HTTP_GET, []() {
    String json = getFieldsJson(fields, fieldCount);
    webServer.send(200, "text/json", json);
  });

  webServer.on("/fieldValue", HTTP_GET, []() {
    String name = webServer.arg("name");
    String value = getFieldValue(name, fields, fieldCount);
    webServer.send(200, "text/json", value);
  });

  webServer.on("/fieldValue", HTTP_POST, []() {
    String name = webServer.arg("name");
    String value = webServer.arg("value");
    String newValue = setFieldValue(name, value, fields, fieldCount);
    webServer.send(200, "text/json", newValue);
  });

  webServer.on("/power", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPower(value.toInt());
    sendInt(power);
  });

  webServer.on("/cooling", HTTP_POST, []() {
    String value = webServer.arg("value");
    cooling = value.toInt();
    EEPROM.write(13, cooling);
    EEPROM.commit();
    broadcastInt("cooling", cooling);
    sendInt(cooling);
  });

  webServer.on("/sparking", HTTP_POST, []() {
    String value = webServer.arg("value");
    sparking = value.toInt();
    EEPROM.write(14, sparking);
    EEPROM.commit();
    broadcastInt("sparking", sparking);
    sendInt(sparking);
  });

  webServer.on("/speed", HTTP_POST, []() {
    String value = webServer.arg("value");
    speed = value.toInt();
    EEPROM.write(12, speed);
    EEPROM.commit();
    broadcastInt("speed", speed);
    sendInt(speed);
  });

  webServer.on("/twinkleSpeed", HTTP_POST, []() {
    String value = webServer.arg("value");
    twinkleSpeed = value.toInt();
    if (twinkleSpeed < 0) twinkleSpeed = 0;
    else if (twinkleSpeed > 8) twinkleSpeed = 8;
    EEPROM.write(9, twinkleSpeed);
    EEPROM.commit();
    broadcastInt("twinkleSpeed", twinkleSpeed);
    sendInt(twinkleSpeed);
  });

  webServer.on("/twinkleDensity", HTTP_POST, []() {
    String value = webServer.arg("value");
    twinkleDensity = value.toInt();
    if (twinkleDensity < 0) twinkleDensity = 0;
    else if (twinkleDensity > 8) twinkleDensity = 8;
    EEPROM.write(10, twinkleDensity);
    EEPROM.commit();
    broadcastInt("twinkleDensity", twinkleDensity);
    sendInt(twinkleDensity);
  });

  webServer.on("/coolLikeIncandescent", HTTP_POST, []() {
    String value = webServer.arg("value");
    coolLikeIncandescent = value.toInt();
    if (coolLikeIncandescent < 0) coolLikeIncandescent = 0;
    else if (coolLikeIncandescent > 1) coolLikeIncandescent = 1;
    EEPROM.write(11, coolLikeIncandescent);
    EEPROM.commit();
    broadcastInt("coolLikeIncandescent", coolLikeIncandescent);
    sendInt(coolLikeIncandescent);
  });

  webServer.on("/solidColor", HTTP_POST, []() {
    String r = webServer.arg("r");
    String g = webServer.arg("g");
    String b = webServer.arg("b");
    setSolidColor(r.toInt(), g.toInt(), b.toInt());
    sendString(String(solidColor.r) + "," + String(solidColor.g) + "," + String(solidColor.b));
  });

  webServer.on("/pattern", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPattern(value.toInt());
    sendInt(currentPatternIndex);
  });

  webServer.on("/patternName", HTTP_POST, []() {
    String value = webServer.arg("value");
    setPatternName(value);
    sendInt(currentPatternIndex);
  });

  webServer.on("/brightness", HTTP_POST, []() {
    String value = webServer.arg("value");
    setBrightness(value.toInt());
    sendInt(brightness);
  });

  webServer.on("/autoplay", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoplay(value.toInt());
    sendInt(autoplay);
  });

  webServer.on("/autoplayDuration", HTTP_POST, []() {
    String value = webServer.arg("value");
    setAutoplayDuration(value.toInt());
    sendInt(autoplayDuration);
  });

  webServer.on("/FormatFS", HTTP_POST, []() {
    if (LittleFS.format()) {
      LOG_INFO("Format FS Ok");
      webServer.send(200, "text/plain", "Ok");
      ESP.restart();
    } else {
      LOG_INFO("Format FS Not Ok");
      webServer.send(200, "text/plain", "Not Ok");
    }
  });

  //list directory
  webServer.on("/list", HTTP_GET, handleFileList);
  //load editor
  webServer.on("/edit", HTTP_GET, []() {
    if (!handleFileRead("/edit.htm")) webServer.send(404, "text/plain", "FileNotFound");
  });
  //create file
  webServer.on("/edit", HTTP_PUT, handleFileCreate);
  //delete file
  webServer.on("/edit", HTTP_DELETE, handleFileDelete);
  //first callback is called after the request has ended with all parsed arguments
  //second callback handles file uploads at that location
  webServer.on("/edit", HTTP_POST, []() {
    webServer.send(200, "text/plain", "");
  }, handleFileUpload);

  webServer.serveStatic("/", LittleFS, "/", "max-age=86400");

  webServer.begin();
  LOG_DEBUG("HTTP web server started");

#if defined(WebSocketsServer_enable)
  webSocketsServer.begin();
  webSocketsServer.onEvent(webSocketEvent);
  LOG_DEBUG("Web socket server started");
#endif

  autoPlayTimeout = millis() + (autoplayDuration * 1000);
}

void loop() {
  // Add entropy to random number generator; we use a lot of it.
  random16_add_entropy(random(65535));

#if defined(WebSocketsServer_enable)
  webSocketsServer.loop();
#endif

  webServer.handleClient();

#if defined(IR_enable)
  handleIrInput();
#endif

  if (power == 0) {
    fill_solid(leds, NUM_LEDS, CRGB::Black);
    FastLED.show();
    // FastLED.delay(15);
    return;
  }

  //   EVERY_N_SECONDS(10) {
  //     LOG_DEBUG("Heap:", system_get_free_heap_size());
  //   }

  // change to a new cpt-city gradient palette
  EVERY_N_SECONDS( secondsPerPalette ) {
    gCurrentPaletteNumber = addmod8( gCurrentPaletteNumber, 1, gGradientPaletteCount);
    gTargetPalette = gGradientPalettes[ gCurrentPaletteNumber ];

    //    paletteIndex = addmod8( paletteIndex, 1, paletteCount);
    //    targetPalette = palettes[paletteIndex];
  }

  EVERY_N_MILLISECONDS(40) {
    // slowly blend the current palette to the next
    nblendPaletteTowardPalette( gCurrentPalette, gTargetPalette, 8);
    //    nblendPaletteTowardPalette(currentPalette, targetPalette, 16);
    gHue++;  // slowly cycle the "base color" through the rainbow
  }

  if (autoplay && (millis() > autoPlayTimeout)) {
    adjustPattern(true);
    autoPlayTimeout = millis() + (autoplayDuration * 1000);
  }

  // Call the current pattern function once, updating the 'leds' array
  patterns[currentPatternIndex].pattern();

  FastLED.show();

  delay(1000 / FRAMES_PER_SECOND);

  // insert a delay to keep the framerate modest
 //FastLED.delay(1000 / FRAMES_PER_SECOND);
}
