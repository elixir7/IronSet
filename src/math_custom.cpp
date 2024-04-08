#include <stdint.h>
#include "math_custom.h"

/**
 * @brief Position of most significant bit
 *
 * @param v Value
 * @return int Position of MSB
 */
int msb(uint32_t v) {
    uint32_t r = 0;
    while (v >>= 1) {
        r++;
    }
    return r;
}

/**
 * @brief Natrual logarithm approximation
 * Uses a polynomial approximation of input domain [1,2] and uses math tricks to extend the use for input domain [1/256,
 * 2^24]
 *
 * @param y Value
 * @return float ln(y)
 */
float ln(float y) {
    if (y <= 0) {
        return -1.0f;
    }

    float       divisor, x, result;
    const float LN_2 = 0.69314718; // pre calculated constant used in calculations
    uint32_t    log2 = 0;

    // Scaling factor. The polynomial below is accurate when the input y>1, therefore using a scaling factor of 256 (aka
    // 2^8) extends this to 1/256 or ~0.04. Given use of uint32_t, the input y must stay below 2^24 or 16777216 (aka
    // 2^(32-8)), otherwise uint_y used below will overflow. Increasing the scaing factor will reduce the lower accuracy
    // bound and also reduce the upper overflow bound. If you need the range to be wider, consider changing uint_y to a
    // uint64_t
    const uint32_t SCALING_FACTOR = 256;
    // this is the natural log of the scaling factor and needs to be precalculated
    const float LN_SCALING_FACTOR = 5.545177444;

    y = y * SCALING_FACTOR;
    log2 = msb((uint32_t)y);

    divisor = (float)(1 << log2);
    // Find the remainder value between [1.0, 2.0] then calculate the natural log of this remainder using a polynomial
    // approximation
    x = y / divisor;
    // This polynomial approximates ln(x) between [1,2]
    result = -1.7417939 + (2.8212026 + (-1.4699568 + (0.44717955 - 0.056570851 * x) * x) * x) * x;

    // Using the log product rule Log(A) + Log(B) = Log(AB) and the log base change rule log_x(A) = log_y(A)/Log_y(x),
    // calculate all the components in base e and then sum them: = Ln(x_remainder) + (log_2(x_integer) * ln(2)) -
    // ln(SCALING_FACTOR)
    result = result + ((float)log2) * LN_2 - LN_SCALING_FACTOR;

    return result;
}

/**
 * @brief Absolute value of integer
 *
 * @param v Value
 * @return int Absolute value
 */
int i_abs(int v) {
    return v * ((v > 0) - (v < 0));
}

float f_abs(float v) {
    return v < 0 ? -v : v;
}

/**
 * @brief Round float to nearest integer
 *
 * @param v Value
 * @return int Rounded integer
 */
int i_round(float v) {
    if (v < 0.0f) {
        return (int)(v - 0.5f);
    } else {
        return (int)(v + 0.5f);
    }
}

/**
 * @brief Modulus that returns wrapped value for negative numbers
 * 
 * @param a 
 * @param b 
 * @return int 
 */
int mod(int a, int b) {
    return ((a % b) + b) % b;
}
