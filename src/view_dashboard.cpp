

/* ================================================================================================================== */
/* [INCL] Includes                                                                                                    */
/* ================================================================================================================== */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "button.h"
#include "filter.h"
#include "ntc.h"
#include "pinmap.h"
#include "pitches.h"
#include "sensor_manager.h"
#include "settings.h"
#include "view.h"

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Arduino.h>

/* ================================================================================================================== */
/* [DEFS] Defines                                                                                                     */
/* ================================================================================================================== */

/* ================================================================================================================== */
/* [TYPE] Type definitions                                                                                            */
/* ================================================================================================================== */

/* ================================================================================================================== */
/* [GLOB] Global variables                                                                                            */
/* ================================================================================================================== */
static view_t view_self = eVIEW_DASHBOARD;
static int    in_edit_mode = false;
static bool   timer_active = false;
static int    timer = 0;
static int    last_millis;
/* ================================================================================================================== */
/* [PFDE] Private functions declaration                                                                               */
/* ================================================================================================================== */
static void   s_init_cb(void);
static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display);
static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */
static void s_init_cb(void) {
    timer_active = false;
    timer = 0;
}

static void s_draw(Adafruit_SSD1306* display) {
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    // Temp measurement
    int T_current = sensor_manager_get(eSENSOR_TEMP);
    int T_target = settings_get()->controller.target;
    display->setCursor(0, 0);
    display->printf(F("T:%3d/%3d"), T_current / 1000, T_target);
    display->print((char)247);
    display->print("C");

    display->setCursor(0, 10);
    display->print(F("Heating..."));

    // Timer
    if (timer_active) {
        if (timer == 0) {
            // Initial run
            // Play sound
            // Reset bar
            // No time has elapsed
            tone(eIO_BUZZER, NOTE_A6, 300);
            display->fillRect(0, 30, SCREEN_WIDTH, 2, SSD1306_BLACK);
        }

        int now = millis();
        timer += now - last_millis;
        last_millis = now;

        if (timer >= settings_get()->hold_timer.duration) {
            // TODO: Happy beeep when  successfull
            tone(eIO_BUZZER, NOTE_C7, 1000);
            delay(1000);
            timer_active = 0;
        }
    }
    display->setCursor(0, 20);
    display->printf(
        F("Timer:%2d.%d/%2d.%ds"),
        timer / 1000,
        (timer % 1000) / 100,
        settings_get()->hold_timer.duration / 1000,
        (settings_get()->hold_timer.duration % 1000) / 100
    );
    display->fillRect(
        0, 30, ((float)timer / ((float)settings_get()->hold_timer.duration)) * SCREEN_WIDTH, 2, SSD1306_WHITE
    );

    //  Draw timer thingy

    display->display();
}

static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display) {

    if (prescalers->doInit) {
        prescalers->doInit = false;
        display->clearDisplay();
        display->display();
        s_draw(display);
    }
    if (prescalers->do50) {
        prescalers->do50 = false;
        s_draw(display);
    }
    return view_self;
}

static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    button_type_e type = *((button_type_e*)btn->arg);

    if (evt == LWBTN_EVT_KEEPALIVE && type == eBTN_SELECT && btn->keepalive.cnt == settings_get()->button.hold_off) {
        settings_factory_reset();
    }

    // Handle when in edit mode
    if (evt == LWBTN_EVT_ONPRESS && type == eBTN_LIM) {
        timer_active = true;
        timer = 0;
        last_millis = millis();
    }
    if (evt == LWBTN_EVT_ONRELEASE && type == eBTN_LIM) {
        // TODO:  Handle premptive holds by beeping sad, also
        if (timer < settings_get()->hold_timer.duration) {
            tone(eIO_BUZZER, NOTE_A2, 100);
            delay(100);
            tone(eIO_BUZZER, NOTE_F2, 100);
            delay(100);
            tone(eIO_BUZZER, NOTE_B1, 300);
            delay(300);
        }
        timer_active = false;
    }

    // Handle when not in edit mode (go to other screens)
    if (!in_edit_mode) {
        if (evt == LWBTN_EVT_ONPRESS) {
            if (type == eBTN_UP) {
                return view_get_prev();
            } else if (type == eBTN_DOWN) {
                return view_get_next();
            }
        }
        return view_self;
    }

    return view_self;
}

/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t dashboard_view_set_callbacks() {
    return (view_callbacks_t
    ){.view_init = s_init_cb,
      .view_draw_cb = s_draw_cb,
      .view_button_event_cb = s_button_type_event_cb,
      .view_on_exit_cb = NULL};
}

/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
