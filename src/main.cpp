#include <Arduino.h>
#include <FastLED.h>
#include <esp_log.h>

#define EVERY_N_MILLIS(interval, id)               \
    static unsigned long __last_time_##id = 0;     \
    if (millis() - __last_time_##id >= (interval)) \
        if (__last_time_##id = millis())

static const char *TAG = "MAIN";

#define NUM_LEDS 16
#define DATA_PIN 4

CRGB leds[NUM_LEDS];

int colorIndex = 0;

const CRGB COLORS[] = {
    CRGB::Red,
    CRGB::Orange,
    CRGB::Yellow,
    CRGB::Green,
    CRGB::Blue,
    CRGB::Indigo,
    CRGB::Violet
};

const int NUM_COLORS = sizeof(COLORS) / sizeof(COLORS[0]);

void setup() { 
    FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
    FastLED.setMaxPowerInVoltsAndMilliamps(5, 500); // volts, mA
    ESP_LOGI(TAG, "Lights set up");
}

void loop() {
    EVERY_N_MILLIS(100, 1) {
        for (int i = 0; i < NUM_LEDS; i++) {
            leds[i] = COLORS[colorIndex];
            colorIndex = colorIndex == 0 ? NUM_COLORS - 1 : colorIndex - 1;
            FastLED.show();
        }
    }
}
