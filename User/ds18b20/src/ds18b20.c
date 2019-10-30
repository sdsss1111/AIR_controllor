

///******************* (C) COPYRIGHT 2013 CEPARK�๦�ܿ���ѧϰ��*****END OF FILE****/


#include "../inc/ds18b20.h"
#include "bsp.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK miniSTM32������
//DS18B20��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2012/9/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////


//��λDS18B20
void DS18B20_Rst(void)
{
    DS18B20_IO_OUT(); //SET PA0 OUTPUT
    DS18B20_DQ_OUT = 0; //����DQ
    bsp_DelayUS(750);    //����750us
    DS18B20_DQ_OUT = 1; //DQ=1
    bsp_DelayUS(15);     //15US
}
//�ȴ�DS18B20�Ļ�Ӧ
//����1:δ��⵽DS18B20�Ĵ���
//����0:����
u8 DS18B20_Check(void)
{
    u8 retry = 0;
    DS18B20_IO_IN();//SET PA0 INPUT
    while (DS18B20_DQ_IN && retry < 200)
    {
        retry++;
        bsp_DelayUS(1);
    };
    if(retry >= 200)return 1;
    else retry = 0;
    while (!DS18B20_DQ_IN && retry < 240)
    {
        retry++;
        bsp_DelayUS(1);
    };
    if(retry >= 240)return 1;
    return 0;
}
//��DS18B20��ȡһ��λ
//����ֵ��1/0
u8 DS18B20_Read_Bit(void) 			 // read one bit
{
    u8 data;
    DS18B20_IO_OUT();//SET PA0 OUTPUT
    DS18B20_DQ_OUT = 0;
    bsp_DelayUS(2);
    DS18B20_DQ_OUT = 1;
    DS18B20_IO_IN();//SET PA0 INPUT
    bsp_DelayUS(12);
    if(DS18B20_DQ_IN)data = 1;
    else data = 0;
    bsp_DelayUS(50);
    return data;
}
//��DS18B20��ȡһ���ֽ�
//����ֵ������������
u8 DS18B20_Read_Byte(void)    // read one byte
{
    u8 i, j, dat;
    dat = 0;
    for (i = 1; i <= 8; i++)
    {
        j = DS18B20_Read_Bit();
        dat = (j << 7) | (dat >> 1);
    }
    return dat;
}
//дһ���ֽڵ�DS18B20
//dat��Ҫд����ֽ�
void DS18B20_Write_Byte(u8 dat)
{
    u8 j;
    u8 testb;
    DS18B20_IO_OUT();//SET PA0 OUTPUT;
    for (j = 1; j <= 8; j++)
    {
        testb = dat & 0x01;
        dat = dat >> 1;
        if (testb)
        {
            DS18B20_DQ_OUT = 0; // Write 1
            bsp_DelayUS(2);
            DS18B20_DQ_OUT = 1;
            bsp_DelayUS(60);
        }
        else
        {
            DS18B20_DQ_OUT = 0; // Write 0
            bsp_DelayUS(60);
            DS18B20_DQ_OUT = 1;
            bsp_DelayUS(2);
        }
    }
}
//��ʼ�¶�ת��
void DS18B20_Start(void)// ds1820 start convert
{
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0x44);// convert
}
//��ʼ��DS18B20��IO�� DQ ͬʱ���DS�Ĵ���
//����1:������
//����0:����
u8 DS18B20_Init(void)
{
    GPIO_InitTypeDef  GPIO_InitStructure;

    //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	 //ʹ��PORTA��ʱ��

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;				//PORTA0 �������
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_1);   //���1

    DS18B20_Rst();

    return DS18B20_Check();
}
//��ds18b20�õ��¶�ֵ
//���ȣ�0.1C
//����ֵ���¶�ֵ ��-550~1250��
short DS18B20_Get_Temp(void)
{
    u8 temp;
    u8 TL, TH;
    u16 tem;
    DS18B20_Start ();                    // ds1820 start convert
    DS18B20_Rst();
    DS18B20_Check();
    DS18B20_Write_Byte(0xcc);// skip rom
    DS18B20_Write_Byte(0xbe);// convert
    TL = DS18B20_Read_Byte(); // LSB
    TH = DS18B20_Read_Byte(); // MSB

    if(TH > 7)
    {
        TH = ~TH;
        TL = ~TL;
        temp = 0; //�¶�Ϊ��
    }
    else temp = 1; //�¶�Ϊ��
    tem = TH; //��ø߰�λ
    tem <<= 8;
    tem += TL; //��õװ�λ
    tem = (u16)((float)tem * 0.625); //ת��
    if(temp)
        return tem; //�����¶�ֵ
    else
        return -tem;
}


void Gettempt_Show(void)
{
    u8 shiwei, gewei, temp;
    short wendu, temperature;

    //extern u8 up_buff[2];
    temperature = DS18B20_Get_Temp();
    wendu = temperature - 30;
    if(wendu < 0)
    {

        wendu = -wendu;					//????
    }
    else
        temp = wendu / 10;
    shiwei = temp / 10;
    gewei = temp % 10;
    if(gewei == 0)
    {
        shineigewei0();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 1)
    {
        shineigewei1();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 2)
    {
        shineigewei2();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 3)
    {
        shineigewei3();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 4)
    {
        shineigewei4();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 5)
    {
        shineigewei5();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 6)
    {
        shineigewei6();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 7)
    {
        shineigewei7();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }
    if(gewei == 8)
    {
        shineigewei8();
        if(shiwei == 3)shineishiwei3();
        else shineishiwei2();
    }
    if(gewei == 9)
    {
        shineigewei9();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else shineishiwei3();
    }

    //memcpy(test_data, up_buff, sizeof(up_buff));
}

