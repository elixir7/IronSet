#include <Arduino.h>
#include <stdbool.h>
#include <stdint.h>

#include "filter.h"
#include "math_custom.h"
#include "ntc.h"
#include "pinmap.h"
#include "sensor_manager.h"

#define ADC_MAX_VALUE (1024)
#define VCC           (3.3f)
#define SHIFT_DEFAULT (6)

typedef struct {
    filter_lp_t filter;
    NTC_t       settings;
} ntc_settings_t;

typedef struct {
    bool           initial;
    ntc_settings_t ntc;
    filter_lp_t    u_filter;
} sensor_manager_t;

static sensor_manager_t self;

void sensor_manager_init(void) {
    pinMode(eIO_NTC, INPUT_ANALOG);
    pinMode(eIO_SENSE_24V, INPUT_ANALOG);

    self.ntc.settings = {.beta_coeff = {.T0 = 25, .R0 = 100000, .beta = 4000}, .resistance_pullup = 4700};

    self.initial = true;
    sensor_manager_update();
}

void sensor_manager_update(void) {

    // 24V measurement
    int   counts = analogRead(eIO_SENSE_24V);
    float vin = 3.3f;
    float R1 = 10e3;
    float R2 = 1e3;
    float v = counts * vin / ADC_MAX_VALUE;
    v = (v * (R1 + R2)) / R2;
    float v_mv = v * 1000; // Convert to mV
    if (self.initial) {
        filter_lp_init(&self.u_filter, v_mv, SHIFT_DEFAULT);
    } else {
        filter_lp_update(&self.u_filter, v_mv);
    }

    // eIO_NTC measurement
    counts = analogRead(eIO_NTC);
    v = counts * vin / ADC_MAX_VALUE;
    v_mv = v * 1000; // Convert to mV
    float temp = NTC_voltage_to_temperature(&self.ntc.settings, v_mv, VCC * 1000);
    if (self.initial) {
        filter_lp_init(&self.ntc.filter, NTC_kelvin_2_mili_celcius(temp), SHIFT_DEFAULT);
    } else {
        filter_lp_update(&self.ntc.filter, NTC_kelvin_2_mili_celcius(temp));
    }

    if (self.initial) {
        self.initial = false;
    }
}

int32_t sensor_manager_get(sensor_e type) {
    switch (type) {
        case eSENSOR_TEMP: return filter_lp_get(&self.ntc.filter);
        case eSENSOR_24V: return filter_lp_get(&self.u_filter);
        default: return 0;
    }

    return 0;
}
