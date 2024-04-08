

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
    display->setCursor(0, 10);
    display->println("Testing");
    display->display();
}

static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display) {

    if (prescalers->doInit) {
        prescalers->doInit = false;

        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(0, 0);
        display->println(F("Dashboard"));
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

    return view_self;
}

/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t dashboard_view_set_callbacks() {
    return (view_callbacks_t
    ){.view_init = NULL,
      .view_draw_cb = s_draw_cb,
      .view_button_event_cb = s_button_type_event_cb,
      .view_on_exit_cb = NULL};
}

/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
