#ifndef __WIFILIB_H_
#define __WIFILIB_H_

#define WIFI_RPY_OK         1
#define WIFI_RPY_ERROR      2
#define WIFI_RPY_CONNECT    3



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
    WIFI_NONE,
}enum_WIFI_CMD;


extern void setWifiData(uint8_t *oriData,int len);
extern int wifi_sendData(uint8_t *data,int len);
extern int wifiInit(void);
extern void wifiLibLoop(void);

#endif
