

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
#include "graphics.h"
#include "ntc.h"
#include "pinmap.h"
#include "pitches.h"
#include "sensor_manager.h"
#include "settings.h"
#include "view.h"

/* ================================================================================================================== */
/* [DEFS] Defines                                                                                                     */
/* ================================================================================================================== */
#define Y_STATUS        (13)
#define X_TIMER_SETTING (SCREEN_WIDTH - 45)
#define FONT_HEIGHT     (7)
#define BAR_HEIGHT      (5)
/* ================================================================================================================== */
/* [TYPE] Type definitions                                                                                            */
/* ================================================================================================================== */

/* ================================================================================================================== */
/* [GLOB] Global variables                                                                                            */
/* ================================================================================================================== */
static view_t view_self = eVIEW_DASHBOARD;

typedef enum { eSTATE_HEATING = 0, eSTATE_READY, eSTATE_FUSING, eSTATE_SUCCESS, eSTATE_FAIL } state_e;

typedef struct {
    Adafruit_SSD1306* display;
    state_e           state;
    int               timer;
    int               last_millis;
    bool              sw_press;
    bool              sw_release;
    int               status_timer;
} state_machine_t;

static state_machine_t self;

/* ================================================================================================================== */
/* [PFDE] Private functions declaration                                                                               */
/* ================================================================================================================== */
static void   s_init_cb(void);
static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display);
static view_t s_button_type_event_cb(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

static const char* s_state_to_str(state_e view);
static void        s_transition_to_state(state_e state);
static void        s_state_machine_update(void);
static void        s_transition_to_heating(void);
static void        s_transition_to_ready(void);
static void        s_transition_to_fusing(void);
static void        s_transition_to_success(void);
static void        s_transition_to_fail(void);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */
static const char* s_state_to_str(state_e view) {
    switch (view) {
        case eSTATE_HEATING: return "Heating";

        case eSTATE_READY: return "Ready";

        case eSTATE_FUSING: return "Fusing";

        case eSTATE_SUCCESS: return "Success";

        case eSTATE_FAIL: return "Fail";

        default: return "unknown state";
    }
}

static void s_state_machine_update(void) {

    // Status
    self.display->fillRect(
        0, Y_STATUS, SCREEN_WIDTH / 2, FONT_HEIGHT + 5, SSD1306_BLACK
    ); // Height must be increased due to  chars like g
    self.display->setCursor(0, Y_STATUS);
    self.display->printf(F("%s"), s_state_to_str(self.state));

    int T_current = sensor_manager_get(eSENSOR_TEMP);
    int T_target = settings_get()->controller.target;

    switch (self.state) {
        case eSTATE_HEATING:
            if (abs(T_target - T_current / 1000) <= 5) {
                s_transition_to_ready();
            }
            break;

        case eSTATE_READY:
            if (abs(T_target - T_current / 1000) > 5) {
                s_transition_to_heating();
            } else if (self.sw_press) {
                s_transition_to_fusing();
            }
            break;

        case eSTATE_FUSING:
            self.display->setCursor(0, Y_STATUS);
            self.display->printf(
                F("Fusing     %2d.%d/%2d.%ds"),
                self.timer / 1000,
                (self.timer % 1000) / 100,
                settings_get()->hold_timer.duration / 1000,
                (settings_get()->hold_timer.duration % 1000) / 100
            );
            self.display->fillRect(
                0,
                SCREEN_HEIGHT - 5,
                ((float)self.timer / ((float)settings_get()->hold_timer.duration)) * SCREEN_WIDTH,
                5,
                SSD1306_WHITE
            );

            if (self.sw_release) {
                s_transition_to_fail();
            } else if (self.timer >= settings_get()->hold_timer.duration) {
                self.display->display(); // Must redraw when timer hits timeout since transition will ignore the draw.
                s_transition_to_success();
            } else {
                int now = millis();
                self.timer += now - self.last_millis;
                self.last_millis = now;
            }
            break;

        case eSTATE_SUCCESS:
        case eSTATE_FAIL:
            if (self.status_timer >= 100) {
                s_transition_to_ready();
                break;
            }

            // Blink
            self.display->setCursor(0, Y_STATUS);
            if ((self.status_timer % 20) < 10) {
                self.display->printf(F("%s"), s_state_to_str(self.state));
            } else {
                self.display->printf(F("%s"), "       ");
            }

            self.status_timer++;
            break;

        default: break;
    }
}

static void s_transition_to_state(state_e state) {
    self.state = state;
}

static void s_transition_to_heating(void) {
    s_transition_to_state(eSTATE_HEATING);
}

static void s_transition_to_ready(void) {
    s_transition_to_state(eSTATE_READY);

    buzzer_custom_blocking(NOTE_C7, 50);
    delay(50);
    buzzer_custom_blocking(NOTE_C7, 50);
    delay(50);
    buzzer_custom_blocking(NOTE_C7, 50);
    delay(50);

    self.display->fillRect(
        SCREEN_WIDTH / 2, Y_STATUS, SCREEN_WIDTH / 2, FONT_HEIGHT, SSD1306_BLACK
    ); // Reset  last  timer time
    self.display->fillRect(0, SCREEN_HEIGHT - BAR_HEIGHT, SCREEN_WIDTH, BAR_HEIGHT, SSD1306_BLACK); // Reset timer bar
    self.sw_press = false;
}

static void s_transition_to_fusing(void) {
    self.sw_release = false;
    self.timer = 0;
    self.last_millis = millis();
    s_transition_to_state(eSTATE_FUSING);
}

static void s_transition_to_success(void) {
    buzzer_success();
    self.status_timer = 0;
    s_transition_to_state(eSTATE_SUCCESS);
}

static void s_transition_to_fail(void) {
    buzzer_fail();
    self.status_timer = 0;
    s_transition_to_state(eSTATE_FAIL);
}

static void s_init_cb(void) {
    settings_get()->controller.enabled = true;
}

static void s_draw(Adafruit_SSD1306* display) {
    display->setTextSize(1);
    display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);

    // Update
    s_state_machine_update();

    // Temperature
    int T_current = sensor_manager_get(eSENSOR_TEMP);
    int T_target = settings_get()->controller.target;
    display->drawBitmap(0, 0, bitmap_temp.bitmap, bitmap_temp.width, bitmap_temp.height, 1);
    display->setCursor(bitmap_temp.width + 5, 0);
    display->printf(F("%3d/%3d"), T_current / 1000, T_target);
    display->print((char)247);
    display->print("C");

    // Timer
    display->drawBitmap(X_TIMER_SETTING, 0, bitmap_clock.bitmap, bitmap_clock.width, bitmap_clock.height, 1);
    display->setCursor(X_TIMER_SETTING + bitmap_clock.width, 0);
    display->printf(
        F("%2d.%ds"), settings_get()->hold_timer.duration / 1000, (settings_get()->hold_timer.duration % 1000) / 100
    );

    display->display();
}

