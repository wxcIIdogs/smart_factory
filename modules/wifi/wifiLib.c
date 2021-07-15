#include "wifiLib.h"
#include "wifiLibConfig.h"
#include "main.h"
#include "usart.h"
#include "string.h"
#include "stdlib.h"
#include "cmsis_os.h"
#include "stdio.h"
static int sg_wifi_cmd_status = 0;
static int sg_rpych = 0;    
static int sg_wifiInitFlag = 0;
static uint8_t sg_revCmdBuff[30];

void setWifiCmdBuff(uint8_t *buff)
{
    if(!buff)
    {        
        if(sg_revCmdBuff[0] != '\0')
        {
            memset(sg_revCmdBuff,0,30);
        }
        strcpy(sg_revCmdBuff,buff);
    }
}
int getWifiCmdStatus()
{
    return sg_wifi_cmd_status;
}


int getwifiRpyFlag()
{
    return sg_wifiInitFlag;
}
void wifi_rpyCallBack(uint8_t *buff)
{
    sg_wifiInitFlag = 0;
}
const static stu_wifiCmdRpyInfo sg_stuWifiCmdStatusBuff[] = 
{
    {"WIFI GOT IP",WIFI_RPY_REPLY},
    {"ALREADY",WIFI_RPY_ALREADY},
    {"ERROR",WIFI_RPY_ERROR},
    {"CONNECTED",WIFI_RPY_CONNECT},
    {"CONNECT",WIFI_RPY_CONNECT},
    {"CLOSE",WIFI_RPY_CLOSE,wifi_rpyCallBack},
    {"DISCONNECT",WIFI_RPY_DISCONNECT,wifi_rpyCallBack},
    {">",WIFI_RPY_TRANSPART},
    {"OK",WIFI_RPY_OK},
};


int setWifiCmdStatus(int rpy)
{
    static int wifiStatus = 1;
    wifiStatus &= rpy;
    if(!wifiStatus)
    {
        printf("%s cmd had error rpyflag = %d\n",sg_revCmdBuff,sg_rpych);
    }
    return wifiStatus;
}


/**
 * 
 * 
 * 
 */
void setWifiData(uint8_t *oriData,int len)
{
//rpy
    for(int i = 0 ; i < ARRAY_LEN(sg_stuWifiCmdStatusBuff) ; i ++)
    {
        if(strstr(oriData,sg_stuWifiCmdStatusBuff[i].revCmdBuff) != NULL)
        {
            sg_rpych |= sg_stuWifiCmdStatusBuff[i].reyStatus;
            if(sg_stuWifiCmdStatusBuff[i].callback != NULL)
            {
                sg_stuWifiCmdStatusBuff[i].callback(sg_stuWifiCmdStatusBuff[i].revCmdBuff);
            }            
        }
    }
     
//cmd     
    if(strstr((char *)oriData,sg_revCmdBuff) != NULL)
    {        
        sg_wifi_cmd_status = 1;
        
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
    sg_wifi_cmd_status = 0;
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
    if(sg_rpych & WIFI_RPY_OK)
        return 1;
    else 
        return 0;
}
int wifi_RST()
{
    wifi_sendCmdData("AT+RST\r\n",8);
    osDelay(1000);
    if(sg_rpych & WIFI_RPY_OK)
        return 1;
    else 
        return 0;
}

//timeout ms
int waitWifiCmdStatus(int timeOut,int rpy)
{
    for(int i = 0; i < timeOut ; i ++)
    {
        osDelay(1);
        if(sg_wifi_cmd_status && (rpy & rpy))
            return WIFI_RPY_SUCCESS;
    }
    return sg_rpych;
    
}
//1 trasport 
//0 cmd
int wifi_SetCipmode(int mode)
{
    char buff[50];
    int len = sprintf(buff,"AT+CIPMODE=%d\r\n",mode);
    setWifiCmdBuff("CIPMODE");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(500,WIFI_RPY_OK);    
    if(rpy == WIFI_RPY_SUCCESS)
        return 1;
    else 
        return 0;
}
int wifi_SetRfPower(int Power_0_to_82)
{
    char buff[50];
    int len = sprintf(buff,"AT+RFPOWER=%d\r\n",Power_0_to_82);
    setWifiCmdBuff("RFPOWER");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(500,WIFI_RPY_OK);    
    if(rpy == WIFI_RPY_SUCCESS)
        return 1;
    else 
        return 0;
}

int wifi_Station_DhcpEnable(int Enable)
{    
    char buff[50];
    int len = sprintf(buff,"AT+CWDHCP_CUR=1,%d\r\n",Enable);
    setWifiCmdBuff("CWDHCP_CUR");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(500,WIFI_RPY_OK);    
    if(rpy == WIFI_RPY_SUCCESS)
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
        setWifiCmdBuff("CIPSTART");
        wifi_sendCmdData((uint8_t *)buff,len);
        osDelay(1000);		
        for(int i = 0;  i < 10 ; i ++)
        {            		
            if(sg_rpych & WIFI_RPY_REPLY)
                continue;
            if(WIFI_RPY_ALREADY & sg_rpych)
                return 1;
            if(sg_rpych & WIFI_RPY_CONNECT && sg_wifi_cmd_status == 1)
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
    setWifiCmdBuff("CIPCLOSE");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(500,WIFI_RPY_OK);    
    if(rpy == WIFI_RPY_SUCCESS)
        return 1;
    else 
        return 0;
}
int wifi_Station_ConnectToAp(char *ssid,char *Pass)
{    
    char buff[100];
    int len = sprintf(buff,"AT+CWJAP_CUR=\"%s\",\"%s\"\r\n",ssid,Pass);
    setWifiCmdBuff("CWJAP_CUR");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(10000,WIFI_RPY_CONNECT); 
    if(rpy == WIFI_RPY_SUCCESS)
        return 1;
    else 
        return 0;
}
int wifi_cipSend()
{
	  char buff[50];
    int len = sprintf(buff,"AT+CIPSEND\r\n");
    setWifiCmdBuff("CIPSEND");
    wifi_sendCmdData((uint8_t *)buff,len);
    int rpy = waitWifiCmdStatus(500,WIFI_RPY_TRANSPART);    
    if(rpy == WIFI_RPY_SUCCESS)
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
    setWifiCmdStatus(wifi_SetRfPower(82));
    wifi_TcpIp_Close();
    setWifiCmdStatus(wifi_Station_DhcpEnable(1));
    setWifiCmdStatus(wifi_Station_ConnectToAp(SSID,PASSWD));
    setWifiCmdStatus(wifi_TcpIp_StartTcpConnection(REMOTE_IP,REMOTE_PORT,CONNECT_TIME_OUT));
    setWifiCmdStatus(wifi_SetCipmode(1));
    sg_wifiInitFlag = setWifiCmdStatus(wifi_cipSend());    
    printf("sg_wifiInitFlag = %d\r\n",sg_wifiInitFlag);
}
/**
 * 
 * 
 * 
 */
int wifiInit()
{
    wifi_userMain();
    return sg_wifiInitFlag;
}

