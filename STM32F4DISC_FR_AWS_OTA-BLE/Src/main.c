
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  ** This notice applies to any and all portions of this file
  * that are not between comment pairs USER CODE BEGIN and
  * USER CODE END. Other portions of this file, whether 
  * inserted by the user or by software development tools
  * are owned by their respective copyright owners.
  *
  * COPYRIGHT(c) 2019 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_hal.h"
#include "usart.h"
#include "gpio.h"
#include "string.h"
#include "stdio.h"

/* USER CODE BEGIN Includes */
#include "sim8001.h"
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/
extern uint8_t  rec_table[10] ;
/* USER CODE END PV */
uint8_t chrtodec(char chr);
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);


/* USER CODE BEGIN PFP */
/* Private function prototypes -----------------------------------------------*/

/* USER CODE END PFP */

/* USER CODE BEGIN 0 */
extern int ota_test( void );
extern int ble_chat_test( void );
extern uint16_t  Read_SIM800_RxBuf(char* dataOutputBuff);
extern void Clear_SIM800_RxBuf(void);
/* USER CODE END 0 */

//#define 	BT_SPP_MC_APP_MODE
#define		BLE_SPP_MODE
//#define		BLE_GATT_Service_MODE
//#define		BLE_SPP_RxTx_MODE
//#define		BLESPP_GATT_RxTx_MODE

/**
  * @brief  The application entry point.
  *
  * @retval None
  */
int main(void)
{
  /* USER CODE BEGIN 1 *//* USER CODE END 1 */

  /* MCU Configuration----------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */
  uint8_t test[2] = {0};
  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */
	
  HAL_UART_Receive_IT(&huart2, &rec_table[2], 1);
	HAL_UART_Receive_IT(&huart3, &rec_table[3], 1);
	
	
		printf("  AT+BTPOWER=1 test ok \r\n \r\n");
	
	
	while(1){}
	
	uint8_t BLE_onoff_status = 0x00;
		//01308562SKY9F99
   if (SendCommand_to_SIM800("AT+BTHOST=01308562SKY9F88\r\n", "OK", 2000,  1) == SUCCESS)
   {       
 			 	printf("7.AT+BTHOST=xxxx write HostName ok \r\n \r\n");
   }  
	
		 #ifdef BLE_SPP_MODE   //*********************************BLESPP***************************************************************************************************************
  /* test for BLE_SPP_MODE模式;  */ 
	printf("This is BLE_SPP_MODE test-190527 \r\n \r\n");
  if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 1) == SUCCESS)
   {
				 	printf("1. Module AT test ok \r\n \r\n");
   } 
  if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR", 3000, 1) == SUCCESS)
  {
				 	printf("2. AT+BTPOWER? test ok \r\n \r\n");
  } 
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTPOWER=1\r\n", "OK", 2000, 1) == SUCCESS)
  {       
				 	printf("3. AT+BTPOWER=1 test ok \r\n \r\n");
  } 
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR:1", 3000, 1) == SUCCESS)
  {
				 	printf("4. AT+BTPOWER? = +BTPWR:1  powerOn \r\n \r\n");
  }  
	if (SendCommand_to_SIM800("AT+BTHOST?\r\n", "OK", 2000, 2) == SUCCESS)
  {        //HAL_Delay(1000);
				 	printf("6. AT+BTHOST? test ok \r\n \r\n");
  } 

	
  if (SendCommand_to_SIM800("AT+BTHOST?\r\n", "OK", 2000, 1) == SUCCESS)
  {        //HAL_Delay(1000);
				 	printf("6. AT+BTHOST? test ok \r\n \r\n");
  } 
	/******BLE-----------------------------*/
