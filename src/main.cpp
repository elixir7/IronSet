#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include "Arduino.h"

#include "button.h"
#include "graphics.h"
#include "pinmap.h"
#include "pitches.h"
#include "sensor_manager.h"
#include "settings.h"
#include "view.h"

int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4}; // notes in the melody:
int noteDurations[] = {4, 8, 8, 4, 4, 4, 4, 4}; // note durations: 4 = quarter note, 8 = eighth note, etc.:

void playMelody(void) {
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(BUZZER, melody[thisNote], noteDuration);

        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);

        // stop the tone playing:
        noTone(BUZZER);
    }
}

void setup() {
    settings_init();

    pinMode(LED_B, OUTPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(BUZZER, OUTPUT);

    pinMode(HEAT1, OUTPUT);
    pinMode(HEAT2, OUTPUT);

    button_init();
    sensor_manager_init();
    view_init();
    //playMelody();
}

void loop() {
    delay(20);
    digitalToggle(LED_R);
    button_update();
    sensor_manager_update();
    view_update();
}