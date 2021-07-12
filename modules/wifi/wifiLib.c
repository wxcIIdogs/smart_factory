#include "wifiLib.h"
#include "wifiLibConfig.h"
#include "main.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
#include "string.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "stdio.h"
static enum_WIFI_CMD sg_wifi_cmd_status = WIFI_NONE;
static int sg_rpych = 0;    
static int sg_wifiInitFlag = 0;
/**
 * 
 * 
 * 
 */
void setWifiData(uint8_t *oriData,int len)
{
//rpy
    if(strstr((char *)oriData,"WIFI GOT IP") != NULL)
    {
        sg_rpych = WIFI_RPY_REPLY;
    }
		else if(strstr((char *)oriData,"ALREADY") != NULL)
    {
        sg_rpych = WIFI_RPY_ALREADY;
    }
    else if(strstr((char *)oriData,"ERROR") != NULL)
    {
        sg_rpych = WIFI_RPY_ERROR;
    }
    else if(strstr((char *)oriData,"CONNECTED") != NULL)
    {
        sg_rpych = WIFI_RPY_CONNECT;
    }
    else if(strstr((char *)oriData,"CONNECT") != NULL)
    {
        sg_rpych = WIFI_RPY_CONNECT;
    }		
    else if(strstr((char *)oriData,"CLOSE") != NULL)
    {
        sg_wifiInitFlag = 0;
    }
    else if(strstr((char *)oriData,"DISCONNECT") != NULL)
    {
        sg_wifiInitFlag = 0;
    }
		else if(strstr((char *)oriData,">") != NULL)
    {
        sg_rpych = WIFI_RPY_TRANSPART;
    } 
    else if(strstr((char *)oriData,"OK") != NULL)
    {
        sg_rpych = WIFI_RPY_OK;
    } 

//cmd     
    if(strstr((char *)oriData,"RFPOWER") != NULL)
    {        
        sg_wifi_cmd_status = WIFI_RFPOWER;
    }
    else if(strstr((char *)oriData,"CWMODE_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWMODE_CUR;
    }
    else if(strstr((char *)oriData,"CIFSR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIFSR;
    }
    else if(strstr((char *)oriData,"CWQAP") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWQAP;
    }
    else if(strstr((char *)oriData,"CWDHCP_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWDHCP_CUR;
    }
    else if(strstr((char *)oriData,"CIPSTART") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSTATUS;
    }
    else if(strstr((char *)oriData,"CIPMUX") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPMUX;
    }
    else if(strstr((char *)oriData,"CIPSERVER") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSERVER;
    }
    else if(strstr((char *)oriData,"CIPCLOSE") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPCLOSE;
    }                          
    else if(strstr((char *)oriData,"CWJAP_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWJAP_CUR;
    } 
    else if(strstr((char *)oriData,"CIPMODE") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPMODE;
    }  
		else if(strstr((char *)oriData,"CIPSEND") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSEND;
    }  		
		
}
/**
 * 
 * 
 * 
 */
int wifi_sendCmdData(uint8_t *data,int len)
{
    sg_rpych = 0;
    sg_wifi_cmd_status = WIFI_NONE;
    return HAL_UART_Transmit(&huart,data,len,0xff);
}
int wifi_sendData(uint8_t *data,int len)
{
    return HAL_UART_Transmit(&huart,data,len,0xff);
}

/**
 * 
 * 
 * 
 */
int wifi_waitAT()
{
    wifi_sendCmdData("AT\r\n",4);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK)
        return 1;
    else 
        return 0;
}
int wifi_RST()
{
    wifi_sendCmdData("AT+RST\r\n",8);
    osDelay(1000);
    if(sg_rpych == WIFI_RPY_OK)
        return 1;
    else 
        return 0;
}
//1 trasport 
//0 cmd
int wifi_SetCipmode(int mode)
{
    char buff[50];
    int len = sprintf(buff,"AT+CIPMODE=%d\r\n",mode);
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CIPMODE)
        return 1;
    else 
        return 0;
}
int wifi_SetRfPower(int Power_0_to_82)
{
    char buff[50];
    int len = sprintf(buff,"AT+RFPOWER=%d\r\n",Power_0_to_82);
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_RFPOWER)
        return 1;
    else 
        return 0;
}

int wifi_Station_DhcpEnable(int Enable)
{    
    char buff[50];
    int len = sprintf(buff,"AT+CWDHCP_CUR=1,%d\r\n",Enable);
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CWDHCP_CUR)
        return 1;
    else 
        return 0;
}
int wifi_TcpIp_StartTcpConnection(char *RemoteIp,uint16_t RemotePort,uint16_t TimeOut)
{    
    char buff[100];
    int len = sprintf(buff,"AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n",RemoteIp,RemotePort,TimeOut);		
		for(int j = 0 ; j <10 ; j ++)
		{
			wifi_sendCmdData((uint8_t *)buff,len);
			osDelay(1000);		
			for(int i = 0;  i < 10 ; i ++)
			{
				osDelay(500);		
				if(sg_rpych == WIFI_RPY_REPLY)
					continue;
				if(WIFI_RPY_ALREADY == sg_rpych)
					return 1;
				if(sg_rpych == WIFI_RPY_CONNECT && sg_wifi_cmd_status == WIFI_CIPSTATUS)
					return 1;
				osDelay(500);		
			}
		}
		return 0;
}

int wifi_TcpIp_Close()
{    
    char buff[50];
    int len = sprintf(buff,"AT+CIPCLOSE\r\n");
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CIPCLOSE)
        return 1;
    else 
        return 0;
}
int wifi_Station_ConnectToAp(char *ssid,char *Pass)
{    
    char buff[100];
    int len = sprintf(buff,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,Pass);
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(1000);
		for(int i = 0 ; i < 10 ; i++)
    {			
			if(sg_rpych == WIFI_RPY_CONNECT && sg_wifi_cmd_status == WIFI_CWJAP_CUR)
        return 1;
			osDelay(1000);
		}
		return 0;
}
int wifi_cipSend()
{
	  char buff[50];
    int len = sprintf(buff,"AT+CIPSEND\r\n");
    wifi_sendCmdData((uint8_t *)buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_TRANSPART && sg_wifi_cmd_status == WIFI_CIPSEND)
        return 1;
    else 
        return 0;
}

void wifi_userMain()
{
    if(wifi_waitAT() == 0)
    {
        printf("AT CMD not return\r\n");
        return;
    }
		wifi_RST();
    sg_wifiInitFlag = wifi_SetRfPower(82);
    wifi_TcpIp_Close();
    sg_wifiInitFlag &= wifi_Station_DhcpEnable(1);    
    sg_wifiInitFlag &= wifi_Station_ConnectToAp(SSID,PASSWD);
    sg_wifiInitFlag &= wifi_TcpIp_StartTcpConnection(REMOTE_IP,REMOTE_PORT,CONNECT_TIME_OUT);
    sg_wifiInitFlag &= wifi_SetCipmode(1);
    sg_wifiInitFlag &= wifi_cipSend();
    printf("sg_wifiInitFlag = %d\r\n",sg_wifiInitFlag);
}
/**
 * 
 * 
 * 
 */
int wifiInit()
{
    sg_wifiInitFlag = 0;
    wifi_userMain();
    return sg_wifiInitFlag;

}

/**
 *  10ms
 *
 */
void wifiLibLoop()
{        
    
}

