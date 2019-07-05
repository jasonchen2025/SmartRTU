#ifndef __SIM8001_H
#define __SIM8001_H
#include "stm32f4xx_hal.h"
#include "usart.h"


#define SIM800_MAX_RECV_LEN		600					//最大接收缓存字节数
#define GSM_USART_PORT        huart2

typedef struct 
{
	 unsigned char Flag_Restart;
	 unsigned char Flag_Restart_Check;
	 unsigned char Flag_RetrySend;
	 unsigned char Flag_StartWork;
	 unsigned char RetryCount_ByCellar;
	 unsigned char Flag_Cellar;
	 unsigned char Flag_RestartCount;
   unsigned char Flag_ReadySend;	 
}SIM800_Class;

extern uint8_t SendCommand_to_SIM800(char *Command, const char *Response, unsigned long Timeout, unsigned char Retry);
extern void  SIM800ReceiveData(char data);
extern void  Open_SIM800_EN_Port(void);
extern uint8_t  HTTP_POST(char *data);
extern uint8_t SIM800_Net_Init(void);


typedef  struct
{ 
	 uint8_t battery;
	 uint8_t  door_status;
   uint8_t IirdiumID[15];
	 uint16_t temperature;
	 uint16_t humidity;
	 uint32_t light;
	 uint32_t MControlID[3];
}Container_SensorData_Typedef;


#endif

