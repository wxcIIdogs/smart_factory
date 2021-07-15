#ifndef __WIFILIB_H_
#define __WIFILIB_H_

#include "main.h"


#define WIFI_RPY_NONO       0x00
#define WIFI_RPY_OK         0x01
#define WIFI_RPY_ERROR      0x02
#define WIFI_RPY_CONNECT    0x04
#define WIFI_RPY_REPLY	    0x08
#define WIFI_RPY_ALREADY	  0x10
#define WIFI_RPY_TRANSPART	0x20
#define WIFI_RPY_DISCONNECT	0x40
#define WIFI_RPY_CLOSE    	0x80
#define WIFI_RPY_SUCCESS    0xFF


#define ARRAY_LEN(__X__)      (sizeof(__X__) / sizeof(__X__[0]));


typedef void (*rpyFucn)(uint8_t *buff);
typedef struct 
{
    uint8_t revCmdBuff[30];
    int reyStatus; 
    rpyFucn callback; 

}stu_wifiCmdRpyInfo;



extern void setWifiData(uint8_t *oriData,int len);
extern int wifi_sendData(uint8_t *data,int len);
extern int wifiInit(void);
extern int getwifiRpyFlag(void);
#endif
