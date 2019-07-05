#ifndef _OTA_WRAPPER_H_
#define _OTA_WRAPPER_H_

#include "infra_types.h"

void *HAL_Malloc(uint32_t size);
void HAL_Free(void *ptr);
 
//int sprintf(char *str, const int len, const char *fmt, ...);

int HAL_SetProductKey(char *product_key);
int HAL_SetDeviceName(char *device_name);
int HAL_SetDeviceSecret(char *device_secret);

#endif

