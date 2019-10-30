#ifndef __KEY_H
#define __KEY_H
#include "../inc/sys.h"
#include "../../bsp/bsp.h"
extern uint8_t test_data[];
/********************************************按键宏定义**********************************************************/
#define key_kaiguan  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)   	//读取按键开关
#define key_wendujia  	        GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)   	//读取按键温度加
#define key_fengsu  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)    	//读取按键风速
#define key_wendujian           GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)    	//读取按键温度减
#define key_moshi   		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)		//读取按键模式

#define fanhuikey_kaiguan 	1	//KEY0按下
#define fanhuikey_wendujia	2	//KEY1按下
#define fanhuikey_fengsu	3	//KEY2按下
#define fanhuikey_wendujian   4	//KEY_UP按下(即WK_UP/KEY_UP)
#define fanhuikey_moshi   5


#define key_kaiguan_pin           GPIO_Pin_6
#define key_wendujia_pin          GPIO_Pin_3
#define key_fengsu_pin            GPIO_Pin_4
#define key_wendujian_pin         GPIO_Pin_2
#define key_moshi_pin                 GPIO_Pin_5

#define key_port                  GPIOA

/********************************************按键宏定义结束***********************************************************/
#define aircontroller_datalen  6
/*******************************************运行状态宏定义****************************************************/
#define HIGH_SPEED    GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_ResetBits(GPIOA, GPIO_Pin_8); GPIO_ResetBits(GPIOA, GPIO_Pin_9);
#define MID_SPEED     GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_SetBits(GPIOA, GPIO_Pin_8); GPIO_ResetBits(GPIOA, GPIO_Pin_9);
#define LOW_SPEED     GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_SetBits(GPIOA, GPIO_Pin_8); GPIO_SetBits(GPIOA, GPIO_Pin_9);
#define CLOUSE        GPIO_SetBits(GPIOB, GPIO_Pin_13);GPIO_ResetBits(GPIOA, GPIO_Pin_8);GPIO_ResetBits(GPIOA, GPIO_Pin_9);
/*******************************************运行状态宏定义结束****************************************************/
void KEY_Init(void);//IO初始化
void moshixuanze(void);
void KEY_CAOZUO(void);
u8 KEY_Scan(u8);  	//按键扫描函数
void ChushiHuashedingwendu();//初始化设定温度
void dingyishangchuanshuzu();

result_t prase_aircontrol(uint8_t *tempbuff); //struct AirControler_Receive *temp

#endif
