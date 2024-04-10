#include <Arduino.h>
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"

#include <EEPROM.h>

static settings_t settings;

void settings_factory_reset(void) {
    // Write default settings
    settings.hold_timer.duration = 5 * 1000;
    settings.controller = (controller_settings_t){.enabled = true, .target = 180, .max = 250, .min = 10};
    settings.button.hold_off = 10;

    settings_save();
}

void settings_init(void) {
    EEPROM.get(0, settings);

    // settings_factory_reset();
}

settings_t* settings_get(void) {
    return &settings;
}

void settings_save(void) {
    EEPROM.put(0, settings);
    tone(eIO_BUZZER, NOTE_A6, 25);
    delay(50);
    tone(eIO_BUZZER, NOTE_C6, 25);
    delay(50);
    tone(eIO_BUZZER, NOTE_E6, 25);
    delay(50);
}