#ifndef __SETTINGS_H__
#define __SETTINGS_H__

typedef struct
{
    int duration; // [ms]
} hold_timer_t;

typedef struct
{
    int target;
} controller_t;

typedef enum
{
    eSCREEN_DASHBOARD = 0,
    eSCREEN_TIMER,
    eSCREEN_CONTROLLER
} screen_e;

typedef struct
{
    screen_e current;
} screen_t;

typedef struct
{
    hold_timer_t hold_timer;
    controller_t controller;
    screen_t screen;
} settings_t;

void settings_init(void);
settings_t *settings_get(void);
void settings_save(void);

#endif