#include "wifiLib.h"
#include "wifiLibConfig.h"
#include "main.h"
#include "usart.h"
#include "FreeRTOS.h"
#include "task.h"
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
    if(strstr(oriData,"OK") != NULL)
    {
        sg_rpych = WIFI_RPY_OK;
    }
    else if(strstr(oriData,"ERROR") != NULL)
    {
        sg_rpych = WIFI_RPY_ERROR;
    }
    else if(strstr(oriData,"CONNECT") != NULL)
    {
        sg_rpych = WIFI_RPY_CONNECT;
    }
    else if(strstr(oriData,"CLOSE") != NULL)
    {
        sg_wifiInitFlag = 0;
    }
    else if(strstr(oriData,"DISCONNECT") != NULL)
    {
        sg_wifiInitFlag = 0;
    }    
//cmd     
    if(strstr(oriData,"RFPOWER") != NULL)
    {        
        sg_wifi_cmd_status = WIFI_RFPOWER;
    }
    else if(strstr(oriData,"CWMODE_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWMODE_CUR;
    }
    else if(strstr(oriData,"CIFSR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIFSR;
    }
    else if(strstr(oriData,"CWQAP") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWQAP;
    }
    else if(strstr(oriData,"CWDHCP_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWDHCP_CUR;
    }
    else if(strstr(oriData,"CIPSTATUS") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSTATUS;
    }
    else if(strstr(oriData,"CIPMUX") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPMUX;
    }
    else if(strstr(oriData,"CIPSERVER") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSERVER;
    }
    else if(strstr(oriData,"CIPCLOSE") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPCLOSE;
    }
    else if(strstr(oriData,"CIPSENDBUF") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPSENDBUF;
    }                            
    else if(strstr(oriData,"CWJAP_CUR") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CWJAP_CUR;
    } 
    else if(strstr(oriData,"CIPMODE") != NULL)
    {
        sg_wifi_cmd_status = WIFI_CIPMODE;
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
    osDelay(500);
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
    wifi_sendCmdData(buff,len);
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
    wifi_sendCmdData(buff,len);
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
    wifi_sendCmdData(buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == wifi_Station_DhcpEnable)
        return 1;
    else 
        return 0;
}
int wifi_TcpIp_StartTcpConnection(char *RemoteIp,uint16_t RemotePort,uint16_t TimeOut)
{    
    char buff[100];
    int len = sprintf(buff,"AT+CIPSERVER=1,%d\r\n",RemotePort);
    wifi_sendCmdData(buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CIPSERVER)
        return 1;
    else 
        return 0;
    int len = sprintf(buff,"AT+CIPSTART=\"TCP\",\"%s\",%d,%d\r\n",RemoteIp,RemotePort,TimeOut);
    wifi_sendCmdData(buff,len);
    osDelay(2000);
    if(sg_rpych == WIFI_RPY_CONNECT && sg_wifi_cmd_status == WIFI_CIPSTATUS)
        return 1;
    else 
        return 0;
}

int wifi_TcpIp_Close()
{    
    char buff[50];
    int len = sprintf(buff,"AT+CIPCLOSE\r\n");
    wifi_sendCmdData(buff,len);
    osDelay(500);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CIPCLOSE)
        return 1;
    else 
        return 0;
}
int wifi_Station_ConnectToAp(char *SSID,char *Pass)
{    
    char buff[100];
    int len = sprintf(buff,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",SSID,Pass);
    wifi_sendCmdData(buff,len);
    osDelay(3000);
    if(sg_rpych == WIFI_RPY_OK && sg_wifi_cmd_status == WIFI_CWJAP_CUR)
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
    sg_wifiInitFlag |= wifi_SetRfPower(82);
    sg_wifiInitFlag |= wifi_TcpIp_Close();
    sg_wifiInitFlag |= wifi_Station_DhcpEnable(1);
    sg_wifiInitFlag |= wifi_SetCipmode(1);
    sg_wifiInitFlag |= wifi_Station_ConnectToAp(SSID,PASSWD);
    sg_wifiInitFlag |= wifi_TcpIp_StartTcpConnection(REMOTE_IP,REMOTE_PORT,CONNECT_TIME_OUT);
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

