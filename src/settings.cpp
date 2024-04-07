#include <Arduino.h>
#include "settings.h"

#include <EEPROM.h>

static settings_t settings;

void settings_init(void)
{
    EEPROM.get(0, settings);

    settings.button.hold_off = 10;
}

settings_t *settings_get(void)
{
    return &settings;
}

void settings_save(void)
{
    EEPROM.put(0, settings);
}