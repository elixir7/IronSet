#include <Arduino.h>
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"

#include <EEPROM.h>

static settings_t settings;

void settings_init(void) {
    EEPROM.get(0, settings);

    settings.button.hold_off = 10;
}

settings_t* settings_get(void) {
    return &settings;
}

void settings_save(void) {
    EEPROM.put(0, settings);
    tone(BUZZER, NOTE_A6, 25);
    delay(50);
    tone(BUZZER, NOTE_C6, 25);
    delay(50);
    tone(BUZZER, NOTE_E6, 25);
    delay(50);
}