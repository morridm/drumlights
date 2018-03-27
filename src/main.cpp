/* David Morris 2018
  Thanks to:
      https://www.adafruit.com
      http://cassettepunk.com
      http://fastled.io
      https://beammyselfintothefuture.wordpress.com/
      http://moving-rainbow.readthedocs.io/en/latest/guide/color-wheel/
      https://www.instructables.com/id/Light-Up-Drum-Kit/
  for all the great tutorials.

  Note: If you are looking for the code I used for my Trinket 5v original see
  "https://github.com/morridm/drumlights/tree/master/archive/Trinket".
  This code assumes you are using an Adafruit Trinket Pro 5v not the regular
  Trinket 5v.
*/

#include <Arduino.h>
#include <Button.h>
#include <ButtonStates.h>
#include <PixelDrumPatterns.h>

static const int LED_STRIP_PIN_NUMBER = 4;

static const int PIEZO_ANALOG_INPUT_PIN = 0;

static const int PIEZO_DIGITAL_INPUT_PIN = 14;

static const int POTENTIOMETER_PIN = A1;

static const int PUSH_BUTTON_PIN = 12;

static const int REPEAT_FIRST = 500;

static const int REPEAT_INCR = 100;

unsigned long rpt = REPEAT_FIRST;

Button btnUP(PUSH_BUTTON_PIN, true, true, 20);

static const struct DrumComponent SNARE = {0,
                                           65,
                                           7,
                                           0,
                                           LED_STRIP_PIN_NUMBER,
                                           PIEZO_ANALOG_INPUT_PIN,
                                           PIEZO_DIGITAL_INPUT_PIN,
                                           POTENTIOMETER_PIN,
                                           PUSH_BUTTON_PIN,
                                           NEO_GRB + NEO_KHZ800};

static const struct DrumComponent HI_TOM = {1,
                                            50,
                                            7,
                                            0,
                                            LED_STRIP_PIN_NUMBER,
                                            PIEZO_ANALOG_INPUT_PIN,
                                            PIEZO_DIGITAL_INPUT_PIN,
                                            POTENTIOMETER_PIN,
                                            PUSH_BUTTON_PIN,
                                            NEO_GRB + NEO_KHZ800};

static const struct DrumComponent MIDDLE_TOM = {2,
                                                65,
                                                7,
                                                0,
                                                LED_STRIP_PIN_NUMBER,
                                                PIEZO_ANALOG_INPUT_PIN,
                                                PIEZO_DIGITAL_INPUT_PIN,
                                                POTENTIOMETER_PIN,
                                                PUSH_BUTTON_PIN,
                                                NEO_GRB + NEO_KHZ800};

static const struct DrumComponent FLOOR_TOM = {3,
                                               81,
                                               7,
                                               0,
                                               LED_STRIP_PIN_NUMBER,
                                               PIEZO_ANALOG_INPUT_PIN,
                                               PIEZO_DIGITAL_INPUT_PIN,
                                               POTENTIOMETER_PIN,
                                               PUSH_BUTTON_PIN,
                                               NEO_GRB + NEO_KHZ800};

static const struct DrumComponent KICK = {4,
                                          106,
                                          8,
                                          0,
                                          LED_STRIP_PIN_NUMBER,
                                          PIEZO_ANALOG_INPUT_PIN,
                                          PIEZO_DIGITAL_INPUT_PIN,
                                          POTENTIOMETER_PIN,
                                          PUSH_BUTTON_PIN,
                                          NEO_GRB + NEO_KHZ800};

static const struct DrumComponent THIS_DRUM_COMPONENT = SNARE;

void animationComplete();

DrumPatterns strip(THIS_DRUM_COMPONENT, &animationComplete);

void animationComplete() {

  uint8_t red = random(0, 255);
  uint8_t green = random(0, 255);
  uint8_t blue = random(0, 255);

  if (strip.ActivePattern == RAINBOW_CYCLE) {
    strip.RainbowCycle(random(0, 10));
  }

  if (strip.ActivePattern == SCANNER) {

    if ((red + green + blue) <= 10) {
      red = 0;
      green = 0;
      blue = 255;
    }

    strip.Scanner(strip.Color(red, green, blue), 25);
  }

  if (strip.ActivePattern == COLOR_WIPE) {
    strip.ColorWipe(strip.Color(red, green, blue), 10);
  }
}

ANIMATION setStripPattern(uint8_t currentButtonState) {

  ANIMATION currentPattern = strip.ActivePattern;

  strip.Update();

  if (currentButtonState == RELEASED) {
    strip.Interval = 0;

    switch (currentPattern) {
    case DRUMBEAT:
      strip.ActivePattern = RAINBOW_CYCLE;
      strip.TotalSteps = 255;
      currentPattern = strip.ActivePattern;
      strip.clearStrip();
      strip.colorWipeAnimation(strip.Color(0, 0, 255), 10);
      break;
    case RAINBOW_CYCLE:
      strip.ActivePattern = SCANNER;
      currentPattern = strip.ActivePattern;
      strip.clearStrip();
      strip.colorWipeAnimation(strip.Color(255, 0, 0), 10);
      break;
    case SCANNER:
      strip.ActivePattern = COLOR_WIPE;
      currentPattern = strip.ActivePattern;
      strip.clearStrip();
      strip.colorWipeAnimation(strip.Color(255, 75, 0), 10);
      break;
    case COLOR_WIPE:
      strip.ActivePattern = DRUMBEAT;
      strip.TotalSteps = 255;
      currentPattern = strip.ActivePattern;
      strip.clearStrip();
      strip.colorWipeAnimation(strip.Color(0, 30, 0), 10);
      break;
    default:
      strip.ActivePattern = DRUMBEAT;
      strip.TotalSteps = 255;
      currentPattern = strip.ActivePattern;
      strip.clearStrip();
      strip.colorWipeAnimation(strip.Color(0, 30, 0), 10);
      break;
    }
  }

  return currentPattern;
}

void setup() {

  strip.begin();
  strip.setBrightness(255);

  strip.Scanner(strip.Color(0, 0, 255), 45);
  strip.ColorWipe(strip.Color(0, 0, 255), 30);

  strip.ActivePattern = DRUMBEAT;

  pinMode(PIEZO_DIGITAL_INPUT_PIN, INPUT);
  pinMode(PUSH_BUTTON_PIN, INPUT_PULLUP);

  strip.clearStrip();
  strip.colorWipeAnimation(strip.Color(0, 30, 0), 10);
  strip.clearStrip();
}

void loop() {

  static boolean brightnessIsBeingAdjusted = false;
  static uint8_t BUTTON_STATE = WAIT;

  if (!brightnessIsBeingAdjusted) {
    setStripPattern(BUTTON_STATE);
  }

  btnUP.read();

  switch (BUTTON_STATE) {

  case WAIT:

    if (btnUP.wasPressed()) {

      BUTTON_STATE = PRESSED;

    } else if (btnUP.wasReleased()) {

      BUTTON_STATE = RELEASED;
      rpt = REPEAT_FIRST;

    } else if (btnUP.pressedFor(rpt)) {

      rpt += REPEAT_INCR;
      BUTTON_STATE = HELD;
    }
    break;

  case HELD:

    strip.adjustBrightness();
    brightnessIsBeingAdjusted = true;
    rpt += REPEAT_INCR;
    BUTTON_STATE = WAIT;
    break;

  case RELEASED:

    BUTTON_STATE = WAIT;
    brightnessIsBeingAdjusted = false;
    break;

  case PRESSED:

    BUTTON_STATE = WAIT;
    break;
  }

  strip.showStrip();
}
