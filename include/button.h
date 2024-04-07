#ifndef __BUTTON__H__
#define __BUTTON__H__

typedef enum
{
    eBTN_UP = 0,
    eBTN_DOWN,
    eBTN_SELECT,
    eBTN_LIM
} button_type_e;

void button_init(void);
void button_update(void);

#endif