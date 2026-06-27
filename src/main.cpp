#include <Arduino.h>
#include <FastLED.h>
#include <esp_log.h>

#define EVERY_N_MILLIS(interval, id)               \
    static unsigned long __last_time_##id = 0;     \
    if (millis() - __last_time_##id >= (interval)) \
        if (__last_time_##id = millis())

static const char *TAG = "MAIN";

const int BRIGHTNESS = 25;

#define NUM_LEDS 9
#define DATA_PIN 4

CRGB leds[NUM_LEDS];

// Each LED blends from its start color toward its target color over
// TRANSITION_MS milliseconds whenever a new step is computed.
CRGB startColors[NUM_LEDS];
CRGB targetColors[NUM_LEDS];
unsigned long transitionStart = 0;
const unsigned long TRANSITION_MS = 250;

int colorIndex = 0;
bool forward = true;
bool rainbowColors = true;
int count = 0;

const CRGB RAINBOW_COLORS[] = {
    CRGB::Red,
    CRGB::Orange,
    CRGB::Yellow,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Indigo,
    CRGB::Violet
};

const CRGB GREEN_COLORS[] = {
    CRGB(0, 50, 0),  // Dark Green
    CRGB(0, 75, 0),
    CRGB(0, 100, 0),
    CRGB(0, 125, 0),
    CRGB(0, 150, 0),
    CRGB(0, 175, 0),
    CRGB(0, 200, 25),  // Start adding blue
    CRGB(0, 225, 50),
    CRGB(0, 250, 75),
    CRGB(0, 255, 100)  // Light Green with Blue
};

const int NUM_RAINBOW_COLORS = sizeof(RAINBOW_COLORS) / sizeof(RAINBOW_COLORS[0]);
const int NUM_GREEN_COLORS = sizeof(GREEN_COLORS) / sizeof(GREEN_COLORS[0]);

void setup() { 
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // volts, mA
    ESP_LOGI(TAG, "Lights set up");
}

void loop() {
    // Every TRANSITION_MS, compute the next set of target colors. The current
    // on-screen colors become the start of the new blend.
    EVERY_N_MILLIS(TRANSITION_MS, 1) {
        const CRGB* colors = rainbowColors ? RAINBOW_COLORS : GREEN_COLORS;
        int colorCount = rainbowColors ? NUM_RAINBOW_COLORS : NUM_GREEN_COLORS;
        for (int i = 0; i < NUM_LEDS; i++) {
            startColors[i] = leds[i];
            targetColors[i] = colors[colorIndex];
            if (forward) {
                colorIndex = colorIndex == colorCount - 1 ? 0 : colorIndex + 1;
            } else {
                colorIndex = colorIndex == 0 ? colorCount - 1 : colorIndex - 1;
            }
        }
        transitionStart = millis();
        count++;
        if (count % 50 == 0) {
            forward = !forward;
            ESP_LOGI(TAG, "Changing direction");
        }

        if (count == 100) {
            rainbowColors = !rainbowColors;
            count = 0;
            ESP_LOGI(TAG, "Changing color mode");
        }
    }

    // Continuously interpolate each LED from its start color toward its target
    // color so the change eases in over TRANSITION_MS instead of snapping.
    unsigned long elapsed = millis() - transitionStart;
    fract8 amount = elapsed >= TRANSITION_MS
                        ? 255
                        : (fract8)((elapsed * 255) / TRANSITION_MS);
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = blend(startColors[i], targetColors[i], amount);
    }
    FastLED.show();
}
