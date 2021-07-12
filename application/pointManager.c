#include "main.h"
#include "revDataInfo.h"
#include "pointManager.h"

/*
*author:Wxc
*info:
*data: 2021-07-07
*/

typedef struct 
{
    short pointID;
    int status;
    unsigned int last_time;
}stu_pointHeart;
static stu_pointHeart sg_pointHeartBuff[100];
static int sg_heartBuffLen = 0;
void setPointId(short id)
{
    short pointID = id;
    for(int i = 0 ; i < sg_heartBuffLen ; i ++)
    {
        if(pointID == sg_pointHeartBuff[i].pointID)
        {
            sg_pointHeartBuff[i].status = 1;
            sg_pointHeartBuff[i].last_time = HAL_GetTick();
			return;
        }
    }
    sg_pointHeartBuff[sg_heartBuffLen].pointID = pointID;
    sg_pointHeartBuff[sg_heartBuffLen].status = 1;
    sg_pointHeartBuff[sg_heartBuffLen].last_time = HAL_GetTick();
    sg_heartBuffLen++;
}

int getPointIDData(uint8_t *buff)
{
    int index = 0;
    for(int i = 0; i < sg_heartBuffLen ; i ++)
    {        
        memcpy(buff+index,&sg_pointHeartBuff[i].pointID,2);
        index += 2;
        buff[index++] = sg_pointHeartBuff[i].status;
    }
    return sg_heartBuffLen;
}

//1s
void loopPointHeart()
{
    for(int i = 0; i < sg_heartBuffLen ; i ++)
    {
        if(HAL_GetTick() - sg_pointHeartBuff[i].last_time > 10000)  //10s
        {
            sg_pointHeartBuff[i].status = 0;
        }
    }
}


