/*
*********************************************************************************************************
*
*	模块名称 : 窗口看门狗程序头文件
*	文件名称 : bsp_wwdg.h
*	版    本 : V1.0
*	说    明 : IWDG例程。
*   修改记录 :
*		版本号    日期          作者                 说明
*		v1.0    2012-12-23    pigpeng   ST固件库版本 V3.5.0版本。
*
*	Copyright (C), 2012-2013,
*********************************************************************************************************
*/

#ifndef _BSP_WWDG_H
#define _BSP_WWDG_H

#define      setTreg       0x7f//: T[6:0],计数器值 	范围0x40 到 0x7F
#define      setWreg       0x60//: W[6:0],窗口值     必须小于 0x80
//#define      WWDG_Prescaler //: 窗口看门狗分频	PCLK1 = 42MHz
//#define      WWDG_Prescaler_1//: WWDG counter clock = (PCLK1/4096)/1
//#define      WWDG_Prescaler_2//: WWDG counter clock = (PCLK1/4096)/2
//#define      WWDG_Prescaler_4//: WWDG counter clock = (PCLK1/4096)/4
//#define      WWDG_Prescaler_8//: WWDG counter clock = (PCLK1/4096)/8


//看门狗重载值为0x000~0xFFF
#define  Reload_Cnt   0xFFF//看门狗重载值0x0FFF


void bsp_InitIwdg(void);//独立看门狗初始化函数
void bsp_Iwdg_Feed(void);//独立喂狗

/* 初始化 窗口看门狗 */
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler);

#endif
