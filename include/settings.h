#ifndef __SETTINGS_H__
#define __SETTINGS_H__

typedef struct {
    int duration; // [ms]
    // TODO: Add min/max
} hold_timer_settings_t;

typedef struct {
    bool enabled;
    int  target;
    int  max;
    int  min;
} controller_settings_t;

typedef struct {
    int hold_off;
} button_settings_t;

typedef struct {
    hold_timer_settings_t hold_timer;
    controller_settings_t controller;
    button_settings_t     button;
} settings_t;

void        settings_factory_reset(void);
void        settings_init(void);
settings_t* settings_get(void);
void        settings_save(void);

#endif