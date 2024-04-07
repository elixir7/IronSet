

/* ================================================================================================================== */
/* [INCL] Includes                                                                                                    */
/* ================================================================================================================== */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "view.h"
#include "settings.h"

#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

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
static view_t s_draw_cb(view_prescalers_t *prescalers);
static view_t s_button_type_event_cb(button_type_e type, uint16_t time_held, button_press_state_t press_state);

/* ================================================================================================================== */
/* [PFUN] Private functions implementations                                                                           */
/* ================================================================================================================== */
static void s_draw(void)
{
    if (screen_timer % 20 < 10)
    {
        display->setTextSize(2);
        display->setCursor(15, 15);
        display->setTextColor(SSD1306_WHITE, SSD1306_BLACK);
        display->printf(F("%2d.%d "), settings_get()->hold_timer.duration / 1000, (settings_get()->hold_timer.duration % 1000) / 100);
        display->setCursor(SCREEN_WIDTH / 2 + 2, 15);
        display->print("s");
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
        display->println(F("Set hold timer"));
        display->display();
        s_draw();
    }
    if (prescalers->do50)
    {
        prescalers->do50 = false;
        s_draw();
    }
    return eVIEW_TIMER;
}

static view_t s_button_type_event_cb(button_type_e type, uint16_t time_held, button_press_state_t press_state)
{
    // TODO: process button events
    return eVIEW_TIMER;
}
/* ================================================================================================================== */
/* [FUNC] Functions implementations                                                                                   */
/* ================================================================================================================== */
view_callbacks_t timer_view_set_callbacks()
{
    display = view_get_display();
    return (view_callbacks_t){
        .view_draw_cb = s_draw_cb,
        .view_button_type_event_cb = s_button_type_event_cb,
        .view_on_exit_cb = NULL};
}
/* ================================================================================================================== */
/* [INTR] Interrupts                                                                                                  */
/* ================================================================================================================== */
