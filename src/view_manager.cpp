#include <Arduino.h>
#include <Wire.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "button.h"
#include "graphics.h"
#include "math_custom.h"
#include "pinmap.h"
#include "view.h"

typedef struct {
    Adafruit_SSD1306 display;
    view_t           current_view;
    view_callbacks_t callbacks;
    uint8_t          prescaler;
} view_manager_t;

static view_manager_t self;
view_data_t           view_data;

//  =========================================================

static view_callbacks_t s_get_callbacks(view_t view);
static void             s_show_startup(void);

// =======================================================0
static view_callbacks_t s_get_callbacks(view_t view) {
    switch (view) {
        case eVIEW_TIMER: return timer_view_set_callbacks();
        case eVIEW_TEMP: return temperature_view_set_callbacks();
        case eVIEW_DASHBOARD: return dashboard_view_set_callbacks();
        case eVIEW_DEBUG: return debug_view_set_callbacks();
        default: return dashboard_view_set_callbacks();
    }
}

static void s_show_startup(void) {
    // Show startup scree
    self.display.clearDisplay();
    self.display.drawBitmap(0, 0, bitmap_heat.bitmap, bitmap_heat.width, bitmap_heat.height, 1);
    self.display.setTextSize(2);
    self.display.setTextColor(SSD1306_WHITE);
    self.display.setCursor(40, 10);
    self.display.println(F("IronSet"));
    self.display.display();
    delay(500);
}

// =====================================================

void view_init() {
    self.display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
    if (!self.display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        while (1) {
            digitalWrite(eIO_LED_R, HIGH);
        }
    }

    s_show_startup();
    self.current_view =
        eVIEW_END; // required since the requested dashboard view on the next line is the same as the uninitialised variable
    view_switch(eVIEW_DASHBOARD);
}

void view_update() {
    // Frame rate counter, reset the prescaler once a second
    INC_WITH_RESET(self.prescaler, 50);

    // 50fps
    view_data.prescalers.do50 = true;
    if (!(self.prescaler % 2)) {
        // 25fps
        view_data.prescalers.do25 = true;
    }
    if (!(self.prescaler % 5)) {
        // 10fps
        view_data.prescalers.do10 = true;
    }
    if (!(self.prescaler % 10)) {
        // 5fps
        view_data.prescalers.do5 = true;
    }
    if (self.prescaler == 7) {
        // 1fps
        view_data.prescalers.do1 = true;
    }

    if (self.callbacks.view_draw_cb != NULL) {
        view_t new_view = self.callbacks.view_draw_cb(&view_data.prescalers, &self.display);
        view_switch(new_view);
    }
}

Adafruit_SSD1306* view_get_display() {
    return &self.display;
}

void view_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    if (self.callbacks.view_button_event_cb != NULL) {
        view_t new_view = self.callbacks.view_button_event_cb(lw, btn, evt);
        view_switch(new_view);
    }
}

void view_switch(view_t new_view) {
    if (self.current_view == new_view) {
        return;
    }

    if (self.callbacks.view_on_exit_cb != NULL) {
        self.callbacks.view_on_exit_cb();
    }

    // Reset state (and add init)
    memset(&view_data, 0, sizeof(view_data));
    view_data.prescalers.doInit = true;

    self.callbacks = s_get_callbacks(new_view);
    if (self.callbacks.view_init != NULL) {
        self.callbacks.view_init();
    }
    self.current_view = new_view;
}

view_t view_get_next(void) {
    return (view_t)mod(self.current_view + 1, eVIEW_END);
}

view_t view_get_prev(void) {
    return (view_t)mod(self.current_view - 1, eVIEW_END);
}