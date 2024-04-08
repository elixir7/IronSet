

/* ================================================================================================================== */
/* [INCL] Includes                                                                                                    */
/* ================================================================================================================== */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "button.h"
#include "pinmap.h"
#include "pitches.h"
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
static int    screen_timer = 0;
static bool   in_edit_mode = false;
static view_t view_self = eVIEW_TEMP;

/* ================================================================================================================== */
/* [PFDE] Private functions declaration                                                                               */
/* ================================================================================================================== */
static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display);
static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */

static void s_draw(Adafruit_SSD1306* display) {
    if (screen_timer % 20 < 10) {
        display->setTextSize(2);
        display->setCursor(15, 15);
        display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        display->printf(F("%3d "), settings_get()->controller.target);
        display->setCursor(SCREEN_WIDTH / 2 + 2, 15);
        display->print((char)247);
        display->print("C");
    } else if (in_edit_mode) {
        display->fillRect(0, 15, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 15, SSD1306_BLACK);
    }

    display->display();
    screen_timer += 1;
}

static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display) {

    if (prescalers->doInit) {
        prescalers->doInit = false;

        screen_timer = 0;
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(0, 0);
        display->println(F("Set temperature"));
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

    // Handle select button
    if (evt == LWBTN_EVT_ONPRESS && type == BTN_SELECT) {
        if (in_edit_mode) {
            settings_save();
        }
        in_edit_mode = !in_edit_mode;
        return view_self;
    }

    // Handle when not in edit mode (go to other screens)
    if (!in_edit_mode) {
        if (evt == LWBTN_EVT_ONPRESS) {
            if (type == BTN_UP) {
                return view_get_prev();
            } else if (type == BTN_DOWN) {
                return view_get_next();
            }
        }
        return view_self;
    }

    // Handle button events when in edit mode
    if (evt == LWBTN_EVT_ONPRESS) {
        if (type == BTN_UP) {
            INC_WITH_VAL_TO_MAX(settings_get()->controller.target, 250, 1);
        } else if (type == BTN_DOWN) {
            DEC_WITH_VAL_TO_MIN(settings_get()->controller.target, 50, 1);
        }
    } else if (evt == LWBTN_EVT_KEEPALIVE) {
        if (type == BTN_UP) {
            INC_WITH_VAL_TO_MAX(settings_get()->controller.target, 250, 1);
        } else if (type == BTN_DOWN) {
            DEC_WITH_VAL_TO_MIN(settings_get()->controller.target, 50, 1);
        }
    }

    return view_self;
}

/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t temperature_view_set_callbacks() {
    return (view_callbacks_t
    ){.view_init = NULL,
      .view_draw_cb = s_draw_cb,
      .view_button_event_cb = s_button_type_event_cb,
      .view_on_exit_cb = NULL};
}

/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
