#ifndef __GRAPHICS__H__
#define __GRAPHICS__H__
#include <pgmspace.h>

typedef struct {
    int                         width;
    int                         height;
    const PROGMEM unsigned char bitmap[];
} bitmap_t;

extern bitmap_t bitmap_seel;
extern bitmap_t bitmap_temp;
extern bitmap_t bitmap_clock;
extern bitmap_t bitmap_heat;
extern bitmap_t bitmap_clock_large;
extern bitmap_t bitmap_hourglass;

#endif