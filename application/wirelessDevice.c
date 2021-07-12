#include "main.h"
#include "revDataInfo.h"
#include "wirelessDevice.h"
#include "pointManager.h"
#include "mqttApp.h"
/*
*author:Wxc
*info:
*data: 2021-07-06
*/
#if 0
void ansyWireLessData(uint8_t *buff,int32_t len,wireLessEnum flag)
{

    static int s_flag = WIRE_HEAD;
    static int s_cmd = 0;
    static int s_len = 0;
    static uint8_t revBuff[256];
    static int s_count = 0;
    static uint8_t s_crc = 0;
    static int step = 0;
    static uint8_t oriBuff[256];
    static int oriBuffLength = 0;

    if(flag == WIRE_HEAD)
    {
        s_flag = WIRE_HEAD;
    }    
    for(int i =0 ; i < len ; i ++)
    {        
        uint8_t data = buff[i];
        oriBuff[oriBuffLength++] = data;        
        switch (s_flag)
        {
        case WIRE_HEAD:
            if(data == 0xaa)
            {
                if(step == 1)
                {
                    s_flag = WIRE_CMD;
                    step = 0;
                }
                else
                {
                    step = 0;
                }
            }         
            if(data == 0x55)
            {
                step ++;
                oriBuff[0] = 0x55; 
                oriBuffLength = 1;
            }
            break;
        case WIRE_CMD:       
            s_cmd = data;
            s_flag = WIRE_LENGTH;  
            s_crc+= s_cmd;
            break;
        case WIRE_LENGTH:     
            s_crc += data;
            if(step == 1)
            {
                step = 0;
                s_flag = WIRE_DATA;
                s_len += data;                
            }       
            else
            {
                step ++;
                s_len = (data<< 8);
            }
            break;
        case WIRE_DATA:   
            revBuff[step++] = data;  
            s_crc += data;          
            if(step == s_len)
            {
                s_flag = WIRE_COUNT;
                step = 0;
            }         
            break;
        case WIRE_COUNT: 
            if(step == 1)
            {
                s_flag = WIRE_CRC;
                s_count += data;
            }           
            else
            {
                step ++;
                s_count += (data<< 8);
            }
            break;
        case WIRE_CRC:  
            if(s_crc == data)
            {
                //check success
                if(s_cmd == 0x01)
                {
                    //upload data
                    
                }
                else if(s_cmd == 0x03)
                {
                    //heart data
                    setPointId(revBuff);
                }
                s_crc = 0;
                s_count = 0;
                s_flag = WIRE_HEAD;
                step = 0;
            }          
            break;                                                        
        default:
            break;
        }    
    }
}
#endif
// typedef enum
// {    
//     WIRE_HEAD = 0,
//     WIRE_CMD,
//     WIRE_LENGTH,
//     WIRE_DATA,
//     WIRE_COUNT,
//     WIRE_CRC,
//     WIRE_NONE,
// }wireLessEnum;

void ansyWireLessData(uint8_t *buff,int32_t len,int cmd)
{
    //set point heart 
    setPointId(cmd);
    //save data to send to mqtt
    app_mqttSetSendBuff(buff,len,cmd);
}

void wirelessRevData(uint8_t *buff,int32_t len)
{
    //rev and ansy data
    static int s_lastTime = 0;
    static int s_flag = WIRE_HEAD;
    static int s_len = 0;
    static int s_count = 0;
    static int s_IPAddress = 0;
    static uint8_t s_revData[0xFF];
    if(HAL_GetTick() - s_lastTime > 50)
    {
        s_flag = WIRE_HEAD;
        s_count = 0;
    }
    for(int i = 0; i < len ; i ++)
    {
        uint8_t data = buff[i];
        if(s_flag != WIRE_HEAD && s_flag != WIRE_CRC && i < len - 1)
        {
            if(data == 0xFE && buff[i+1] == 0xFD)
            {
                data = 0xFF;
                i++;
            }
            if(data == 0xFE && buff[i+1] == 0xFC)
            {
                data = 0xFE;
                i++;
            }            
        }
        switch (s_flag)
        {
        case WIRE_HEAD:
            if(data == 0xFE)        
            {
                s_flag = WIRE_LENGTH;
            }
			break;
        case WIRE_LENGTH:
            s_len = data;
            s_flag = WIRE_CMD;
            break;
        case WIRE_CMD:
            s_count ++;
            if(s_count == 2)
            {
                s_count = 0;
                s_flag = WIRE_COUNT;
                s_IPAddress = 0;
            }            
            break;
        case WIRE_COUNT:
            s_count ++;            
            if(s_count == 1)
            {
                s_IPAddress += (data << 8);
            }
            else
            {
                s_IPAddress += data;
                s_count = 0;
                s_flag = WIRE_DATA;                
            }
            break;
        case WIRE_DATA:            
            s_revData[s_count++] = data;
            if(s_count == s_len - 4)
            {
                s_count = 0;
                s_flag = WIRE_CRC;
            }
            break;
        case WIRE_CRC:
            if(data == 0xFF)
            {
                //success
                s_flag = WIRE_HEAD;
                ansyWireLessData(s_revData,s_len,s_IPAddress);
            }
        default:
            break;
        }
    }
}


/*************************************************
Function: 								// ????
Description: 							// ???????????
Calls: 									// ???????????
Input: 									// ??????,????????
Output: 								// ?????????
Return: 								// ????????
Others: 								// ????
*************************************************/
void initWirelessUsart()
{	
	createFifoRev(&huart3,wirelessRevData,UART_REV_DMA);
}

void wirelessWriteDebug(uint8_t *data,int len)
{
	
	HAL_UART_Transmit(&huart3,data,len,0xFF);
}


void wirelessSendData(uint8_t *revbuff,int len)
{
    int index = 0;
    uint8_t buff[0xff+30];    
	buff[index++] = 0xFE;    
    if(len + 2 == 0xFF)
    {
        buff[index++] = 0xFE;    
        buff[index++] = 0xFD;            
    }
    else if(len + 2 == 0xFE)
    {
        buff[index++] = 0xFE;    
        buff[index++] = 0xFC; 
    }    
    else
        buff[index++] = len + 2;    

	buff[index++] = 0x81;
	buff[index++] = 0x80;

    for(int i = 0; i < len ; i ++)
    {
        if(revbuff[i] == 0xFF)
        {
            buff[index++] = 0xFE;    
            buff[index++] = 0xFD;            
        }
        else if(revbuff[i]  == 0xFE)
        {
            buff[index++] = 0xFE;    
            buff[index++] = 0xFC; 
        }    
        else
            buff[index++] = revbuff[i];   
    }	
	

	buff[index++] = 0xFF;
    wirelessWriteDebug(buff,index);
}

