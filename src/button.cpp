#include <Arduino.h>
#include <stdint.h>

#include "lwbtn.h"
#include "button.h"
#include "pinmap.h"
#include "pitches.h"
#include "settings.h"

typedef struct
{
    bool pressed;
    bool play_sound;
    bool blink;
    button_type_e type;
} button_manager_t;

static button_manager_t self;

// ============= Private variables ===================
const uint32_t pins[] = {BTN_UP, BTN_DOWN, BTN_SELECT, BTN_LIM};
static lwbtn_btn_t btns[] = {
    {.arg = (void *)&pins[0]},
    {.arg = (void *)&pins[1]},
    {.arg = (void *)&pins[2]},
    {.arg = (void *)&pins[3]},
};

// ============ Private function declaration =================
static uint8_t s_get_states(struct lwbtn *lw, struct lwbtn_btn *btn);
static void s_button_event(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt);

//============ Private function implementation ===============
static uint8_t s_get_states(struct lwbtn *lw, struct lwbtn_btn *btn)
{
    return !digitalRead(*((uint32_t *)btn->arg));
    // return !digitalRead(BTN_UP);
}

static void s_button_event(struct lwbtn *lw, struct lwbtn_btn *btn, lwbtn_evt_t evt)
{
    if (evt == LWBTN_EVT_ONPRESS)
    {
        tone(BUZZER, NOTE_C7, 50);
        digitalWrite(LED_B, HIGH);
        delay(10);
        digitalWrite(LED_B, LOW);
    }
    else if (evt == LWBTN_EVT_ONRELEASE)
    {
        tone(BUZZER, NOTE_C3, 50);
        digitalWrite(LED_B, HIGH);
        delay(50);
        digitalWrite(LED_B, LOW);
    }
    else if (evt == LWBTN_EVT_ONCLICK)
    {
    }
    else if (evt == LWBTN_EVT_KEEPALIVE)
    {
        digitalWrite(LED_B, HIGH);
        delay(10);
        digitalWrite(LED_B, LOW);
    }

    // TODO: Change modes by clicking X amount of times?
}

//======================================================

void button_init(void)
{
    pinMode(BTN_UP, INPUT_PULLUP);
    pinMode(BTN_DOWN, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);
    pinMode(BTN_LIM, INPUT_PULLUP);

    lwbtn_init_ex(NULL, btns, sizeof(btns) / sizeof(btns[0]), s_get_states, s_button_event);

    // attachInterrupt(digitalPinToInterrupt(BTN_UP), button_ISR_up, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BTN_DOWN), button_ISR_down, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BTN_SELECT), button_ISR_select, FALLING);
    // attachInterrupt(digitalPinToInterrupt(BTN_LIM), button_ISR_lim, FALLING);
}

// void button_ISR_up(void)
// {
//     button_ISR(eBTN_UP);
// }

// void button_ISR_down(void)
// {
//     button_ISR(eBTN_DOWN);
// }

// void button_ISR_select(void)
// {
//     button_ISR(eBTN_SELECT);
// }

// void button_ISR_lim(void)
// {
//     button_ISR(eBTN_LIM);
// }

void button_update(void)
{
    lwbtn_process(millis());
}

// int timer;
// void button_update(void)
// {

//     // TODO: Check for  holding buttons
//     // Requires delay
//     if (!digitalRead(BTN_UP) || !digitalRead(BTN_DOWN) || !digitalRead(BTN_SELECT))
//     {
//         timer++;
//     }
//     else
//     {
//         timer = 0;
//     }

//     if (timer > 40 && timer % 5 == 0)
//     {
//         if (digitalRead(BTN_UP) == 0)
//         {
//             button_ISR(eBTN_UP);
//         }
//         else if (digitalRead(BTN_DOWN) == 0)
//         {
//             button_ISR(eBTN_DOWN);
//         }
//         else if (digitalRead(BTN_DOWN) == 0)
//         {
//             button_ISR(eBTN_SELECT);
//         }
//     }

//     // No hold nor events
//     if (!self.pressed)
//     {
//         return;
//     }

//     // Blink feedback?
//     if (self.blink)
//     {
//         self.blink = false;
//         digitalWrite(LED_B, HIGH);
//         delay(10);
//         digitalWrite(LED_B, LOW);
//     }

//     // Sound feedbck?
//     if (self.play_sound)
//     {
//         self.play_sound = false;
//         tone(BUZZER, NOTE_C7, 50);
//     }

//     // Process button presses
//     // switch (settings_get()->screen.current)
//     // {
//     // case eSCREEN_CONTROLLER:
//     //     break;
//     // case eSCREEN_TIMER:
//     // {
//     //     int change = 0;
//     //     if (self.type == eBTN_UP)
//     //     {
//     //         change = 100;
//     //     }
//     //     else if (self.type == eBTN_DOWN)
//     //     {
//     //         change = -100;
//     //     }

//     //     settings_get()->hold_timer.duration = settings_get()->hold_timer.duration + change;

//     //     if (self.type == eBTN_SELECT)
//     //     {
//     //         settings_save(); // TODO: Saving should only happen while exited a menu
//     //     }
//     //     break;
//     // }

//     // default:
//     // case eSCREEN_DASHBOARD:
//     //     break;
//     // }

//     self.pressed = false;
// }

// void button_ISR(button_type_e type)
// {
//     self.type = type;
//     self.blink = true;
//     self.play_sound = true;
//     self.pressed = true;
// }