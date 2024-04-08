#include <Arduino.h>
#include <stdint.h>

#include "button.h"
#include "lwbtn.h"
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"
#include "view.h"

// ============= Private variables ===================
const uint32_t     pins[] = {BTN_UP, BTN_DOWN, BTN_SELECT, BTN_LIM};
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
    // return !digitalRead(BTN_UP);
}

static void s_button_event(struct lwbtn* lw, struct lwbtn_btn* btn, lwbtn_evt_t evt) {
    if (evt == LWBTN_EVT_ONPRESS) {
        tone(BUZZER, NOTE_C7, 25);
        digitalWrite(LED_B, HIGH);
        delay(25);
        digitalWrite(LED_B, LOW);
        view_button_event(lw, btn, evt);
    } else if (evt == LWBTN_EVT_KEEPALIVE && btn->keepalive.cnt >= settings_get()->button.hold_off) {
        // Only fire keep alive after a delay
        tone(BUZZER, NOTE_C7, 25);
        digitalWrite(LED_B, HIGH);
        delay(25);
        digitalWrite(LED_B, LOW);
        view_button_event(lw, btn, evt);
    }
}

//======================================================

void button_init(void) {
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_LIM, INPUT_PULLUP);

    lwbtn_init_ex(NULL, btns, sizeof(btns) / sizeof(btns[0]), s_get_states, s_button_event);
}

void button_update(void) {
    lwbtn_process(millis());
}