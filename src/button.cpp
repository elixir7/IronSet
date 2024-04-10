#include <Arduino.h>
#include <stdint.h>

#include "button.h"
#include "lwbtn.h"
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"
#include "view.h"

// ============= Private variables ===================
const uint32_t     pins[] = {eIO_BTN_UP, eIO_BTN_DOWN, eIO_BTN_SELECT, eIO_BTN_LIM};
static lwbtn_btn_t btns[] = {
    {.arg = (void*)&pins[0]},
    {.arg = (void*)&pins[1]},
    {.arg = (void*)&pins[2]},
    {.arg = (void*)&pins[3]},
};

// ============ Private function declaration =================
static uint8_t s_get_states(struct lwbtn* lw, struct lwbtn_btn* btn);
static void    s_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt);

//============ Private function implementation ===============
static uint8_t s_get_states(struct lwbtn* lw, struct lwbtn_btn* btn) {
    return !digitalRead(*((uint32_t*)btn->arg));
}

static void s_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    if (evt == LWBTN_EVT_ONPRESS) {
        tone(eIO_BUZZER, NOTE_C7, 25);
        digitalWrite(eIO_LEB_B, HIGH);
        delay(25);
        digitalWrite(eIO_LEB_B, LOW);
        view_button_event(lw, btn, evt);
    } else if (evt == LWBTN_EVT_KEEPALIVE && btn->keepalive.cnt >= settings_get()->button.hold_off) {
        // Only fire keep alive after a delay
        // tone(eIO_BUZZER, NOTE_C7, 25);
        digitalWrite(eIO_LEB_B, HIGH);
        delay(25);
        digitalWrite(eIO_LEB_B, LOW);
        view_button_event(lw, btn, evt);
    } else if (evt == LWBTN_EVT_ONRELEASE) {
        view_button_event(lw, btn, evt);
    } else if (evt == LWBTN_EVT_ONCLICK) {
        view_button_event(lw, btn, evt);
    }
}

//======================================================

void button_init(void) {
    pinMode(eIO_BTN_UP, INPUT_PULLUP);
    pinMode(eIO_BTN_DOWN, INPUT_PULLUP);
    pinMode(eIO_BTN_SELECT, INPUT_PULLUP);
    pinMode(eIO_BTN_LIM, INPUT_PULLUP);

    lwbtn_init_ex(NULL, btns, sizeof(btns) / sizeof(btns[0]), s_get_states, s_button_event);
}

void button_update(void) {
    lwbtn_process(millis());
}