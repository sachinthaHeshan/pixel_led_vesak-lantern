#include <FastLED.h>

// --- LED Configuration ---
#define LED_PIN     2
#define NUM_LEDS    50
#define BRIGHTNESS  100
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB

CRGB leds[NUM_LEDS];

// --- Segment Boundaries ---
#define TAIL_START        0
#define TAIL_END          4
#define BACKGROUND_START  5
#define BACKGROUND_END   13
#define SECTION1_START   14
#define SECTION1_END     22
#define SECTION2_START   23
#define SECTION2_END     31
#define SECTION3_START   32
#define SECTION3_END     40
#define SECTION4_START   41
#define SECTION4_END     49

// --- Color Sequences ---
CRGB colorSequence[] = { CRGB::Blue, CRGB::Green, CRGB::Red, CRGB::White, CRGB::Yellow };
CRGB secondaryColorSequence[] = { CRGB::Yellow, CRGB::Purple, CRGB::Pink, CRGB::Cyan, CRGB::Teal };
const int numColors = sizeof(colorSequence) / sizeof(CRGB);
const int numSecondaryColors = sizeof(secondaryColorSequence) / sizeof(CRGB);

// --- Pattern Control ---
unsigned long patternStartTime = 0;
const unsigned long patternDuration = 20000;  // 20 seconds
int currentPattern = 0;
const int numPatterns = 5;

// === Setup ===
void setup() {
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
}

// === Main Loop ===
void loop() {
  if (millis() - patternStartTime >= patternDuration) {
    patternStartTime = millis();
    currentPattern = (currentPattern + 1) % numPatterns;
  }

  switch (currentPattern) {
    case 0: pattern1(); break;
    case 1: pattern2(); break;
    case 2: pattern3(); break;
    case 3: pattern4(); break;
    case 4: pattern5(); break;
  }
}

// === Pattern Implementations ===

void pattern1() {
  static uint32_t startTime = millis();
  float progress = (millis() - startTime) / 3000.0;
  if (progress >= 1.0) {
    startTime = millis();
    progress = 0;
  }

  uint8_t hue = progress * 255;
  uint8_t brightness = 255 * (1.0 - progress);

  showTail(hue, brightness);
  showBackground(hue, brightness);

  static uint8_t sectionHue = 0;
  fillSectionsWithHue(sectionHue++);

  FastLED.show();
  delay(30);
}

void pattern2() {
  static uint32_t fadeStart = millis();
  float fadeProgress = (millis() - fadeStart) / 3000.0;
  if (fadeProgress >= 1.0) {
    fadeStart = millis();
    fadeProgress = 0;
  }

  uint8_t hue = fadeProgress * 255;
  uint8_t brightness = 255 * (1.0 - fadeProgress);

  showTail(hue, brightness);
  showBackground(hue, brightness);

  static uint8_t baseHue = 0;
  baseHue++;

  fillGradientSection(SECTION1_START, SECTION1_END, baseHue);
  fillGradientSection(SECTION3_START, SECTION3_END, baseHue);
  fillGradientSection(SECTION2_START, SECTION2_END, baseHue + 64);
  fillGradientSection(SECTION4_START, SECTION4_END, baseHue + 64);

  FastLED.show();
  delay(30);
}

void pattern3() {
  static uint32_t colorChangeStart = millis();
  static uint8_t colorIndex = 0;
  if (millis() - colorChangeStart >= 1000) {
    colorChangeStart = millis();
    colorIndex = (colorIndex + 1) % numColors;
  }

  CRGB color = colorSequence[colorIndex];

  fadeTailAndBackground();

  setSectionColor(SECTION1_START, SECTION1_END, color);
  setSectionColor(SECTION2_START, SECTION2_END, color);
  setSectionColor(SECTION3_START, SECTION3_END, color);
  setSectionColor(SECTION4_START, SECTION4_END, color);

  FastLED.show();
}

void pattern4() {
  static uint32_t colorChangeStart = millis();
  static uint8_t colorIndex = 0;
  if (millis() - colorChangeStart >= 1000) {
    colorChangeStart = millis();
    colorIndex = (colorIndex + 1) % numColors;
  }

  CRGB color = colorSequence[colorIndex];

  fadeTailAndBackground();

  setSectionWithGradientMiddleColor(SECTION1_START, SECTION1_END, color);
  setSectionWithGradientMiddleColor(SECTION2_START, SECTION2_END, color);
  setSectionWithGradientMiddleColor(SECTION3_START, SECTION3_END, color);
  setSectionWithGradientMiddleColor(SECTION4_START, SECTION4_END, color);

  FastLED.show();
}

void pattern5() {
  static uint32_t colorChangeStart = millis();
  static uint8_t index1 = 0, index2 = 0;

  if (millis() - colorChangeStart >= 1000) {
    colorChangeStart = millis();
    index1 = (index1 + 1) % numColors;
    index2 = (index2 + 1) % numSecondaryColors;
  }

  CRGB primary = colorSequence[index1];
  CRGB secondary = secondaryColorSequence[index2];

  fadeTailAndBackground();

  setAlternateLEDsWithDualColor(SECTION1_START, SECTION1_END, primary, secondary);
  setAlternateLEDsWithDualColor(SECTION3_START, SECTION3_END, primary, secondary);
  setAlternateLEDsWithDualColor(SECTION2_START, SECTION2_END, secondary, primary);
  setAlternateLEDsWithDualColor(SECTION4_START, SECTION4_END, secondary, primary);

  FastLED.show();
}

// === Effects & Helpers ===

void showTail(uint8_t hue, uint8_t brightness) {
  fillHSVRange(TAIL_START, TAIL_END, hue, brightness);
}

void showBackground(uint8_t hue, uint8_t brightness) {
  fillHSVRange(BACKGROUND_START, BACKGROUND_END, hue, brightness);
}

void fadeTailAndBackground() {
  static uint32_t fadeStart = millis();
  float fadeProgress = (millis() - fadeStart) / 3000.0;
  if (fadeProgress >= 1.0) {
    fadeStart = millis();
    fadeProgress = 0;
  }

  uint8_t hue = fadeProgress * 255;
  uint8_t brightness = 255 * (1.0 - fadeProgress);

  showTail(hue, brightness);
  showBackground(hue, brightness);
}

void fillHSVRange(int start, int end, uint8_t hue, uint8_t brightness) {
  for (int i = start; i <= end; i++) {
    leds[i] = CHSV(hue, 255, brightness);
  }
}

void fillSectionsWithHue(uint8_t hue) {
  for (int i = SECTION1_START; i <= SECTION4_END; i++) {
    leds[i] = CHSV(hue, 255, 255);
  }
}

void setSectionColor(int start, int end, CRGB color) {
  for (int i = start; i <= end; i++) {
    leds[i] = color;
  }
}

void setAlternateLEDsWithDualColor(int start, int end, CRGB c1, CRGB c2) {
  for (int i = start; i <= end; i++) {
    leds[i] = (i % 2 == 0) ? c1 : c2;
  }
}

void setSectionWithGradientMiddleColor(int start, int end, CRGB color) {
  int mid = (start + end) / 2;
  uint8_t gradientHue = (millis() / 10) % 255;
  CRGB gradientColor = CHSV(gradientHue, 255, 255);

  for (int i = start; i <= end; i++) {
    leds[i] = (i == mid) ? gradientColor : color;
  }
}

void fillGradientSection(int start, int end, uint8_t baseHue) {
  int length = end - start + 1;
  for (int i = 0; i < length; i++) {
    leds[start + i] = CHSV(baseHue + (i * 255 / length), 255, 255);
  }
}
