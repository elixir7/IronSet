#ifndef __BUTTON__H__
#define __BUTTON__H__

typedef enum
{
    eBTN_UP = 0,
    eBTN_DOWN,
    eBTN_SELECT,
    eBTN_LIM
} button_type_e;

typedef enum
{
    eRELEASED = 0,
    eINITIAL_PRESS,
    eHOLD,
} button_press_state_t;

void button_init(void);

void button_ISR_up(void);
void button_ISR_down(void);
void button_ISR_select(void);
void button_ISR_lim(void);

void button_ISR(button_type_e type);
void button_update(void);

#endif