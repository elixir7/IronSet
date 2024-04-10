
#ifndef __PINMAP__H__
#define __PINMAP__H__
#include <Arduino.h>

typedef enum {
    eIO_LED_R = PA10,
    eIO_LEB_B = PA9,
    eIO_BTN_UP = PB0,
    eIO_BTN_SELECT = PB1,
    eIO_BTN_DOWN = PB2,
    eIO_BTN_LIM = PA3,
    eIO_BUZZER = PB4,
    eIO_SENSE_24V = PA4,
    eIO_NTC = PA5,
    eIO_HEAT1 = PA1,
    eIO_HEAT2 = PA2
} io_e;

// #define eIO_LED_R      (PA10)
// #define eIO_LEB_B      (PA9)
// #define eIO_BTN_UP     (PB0)
// #define eIO_BTN_SELECT (PB1)
// #define eIO_BTN_DOWN   (PB2)
// #define eIO_BTN_LIM    (PA3)
// #define eIO_BUZZER     (PB4)

// #define eIO_SENSE_24V  (PA4)
// #define eIO_NTC        (PA5)

// #define eIO_HEAT1      (PA1)
// #define eIO_HEAT2      (PA2)

#endif