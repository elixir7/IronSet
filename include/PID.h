#ifndef __PID__H__
#define __PID__H__

typedef struct {

    /* Controller gains */
    float Kp;
    float Ki;
    float Kd;

    /* Derivative low-pass filter time constant */
    float tau;

    /* Output limits */
    float limMin;
    float limMax;

    /* Integrator limits */
    float limMinInt;
    float limMaxInt;

    /* Sample time (in seconds) */
    float T;

    /* Controller "memory" */
    float integrator;
    float prevError; /* Required for integrator */
    float differentiator;
    float prevMeasurement; /* Required for differentiator */

    /* Controller output */
    float out;

} pid_controller_custom_t;

void  pid_init(pid_controller_custom_t* pid);
float pid_update(pid_controller_custom_t* pid, float setpoint, float measurement);

#endif
