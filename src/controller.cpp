#include <Arduino.h>

#include "PID.h"
#include "controller.h"
#include "math_custom.h"
#include "pinmap.h"
#include "sensor_manager.h"
#include "settings.h"

#define KP           (0.01f)
#define KI           (0.0f)
#define KD           (0.0f)
#define TAU          (0.0f)
#define LIM_MIN      (0.0f)
#define LIM_MAX      (1.0f)
#define LIM_INT_MIN  (0.0f)
#define LIM_INT_MAX  (LIM_MAX * 0.8f)
#define CONTROL_FREQ (10)
#define SAMPLE_TIME  (1.0f / CONTROL_FREQ)

pid_controller_custom_t controller = {0};

void it_callback() {
    // digitalToggle(eIO_LEB_B);
    controller_update();
}

void controller_init(void) {
    pinMode(eIO_HEAT1, OUTPUT);
    pinMode(eIO_HEAT2, OUTPUT);

    controller = (pid_controller_custom_t){
        .Kp = KP,
        .Ki = KI,
        .Kd = KD,
        .tau = TAU,
        .limMin = LIM_MIN,
        .limMax = LIM_MAX,
        .limMinInt = LIM_INT_MIN,
        .limMaxInt = LIM_INT_MAX,
        .T = SAMPLE_TIME,
    };

    pid_init(&controller);

    TIM_TypeDef*   Instance = TIM1;
    HardwareTimer* tim = new HardwareTimer(Instance);

    tim->setOverflow(CONTROL_FREQ, HERTZ_FORMAT);
    tim->attachInterrupt(it_callback);
    tim->resume();
}

void controller_update(void) {
    if (settings_get()->controller.enabled) {
        float T_measurement = sensor_manager_get(eSENSOR_TEMP) / 1000;
        float T_setpoint = settings_get()->controller.target;
        float output = pid_update(&controller, T_setpoint, T_measurement);
        int   analog_output = i_round(output * 255); // Round
        analogWrite(eIO_HEAT1, analog_output);
        analogWrite(eIO_HEAT2, analog_output);
        analogWriteFrequency(1000);
    } else {
        // analogWrite(eIO_HEAT1, 0);
        // analogWrite(eIO_HEAT2, 0);
        // analogWriteFrequency(1000);
    }
}