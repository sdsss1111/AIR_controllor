#ifndef __DS18B20_H
#define __DS18B20_H
#include "../inc/sys.h"
//////////////////////////////////////////////////////////////////////////////////
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK MiniSTM32������
//DS18B20��������
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/3/12
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2019
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

#define EndDevisAircontroler

//IO��������
#define DS18B20_IO_IN()  {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=8<<0;}
#define DS18B20_IO_OUT() {GPIOB->CRL&=0XFFFFFFF0;GPIOB->CRL|=3<<0;}
////IO��������
#define	DS18B20_DQ_OUT PBout(1) //���ݶ˿�	PA0
#define	DS18B20_DQ_IN  PBin(1)  //���ݶ˿�	PA0 




u8 DS18B20_Init(void);			//��ʼ��DS18B20
short DS18B20_Get_Temp(void);	//��ȡ�¶�
void DS18B20_Start(void);		//��ʼ�¶�ת��
void DS18B20_Write_Byte(u8 dat);//д��һ���ֽ�
u8 DS18B20_Read_Byte(void);		//����һ���ֽ�
u8 DS18B20_Read_Bit(void);		//����һ��λ
u8 DS18B20_Check(void);			//����Ƿ����DS18B20
void DS18B20_Rst(void);			//��λDS18B20
void Gettempt_Show(void);
#endif















