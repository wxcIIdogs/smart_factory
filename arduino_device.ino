

//https://www.arduino.cc/reference/en/
int incomingByte = 0; // for incoming serial data

const int LED_R = 10;
const int LED_G = 11;
const int LED_B = 12;
const int FIRE_PIN = 13;
const int WATER_PIN = 14;
string rinputString = "";
string winputString = "";
int inputComplete = 0;
int sg_led_level = 0;

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



int readFireStatus()
{
    int val = analogRead(FIRE_PIN);  // read the input pin  
    return val;
}

void setLedStatus(int level)
{
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_G, LOW);
    digitalWrite(LED_B, LOW);    
    sg_led_level = level;
    if(level == 1)
    {
        digitalWrite(LED_R, HIGH);
    }
    else if(level == 2)
    {
        digitalWrite(LED_G, HIGH);
    }
    else if(level == 3)
    {
        digitalWrite(LED_B, HIGH);
    }
}

void readDHT11(short *hum,short *tmp)
{

}
int readwaterLevel()
{
    int val = analogRead(WATER_PIN);  // read the input pin  
    return val;
}

short readBMP180()
{
    return 0;
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    rinputString += inChar;
    // if the incoming character is a newline, set a flag so the main loop can
    // do something about it:
    if(inChar == 0xFF)
    {
        inputComplete = 1;
        winputString = rinputString;
        rinputString = "";
    }
  }
}
void setup() 
{
    Serial.begin(115200); // opens serial port, sets data rate to 115200 bps
    pinMode(LED_R, OUTPUT);
    pinMode(LED_G, OUTPUT);
    pinMode(LED_B, OUTPUT);
    rinputString.reserve(0xFF);
    winputString.reserve(0xFF);
}
void optPoint(unsigned char *opt,int len)
{
    int index = 0;
    while(index < len)
    {
        int cmd = opt[index++];
        if(cmd == 0xA0)
        {
            //open led
            setLedStatus(opt[index++]);
        }
    }
}

void ansyWireLessData(string inputString)
{
    int flag = WIRE_LENGTH;
    int len = 0;
    int count = 0;
    unsigned char buff[0xff];
    for(int i = 1; i < inputString.length() ; i ++)
    {
        unsigned char data = inputString[i];
        if(inputString[i] == 0xFE && inputString[i + 1] == 0xFD)
        {
            i ++;
            data = 0XFF;
        }
        if(inputString[i] == 0xFE && inputString[i + 1] == 0xFC)
        {
            i ++;
            data = 0XFE;
        }    
        switch(flag)
        {
            case WIRE_LENGTH:
                len = data;
                flag = WIRE_CMD;
            break;
            case WIRE_CMD:
                count ++;
                if(count == 4)
                {
                    flag = WIRE_DATA;                    
                    count  = 0;
                }                
            break;
            case WIRE_DATA:                
                buff[count++] = data;
                if(count == len)
                {
                   flag = WIRE_CRC;
                }
            break;
            case WIRE_CRC:
                if(data == 0xFF)
                {
                    flag = WIRE_LENGTH;
                    count = 0;
                    optPoint(buff,len);
                }
            break;
            default:

            break;

        } 
    }
}


void sendSensorDataWireLess()
{
    unsigned char sendBuff[0xFF + 30];
    int sendIndex = 0;
    sendBuff[sendIndex++] = 0XFE;
    sendBuff[sendIndex++] = 0; //len
    sendBuff[sendIndex++] = 90; //source point
    sendBuff[sendIndex++] = 91; //target point
    sendBuff[sendIndex++] = 0x01;    //network ip
    sendBuff[sendIndex++] = 0x00;    //network ip

    //led status
    sendBuff[sendIndex++] = 0x05;       //cmd
    sendBuff[sendIndex++] = sg_led_level;    //data

    sendBuff[sendIndex++] = 0x04;        //cmd
    sendBuff[sendIndex++] = readFireStatus()    //data

    short hum,tmp;
    readDHT11(&hum,&tmp);
    sendBuff[sendIndex++] = 0x01;        //cmd
    sendBuff[sendIndex++] = tmp >> 8;    //data
    sendBuff[sendIndex++] = tmp &0x00FF;    //data

    sendBuff[sendIndex++] = 0x02;        //cmd
    sendBuff[sendIndex++] = hum >> 8;    //data
    sendBuff[sendIndex++] = hum &0x00FF;    //data

    int waterLevel = readwaterLevel();
    sendBuff[sendIndex++] = 0x0A;        //cmd
    sendBuff[sendIndex++] = waterLevel;    //data

    //get pressure
    // sendBuff[sendIndex++] = 0x03;        //cmd
    // sendBuff[sendIndex++] = hum >> 8;    //data
    // sendBuff[sendIndex++] = hum &0x00FF;    //data
    sendBuff[sendIndex++] = 0xFF;    //data
    Serial.print(sendBuff,sendIndex);
}



void loop() 
{
    if(inputComplete == 1)
    {
        ansyWireLessData(winputString);
    }
}