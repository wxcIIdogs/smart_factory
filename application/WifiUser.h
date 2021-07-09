#ifnde __WIFILIB__
#define __WIFILIB__













extern int getWifiConnectFlag(void);
extern void initWifiusart(void);
extern void wifiWriteData(uint8_t *data,int len);
#endif
