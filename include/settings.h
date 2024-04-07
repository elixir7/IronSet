#ifndef __SETTINGS_H__
#define __SETTINGS_H__

typedef struct
{
    int duration; // [ms]
} hold_timer_t;

typedef struct
{
    int target;
    int current;
    int P;
    int I;
    int D;
} controller_t;

typedef struct {
    int hold_off;
}button_settings_t;

typedef struct
{
    hold_timer_t hold_timer;
    controller_t controller;
    button_settings_t button;
} settings_t;

void settings_init(void);
settings_t *settings_get(void);
void settings_save(void);

#endif