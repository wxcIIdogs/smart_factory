/**
  ******************************************************************************
  * File Name          : app.c
  * Description        : Foreground application function.
	* Author             : Burin Sapsiri <burin@deaware.com>
  ******************************************************************************
	*/


#include "FreeRTOS.h"
#include "mqttApp.h"
#include <string.h>
#include "mqtt-msg.h"
#include "WifiUser.h"
#include "stdio.h"
#include "wirelessDevice.h"
#include "task.h"
#include "queue.h"
#include "pointManager.h"
static const uint8_t target_host[4] = {123, 56, 87, 60};
static const uint16_t target_port = 1883;


static uint8_t mqtt_buf[MQTT_BUF_LEN];
static uint8_t mqtt_is_connected = 0;
static mqtt_connection_t mqtt_conn;
static mqtt_connect_info_t connect_info;

#define MQTT_KEEP_ALIVE     30

static mqtt_message_t *connect_msg;

QueueHandle_t mqtt_SendQueue;
QueueHandle_t mqtt_RevQueue;




static void mqtt_init(void);
static void mqtt_connect(uint8_t *host, uint16_t port, char* cid);

static void mqtt_subscribe(char* topic, int qos);
static void mqtt_ping(void);
static void mqtt_connected(void);
static void mqtt_disconnected(void);
static void mqtt_published(void);
static void mqtt_subscribed(void);
static void mqtt_received(char *topic, uint16_t topic_len, char *data, uint16_t data_len);



static void mqtt_init(void)
{
	mqtt_msg_init(&mqtt_conn, mqtt_buf, MQTT_BUF_LEN);
}

static void mqtt_connect(uint8_t *host, uint16_t port, char* cid)
{
	connect_info.client_id = cid;
	connect_info.username = 0;
	connect_info.password = 0;
	connect_info.will_topic = 0;
	connect_info.will_message = 0;
	connect_info.keepalive = MQTT_KEEP_ALIVE;
	connect_info.will_qos = 0;
	connect_info.will_retain = 0;
	connect_info.clean_session = 0;
	
	connect_msg = mqtt_msg_connect(&mqtt_conn, &connect_info);
	send(APP_SOCKET_NO, connect_msg->data, connect_msg->length);
	// connect(APP_SOCKET_NO, host, port);
}

void mqtt_publish(char *topic, char *payload, uint16_t payload_length, int qos)
{
	mqtt_message_t *msg_ptr;
	int8_t rslt = 0;
	
	if (qos < 0) {
		qos = 0;
	}
	
	/* Not implement for qos 1, qos 2 yet. */
	if (qos > 0) {
		qos = 0;
	}
	
	msg_ptr = mqtt_msg_publish(&mqtt_conn, topic, payload, payload_length, qos, 0, &mqtt_conn.message_id);
	
	rslt = send(APP_SOCKET_NO, msg_ptr->data, msg_ptr->length);
	
	DEBUG_PRINT("send : %d\n", rslt)
	
	if (qos == 0) {
		mqtt_published();
	}
}

static void mqtt_subscribe(char* topic, int qos)
{
	mqtt_message_t *msg_ptr;

	if (qos < 0) {
		qos = 0;
	}
	
	/* Not implement for qos 1, qos 2 yet. */
	if (qos > 0) {
		qos = 0;
	}
	
	msg_ptr = mqtt_msg_subscribe(&mqtt_conn, topic, qos, &mqtt_conn.message_id);
	
	send(APP_SOCKET_NO, msg_ptr->data, msg_ptr->length);
	
	if (qos == 0) {
		mqtt_subscribed();
	}
}

static void mqtt_ping(void)
{
	mqtt_message_t *msg_ptr;
	msg_ptr = mqtt_msg_pingreq(&mqtt_conn);
	
	send(APP_SOCKET_NO, msg_ptr->data, msg_ptr->length);
}

static void mqtt_connected(void)
{
	DEBUG_PRINT("MQTT connectted.")
	mqtt_is_connected = 1;
}

static void mqtt_disconnected(void)
{
	DEBUG_PRINT("MQTT disconnectted.")
	
	mqtt_is_connected = 0;
}

static void mqtt_published(void)
{
//	DEBUG_PRINT("MQTT published.")
}

static void mqtt_subscribed(void)
{
	DEBUG_PRINT("MQTT subscribed.")
}




static void mqtt_received(char *topic, uint16_t topic_len, char *data, uint16_t data_len)
{
	stu_revDataInfo temp;

	memcpy((char*)temp.topic, topic, topic_len);
	temp.topic[topic_len] = '\0';
	
	memcpy((char*)temp.buff, data, data_len);
	temp.buff[data_len] = '\0';
	temp.data_len = data_len;
	
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;;
	xQueueSendToBackFromISR(mqtt_RevQueue,( void * )&temp,&xHigherPriorityTaskWoken);
//	xQueueSend(mqtt_RevQueue,(void *)&temp,1);

	// DEBUG_PRINT("MQTT received from %s : %s", buf, (buf + topic_len + 1))
}

void app_connected(void)
{
	DEBUG_PRINT("Connected")
	
	send(APP_SOCKET_NO, connect_msg->data, connect_msg->length);
}

void app_disconnected(void)
{
	DEBUG_PRINT("Disconnected")
	
	mqtt_disconnected();
}

