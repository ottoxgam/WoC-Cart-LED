#include <FastLED.h>
#include <EasyButton.h>
#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <ESP8266WebServer.h>
#include <Arduino.h>

ESP8266WebServer server(80);


FASTLED_USING_NAMESPACE

#define DATA_PIN    3
#define LED_TYPE    WS2811
#define COLOR_ORDER BRG
#define NUM_LEDS    38
CRGB leds[NUM_LEDS];

#define BRIGHTNESS         255
#define FRAMES_PER_SECOND  100
#define BUTTON_PIN D8
#define OFF_PIN D7

EasyButton button(BUTTON_PIN);
EasyButton offbutton(OFF_PIN);

CRGBPalette16 currentPalette;
TBlendType    currentBlending;
extern CRGBPalette16 myRedWhiteBluePalette;
extern const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM;
static uint8_t startIndex = 0;

#ifndef STASSID
#define STASSID "PixelESP8226"
#define STAPSK "Cdr3T1Lee!"
#endif

IPAddress apIP(192, 168, 1, 1);
const char* ssid = STASSID;
const char* password = STAPSK;

void setup() {
  Serial.begin(115200);
  Serial.println("Booting");
    //WiFi.mode(WIFI_AP);
    //WiFi.softAPConfig(apIP, apIP, IPAddress(255, 255, 255, 0));
    //WiFi.softAP(ssid, password);

    //MDNS.begin("esp8266", WiFi.softAPIP());
    //Serial.println("Ready");
    //Serial.print("IP address: ");
    //Serial.println(WiFi.softAPIP());

    //ArduinoOTA.begin();
    //server.on("/",[](){
    //  delay(100);
    //  Serial.println("Hello!");
    //  server.send(200,"text/plain", "Hello!!");
    //});
    //server.begin();

  //ArduinoOTA.begin();
  
  delay(1000); // 3 second delay for recovery
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  FastLED.setBrightness(BRIGHTNESS);
      button.begin();
      offbutton.begin();
    button.onPressed(APressed);
    offbutton.onPressed(BPressed);
    currentPalette = RainbowColors_p;

}

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { rainbow, rainbowWithGlitter, confetti, cloudy, sinelon, juggle, bpm, purplegreen, rwb, christmas, allblack};

uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
uint8_t gHue = 0; // rotating "base color" used by many of the patterns
  
void loop()
{
  //ArduinoOTA.handle();
  //server.handleClient();
  gPatterns[gCurrentPatternNumber]();
  FastLED.show();  
  FastLED.delay(1000/FRAMES_PER_SECOND); 
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
      button.read();
}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void APressed() {
    nextPattern();
    Serial.println("Next Pattern");

}

void BPressed() {
   allblack();
   Serial.println("To Black");

}
void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(90);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
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
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}
void FillRWB(uint8_t colorIndex)
{
    uint8_t brightness = 255;
    CRGBPalette16 palette = myRedWhiteBluePalette_p;
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( palette, colorIndex, brightness, LINEARBLEND);
        colorIndex += 2;
    }
}

void rwb()
{
    uint8_t brightness = 255;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    FillRWB( startIndex);
   
}
  
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Red, // 'white' is too bright compared to red and blue
    CRGB::Red,
    CRGB::Red,
    
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    
    CRGB::Blue,
    CRGB::Red,
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue
};

void FillFromPalette(uint8_t colorIndex)
{
    uint8_t brightness = 255;
    CRGBPalette16 palette = currentPalette;
    for( int i = 0; i < NUM_LEDS; i++) {
        leds[i] = ColorFromPalette( palette, colorIndex, brightness, currentBlending);
        colorIndex += 2;
    }
}

void purplegreen()
{
    uint8_t brightness = 255;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 2;
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    currentBlending = LINEARBLEND;
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
    FillFromPalette( startIndex);
   
}
void christmas()
{
    uint8_t brightness = 255;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    CRGB red = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    currentBlending = NOBLEND;
    currentPalette = CRGBPalette16(
                                   green,  green,  green,  green,
                                   red, red, red,  red,
                                   red,  red,  red,  red,
                                   green, green, green,  green );
    FillFromPalette( startIndex);
   
}
void cloudy()
{
    uint8_t brightness = 255;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    currentPalette = CloudColors_p;           
    currentBlending = LINEARBLEND;
    FillFromPalette( startIndex);
}
void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

void allblack()
{
    uint8_t brightness = 255;
    static uint8_t startIndex = 0;
    startIndex = startIndex + 1;
    CRGB red = CHSV( HUE_RED, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    currentBlending = NOBLEND;
    currentPalette = CRGBPalette16(
                                   black,  black,  black,  black );
    FillFromPalette( startIndex);
   
}
