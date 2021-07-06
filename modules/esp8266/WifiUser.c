
#include "Wifi.h"

//#######################################################################################
void	Wifi_UserInit(void)
{
	Wifi_SetMode(WifiMode_Station);
  //Wifi_SetMode(WifiMode_SoftAp); 

  while (Wifi_Station_ConnectToAp("wxc","853976858",NULL) == false);
 
    
}
//#######################################################################################
void  Wifi_UserProcess(void)
{
  static uint8_t last=0;
  if(strstr(Wifi.MyIP,"0.0.0.0")!=NULL)
  {    
    if(last!=1)
		{
			
		}
    last=1;
  }
  else
  {   
    if(last!=2)
    {
			//Wifi.MyGateWay
      Wifi_TcpIp_StartTcpConnection(0,"192.168.0.106",777,10);
    }
    last=2;
  }
}
//#######################################################################################
void  Wifi_UserGetUdpData(uint8_t LinkId,uint16_t DataLen,uint8_t *Data)
{
  Wifi_TcpIp_SendDataUdp(LinkId,2,(uint8_t*)"OK");
}
//#######################################################################################
void  Wifi_UserGetTcpData(uint8_t LinkId,uint16_t DataLen,uint8_t *Data)
{
  Wifi_TcpIp_SendDataTcp(LinkId,2,(uint8_t*)"OK");
}
//#######################################################################################
