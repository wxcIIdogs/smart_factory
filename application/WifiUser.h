#ifndef __WIFILIB__
#define __WIFILIB__
#include "main.h"












extern int getWifiConnectFlag(void);
extern void initWifiusart(void);
extern int wifiWriteData(int id,uint8_t *data,int len);
#endif
