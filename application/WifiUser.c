
#include "WifiLib.h"
#include "mqttApp.h"
#include "revDataInfo.h"

#include "stdio.h"
static int wifiConnectFlag = 0;
//ge wifi connect 1:ok  0:error
int getWifiConnectFlag()
{
  return wifiConnectFlag;
}

void wifiRevData(uint8_t *buff,int32_t len)
{
  if(getWifiConnectFlag())
  {
    //connect success
    app_received(buff,len);
  }
  else
  {
    setWifiData(buff,len);
	printf("%s\r\n",buff);
  }
  
}
/**
 *  10ms
 *
 */
void wifiLibLoop()
{        
    if(!getwifiRpyFlag())
    {
        wifiConnectFlag = wifiInit();
    }
}


void initWifiusart()
{		
	createFifoRev(&huart2,wifiRevData,UART_REV_DMA);
	wifiConnectFlag = wifiInit();
}

int wifiWriteData(int id,uint8_t *data,int len)
{
  return wifi_sendData(data,len);
}

