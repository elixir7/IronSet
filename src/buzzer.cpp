#include <Arduino.h>
#include "buzzer.h"
#include "pinmap.h"
#include "pitches.h"

static int melody[] = {NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4}; // notes in the melody
static int durations[] = {4, 8, 8, 4, 4, 4, 4, 4}; // note durations: 4 = quarter note, 8 = eighth note, etc.

void buzzer_init(void) {
    pinMode(eIO_BUZZER, OUTPUT);
}

void buzzer_startup(void) {
    buzzer_melody(melody, durations, sizeof(melody) / sizeof(melody[0]));
}

void buzzer_melody(int melody[], int note_durations[], int length) {
    for (int thisNote = 0; thisNote < length; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        // e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / note_durations[thisNote];

        buzzer_custom_blocking(melody[thisNote], noteDuration);
        delay(noteDuration * 0.3);
    }
}

void buzzer_button_press(void) {
    buzzer_custom_blocking(NOTE_C7, 25);
    // digitalWrite(eIO_LEB_B, HIGH);
    // delay(25);
    // digitalWrite(eIO_LEB_B, LOW);
}

void buzzer_success(void) {
    buzzer_custom_blocking(NOTE_C7, 50);
    delay(50);
    buzzer_custom_blocking(NOTE_E7, 50);
    delay(50);
    buzzer_custom_blocking(NOTE_G7, 50);
    delay(50);
}

void buzzer_fail(void) {
    buzzer_custom_blocking(NOTE_DS3, 300);
}

void buzzer_custom_non_blocking(int frequency, int duration) {
    tone(eIO_BUZZER, frequency, duration);
}

void buzzer_custom_blocking(int frequency, int duration) {
    tone(eIO_BUZZER, frequency, duration);
    delay(duration);
}
