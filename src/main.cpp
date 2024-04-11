#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>
#include <Wire.h>
#include "Arduino.h"

#include "button.h"
#include "buzzer.h"
#include "controller.h"
#include "graphics.h"
#include "pinmap.h"
#include "pitches.h"
#include "sensor_manager.h"
#include "settings.h"
#include "view.h"

void setup() {
    settings_init();

    pinMode(eIO_LEB_B, OUTPUT);
    pinMode(eIO_LED_R, OUTPUT);

    button_init();
    buzzer_init();
    sensor_manager_init();
    controller_init();
    view_init();
    // buzzer_startup();
}

void loop() {
    button_update();
    sensor_manager_update();
    view_update();

    digitalToggle(eIO_LED_R);
    delay(20);
}