#ifndef __WIRELESSDEVICE__H
#define __WIRELESSDEVICE__H

#include "main.h"

typedef enum
{    
    WIRE_HEAD = 0,
    WIRE_CMD,
    WIRE_LENGTH,
    WIRE_DATA,
    WIRE_COUNT,
    WIRE_CRC,
    WIRE_NONE,
}wireLessEnum;



extern 	void initWirelessUsart(void);
extern 	void wirelessWriteDebug(uint8_t *data,int len);
extern  void wirelessSendData(uint8_t *revbuff,int len);

#endif
