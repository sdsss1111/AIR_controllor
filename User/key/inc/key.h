#ifndef __KEY_H
#define __KEY_H
#include "../inc/sys.h"
#include "../../bsp/bsp.h"
extern uint8_t test_data[];
/********************************************�����궨��**********************************************************/
#define key_kaiguan  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_6)   	//��ȡ��������
#define key_wendujia  	        GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_3)   	//��ȡ�����¶ȼ�
#define key_fengsu  		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_4)    	//��ȡ��������
#define key_wendujian           GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_2)    	//��ȡ�����¶ȼ�
#define key_moshi   		GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_5)		//��ȡ����ģʽ

#define fanhuikey_kaiguan 	1	//KEY0����
#define fanhuikey_wendujia	2	//KEY1����
#define fanhuikey_fengsu	3	//KEY2����
#define fanhuikey_wendujian   4	//KEY_UP����(��WK_UP/KEY_UP)
#define fanhuikey_moshi   5


#define key_kaiguan_pin           GPIO_Pin_6
#define key_wendujia_pin          GPIO_Pin_3
#define key_fengsu_pin            GPIO_Pin_4
#define key_wendujian_pin         GPIO_Pin_2
#define key_moshi_pin                 GPIO_Pin_5

#define key_port                  GPIOA

/********************************************�����궨�����***********************************************************/
#define aircontroller_datalen  6
/*******************************************����״̬�궨��****************************************************/
#define HIGH_SPEED    GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_ResetBits(GPIOA, GPIO_Pin_8); GPIO_ResetBits(GPIOA, GPIO_Pin_9);
#define MID_SPEED     GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_SetBits(GPIOA, GPIO_Pin_8); GPIO_ResetBits(GPIOA, GPIO_Pin_9);
#define LOW_SPEED     GPIO_ResetBits(GPIOB, GPIO_Pin_13); GPIO_SetBits(GPIOA, GPIO_Pin_8); GPIO_SetBits(GPIOA, GPIO_Pin_9);
#define CLOUSE        GPIO_SetBits(GPIOB, GPIO_Pin_13);GPIO_ResetBits(GPIOA, GPIO_Pin_8);GPIO_ResetBits(GPIOA, GPIO_Pin_9);
/*******************************************����״̬�궨�����****************************************************/
void KEY_Init(void);//IO��ʼ��
void moshixuanze(void);
void KEY_CAOZUO(void);
u8 KEY_Scan(u8);  	//����ɨ�躯��
void ChushiHuashedingwendu();//��ʼ���趨�¶�
void dingyishangchuanshuzu();

result_t prase_aircontrol(uint8_t *tempbuff); //struct AirControler_Receive *temp

#endif
