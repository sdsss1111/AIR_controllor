/*
*********************************************************************************************************
*
*	模块名称 : 汉字点阵字库。
*	文件名称 : hz24.c
*	版    本 : V1.0
*	说    明 : 只包含本程序用到汉字字库
*	修改记录 :
*		版本号  日期       作者    说明
*		v1.0    2015-04-05 armfly  ST固件库V3.5.0版本。
*
*	Copyright (C), 2015-2016, 安富莱电子 www.armfly.com
*
*********************************************************************************************************
*/

#include "fonts.h"

/*
	FLASH中内嵌小字库，只包括本程序用到的汉字点阵
	每行点阵数据，头2字节是汉子的内码，后面字节是点阵汉字的字模数据。
*/
#ifdef USE_SMALL_FONT
/*
*/
unsigned char const g_Hz24[] =
{

    //0xD7,0xBC, 0x01,0x40,0x41,0x20,0x31,0x24,0x13,0xFE,0x02,0x20,0x16,0x28,0x1B,0xFC,0x12,0x20,// 准 //
    //           0x22,0x28,0xE3,0xFC,0x22,0x20,0x22,0x20,0x22,0x24,0x23,0xFE,0x22,0x00,0x02,0x00,

    /* 最后一行必须用0xFF,0xFF结束，这是字库数组结束标志 */
    0xFF, 0xFF

};
#else
unsigned char const g_Hz24[] = {0xFF, 0xFF};
#endif

