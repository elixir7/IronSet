#ifndef __VIEW__H__
#define __VIEW__H__

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include <stdbool.h>
#include <stdint.h>

#include "lwbtn.h"
#include "button.h"

// OLED
#define SCREEN_WIDTH 128    // OLED display width, in pixels
#define SCREEN_HEIGHT 32    // OLED display height, in pixels
#define OLED_RESET -1       // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32

//-------Math-----------------
// Increment var and loop to 0 when max is reached
#define INC_WITH_RESET(var, max) (var) = (++(var) >= (max)) ? 0 : (var)
// Add val to var and stop when max is reached
#define INC_WITH_VAL_TO_MAX(var, max, val) \
    (var) = ((var) >= (max)) ? (max) : ((((var) + (val)) >= (max)) ? (max) : ((var) + (val)))
// Increment val and stop when max is reached
#define INC_TO_MAX(var, max) (var) = ((var) >= (max)) ? (max) : ((++(var) >= (max)) ? (max) : (var))
// Subtract val from var and stop when min is reached
// NOTE: This can underflow, be careful with what val is picked
#define DEC_WITH_VAL_TO_MIN(var, min, val) \
    (var) = ((var) <= (min)) ? (min) : ((((var) - (val)) <= (min)) ? (min) : ((var) - (val)))
// Decrement var and stop when min is reached
#define DEC_TO_MIN(var, min) (var) = ((var) <= (min)) ? (min) : ((--(var) <= (min)) ? (min) : (var))

/**
 * @brief prescalers to determine what to draw next
 * @note 10Hz is split into two parts to not draw too much in the same idle cycle
 */
typedef struct
{
    bool doInit : 1;
    bool do50 : 1;
    bool do25 : 1;
    bool do10_1 : 1;
    bool do10_2 : 1;
    bool do5_1 : 1;
    bool do5_2 : 1;
    bool do1_1 : 1;
    bool do1_2 : 1;
    uint8_t : 0;
} view_prescalers_t;

typedef struct
{
    // union {
    //     struct {
    //         int16_t drawn_temp;
    //         int16_t drawn_target;
    //         uint8_t drawn_state;
    //     } timer_view;
    // };
    view_prescalers_t prescalers;
} view_data_t;

extern view_data_t view_data;

/**
 * @brief enum of all available views
 *
 */
typedef enum
{
    eVIEW_DASHBOARD,
    eVIEW_TIMER,
    eVIEW_TEMP
} view_t;

/**
 * @brief callbacks for the different views
 */
typedef struct
{
    view_t (*view_draw_cb)(view_prescalers_t *prescalers);
    view_t (*view_button_type_event_cb)(button_type_e type, uint16_t time_held, button_press_state_t press_state);
    void (*view_on_exit_cb)(void);
} view_callbacks_t;

// view_callbacks_t dash_view_set_callbacks();
view_callbacks_t timer_view_set_callbacks();
// view_callbacks_t temp_view_set_callbacks();

// public
void view_init();
void view_update();
Adafruit_SSD1306 *view_get_display();
void view_button_event(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt)

#endif