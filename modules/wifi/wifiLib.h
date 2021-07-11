#ifndef __WIFILIB_H_
#define __WIFILIB_H_

#include "main.h"

#define WIFI_RPY_OK         1
#define WIFI_RPY_ERROR      2
#define WIFI_RPY_CONNECT    3
#define WIFI_RPY_REPLY	    4
#define WIFI_RPY_ALREADY	  5
#define WIFI_RPY_TRANSPART	6


typedef enum
{
    WIFI_RFPOWER = 0,
    WIFI_CWMODE_CUR,
    WIFI_CIFSR,
    WIFI_CWQAP,
    WIFI_CWDHCP_CUR,
    WIFI_CIPSTATUS,
    WIFI_CIPMUX,
    WIFI_CIPSERVER,
    WIFI_CIPCLOSE,
    WIFI_CIPSENDBUF,
    WIFI_CWJAP_CUR,
    WIFI_CIPMODE,
		WIFI_CIPSEND,
    WIFI_NONE,
}enum_WIFI_CMD;


extern void setWifiData(uint8_t *oriData,int len);
extern int wifi_sendData(uint8_t *data,int len);
extern int wifiInit(void);
extern void wifiLibLoop(void);

#endif
