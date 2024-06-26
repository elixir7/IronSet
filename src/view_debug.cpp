/* ================================================================================================================== */
/* [INCL] Includes                                                                                                    */
/* ================================================================================================================== */
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "button.h"
#include "buzzer.h"
#include "filter.h"
#include "ntc.h"
#include "pinmap.h"
#include "pitches.h"
#include "sensor_manager.h"
#include "settings.h"
#include "view.h"

/* ================================================================================================================== */
/* [DEFS] Defines                                                                                                     */
/* ================================================================================================================== */

/* ================================================================================================================== */
/* [TYPE] Type definitions                                                                                            */
/* ================================================================================================================== */

/* ================================================================================================================== */
/* [GLOB] Global variables                                                                                            */
/* ================================================================================================================== */
static int    screen_timer = 0;
static view_t view_self = eVIEW_DEBUG;

/* ================================================================================================================== */
/* [PFDE] Private functions declaration                                                                               */
/* ================================================================================================================== */
static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display);
static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */
static void s_draw(Adafruit_SSD1306* display) {
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    int u_filt = sensor_manager_get(eSENSOR_24V);
    display->setCursor(0, 0);
    display->printf(F("24V: %2d.%d V"), u_filt / 1000, (u_filt % 1000) / 100);

    // Temp measurement
    int t_filt = sensor_manager_get(eSENSOR_TEMP);
    display->setCursor(0, 10);
    display->printf(F("T: %2d.%d "), t_filt / 1000, (t_filt % 1000) / 100);
    display->print((char)247);
    display->print("C");

    int R_filt = sensor_manager_get(eSENSOR_R);
    display->setCursor(0, 20);
    display->printf(F("R: %6d  Ohm"), R_filt);

    display->display();
    screen_timer += 1;
}

// float  duty = 0;
// double T = 0;
// double V = 0;
// float  R = 0;
// int    index_duty = 0;

static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display) {

    if (prescalers->doInit) {
        prescalers->doInit = false;

        screen_timer = 0;
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(0, 0);
        display->display();
        s_draw(display);
    }

    if (prescalers->do50) {
        prescalers->do50 = false;
        s_draw(display);
        // T = sensor_manager_get(eSENSOR_TEMP) / ((float)1000);
        // V = sensor_manager_get(eSENSOR_24V) / ((float)1000);
        // R = sensor_manager_get(eSENSOR_R);
    }
    return view_self;
}

static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    button_type_e type = *((button_type_e*)btn->arg);

    // Factory reset
    if (evt == LWBTN_EVT_KEEPALIVE && type == eBTN_SELECT && btn->keepalive.cnt == settings_get()->button.hold_off) {
        settings_factory_reset();
        return view_self;
    }

    // Play sound on up/down press
    if ((evt == LWBTN_EVT_ONPRESS || evt == LWBTN_EVT_KEEPALIVE) && (type == eBTN_UP || type == eBTN_DOWN)) {
        buzzer_button_press();
    }

    if (type == eBTN_UP && evt == LWBTN_EVT_ONPRESS) {
        return view_get_prev();
    } else if (type == eBTN_DOWN && evt == LWBTN_EVT_ONPRESS) {
        return view_get_next();
    } else if (type == eBTN_SELECT && evt == LWBTN_EVT_ONPRESS) {
        // if (duty < 0.1) {
        //     duty = 0.9;
        // } else {
        //     duty = 0;
        // }

        // index_duty = (index_duty + 1) % 10;
        // duty = 0.1 * index_duty;

        // if (duty) {
        //     duty = 0;
        // } else {
        //     duty = 0.90;
        // }
        // analogWrite(eIO_HEAT1, 255 * duty);
        // analogWrite(eIO_HEAT2, 255 * duty);
        // analogWriteFrequency(10);
    }

    return view_self;
}

/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t debug_view_set_callbacks() {
    return (view_callbacks_t
    ){.view_init = NULL,
      .view_draw_cb = s_draw_cb,
      .view_button_event_cb = s_button_type_event_cb,
      .view_on_exit_cb = NULL};
}

/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
