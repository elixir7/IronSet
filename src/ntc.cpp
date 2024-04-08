#include "ntc.h"
#include "math_custom.h"

#define ABSOLUTE_ZERO_KELVIN (273.15f)
#define TEMP_MIN_KELVIN      (ABSOLUTE_ZERO_KELVIN - 100)
#define TEMP_MAX_KELVIN      (ABSOLUTE_ZERO_KELVIN + 200)

float NTC_voltage_to_temperature(NTC_t* ntc, uint16_t v_mv, uint16_t v_vdd_mv) {
    if (ntc->resistance_pullup == 0) {
        return -1.0f;
    }

    uint32_t r_ntc = ntc->resistance_pullup * v_mv / (v_vdd_mv - v_mv);
    ntc->resistance_ntc = r_ntc;

    return NTC_resistance_to_temperature(r_ntc, &ntc->beta_coeff);
}

/**
 * @brief Convert thermistor resistance to temperature
 * Uses Beta equation
 *
 * @param R Resistance (Ohm)
 * @param beta_coeff NTC_t struct pointer
 * @return float Temperature (Kelvin)
 */
float NTC_resistance_to_temperature(uint32_t R, NTC_beta_coeff_t* beta_coeff) {
    // Any beta coefficients are zero will result in division by zero
    if (beta_coeff->beta == 0 || beta_coeff->R0 == 0 || beta_coeff->T0 == 0 || R == 0) {
        return -1.0f;
    }

    float    T0 = ABSOLUTE_ZERO_KELVIN + beta_coeff->T0;
    uint32_t R0 = beta_coeff->R0;
    uint16_t B = beta_coeff->beta;

    // Beta Equation
    float T = 1 / (ln((float)R / R0) / B + 1 / T0);

    // Reasonable output
    if (T < TEMP_MIN_KELVIN || T > TEMP_MAX_KELVIN) {
        return -1.0f;
    }

    return T;
}

int NTC_kelvin_2_mili_celcius(float temp) {
    if (temp < 0.0) {
        return 0;
    }
    return i_round((temp - 273.15) * 1000);
}

/**
 * @brief Convert mili Celcius to Kelvin
 *
 * @param temp Temperature (mCelcius)
 * @return float Tmperature (Kelvin)
 */
float NTC_mili_celcius_2_kelvin(int temp) {
    return (float)temp / 1000 + 273.15;
}
