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
��SIM800  дһ������  
*Response �ж���Ӧ���ݵ�ָ��
Timeout  ��ʱʱ�� ��λms
Retry    ��ೢ�ԵĴ��� ��СΪ1

����  ERROR  or SUCCESS
*/
//uint8_t SendCommand_to_SIM800(char *Command, const char *Response, unsigned long Timeout, unsigned char Retry)

