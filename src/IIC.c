#include "IIC.h"
#include "UART.h"
#include "Delay.h"
#include "DigitLED.h"
#include "stdio.h"

unsigned int LM75_old = 0;
sbit SDA = P2^4;
sbit SCL = P2^5;

void LM75_Update()
{
    extern unsigned int LM75_old;
    unsigned int now;
    now = LM75_GetTemp();
    if(now != LM75_old)
    {
        LM75_old = now;
        UartSendStr("LM75: ");
        if (LM75_old / 1000 != 0)
            UartSend(LM75_old / 1000 + '0');
        UartSend(LM75_old % 1000 / 100 + '0');
        UartSend(LM75_old % 100 / 10 + '0');
        UartSend('.');
        UartSend(LM75_old % 10 + '0');
        UartSendStr("C\r\n");
        DigitLED_Write(LM75_old);
    }
}

unsigned int LM75_GetTemp(void)
{
    unsigned int t = 0;
    IIC_Start();
    IIC_SendData(0x9E); //device addr+write, 1001 1110B
    IIC_RecvACK();
    IIC_SendData(0x00); //temperature
    IIC_RecvACK();

    Delay30us();

    IIC_Start();
    IIC_SendData(0x9F); //device addr+read, 1001 1111B
    IIC_RecvACK();
    t = IIC_RecvData() << 8;
    IIC_SendACK();
    t += IIC_RecvData();
    IIC_SendNAK();
    IIC_Stop();
    t >>= 5;
    //UartSend(t);
    return t;
}

void IIC_Init()
{
    P_SW2 |= 0x80;       // SW IIC P2.4/P2.5
    P2M0 |= 0x30;
    P2M1 |= 0x30; 
    UartInitReport("LM75");
}

void IIC_Start()
{
    SDA = 1;
    SCL = 1;
    Delay12us();
    SDA = 0;
    Delay12us();
    SCL = 0;
}

void IIC_SendData(char dat)
{
    unsigned int i;
    for(i=0; i<8; i++)
    {
        SCL = 0;
        Delay12us();
        if(dat & 0x80) 
            SDA = 1;
        else 
            SDA = 0;
        Delay12us();
        SCL = 1;
        dat <<= 1;
    }
    SCL = 0;
}

bit IIC_RecvACK()
{
    bit ack = 0;
    SCL = 1;
    Delay12us();
    ack = SDA;
    SCL = 0;
    Delay12us();
    return ack;
}

char IIC_RecvData()
{
    unsigned int i;
    unsigned char dat;
    for(i=0; i<8; i++)
    {
        SCL = 1;
        Delay12us();
        dat <<= 1;
        if(SDA) dat += 1;
        SCL = 0;
        Delay12us();
    }
    return dat;
}

void IIC_SendACK()
{
    SCL = 0;
    SDA = 0;
    Delay12us();
    SCL = 1;
    Delay12us();
    SCL = 0;
    SDA = 1;
    Delay12us();
}

void IIC_SendNAK()
{
    SCL = 0;
    SDA = 1;
    Delay12us();
    SCL = 1;
    Delay12us();
    SCL = 0;
    SDA = 1;
    Delay12us();
}

void IIC_Stop()
{
    SDA = 0;
    SCL = 1;
    Delay12us();
    SDA = 1;
    Delay12us();
}