void app_received(uint8_t *data, uint16_t len)
{
	int msg_type = 0;
	char *topic_ptr;
	uint16_t topic_len = MQTT_TOPIC_MAX;
	char *data_ptr;
	uint16_t data_len = MQTT_DATA_MAX;
	
//	DEBUG_PRINT("Received : %d", len)
	
	msg_type = mqtt_get_type(data);
	
	switch (msg_type) {
		case MQTT_MSG_TYPE_CONNECT:
				app_connected();
			break;
		case MQTT_MSG_TYPE_CONNACK:
			if (data[2] == 0x01) 
			{
				mqtt_connected();
			}
			break;
		case MQTT_MSG_TYPE_PUBLISH:
			topic_ptr = (char*)mqtt_get_publish_topic(data, &topic_len);
			data_ptr = (char*)mqtt_get_publish_data(data, &data_len);
			mqtt_received(topic_ptr, topic_len, data_ptr, data_len);
			break;
		case MQTT_MSG_TYPE_PUBACK:
			break;
		case MQTT_MSG_TYPE_PUBREC:
			break;
		case MQTT_MSG_TYPE_PUBREL:
			break;
		case MQTT_MSG_TYPE_PUBCOMP:
			break;
		case MQTT_MSG_TYPE_SUBSCRIBE:
			break;
		case MQTT_MSG_TYPE_SUBACK:
			if (data[4] != 0x80) {
				mqtt_subscribed();
			}
			break;
		case MQTT_MSG_TYPE_UNSUBSCRIBE:
			break;
		case MQTT_MSG_TYPE_UNSUBACK:
			break;
		case MQTT_MSG_TYPE_PINGREQ:
			break;
		case MQTT_MSG_TYPE_PINGRESP:
			break;
		case MQTT_MSG_TYPE_DISCONNECT:
			break;
		default:
			break;
	}
}

void app_sent(void)
{
	DEBUG_PRINT("Sent");
}

void app_init(void)
{
	mqtt_SendQueue = xQueueCreate(20,sizeof(stu_mqttSendInfo));
	mqtt_RevQueue = xQueueCreate(20,sizeof(stu_revDataInfo));
	mqtt_init();
	mqtt_connect((uint8_t*)target_host, target_port, "wxcClient");
	mqtt_subscribe("987654321",1);
}

void app_tick(void)
{
	static uint32_t prev = 0;
	static uint32_t prev2_heart = 0;
	uint32_t now = HAL_GetTick();	
	
	if (now - prev > (MQTT_KEEP_ALIVE * 1000)) 
	{
		prev = now;
		mqtt_ping();
	}
	if (now - prev2_heart > 10000)	//10s send heart
	{
		prev2_heart = now;
		if (mqtt_is_connected)
		{
			stu_mqttSendInfo temp;
			temp.len = getPointIDData(temp.buff);
			uint8_t buff[0xff+20];
			int index = 0;
			buff[index++] = 0x02;
			buff[index++] = temp.len * 3;			
			memcpy(buff+ index, temp.buff,temp.len * 3);			
			index += (temp.len * 3);
			memcpy(buff+index,&prev2_heart,4);
			index += 4;
			for(int i = 2 ; i < index ; i ++)
			{
				temp.crc += buff[i];
			}
			buff[index++] = temp.crc;

			mqtt_publish("123456789",(char *)buff,index, 1);
		}
	}
}

//1ms
void app_loopSendData()
{
	stu_mqttSendInfo temp;
	if(mqtt_is_connected && xQueueReceive(mqtt_SendQueue,(void *)&temp,1) )
	{
		uint8_t buff[0xff+20];
		int index = 0;
		buff[index++] = temp.cmd;
		buff[index++] = temp.len - 2;
		buff[index++] = temp.pointIP >> 8;
		buff[index++] = temp.pointIP & 0x00FF;
		memcpy(buff+ index, temp.buff,temp.len - 4);
		index += (temp.len - 4);
		memcpy(buff+index,&temp.count,4);
		index += 4;
		buff[index++] = temp.crc;
		mqtt_publish("123456789",(char *)buff,index, 1);
	}
	stu_revDataInfo rev_temp;
	if(xQueueReceive(mqtt_RevQueue,(void *)&rev_temp,1))
	{
		if(strcmp(rev_temp.topic,"987654321") == 0)
		{
			if(rev_temp.buff[0] == 0xA0)
				wirelessSendData((uint8_t *)rev_temp.buff + 1,rev_temp.data_len - 1);
		}
	}
}
void app_mqttSetSendBuff(uint8_t *buff,int len,int cmd)
{
	static int s_count = 0;
	stu_mqttSendInfo temp;
	temp.count = s_count++;
	memcpy(temp.buff,buff,len);
	temp.pointIP = cmd;
	temp.len = len;
	temp.cmd = 0x01;
	temp.crc += (temp.cmd + temp.len + temp.pointIP + temp.count);
	for(int i = 0 ; i < len ; i ++)
	{
		temp.crc += temp.buff[i];
	}
	if(!mqtt_is_connected)
		return;		
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;;
	xQueueSendToBackFromISR(mqtt_SendQueue,( void * )&temp,&xHigherPriorityTaskWoken);
	//xQueueSend(mqtt_SendQueue,( void * )&temp,0);
}

