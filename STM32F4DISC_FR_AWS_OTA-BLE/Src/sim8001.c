#include "sim8001.h"
#include "string.h"
 
Container_SensorData_Typedef  Container_SensorData;
uint16_t  point_sim800 = 0;

/*sim800  串口接收缓存区 */
char SIM800_RX_BUF[SIM800_MAX_RECV_LEN] = {0}; 		

/*
清空gsm 接收 buffer  同时复位point
*/
void Clear_SIM800_RxBuf(void)                         
{
	 memset(SIM800_RX_BUF,0, SIM800_MAX_RECV_LEN);      //清空
   point_sim800 = 0;                    
}
/*
删除响应部分，更新Point
*/
//void Clear_Response_SIM800_RxBuf(uint16_t pos_start,uint16_t rsplen)
//{
//	memset(SIM800_RX_BUF,0, SIM800_MAX_RECV_LEN);      //清空
//	
//  point_sim800 = point_sim800 - rsplen;     
//	
//}

/*
读取buffer 数据 
*/
uint16_t Read_SIM800_RxBuf(char* dataOutputBuff)                         
{
	 memcpy(dataOutputBuff,SIM800_RX_BUF, point_sim800);   
	 return point_sim800;
}
       

/*
gsm 接收中断处理函数 
*/
void  SIM800ReceiveData(char data)
{ 
	  if(point_sim800 < SIM800_MAX_RECV_LEN)		        //还可以接收数据
		{   
			  SIM800_RX_BUF[point_sim800++] = data;
		}
}

/*
向SIM800 发送数据
*/
void SIM800_Send(char *data)  
{  
    uint16_t len;  
	  len = strlen(data);
 
    HAL_UART_Transmit(&GSM_USART_PORT,(uint8_t *)data,len,0xffff);
}


/*
向SIM800  写一条命令  
*Response 判断响应数据的指针
Timeout  超时时间 单位ms
Retry    最多尝试的次数 最小为1

返回  ERROR  or SUCCESS
*/
uint8_t SendCommand_to_SIM800(char *Command, const char *Response, unsigned long Timeout, unsigned char Retry)
{
	 uint8_t i;
	 unsigned long overtime = 0;
	 uint16_t responseLen = 0;
	  
	 Clear_SIM800_RxBuf();
	
	 for (i= 0; i < Retry; i++)
	 {
		  SIM800_Send(Command); 		 

		  while (overtime < Timeout)
		  {
				 /*延时阻塞*/
			   HAL_Delay(100);         
			   overtime += 100;
 
			   if( strstr(SIM800_RX_BUF,Response) != NULL)
			   {				
					   printf("[ %s ]\r\n",SIM800_RX_BUF);
					   
				     //Clear_SIM800_RxBuf();//190529 只删除响应部分
					   //responseLen  = strlen(strstr(SIM800_RX_BUF,Response)); 
					   //memset(SIM800_RX_BUF+(point_sim800 - responseLen),0, sizeof(Response)); 
					   //Clear_Response_SIM800_RxBuf();//190529 只删除响应部分
					   if(strstr(SIM800_RX_BUF,"+BLESPPWREQ:") != NULL)
						 {      
						      
						 }
						 else
						 {Clear_SIM800_RxBuf();}//190529	
					 
				     return SUCCESS;
			   }
 		  }
		  overtime = 0;
	 }
	 
   Clear_SIM800_RxBuf();
	 return ERROR;
}

/*打开SIM800电源*/
void  Open_SIM800_EN_Port(void)
{
	    /*使能EN引脚 */
	    //GSM_EN_ON(); 
      /*阻塞5s 等待稳定*/
      HAL_Delay(5000);
}


uint8_t SIM800_Net_Init(void)
{
		/* 查看 GPRS 是否 OK */
		if (SendCommand_to_SIM800("AT+CGATT?\r\n", "+CGATT: 1", 5000, 2) == SUCCESS);
		else 
		{
			   return ERROR;
		}
		HAL_Delay(90);
		/* Type  of  Internet  connection is GPRS */
		if (SendCommand_to_SIM800("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n", "OK", 4000, 1) == SUCCESS);
		else 
		{
			   return ERROR;
		}
		HAL_Delay(90);
		/* 设置 APN CMNET cmiot*/
		if (SendCommand_to_SIM800("AT+SAPBR=3,1,\"APN\",\"cmiot\"\r\n", "OK", 4000, 1) == SUCCESS);
		else 
		{
			   return ERROR;
		}
		HAL_Delay(90);
		/* Open bearer */
		if (SendCommand_to_SIM800("AT+SAPBR=1,1\r\n", "OK\r\n", 15000, 1) == SUCCESS);
		else 
		{
				// return ERROR;
		}
		 
		HAL_Delay(90);
		
		return SUCCESS;
}

/*
post data
*/
uint8_t  HTTP_POST(char *data)
{
	  char posttable[300] = {0};
		char ipbuf[200] = {0};
		
		if(SIM800_Net_Init() != SUCCESS)
		{
			   return ERROR;
		}
		
		/* init http */
		if (SendCommand_to_SIM800("AT+HTTPINIT\r\n", "OK", 5000, 1) == SUCCESS);
		else 
		{
			   return ERROR;
		}
		HAL_Delay(90);
	  /* Bearer profile identifier is  CID*/
 		if (SendCommand_to_SIM800("AT+HTTPPARA=\"CID\",1\r\n", "OK", 5000, 1) == SUCCESS);
		else 
		{
			   return ERROR;
		}
		HAL_Delay(90);
	  /* 设置为 URL 设置 ip 地址 Container_SensorData  */
	  sprintf(ipbuf,"AT+HTTPPARA=\"URL\",\"106.15.188.8:80/api/Iridium_str/%x%x%x\"\r\n",
		      Container_SensorData.MControlID[0],Container_SensorData.MControlID[1],Container_SensorData.MControlID[2]);
 		if (SendCommand_to_SIM800(ipbuf , "OK",5000, 1) == SUCCESS);
		else 
		{
				return ERROR;
		}
		HAL_Delay(90);
		
		if(strlen(data) > 0 )
		{
		    sprintf(posttable,"AT+HTTPDATA=%d,10000\r\n",strlen(data));
		}
		else
		{
        return ERROR;
		}
		
	  /* 设置 post长度 和超时时间*/
 		if (SendCommand_to_SIM800(posttable, "DOWNLOAD", 5000, 1) == SUCCESS);
		else 
		{
        return ERROR;
		}
		HAL_Delay(90);
	  /* 填充 post 数据*/
 		if (SendCommand_to_SIM800(data, "OK", 11000, 1) == SUCCESS);
		else 
		{
        return ERROR;
		}
		HAL_Delay(90);
	  /* 执行 */
 		if (SendCommand_to_SIM800("AT+HTTPACTION=1\r\n", "200", 15000, 1) == SUCCESS);
		else 
		{
        return ERROR;
		}
	  /*正常 退出 HTTP 服务 */
 		if (SendCommand_to_SIM800("AT+HTTPTERM\r\n", "OK", 3000, 1) == SUCCESS);
		else 
		{
        return ERROR;
		}
		
		return SUCCESS;
}
