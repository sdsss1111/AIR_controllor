/*
*********************************************************************************************************
*
*	ģ������ : ���ڿ��Ź�����ͷ�ļ�
*	�ļ����� : bsp_wwdg.h
*	��    �� : V1.0
*	˵    �� : IWDG���̡�
*   �޸ļ�¼ :
*		�汾��    ����          ����                 ˵��
*		v1.0    2012-12-23    pigpeng   ST�̼���汾 V3.5.0�汾��
*
*	Copyright (C), 2012-2013,
*********************************************************************************************************
*/

#ifndef _BSP_WWDG_H
#define _BSP_WWDG_H

#define      setTreg       0x7f//: T[6:0],������ֵ 	��Χ0x40 �� 0x7F
#define      setWreg       0x60//: W[6:0],����ֵ     ����С�� 0x80
//#define      WWDG_Prescaler //: ���ڿ��Ź���Ƶ	PCLK1 = 42MHz
//#define      WWDG_Prescaler_1//: WWDG counter clock = (PCLK1/4096)/1
//#define      WWDG_Prescaler_2//: WWDG counter clock = (PCLK1/4096)/2
//#define      WWDG_Prescaler_4//: WWDG counter clock = (PCLK1/4096)/4
//#define      WWDG_Prescaler_8//: WWDG counter clock = (PCLK1/4096)/8


//���Ź�����ֵΪ0x000~0xFFF
#define  Reload_Cnt   0xFFF//���Ź�����ֵ0x0FFF


void bsp_InitIwdg(void);//�������Ź���ʼ������
void bsp_Iwdg_Feed(void);//����ι��

/* ��ʼ�� ���ڿ��Ź� */
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler);

#endif
