/*
 * Copyright (C)  
 */
 
#include <stdio.h>  
//#include <sim8001.h>
 
 
 
int ble_chat_test( void )
{
     
    printf("This is ble_chat_to_usart print test \r\n \r\n");
	  //if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 2) == SUCCESS)
    //{
			//printf("ble_chat_to_usart: send AT ok \r\n \r\n");
   // }  
	
    return 0;
	
}
/*
向SIM800  写一条命令  
*Response 判断响应数据的指针
Timeout  超时时间 单位ms
Retry    最多尝试的次数 最小为1

返回  ERROR  or SUCCESS
*/
//uint8_t SendCommand_to_SIM800(char *Command, const char *Response, unsigned long Timeout, unsigned char Retry)