//	if (SendCommand_to_SIM800("AT+BLEADDR?\r\n", "+BLEADDR", 2000, 2) == SUCCESS)
//  {         
//				 	printf("15.AT+BLEADDR? test ok \r\n \r\n");
//  }  
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 1) == SUCCESS)
  {         
				 	printf("16. AT+BLESTATUS? test ok \r\n \r\n");
  }   
	printf("____ BLESPP  start ....\r\n");
  ///******开启BLESPP服务 AT+BLESPP=1  注册spp 
  if (SendCommand_to_SIM800("AT+BLESPP=1\r\n", "OK", 2000, 1) == SUCCESS)
  {         
				 	printf("19. AT+BLESPP=1 regi BLESPP server ok \r\n \r\n");
  }  
	HAL_Delay(1000);
	///******查询BLESPP状态 AT+BLESTATUS?
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 1) == SUCCESS)
  {         
				 	printf("AT+BLESTATUS? test ok \r\n \r\n");
  }  
	uint8_t  ble_counter =0;
	uint16_t  tempRecvDataLen =0;
	uint8_t  userDataLen =0;
	uint8_t  userDataHexLen =0;
	uint8_t  temp_idx  =0;
	char SIM800_RX_BUF_copy[600] = {0}; 	
	char BLE_RX_DataBuff[1000] = {0}; 
	char userData[600] ={0};
	char userID_RawData[600] ={0};
	char PktData_element_Forward[600] ={0};
	uint8_t userDataHex[300] ={0};
	memset(userData,'\0',sizeof(userData)); 
	memset(userDataHex,'\0',sizeof(userDataHex)); 
	
	uint8_t  pos_idx  =0; 
  uint8_t  curSOP_posPtr = 1;
  uint8_t  curEOP_posPtr = 0;
  uint8_t  curPkt_Len = 0;
  uint8_t  curPkt_TypeVal = 0;
	uint8_t  curframe[300] ={0};  
  uint8_t  curframelen = 0; 
	uint8_t  curPktExpectedLen = 0;
	uint8_t  SopEopFinsh_Flag = 0;//   ---80 succ , 83 fail, idle 0  0125
  uint8_t  SopFindInFrame_Flag = 0;//   ---80 succ , 83 fail, idle 0  0124
	
	uint8_t BTheartStatus = 0;
  uint8_t BT_heartAck_RecvStatus = 0;  // ---  80 recvd, 83 wait for Ack , 0 idle
  uint8_t BTheart_Reset = 0;            // ---80  reset heart timing
	
	uint8_t  userID_val[10]={0};
	uint8_t  userID_len=0;
	
	uint16_t  tmp_len=0;
	uint16_t  tmp_tail_len=0;
	
	uint8_t ack_heart_notbind[5] = {0xAB,0x00,0x00,0x00,0xCD};
	uint8_t heart_notbind[5] = {0xAB,0x01,0x00,0x00,0xCD};
	char  uID_heart_packet[100]={0xAB,0x01,0x00,0x01,0x01,0xCD}; 
	char   data_byte_sendbuff[600]; 
	
	uint16_t  rawBLEData_len=0;
	uint16_t  BLESPPWREQ_matched_Len=0;
	uint16_t  RTCR_matched_Len=0; 
	uint16_t  temp_BLEDataFrag_len=0; 
	uint16_t  temp_fragment_counter=0; 
	 
	char raw_BLEData_buff[600] = {0}; 
  char temp_raw_BLEData_buff[600] = {0};	
	uint8_t newID_heartStatus = 0;
	/*-------------query, send & receive BLE data-----------------------------*/ 
	while (1)
  {  HAL_Delay(600);
		 memset(SIM800_RX_BUF_copy, 0, sizeof(SIM800_RX_BUF_copy)); 
		 tempRecvDataLen = Read_SIM800_RxBuf(SIM800_RX_BUF_copy);
		 if(tempRecvDataLen>1) {   
			  printf("__UART__200msRecvData<%s>,len<%d>,\r\n",SIM800_RX_BUF_copy,tempRecvDataLen);  
		    Clear_SIM800_RxBuf();
		 } //
		 if( strstr(SIM800_RX_BUF_copy,"+BLESPPCON: 1") != NULL){	//+BLESPPCON: 1		 
				     printf("$2:+BLESPPCON  BLE CONNECTED success received ok \r\n \r\n");  
		 } 
		 if( strstr(SIM800_RX_BUF_copy,"+BLESPPCON: 0") != NULL){			 
				     printf("$2:+BLESPPCON  BLE DISCONNECTED -fail -- ok \r\n \r\n");  
		 } 
		 /*+BLESPPWREQ   assemble BLE data packet: SIM800_RX_BUF_copy --> BLE_RX_DataBuff[1000] */
		 rawBLEData_len = tempRecvDataLen;
		 memset(raw_BLEData_buff, 0, sizeof(raw_BLEData_buff)); 
		 //strcpy(raw_BLEData_buff,SIM800_RX_BUF_copy); 
		 strcpy(raw_BLEData_buff,strstr(SIM800_RX_BUF_copy,"+BLESPPWREQ:"));
		 temp_fragment_counter = 0; 
		 //-----SIM800_RX_BUF_copy解析的分片装入//userData , userDataLen 数据帧字符串 中
		 memset(SIM800_RX_BUF_copy, 0, sizeof(SIM800_RX_BUF_copy)); 
		 memset(userData, 0, sizeof(userData)); 
		 userDataLen = 0;
		 while(rawBLEData_len>31)//+BLESPPWREQ
		 {   
			 BLESPPWREQ_matched_Len = strlen(strstr(raw_BLEData_buff,"+BLESPPWREQ:"));
			 //printf("$---frag--assamble 1:origin{%s ,rawBLEData_len=%d,BLESPPWREQ_matched_Len=%d}\r\n",raw_BLEData_buff,rawBLEData_len,BLESPPWREQ_matched_Len);   
			 if(BLESPPWREQ_matched_Len>31)
				 {
				  //RTCR_matched_Len = strlen(strstr(raw_BLEData_buff,"\r\n"));//Len1-31-len2 =dataLen 
				  RTCR_matched_Len = strlen(strstr(raw_BLEData_buff+31,"+BLE"));
					//printf("$---frag--assamble 2: origin{%s ,RTCR_matched_len=%d}r\n",strstr(raw_BLEData_buff+31,"+BLE"),RTCR_matched_Len);
				  if(BLESPPWREQ_matched_Len>70)
					{// BQ_len1 - BQ_len2>=40+31=71   多片
						//printf("$---frag--assamble... \r\n");
				    temp_BLEDataFrag_len =  BLESPPWREQ_matched_Len - RTCR_matched_Len -31-4;//CD之后的换行符占用2~4字节
						if( temp_BLEDataFrag_len >40 ) temp_BLEDataFrag_len =40 ;
						if(temp_BLEDataFrag_len>0)//CurFragData is valid
						{  temp_fragment_counter++;
							 memset(temp_raw_BLEData_buff, 0, sizeof(temp_raw_BLEData_buff)); 
							 strcpy(temp_raw_BLEData_buff,(strstr(raw_BLEData_buff,"+BLESPPWREQ:")+31));//
							 //printf(" #temp_raw_BLEData_buff-data -cut+BQmac,: {%s} r\n",temp_raw_BLEData_buff); 
							 printf(" #%d pkt_frag data: {",temp_fragment_counter); 
							 
							 for( temp_idx=0;temp_idx<temp_BLEDataFrag_len;temp_idx++)
							 {
								  userData[userDataLen+temp_idx] = temp_raw_BLEData_buff[temp_idx];
							    printf("%c",temp_raw_BLEData_buff[temp_idx]); 
							 }
							 userDataLen = userDataLen+temp_BLEDataFrag_len;
							 printf(" },frag_data_len=%d .",temp_BLEDataFrag_len); 
							 
							 memset(raw_BLEData_buff, 0, sizeof(raw_BLEData_buff)); 
							 rawBLEData_len = BLESPPWREQ_matched_Len - 31 - temp_BLEDataFrag_len;
							 //printf("remain_rawBLEData_len = %d,",rawBLEData_len); 
							 strcpy(raw_BLEData_buff,(strstr(temp_raw_BLEData_buff,"+BLESPPWREQ:"))); 
							 //printf(" **cutoff #%d pkt_frag data,remain_rawBLEData: %s",temp_fragment_counter,raw_BLEData_buff); 
							 if(rawBLEData_len<32)
							 {
							     break;
							 }
						}
				  }
					else
					{// BQ_len1 - BQ_len2<40+31=71 单片
					  memset(temp_raw_BLEData_buff, 0, sizeof(temp_raw_BLEData_buff)); 
							 strcpy(temp_raw_BLEData_buff,(strstr(raw_BLEData_buff,"+BLESPPWREQ:")+31));//
						   temp_BLEDataFrag_len = strlen(temp_raw_BLEData_buff);
						
						   for( temp_idx=0;temp_idx<temp_BLEDataFrag_len;temp_idx++)
							 {
								  userData[userDataLen+temp_idx] = temp_raw_BLEData_buff[temp_idx]; 
							 }
						   userDataLen = userDataLen+temp_BLEDataFrag_len;
							 temp_fragment_counter++;
							 printf("\r\n#_%d pkt_frag data: {%s}",temp_fragment_counter,temp_raw_BLEData_buff);   
						   rawBLEData_len =0;
					}
			   
		   }
			 else
			 {
			    break;
			 }
		 } 
		  
			    /*-------------包解析-----------------------------*/ 
			    if(userDataLen>4){
						 userData[userDataLen]='\0';
						 //printf("$3:resolve  AB frameAssembled data - len=%d: \r\n {{%s}} \r\n",userDataLen,userData ); 
					  userDataHexLen = (userDataLen - 2)/2;
						for( temp_idx=0;temp_idx<userDataLen;temp_idx++)
						{  //AB=171 = 10*16+11 
              userDataHex[temp_idx] = (chrtodec(userData[2*temp_idx+0]))*16 + (chrtodec(userData[2*temp_idx+1])); 
						}
					 for( temp_idx=0;temp_idx<userDataHexLen;temp_idx++)
			     {
					      printf("%X.",userDataHex[temp_idx]);  
					 }
					 printf(",HexUserDalaLen=%d\r\n",userDataHexLen);
					 curSOP_posPtr = 0;
					 curframelen = userDataHexLen;
					 for( temp_idx=0;temp_idx<userDataHexLen;temp_idx++)
			     { 
						  curframe[temp_idx] =userDataHex[temp_idx]; 
					 }  
					 /*------------帧识别-----------------------------*/ 
					  while(curSOP_posPtr< (curframelen-3))
					  { 
//							 printf(",#%d,", curframe[curSOP_posPtr]);
						   if(curframe[curSOP_posPtr] == 0xab)
							 { //----------find sop 0xAB---------------
								 // printf(",1-1\r\n" ); 
                        if((curframe[curSOP_posPtr+1] == 0x00)|(curframe[curSOP_posPtr+1] == 0x01)|(curframe[curSOP_posPtr+1] == 0x02)) 
                        {//----------------------type00 01 02 filter--------------------------  
                          curPkt_Len = curframe[curSOP_posPtr+2]*256 + curframe[curSOP_posPtr+3];  
                          curPktExpectedLen = curPkt_Len; 
                          if((curSOP_posPtr+curPkt_Len+3)<curframelen) //--{0xAB,0x02,0x00,0x03,0xAA,0xBB,0xCC,0xCD}  
                          { 
                             printf("&10_BLE_DataFrameLen_valid.\r\n"); 
                             //RS232Handle:writestr(frame:byte(curSOP_posPtr+curPkt_Len+4))
													   //printf("&curSopPos=%d,curPktLen=%d,curframelen=%d\r\n",curSOP_posPtr,curPkt_Len,curframelen); 
                            if(curframe[curSOP_posPtr+curPkt_Len+4] == 0xcd)
														{
                             //then -----pick the pkt
                                  if((curSOP_posPtr+curPkt_Len+4)>(curframelen-4)) 
                                  {
                                    SopEopFinsh_Flag = 80; //--0125  no frament
                                   // printf("&19_.\r\n");   
                                  }
                                  //--------------type 00 -----------------
                                  if(curframe[curSOP_posPtr+1] == 0x00)// --ACK
                                  { 
                                    //PktData_element = string.sub(curframe,curSOP_posPtr,curSOP_posPtr+4)
                                    printf("&11_BLE_type0_ACK_received.\r\n");  					 
                                    //EnQueueAckTx(PktData_element)
                                    BTheart_Reset = 80; 
                                    //cacheSateDataToBT_AckStatus = 80; // ---recv ack , enable toBTsend   
                                    curSOP_posPtr = curSOP_posPtr +4;   
																	}
                                  //--------------type 01 ----------------recv heartPkt----0124  
                                  else if(curframe[curSOP_posPtr+1] == 0x01) //--type = 0x01 
                                  {  
                                    printf("&18_BLE_type1_Heart_received\r\n");  
                                    BTheart_Reset = 80;  
                                   // cacheSateDataToBT_AckStatus = 80; // ---recv heartPkt , enable toBTsend  
                                    curSOP_posPtr = curSOP_posPtr +4;   
//																		memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
//                                    sprintf(data_byte_sendbuff,"%s%s%s","AT+BLESPPSIND=","12345","\r\n");//	 																			
//                                    printf("&uint8-to-byteString:data_byte_sendbuff=%s \r\n",data_byte_sendbuff); 
//																		SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);// {0xAB,0x00,0x00,0x00,0xCD}
//		                                printf("$-:SendAckToBLEapp-0xAB,0x00,0x00,0x00,0xCD----:  \r\n");
																		memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
																		sprintf(data_byte_sendbuff,"%s%02x%02x%02x%02x%02x%s","AT+BLESPPSIND=",ack_heart_notbind[0],ack_heart_notbind[1],ack_heart_notbind[2],ack_heart_notbind[3],ack_heart_notbind[4],"\r\n");																		
                                    SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);// {0xAB,0x00,0x00,0x00,0xCD}
																		 //printf("$-:SendAckToBLEapp- heart ack, finish --:  \r\n");
//																		memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
//																		sprintf(data_byte_sendbuff,"%s%02x%02x%02x%02x%02x%s","AT+BLESPPSIND=",ack_heart_notbind[0],0x01,0x7e,ack_heart_notbind[3],ack_heart_notbind[4],"\r\n");																		
//                                    SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);// {0xAB,0x00,0x00,0x00,0xCD}
//																		memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
//																		sprintf(data_byte_sendbuff,"%s%s%s","AT+BLESPPSIND=","12345","\r\n");//	 	
//																					data_byte_sendbuff[14] =0x39;
//																		data_byte_sendbuff[15] =0xab; 	
//																		data_byte_sendbuff[16] =0x00; 	
//																		data_byte_sendbuff[17] =0xcd; 	
//																		data_byte_sendbuff[18] =0x39; 	
//																		SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);//  
//																		 printf("$-:SendAckToBLEapp- xab,0x00, finish --:  \r\n");
																	}
                                  //--------------type 02 -----------------  
                                  else if(curframe[curSOP_posPtr+1] == 0x02) //--type = 0x02   
                                  {  //userData , userDataLen 数据帧字符串
																		//payload filter --取出ID   Payload = 12|G|api/user/1/device/bind?device_id=1&foo=bar
																		//'/device/bind' = HEX   2F6465766963652F62696E64   
																		//  user/= HEX 75 73 65 72 2f
																		 printf("&12_BLE_type2_HTTPup_received  \r\n");    
                                    //uint8_t bindpos_start,bindpos_end=string.find(string.sub(curframe,(curSOP_posPtr+4),(curSOP_posPtr+curPkt_Len+3))//, '/device/bind')
                                    if(strstr(userData,"2F6465766963652F62696E64") != NULL)
																		{  
																			 printf("&12_BLE_type2_HTTPup_Bind_resolve... \r\n");  
  																		 //printf("$-: (/device/bind) searched--  \r\n");
																			 tmp_len = strlen(strstr(userData,"757365722F")); 
																			 //printf("$-: (user/) searched--, len=%d  \r\n",tmp_len);
																			 tmp_tail_len = strlen(strstr(userData,"2F6465766963652F62696E64"));
																			  //printf("$-:device/bind searched--, len=%d:  \r\n",tmp_tail_len); 
																			if(tmp_len-tmp_tail_len>10)//get user ID
																			{
																				 userID_len=(tmp_len-tmp_tail_len-10);  
			                                   strcpy(userID_RawData,strstr(userData,"757365722F")); 
																				 for( temp_idx=0;temp_idx<userID_len;temp_idx++)
																				 { 
																						userID_val[temp_idx] =userID_RawData[temp_idx+10]; 
																				 }  
																		  }   
																			if(userID_len>0)
																		  { 
																		 	printf("&newBind_userID val={ " ); 
																			for( temp_idx=0;temp_idx<userID_len;temp_idx++)//userID_len/2=userID_Hexlen
																			{
																					printf("%c",userID_val[temp_idx] ); 
																			}
																			printf("},userID_len= %d\r\n",userID_len);   
																			
                                      newID_heartStatus = 83;		
																			memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
																			memset(uID_heart_packet, 0, sizeof(uID_heart_packet)); 
																			uID_heart_packet[0] = 'A';
																			uID_heart_packet[1] = 'B';
																			uID_heart_packet[2] = '0';
																			uID_heart_packet[3] = '1';
																			uID_heart_packet[4] = '0';
																			uID_heart_packet[5] = '0'; 
																			uID_heart_packet[6] = '0';
																			uID_heart_packet[7] = 48+userID_len/2; 
																			for( temp_idx=0;temp_idx<userID_len;temp_idx++)
																			{  
																				uID_heart_packet[8+temp_idx] = userID_val[temp_idx];
																			}   
																			uID_heart_packet[8+userID_len] = 'C';
																			uID_heart_packet[9+userID_len] = 'D';
																			uID_heart_packet[10+userID_len] = '\0';
																			printf("&Update NewID_Binded heart pkt send ...=: (%s) \r\n",uID_heart_packet);   
																			sprintf(data_byte_sendbuff,"%s%s%s","AT+BLESPPSIND=",uID_heart_packet,"\r\n");																		
																			SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);// 
																			//printf("&UpdateNewID---SendAckToBLEapp- ID_Binded heart pkt to BLE, finish --:  \r\n");         
																			
																		  }
																		}
																		
																	        
                                     //------------------unbind --------------------
                                     //local unbindpos_start,unbindpos_end=string.find(string.sub(curframe,(curSOP_posPtr+4),(curSOP_posPtr+curPkt_Len+3)), '/device/unbind')                            
//                                    if(unbindpos_start~=nil)---????
//                                    then
//                                       if Echo232BTflag == 80 then 
//                                       RS232Handle:writestr("&17")
//                                       end 
//                                       userID_len = 0
//                                       newID_heartStatus  =  83  
//                                    end
//                                    ----
//                                    --PktData_element = (string.sub(frame,(curSOP_posPtr+1),(curSOP_posPtr+1)))..(string.sub(frame,(curSOP_posPtr+4),(curSOP_posPtr+curPkt_Len+3))) --type + payload
                                    //if curPkt_Len > 5 // ---payload len >1 0124  at least 5byte payload
                                    //then
                                    //PktData_element =(string.sub(curframe,(curSOP_posPtr+4),(curSOP_posPtr+curPkt_Len+3))) //--  payload 20190118
                                     
//                                    --RS232Handle:writestr("&&33")
//                                    --RS232Handle:writestr(PktData_element)   
                                    //EnQueueTx_busyFlag = 83    
                                    //EnQueueTx(PktData_element)   ---发送到卫星
                                    //EnQueueTx_busyFlag = 0  
																		
																		// ---发送到卫星
																		printf("\r\n&&33_Data_To_Satellite={ \r\n" ); 
																		for( temp_idx=0;temp_idx<curPkt_Len;temp_idx++)
																		{
																		   PktData_element_Forward[temp_idx] = curframe[curSOP_posPtr+4+temp_idx];
																			 printf("%X ",PktData_element_Forward[temp_idx] ); 
																		}
																		printf("}\r\n ={" ); 
																		for( temp_idx=0;temp_idx<curPkt_Len;temp_idx++)
																		{ 
																			 printf("%c",PktData_element_Forward[temp_idx] ); 
																		}
																		printf("} " ); 
                                    SendCommand_to_SIM800("AT+BLESPPSIND=AB000000CD\r\n", "OK", 1000, 1);
		                                printf("&SendAck to BLE-0xAB,0x00,0x00,0x00,0xCD----:  \r\n");
                                   // BTheart_Reset = 80                                    
                                    curSOP_posPtr = curSOP_posPtr +4+curPkt_Len;     
																	}
                                 // end  ----type --------------------
                             
																} //--------EOP find -pick the pkt
                          // else //--(curSOP_posPtr+curPkt_Len+3)>=curframelen
//                                ----------fragment -----0125--strart---------------------------------------
//                               if((curframe:byte(curSOP_posPtr+1) == 0x02) and (curPkt_Len<240) )--type = 0x02    87+33*3bytes
//                               then
//                                  if Echo232BTflag == 80 then 
//                                       RS232Handle:writestr("&15") 
//                                  end
//                                       
//                                  if(curSOP_posPtr < (curframelen-18))  ---   
//                                  then  --- is a fragment
//                                       cacheOldFrame[1] = (string.sub(curframe,curSOP_posPtr,-1))  
//                                       oldFrame_len =  curframelen-curSOP_posPtr+1
//                                       oldFrameFragment_Flag = 83 
//                                       oldPktExpected_len = curPkt_Len   
//                                       if Echo232BTflag == 80 then 
//                                       RS232Handle:writestr("&16") 
//                                       end
//                                       curSOP_posPtr = curframelen   ---to next circile
//                                  end
//                                  
															//}// end
                                //----------fragment -----0125-end----------------------------------------
													}//  ----if((curSOP_posPtr+curPkt_Len+3)<curframelen)  -data len enough
                           
                           
												 }//----------------------type00 01 02 filter-------------------------- 
											}//  ----------find sop 0xAB---------------
                      curSOP_posPtr = curSOP_posPtr +1;           
						
						
						}
					 
	      }
		  //}
		  memset(SIM800_RX_BUF_copy, 0, sizeof(SIM800_RX_BUF_copy)); 
	    ble_counter++;
			if( ble_counter == 20) {
//			 	SendCommand_to_SIM800("AT+BLESPPSIND=1234ABCD\r\n", "OK", 1000, 1);  //[12 34 AB CD]
//				SendCommand_to_SIM800("AT+BLESPPSIND=\"5678efgh\"\r\n", "OK", 1000, 1);
//				memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
//        sprintf(data_byte_sendbuff,"%s%02x%02x%02x%02x%02x%s","AT+BLESPPSIND=",ack_heart_notbind[0],ack_heart_notbind[1],ack_heart_notbind[2],ack_heart_notbind[3],ack_heart_notbind[4],"\r\n");																		
//        printf("&uint8-to-byteString:data_byte_sendbuff=%s \r\n",data_byte_sendbuff); 
//				SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);
//				
//				char* str = "AT+BLESPPSIND=yyyyyyyy\r\n";
//				str[14] = 'K';
//				str[15] = 0xAB;
//				str[16] = 0x38;
//				str[17] = 'O';
//				SendCommand_to_SIM800(str, "OK", 1000, 1); 
//				data_byte_sendbuff[14] =0x39;
//			  data_byte_sendbuff[15] =0x38; 	
//        data_byte_sendbuff[16] =0x38; 	
//        data_byte_sendbuff[17] =0x38; 	
//        data_byte_sendbuff[18] =0x39; 	
//				SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);
				  if(userID_len>0)
					{
             memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
					 	memset(uID_heart_packet, 0, sizeof(uID_heart_packet)); 
						uID_heart_packet[0] = 'A';
						uID_heart_packet[1] = 'B';
						uID_heart_packet[2] = '0';
						uID_heart_packet[3] = '1';
						uID_heart_packet[4] = '0';
						uID_heart_packet[5] = '0'; 
						uID_heart_packet[6] = '0';
						uID_heart_packet[7] = 48+userID_len/2; 
						for( temp_idx=0;temp_idx<userID_len;temp_idx++)
						{  
							uID_heart_packet[8+temp_idx] = userID_val[temp_idx];
					  }   
						uID_heart_packet[8+userID_len] = 'C';
						uID_heart_packet[9+userID_len] = 'D';
						uID_heart_packet[10+userID_len] = '\0';
						printf("$-:ID_Binded heart pkt=: (%s) \r\n",uID_heart_packet);   
						sprintf(data_byte_sendbuff,"%s%s%s","AT+BLESPPSIND=",uID_heart_packet,"\r\n");																		
            SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);// {0xAB,0x00,0x00,0x00,0xCD}
						printf("$-:SendAckToBLEapp- ID_Binded heart pkt to BLE, finish --:  \r\n");                         
																			
					}
				  else
					{
					  memset(data_byte_sendbuff, 0, sizeof(data_byte_sendbuff)); 
						sprintf(data_byte_sendbuff,"%s%02x%02x%02x%02x%02x%s","AT+BLESPPSIND=",heart_notbind[0],heart_notbind[1],heart_notbind[2],heart_notbind[3],heart_notbind[4],"\r\n");																		
            SendCommand_to_SIM800(data_byte_sendbuff, "OK", 1000, 1);//  
						printf("$-:SendHeartToBLEapp- notBinded heart pkt to BLE, finish --:  \r\n");
					}
		    printf("$4:AT+BLE-----6s circle:  \r\n");
				ble_counter =0;
			} 
	 }	   
   #endif
	
	
	
	 #ifdef BLESPP_GATT_RxTx_MODE   //*********************************BLESPP_GATT_RxTx_MODE*********************************************************************************************************
	 printf("This is BLESPP_GATT_RxTx_MODE dual Service test \r\n");
   if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 1) == SUCCESS)
   {
				 	printf("1. Module AT test ok  \r\n");
   } 
	 BLE_onoff_status = 0x00;
  if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 1) == SUCCESS)
   {
				 	printf("1. Module AT test ok  \r\n");
   } 
  if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR:1", 3000, 1) == SUCCESS)
  {
				 	printf("2. AT+BTPOWER? test ok \r\n");
		      BLE_onoff_status = 0x01;
  }  
	if(BLE_onoff_status == 0x00)
	{
		 	printf("AT BTPOWER is off \r\n");
	if (SendCommand_to_SIM800("AT+BTPOWER=1\r\n", "OK", 2000, 1) == SUCCESS)
  {       
				 	printf("3. AT+BTPOWER=1  open BT ok  \r\n");
  } 
  }
	 /******BLESPP regi-----------------------------*/
	if (SendCommand_to_SIM800("AT+BLEADDR?\r\n", "+BLEADDR", 2000, 2) == SUCCESS)
  {         
				 	printf("4.AT+BLEADDR? test ok \r\n");
  }  
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 2) == SUCCESS)
  {         
				 	printf("5. AT+BLESTATUS? test ok \r\n");
  }   
  ///******开启BLESPP服务 AT+BLESPP=1  注册spp 
  if (SendCommand_to_SIM800("AT+BLESPP=1\r\n", "OK", 2000, 2) == SUCCESS)
  {         
				 	printf("6. AT+BLESPP=1 regi BLESPP server ok \r\n \r\n");
  }  
	HAL_Delay(1000);
	//*********************************GATT_Service config*************************** 
  //注册GATT服务 +BLESREG: <server_index>,<user_id>
	if (SendCommand_to_SIM800("AT+BLESREG\r\n", "+BLESREG", 2000, 1) == SUCCESS)
  {         
				 	printf("17.AT+BLESREG regi GATT service ok \r\n \r\n");
  }   
	//查询GATT服务 +BLESREG: <server_index>,<user_id>  
	if (SendCommand_to_SIM800("AT+BLESREG?\r\n", "+BLESREG", 2000, 1) == SUCCESS) 
  {         
				 	printf("16-a.AT+BLESREG?  query GATT service ok \r\n \r\n");
  }   
	 
	//添加服务  AT+BLESSAD=1,“123456”,15,1,1   ，+BLESAD: 1,ABCDEFF0,123456,1,1,256
	//AT+BLESSAD=<server_index>,<uuid>,<num_handles>,<is_primary>,<inst>
	//+BLESSAD:<service_index>,<user_id>,<uuid>,<is_primary>,<inst>,<service_handle>
	if (SendCommand_to_SIM800("AT+BLESSAD=1,\"123456\",15,1,1\r\n", "+BLESSAD", 2000, 1) == SUCCESS)
  {         
				 	printf("18.AT+BLESSAD=1,... add GATT service ok \r\n \r\n");
  }  
	//服务service上添加可读写的characteristic: AT+BLESSC=1,"ABCDEF",1,10,17  ，+BLESSC: 1,ABCDEFF0,256,ABCDEF,1,258
	if (SendCommand_to_SIM800("AT+BLESSC=1,\"ABCDEF\",1,10,17\r\n", "+BLESSC", 2000, 1) == SUCCESS)
  {         
				 	printf("19.AT+BLESSC=1,,... add characteristic 1 RW ok \r\n \r\n");
  }  
	//服务service上添加Notify的characteristic: AT+BLESSC=1,"ABCDEF",1,16,17 ，+BLESSC: 2,ABCDEFF0,256,ABCDEF,1,260
	if (SendCommand_to_SIM800("AT+BLESSC=1,\"ABCDEF\",1,16,17\r\n", "+BLESSC", 2000, 1) == SUCCESS)
  {         
				 	printf("20.AT+BLESSC=1,,... add characteristic 2 Notify ok \r\n \r\n");
  }  
	//服务service上添加descriptor: AT+BLESSD=1,"0229",1,0，+BLESSD: 1,ABCDEFF0,256,0229,1,261
	if (SendCommand_to_SIM800("AT+BLESSD=1,\"0229\",1,0\r\n", "+BLESSD", 2000, 1) == SUCCESS)
  {         
				 	printf("21.AT+BLESSD=1,,... add descriptor 1  ok \r\n \r\n");
  }  
	//启动GATT服务 service : AT+BLESSSTART=1,0  ,+BLESSSTART: 1,ABCDEFF0,256
	if (SendCommand_to_SIM800("AT+BLESSSTART=1,0\r\n", "+BLESSSTART", 2000, 1) == SUCCESS)
  {         
				 	printf("22.AT+BLESSSTART=1,0 Start GATT service ok \r\n \r\n");
  }  
	//开始广播: AT+BLESLSTART=1  ,+BLESLSTART: 1,ABCDEFF0
	if (SendCommand_to_SIM800("AT+BLESLSTART=1\r\n", "+BLESLSTART", 2000, 1) == SUCCESS)
  {         
				 	printf("23.AT+BLESLSTART=1 Start Broadcast ok \r\n \r\n");
  } 
	
	///******查询BLESPP状态 AT+BLESTATUS?
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 2) == SUCCESS)
  {         
				 	printf("AT+BLESTATUS? test ok \r\n");
  }  
	
	/******BLE GATT regi-----------------------------*/
	
	
	
	 #endif
	
	
	 #ifdef BLE_SPP_RxTx_MODE   //*********************************BLE_SPP_RxTx_MODE*********************************************************************************************************
  /* test for LE_SPP_RxTx_MODE模式;  */ 
	printf("This is BLE_SPP_RxTx_MODE test \r\n");
	BLE_onoff_status = 0x00;
  if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 1) == SUCCESS)
   {
				 	printf("1. Module AT test ok  \r\n");
   } 
  if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR:1", 3000, 1) == SUCCESS)
  {
				 	printf("2. AT+BTPOWER? test ok \r\n");
		      BLE_onoff_status = 0x01;
  }  
	if(BLE_onoff_status == 0x00)
	{
		 	printf("AT BTPOWER is off \r\n");
	if (SendCommand_to_SIM800("AT+BTPOWER=1\r\n", "OK", 2000, 1) == SUCCESS)
  {       
				 	printf("3. AT+BTPOWER=1  open BT ok  \r\n");
  } 
  }
	if (SendCommand_to_SIM800("AT+BTSTATUS?\r\n", "+BTSTATUS", 1000, 1) == SUCCESS)
  {       
				 	printf("4. AT+BTSTATUS? test ok \r\n");
  }  
	 
	/******BLE-----------------------------*/
	if (SendCommand_to_SIM800("AT+BLEADDR?\r\n", "+BLEADDR", 2000, 2) == SUCCESS)
  {         
				 	printf("15.AT+BLEADDR? test ok \r\n");
  }  
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 2) == SUCCESS)
  {         
				 	printf("16. AT+BLESTATUS? test ok \r\n");
  }   
  ///******开启BLESPP服务 AT+BLESPP=1  注册spp 
  if (SendCommand_to_SIM800("AT+BLESPP=1\r\n", "OK", 2000, 2) == SUCCESS)
  {         
				 	printf("19. AT+BLESPP=1 regi BLESPP server ok \r\n \r\n");
  }  
	HAL_Delay(1000);
	///******查询BLESPP状态 AT+BLESTATUS?
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 2) == SUCCESS)
  {         
				 	printf("AT+BLESTATUS? test ok \r\n");
  }  
	uint8_t  ble_counter =0;
	
	/*-------------query, send & receive BLE data-----------------------------*/ 
	while (1)
  {  HAL_Delay(50);
		 char SIM800_RX_BUF_copy[200] = {0}; 	
		 Read_SIM800_RxBuf(SIM800_RX_BUF_copy);   
	  if( strstr(SIM800_RX_BUF_copy,"+BLESPPCON") != NULL)
			   {				
				     Clear_SIM800_RxBuf();
				     printf("$2:+BLESPPCON  BLE CONNECTED success received ok  \r\n");  
			   }  
		  /*query +BTSPPDATA*/
		 if( strstr(SIM800_RX_BUF_copy,"+BLESPPWREQ") != NULL)
		     {	Clear_SIM800_RxBuf();  
					 printf("$3:+BLESPPWREQ   data received: \r\n"); 
					 printf("< %s >\r\n",SIM800_RX_BUF_copy);  
		     }
	    ble_counter++;
			if( ble_counter == 100) //1s
			{
				SendCommand_to_SIM800("AT+BLESPPSIND=\"1234ABCD\"\r\n", "OK", 1000, 1);
		    printf("$4:AT+BLE-----:  \r\n");
				ble_counter =0;
			} 
	 }	   
   #endif
  
	
	#ifdef BT_SPP_MC_APP_MODE //*********************************BT_SPP_MC_APP_MODE*********************************************************************************************************
  /* test for SPP-MC-APP模式;  */
	printf("This is BT_SPP_MC_APP_MODE test \r\n \r\n");
  if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 2) == SUCCESS)
   {
				 	printf("1. Module AT test ok \r\n \r\n");
   } 
  if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR", 3000, 2) == SUCCESS)
  {
				 	printf("2. AT+BTPOWER? test ok \r\n \r\n");
  } 
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTPOWER=1\r\n", "OK", 2000, 2) == SUCCESS)
  {       
				 	printf("3. AT+BTPOWER=1 test ok \r\n \r\n");
  } 
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR:1", 3000, 2) == SUCCESS)
  {
				 	printf("4. AT+BTPOWER? = +BTPWR:1  powerOn \r\n \r\n");
  } 
	if (SendCommand_to_SIM800("AT+BTSPPCFG=?\r\n", "+BTSPPCFG", 1000, 2) == SUCCESS)
  {   //query    
				 	printf("--. AT+BTSPPCFG=? test ok \r\n \r\n");
  }   
	if (SendCommand_to_SIM800("AT+BTSPPCFG=\"MC\",2\r\n", "+BTSPPCFG", 1000, 2) == SUCCESS)
  {   //query    
				 	printf("5. AT+BTSPPCFG=\"MC\",2 test ok \r\n \r\n");
  }   
	if (SendCommand_to_SIM800("AT+BTHOST?\r\n", "OK", 2000, 2) == SUCCESS)
  {       //HAL_Delay(1000);
				 	printf("8. AT+BTHOST? test ok \r\n \r\n");
  }   
	 ///****注销BLESPP服务 AT+BLESPP=0   
		printf("BLE spp closing ...\r\n");
  if (SendCommand_to_SIM800("AT+BLESPP=0\r\n", "OK", 2000, 1) == SUCCESS)
  {         
				 	printf("19. AT+BLESPP=1 close BLESPP server ok \r\n \r\n");
  }  
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTSTATUS?\r\n", "+BTSTATUS", 1000, 2) == SUCCESS)
  {       
				 	printf("9. AT+BTSTATUS? test ok \r\n \r\n");
  }  
	//HAL_Delay(1000); 
	if (SendCommand_to_SIM800("AT+BTSPPCFG=\"MC\",1\r\n", "+BTSPPCFG", 1000, 2) == SUCCESS)
  {   //config    
				 	printf("10. AT+BTSPPCFG=\"MC\",1  config SPP.MC=1 ok \r\n \r\n");
  }   
		//HAL_Delay(1000); 
		if (SendCommand_to_SIM800("AT+BTSPPCFG=\"MC\",2\r\n", "+BTSPPCFG", 1000, 2) == SUCCESS)
  {   //query    
				 	printf("11.AT+BTSPPCFG=\"MC\",2 test ok \r\n \r\n");
  }   
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTCOD?\r\n", "OK", 2000, 2) == SUCCESS)
  {        //HAL_Delay(1000); 
				 	printf("12.AT+BTCOD? test ok \r\n \r\n"); //+BTCOD: <en>,<mjr_srv>,<mjr_cls>,<mnr_cls>
  }  
	uint8_t  counter =0;
	/*-------------query, send & receive BLE data-----------------------------*/ 
	while (1)
  {  HAL_Delay(50);
		 char SIM800_RX_BUF_copy[200] = {0}; 	
		 Read_SIM800_RxBuf(SIM800_RX_BUF_copy);  
		 /*query +BTPAIRING*/
	   if( strstr(SIM800_RX_BUF_copy,"+BTPAIRING") != NULL)
			   {	 				 
				     Clear_SIM800_RxBuf();
					 printf("$1:+BTPAIRING received ok \r\n \r\n"); 
					  SendCommand_to_SIM800("AT+BTPAIR=1,1\r\n", "+BTPAIR", 1000, 2);
			   } 
		 if( strstr(SIM800_RX_BUF_copy,"+BTCONNECTING") != NULL)
			   {				
				     Clear_SIM800_RxBuf();
				     printf("$2:+BTCONNECTING received ok \r\n \r\n"); 
					 SendCommand_to_SIM800("AT+BTACPT=1\r\n", "+BTCONNECT", 1000, 2);
			   } 
	 
		  /*query +BTSPPDATA*/
		 if( strstr(SIM800_RX_BUF_copy,"+BTSPPDATA") != NULL)
		     {	Clear_SIM800_RxBuf();  
					 printf("$3:+BTSPPDATA received: \r\n"); 
					 printf("< %s >\r\n",SIM800_RX_BUF_copy);  
		     }
	    counter++;
    if( counter == 20) //1s
		{				
		 /*query connection and send BLE data*/
		 if (SendCommand_to_SIM800("AT+BTSPPCFG?\r\n", "S,1,1", 1000, 2) == SUCCESS)
     {   //   
			    printf("$4:+BTSPPCFG: S,1,1 --BT_SPP is connected:  \r\n");
			    SendCommand_to_SIM800("AT+BTRSSI=1\r\n", "+BTRSSI:", 1000, 2); 
			         SendCommand_to_SIM800("AT+BTSPPSEND=1,10\r\n", ">", 2000, 1);
			         HAL_Delay(100); 
					     if (SendCommand_to_SIM800("data1234567", "SEND OK", 2000, 1) == SUCCESS) 
							 {
								//HAL_Delay(1000); 
								 printf("$5:Send BT_SPP data ok <data1234567> \r\n \r\n");
							 } 		  
     }   
     counter = 0;		 
		 HAL_Delay(100); 
	  } 
	 }
	 #endif //************************************************************
		

  
	 
	 
  #ifdef BLE_GATT_Service_MODE   //*********************************BLE_GATT_Service_MODE *********************************************************************************************************
  /* test for BLE_GATT_Service_MODE模式;  */ 
	printf("This is BLE_GATT_Service_MODE test \r\n \r\n");
  if (SendCommand_to_SIM800("AT\r\n", "OK", 3000, 2) == SUCCESS)
   {
				 	printf("1. Module AT test ok \r\n \r\n");
   } 
  if (SendCommand_to_SIM800("AT+BTPOWER?\r\n", "+BTPWR", 3000, 2) == SUCCESS)
  {
				 	printf("2. AT+BTPOWER? test ok \r\n \r\n");
  } 
	//HAL_Delay(1000);
	if (SendCommand_to_SIM800("AT+BTPOWER=1\r\n", "OK", 2000, 2) == SUCCESS)
  {       
				 	printf("3. AT+BTPOWER=1 test ok \r\n \r\n");
  } 
	/*BLE-----*/
	if (SendCommand_to_SIM800("AT+BLEADDR?\r\n", "+BLEADDR", 2000, 2) == SUCCESS)
  {         
				 	printf("15.AT+BLEADDR? test ok \r\n \r\n");
  }  
	if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 2) == SUCCESS)
  {         
				 	printf("16. AT+BLESTATUS? test ok \r\n \r\n");
  }  
	//*********************************GATT_Service config***********BLE_GATT_Service_MODE****************
	//查询GATT服务 +BLESREG: <server_index>,<user_id>  
	if (SendCommand_to_SIM800("AT+BLESREG?\r\n", "+BLESREG", 2000, 1) == SUCCESS) 
  {         
				 	printf("16-a.AT+BLESREG?  query GATT service ok \r\n \r\n");
  }   
  //注册GATT服务 +BLESREG: <server_index>,<user_id>
	if (SendCommand_to_SIM800("AT+BLESREG\r\n", "+BLESREG", 2000, 1) == SUCCESS)
  {         
				 	printf("17.AT+BLESREG regi GATT service ok \r\n \r\n");
  }  
	//查询GATT服务 +BLESREG: <server_index>,<user_id>  
	if (SendCommand_to_SIM800("AT+BLESREG?\r\n", "+BLESREG", 2000, 1) == SUCCESS) 
  {         
				 	printf("16-a.AT+BLESREG?  query GATT service ok \r\n \r\n");
  }   
	//添加服务  AT+BLESSAD=1,“123456”,15,1,1   ，+BLESAD: 1,ABCDEFF0,123456,1,1,256
	//AT+BLESSAD=<server_index>,<uuid>,<num_handles>,<is_primary>,<inst>
	//+BLESSAD:<service_index>,<user_id>,<uuid>,<is_primary>,<inst>,<service_handle>
	if (SendCommand_to_SIM800("AT+BLESSAD=1,\"123456\",15,1,1\r\n", "+BLESSAD", 2000, 1) == SUCCESS)
  {         
				 	printf("18.AT+BLESSAD=1,... add GATT service ok \r\n \r\n");
  }  
	//服务service上添加可读写的characteristic: AT+BLESSC=1,"ABCDEF",1,10,17  ，+BLESSC: 1,ABCDEFF0,256,ABCDEF,1,258
	if (SendCommand_to_SIM800("AT+BLESSC=1,\"ABCDEF\",1,8,8\r\n", "+BLESSC", 2000, 1) == SUCCESS)
  {         
				 	printf("19.AT+BLESSC=1,,... add characteristic 1 RW ok \r\n \r\n");
  }  
	//服务service上添加Notify的characteristic: AT+BLESSC=1,"ABCDEF",1,16,17 ，+BLESSC: 2,ABCDEFF0,256,ABCDEF,1,260
	if (SendCommand_to_SIM800("AT+BLESSC=1,\"ABCDEF\",1,16,17\r\n", "+BLESSC", 2000, 1) == SUCCESS)
  {         
				 	printf("20.AT+BLESSC=1,,... add characteristic 2 Notify ok \r\n \r\n");
  }  
	//服务service上添加descriptor: AT+BLESSD=1,"0229",1,0，+BLESSD: 1,ABCDEFF0,256,0229,1,261
	if (SendCommand_to_SIM800("AT+BLESSD=1,\"0229\",1,0\r\n", "+BLESSD", 2000, 1) == SUCCESS)
  {         
				 	printf("21.AT+BLESSD=1,,... add descriptor 1  ok \r\n \r\n");
  }  
	//启动GATT服务 service : AT+BLESSSTART=1,0  ,+BLESSSTART: 1,ABCDEFF0,256
	if (SendCommand_to_SIM800("AT+BLESSSTART=1,0\r\n", "+BLESSSTART", 2000, 1) == SUCCESS)
  {         
				 	printf("22.AT+BLESSSTART=1,0 Start GATT service ok \r\n \r\n");
  }  
	//开始广播: AT+BLESLSTART=1  ,+BLESLSTART: 1,ABCDEFF0
	if (SendCommand_to_SIM800("AT+BLESLSTART=1\r\n", "+BLESLSTART", 2000, 1) == SUCCESS)
  {         
				 	printf("23.AT+BLESLSTART=1 Start Broadcast ok \r\n \r\n");
  } 
	while(1)
  {
	//查询BLE状态   +BLESTATUS: <conn_id>,<gatts_type>,<userid>,<addr>	
		if (SendCommand_to_SIM800("AT+BLESTATUS?\r\n", "+BLESTATUS", 2000, 1) == SUCCESS)
		{     	
						printf("16. AT+BLESTATUS? test ok \r\n \r\n"); 
			if (SendCommand_to_SIM800("AT+BLESSC?\r\n", "+BLESSC", 2000, 1) == SUCCESS)
     {         
				 	printf("AT+BLESSC? test ok \r\n");
     }  
		  SendCommand_to_SIM800("AT+BLESIND=2,\"9876543\"\r\n", "+BLESIND", 2000, 1);
		} 
	  HAL_Delay(5000);
  }	
	 
	//设置广播参数 AT+BLEADV=1,1,1,0,25,"4c00","02291234","2902"   +BLEADV
	  //AT+BLEADV=1,1,1,0,25,\"4c00\",\"02291234\",\"FEE0\"
	  //<server_index>,<s can_rsp>,<include_name>,<include_txpower>,<appearance>,<manufacturer_data>,<service_data>,<service_uuid>
