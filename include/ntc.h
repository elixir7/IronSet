#ifndef __NTC_H__
#define __NTC_H__
/* ================================================================================================================= */
/* ================================================================================================================= */
/* [INCL] Includes                                                                                                   */
/* ================================================================================================================= */

/* ----------------------------------  Standard  ------------------------------------------------------------------- */
#include <stdbool.h>
#include <stdint.h>

/* ------------------------------------  SDK  ---------------------------------------------------------------------- */

/* -----------------------------------  Plejd  --------------------------------------------------------------------- */
/* -----------------------------------  Local  --------------------------------------------------------------------- */

/* ================================================================================================================= */
/* [DEFS] Defines                                                                                                    */
/* ================================================================================================================= */

/* ================================================================================================================= */
/* [TYPE] Type definitions                                                                                           */
/* ================================================================================================================= */
typedef struct {
    uint16_t T0;   // Celcius
    uint32_t R0;   // Ohm
    uint16_t beta; // Kelvin
} NTC_beta_coeff_t;

typedef struct {
    NTC_beta_coeff_t beta_coeff;
    uint32_t         resistance_pullup; // Ohm
    uint32_t         resistance_ntc;    // Ohm
} NTC_t;

/* ================================================================================================================= */
/* [FDEF] Function declarations                                                                                      */
/* ================================================================================================================= */

float NTC_voltage_to_temperature(NTC_t* ntc, uint16_t v_mv, uint16_t v_vdd_mv);
float NTC_resistance_to_temperature(uint32_t R, NTC_beta_coeff_t* beta_coeff);

int   NTC_kelvin_2_mili_celcius(float temp);
float NTC_mili_celcius_2_kelvin(int temp);

#endif /* __NTC_H__ */
