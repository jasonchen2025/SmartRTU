/*
 * Copyright (c) 2014-2016 Alibaba Group. All rights reserved.
 * License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */



#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "infra_compat.h"
 
 char _product_key[IOTX_PRODUCT_KEY_LEN + 1]       = "a1MZxOdcBnO";
char _product_secret[IOTX_PRODUCT_SECRET_LEN + 1] = "h4I4dneEFp7EImTv";
char _device_name[IOTX_DEVICE_NAME_LEN + 1]       = "test_01";
char _device_secret[IOTX_DEVICE_SECRET_LEN + 1]   = "t9GmMf2jb3LgWfXBaZD2r3aJrfVWBv56";
//#include "kv.h"
#include "infra_defs.h"

#define __DEMO__

 
 
 

 

 
 


#ifdef __APPLE__
uint64_t HAL_UptimeMs(void)
{
 
}
#else
 
char *HAL_GetTimeStr(_IN_ char *buf, _IN_ int len)
{
 
}
#endif

 

 

 static int kv_get(const char *key, void *value, int *value_len)
{
//    if (!file) {
//        file = kv_open(KV_FILE_NAME);
//        if (!file) {
//            kv_err("kv_open failed");
//            return -1;
//        }
//    }

//    return __kv_get(file, key, value, value_len);
}

 int HAL_SetDeviceSecret(char *device_secret)
{
    int len = strlen(device_secret);

    if (len > IOTX_DEVICE_SECRET_LEN) {
        return -1;
    }
    memset(_device_secret, 0x0, IOTX_DEVICE_SECRET_LEN + 1);
    strncpy(_device_secret, device_secret, len);

    return len;
}
int HAL_Kv_Get(const char *key, void *val, int *buffer_len)
{
    return kv_get(key, val, buffer_len);
}



int HAL_GetPartnerID(char *pid_str)
{
 
}

int HAL_GetModuleID(char *mid_str)
{
 
}


char *HAL_GetChipID(_OU_ char *cid_str)
{
 
}


int HAL_GetDeviceID(_OU_ char *device_id)
{
 
}

int HAL_SetProductKey(_IN_ char *product_key)
{
    int len = strlen(product_key);

    if (len > IOTX_PRODUCT_KEY_LEN) {
        return -1;
    }
    memset(_product_key, 0x0, IOTX_PRODUCT_KEY_LEN + 1);
    strncpy(_product_key, product_key, len);

    return len;
}


int HAL_SetDeviceName(_IN_ char *device_name)
{
    int len = strlen(device_name);

    if (len > IOTX_DEVICE_NAME_LEN) {
        return -1;
    }
    memset(_device_name, 0x0, IOTX_DEVICE_NAME_LEN + 1);
    strncpy(_device_name, device_name, len);

    return len;
}


 


int HAL_SetProductSecret(_IN_ char *product_secret)
{
    int len = strlen(product_secret);

    if (len > IOTX_PRODUCT_SECRET_LEN) {
        return -1;
    }
    memset(_product_secret, 0x0, IOTX_PRODUCT_SECRET_LEN + 1);
    strncpy(_product_secret, product_secret, len);

    return len;
}

  




 
/*
 * This need to be same with app version as in uOTA module (ota_version.h)

    #ifndef SYSINFO_APP_VERSION
    #define SYSINFO_APP_VERSION "app-1.0.0-20180101.1000"
    #endif
 *
 */
int HAL_GetFirmwareVesion(_OU_ char *version)
{
 
}

void *HAL_SemaphoreCreate(void)
{
 
}

void HAL_SemaphoreDestroy(_IN_ void *sem)
{
 
}

void HAL_SemaphorePost(_IN_ void *sem)
{
    
}

int HAL_SemaphoreWait(_IN_ void *sem, _IN_ uint32_t timeout_ms)
{
 
}

int HAL_ThreadCreate(
            _OU_ void **thread_handle,
            _IN_ void *(*work_routine)(void *),
            _IN_ void *arg,
//            _IN_ hal_os_thread_param_t *hal_os_thread_param,
            _OU_ int *stack_used)
{
  
}

void HAL_ThreadDetach(_IN_ void *thread_handle)
{
   
}

void HAL_ThreadDelete(_IN_ void *thread_handle)
{
 
}

static FILE *fp;

#define otafilename "/tmp/alinkota.bin"

void HAL_Firmware_Persistence_Start(void)
{
 
}

int HAL_Firmware_Persistence_Write(_IN_ char *buffer, _IN_ uint32_t length)
{
 
}

int HAL_Firmware_Persistence_Stop(void)
{
 
}

int HAL_Config_Write(const char *buffer, int length)
{
 
}

int HAL_Config_Read(char *buffer, int length)
{
  
}

#define REBOOT_CMD "reboot"
void HAL_Sys_reboot(void)
{
 
}

#define ROUTER_INFO_PATH        "/proc/net/route"
#define ROUTER_RECORD_SIZE      256

char *_get_default_routing_ifname(char *ifname, int ifname_size)
{
 
}


uint32_t HAL_Wifi_Get_IP(char ip_str[NETWORK_ADDR_LEN], const char *ifname)
{
 
}

//static kv_file_t *kvfile = NULL;

int HAL_Kv_Set(const char *key, const void *val, int len, int sync)
{
   
}

int HAL_Kv_Del(const char *key)
{
 
}

static long long os_time_get(void)
{
 
}

static long long delta_time = 0;

void HAL_UTC_Set(long long ms)
{
  
}

long long HAL_UTC_Get(void)
{
    
}

void *HAL_Timer_Create(const char *name, void (*func)(void *), void *user_data)
{
 
}

int HAL_Timer_Start(void *timer, int ms)
{
 
}

int HAL_Timer_Stop(void *timer)
{
 
}

int HAL_Timer_Delete(void *timer)
{
    
 
}

void HAL_Reboot(void)
{
   
}

