/**
  ******************************************************************************
  * File Name          : app.h
  * Description        : Foreground application function.
	* Author             : Burin Sapsiri <burin@deaware.com>
  ******************************************************************************
	*/

#ifndef _MQTTAPP_H
#define _MQTTAPP_H

#include <main.h>
#include "Wifi.h"

#define MQTT_BUF_LEN				1024
#define MQTT_TOPIC_MAX              32
#define MQTT_DATA_MAX               32


#define APP_SOCKET_NO               0

#define send                    Wifi_TcpIp_SendDataTcp





void app_connected(void);
void app_disconnected(void);
void app_received(uint8_t *data, uint16_t len);
void app_sent(void);
void app_init(void);
void app_tick(void);

#endif /*_APP_H*/
