#ifndef __SENSOR__MANAGER__H__
#define __SENSOR__MANAGER__H__

#include <stdint.h>

typedef enum { eSENSOR_TEMP = 0, eSENSOR_24V } sensor_e;

void    sensor_manager_init(void);
void    sensor_manager_update(void);
int32_t sensor_manager_get(sensor_e type);

#endif