//	if (SendCommand_to_SIM800("AT+BLEADV=1,1,1,0,25,\"4c00\",\"02291234\",\"FEE0\"\r\n", "+BLEADV", 2000, 2) == SUCCESS)
//  {         
//				 	printf("18. AT+BLEADV=1,1... set BroadCast Para ok \r\n \r\n");
//  }  
	//启动GATT服务：
	
	
 #endif  
	
  /* USER CODE END 2 */
   
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
      
			
		  
		 
  /* USER CODE END WHILE */
     
  /* USER CODE BEGIN 3 */

  }
  /* USER CODE END 3 */

}

/**
  * @brief chr to uint8 dec
  * @retval None
  */
uint8_t chrtodec(char chr)
{
        uint8_t value=0; 
        if((chr>='a')&&(chr<='z'))
                chr=chr-32; 
        if((chr>='0')&&(chr<='9'))
                value=chr-48;
        else if((chr>='A')&&(chr<='Z'))
                value=chr-65+10; 
        return value;
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{

  RCC_OscInitTypeDef RCC_OscInitStruct;
  RCC_ClkInitTypeDef RCC_ClkInitStruct;

    /**Configure the main internal regulator output voltage 
    */
  __HAL_RCC_PWR_CLK_ENABLE();

  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 192;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV4;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Initializes the CPU, AHB and APB busses clocks 
    */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV4;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }

    /**Configure the Systick interrupt time 
    */
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

    /**Configure the Systick 
    */
  HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

  /* SysTick_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(SysTick_IRQn, 0, 0);
}

/* USER CODE BEGIN 4 */
#if defined (DEBUG_USART)	
#ifdef __GNUC__
/* With GCC/RAISONANCE, small printf (option LD Linker->Libraries->Small printf
set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif 
PUTCHAR_PROTOTYPE
{
	while(__HAL_UART_GET_FLAG(&huart3, UART_FLAG_TC) == RESET);
#ifdef __STM32F0xx_HAL_H
	
	huart1.Instance->TDR = (uint8_t)ch;
#endif

 
 huart3.Instance->DR = (uint8_t)ch;
 

#ifdef __STM32L1xx_HAL_H
	huart3.Instance->DR = (uint8_t)ch;
#endif

	return ch;
}
#endif
/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM2 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM2) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @param  file: The file name as string.
  * @param  line: The line in file as a number.
  * @retval None
  */
void _Error_Handler(char *file, int line)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  while(1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
