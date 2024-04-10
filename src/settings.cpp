#include <Arduino.h>
#include <EEPROM.h>
#include "buzzer.h"
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"

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

    // TODO: if settings are all 0 it should automatically force factory reset values
    // settings_factory_reset();
}

settings_t* settings_get(void) {
    return &settings;
}

void settings_save(void) {
    EEPROM.put(0, settings);
    buzzer_success();
}