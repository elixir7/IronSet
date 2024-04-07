

/* ================================================================================================================== */
/* [INCL] Includes                                                                                                    */
/* ================================================================================================================== */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "pinmap.h"
#include "pitches.h"
#include "settings.h"
#include "view.h"
#include "button.h"

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
static Adafruit_SSD1306 *display;
static int screen_timer = 0;

/* ================================================================================================================== */
/* [PFDE] Private functions declaration                                                                               */
/* ================================================================================================================== */
static void s_init(Adafruit_SSD1306 *display_ref);
static view_t s_draw_cb(view_prescalers_t *prescalers);
static view_t s_button_type_event_cb(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */
static void s_init(Adafruit_SSD1306 *display_ref)
{
    display = display_ref;
}

static void s_draw(void)
{
    if (screen_timer % 20 < 10)
    {
        display->setTextSize(2);
        display->setCursor(15, 15);
        display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        display->printf(F("%3d"), settings_get()->controller.target);
        display->setCursor(SCREEN_WIDTH / 2 + 2, 15);
        display->print(" C");
    }
    else
    {
        display->fillRect(0, 15, SCREEN_WIDTH / 2, SCREEN_HEIGHT - 15, SSD1306_BLACK);
    }

    display->display();
    screen_timer += 1;
}

static view_t s_draw_cb(view_prescalers_t *prescalers)
{

    if (prescalers->doInit)
    {
        prescalers->doInit = false;

        screen_timer = 0;
        display->clearDisplay();
        display->setTextSize(1);
        display->setTextColor(SSD1306_WHITE);
        display->setCursor(0, 0);
        display->println(F("Set temperature"));
        display->display();
        s_draw();
    }
    if (prescalers->do50)
    {
        prescalers->do50 = false;
        s_draw();
    }
    return eVIEW_TEMP;
}

static view_t s_button_type_event_cb(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt)
{
    button_type_e type = *((button_type_e *)btn->arg);

    if (evt == LWBTN_EVT_ONPRESS)
    {
        if (type == BTN_UP)
        {
            INC_WITH_VAL_TO_MAX(settings_get()->controller.target, 250, 1);
        }
        else if (type == BTN_DOWN)
        {
            DEC_WITH_VAL_TO_MIN(settings_get()->controller.target, 50, 1);
        }
        else if (type == BTN_SELECT)
        {
            return eVIEW_TIMER;
        }
    }
    else if (evt == LWBTN_EVT_KEEPALIVE)
    {
        if (type == BTN_UP)
        {
            INC_WITH_VAL_TO_MAX(settings_get()->controller.target, 250, 1);
        }
        else if (type == BTN_DOWN)
        {
            DEC_WITH_VAL_TO_MIN(settings_get()->controller.target, 50, 1);
        }
    }

    return eVIEW_TEMP;
}

static void s_on_exit_cb(void)
{
    settings_save();
}
/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t controller_view_set_callbacks()
{
    return (view_callbacks_t){
        .view_init = s_init,
        .view_draw_cb = s_draw_cb,
        .view_button_event_cb = s_button_type_event_cb,
        .view_on_exit_cb = s_on_exit_cb};
}
/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