static view_t s_draw_cb(view_prescalers_t* prescalers, Adafruit_SSD1306* display) {

    if (prescalers->doInit) {
        prescalers->doInit = false;
        self.display = display;

        s_transition_to_state(eSTATE_HEATING);
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

    if (evt == LWBTN_EVT_ONPRESS && type != eBTN_LIM) {
        buzzer_button_press();
    }

    // Limit button press
    if (type == eBTN_LIM && evt == LWBTN_EVT_ONPRESS) {
        self.sw_press = true;
    } else if (type == eBTN_LIM && evt == LWBTN_EVT_ONRELEASE) {
        self.sw_release = true;
    }

    //  Next / Prev  screen
    if (type == eBTN_UP && evt == LWBTN_EVT_ONPRESS) {
        return view_get_prev();
    } else if (type == eBTN_DOWN && evt == LWBTN_EVT_ONPRESS) {
        return view_get_next();
    }

    return view_self;
}

static void s_exit_cb(void) {
    settings_get()->controller.enabled = false;
}

/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t dashboard_view_set_callbacks() {
    return (view_callbacks_t
    ){.view_init = s_init_cb,
      .view_draw_cb = s_draw_cb,
      .view_button_event_cb = s_button_type_event_cb,
      .view_on_exit_cb = s_exit_cb};
}

/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
