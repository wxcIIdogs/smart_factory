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
#include "WifiUser.h"

#define MQTT_BUF_LEN				1024
#define MQTT_TOPIC_MAX              32
#define MQTT_DATA_MAX               32


#define APP_SOCKET_NO               0

#define send                    	wifiWriteData
#define DEBUG_PRINT(format,...)		printf(format,##__VA_ARGS__);

typedef struct 
{
	uint8_t buff[0xFF];
	uint8_t crc;	
	uint8_t cmd;
	int len;
	int pointIP;
	int count;	
}stu_mqttSendInfo;

typedef struct
{
	char topic[30];
	char buff[0xff];
	int data_len;
}stu_revDataInfo;

extern void mqtt_publish(char *topic, char *payload, uint16_t payload_length, int qos);


void app_connected(void);
void app_disconnected(void);
void app_received(uint8_t *data, uint16_t len);
void app_sent(void);
void app_init(void);
void app_tick(void);
void app_loopSendData(void);
void app_mqttSetSendBuff(uint8_t *buff,int len,int cmd);
#endif /*_APP_H*/
