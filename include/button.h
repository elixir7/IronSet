#ifndef __BUTTON__H__
#define __BUTTON__H__

#include "pinmap.h"

typedef enum
{
    eBTN_UP = eIO_BTN_UP,
    eBTN_DOWN = eIO_BTN_DOWN,
    eBTN_SELECT = eIO_BTN_SELECT,
    eBTN_LIM = eIO_BTN_LIM
} button_type_e;

void button_init(void);
void button_update(void);

#endif