/*
*********************************************************************************************************
*
*	ģ������ : ���ֵ����ֿ⡣
*	�ļ����� : hz32.c
*	��    �� : V1.0
*	˵    �� : ֻ�����������õ������ֿ�
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2015-04-05 armfly  ST�̼���V3.5.0�汾��
*
*	Copyright (C), 2015-2016, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "fonts.h"

/*
	FLASH����ǶС�ֿ⣬ֻ�����������õ��ĺ��ֵ���
	ÿ�е������ݣ�ͷ2�ֽ��Ǻ��ӵ����룬�����ֽ��ǵ����ֵ���ģ���ݡ�
*/
#ifdef USE_SMALL_FONT
/*
*/
unsigned char const g_Hz32[] =
{

    //0xD7,0xBC, 0x01,0x40,0x41,0x20,0x31,0x24,0x13,0xFE,0x02,0x20,0x16,0x28,0x1B,0xFC,0x12,0x20,// ׼ //
    //           0x22,0x28,0xE3,0xFC,0x22,0x20,0x22,0x20,0x22,0x24,0x23,0xFE,0x22,0x00,0x02,0x00,

    /* ���һ�б�����0xFF,0xFF�����������ֿ����������־ */
    0xFF, 0xFF

};
#else
unsigned char const g_Hz32[] = {0xFF, 0xFF};
#endif
