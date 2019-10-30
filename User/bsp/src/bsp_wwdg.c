/*
*********************************************************************************************************
*
*	ģ������ : ���ڿ��Ź�����
*	�ļ����� : bsp_wwdg.c
*	��    �� : V1.0
*	˵    �� : WWDG���̡�
*	�޸ļ�¼ :
*		�汾��   ����        ����     ˵��
*		V1.0
*
*	Copyright (C),
*********************************************************************************************************
*/

#include "bsp.h"


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitWwdg
*	����˵��: ���ڿ��Ź�����
*	��    �Σ�
*             _ucTreg       : T[6:0],������ֵ 	��Χ0x40 �� 0x7F
*             _ucWreg       : W[6:0],����ֵ     ����С�� 0x80
*            WWDG_Prescaler : ���ڿ��Ź���Ƶ	PCLK1 = 42MHz
*            WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
*	     WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
*	     WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
*	     WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* ���ϵͳ�Ƿ�Ӵ��ڿ��Ź���λ�лָ� */
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
    {
        /* �����λ��־ */
        RCC_ClearFlag();
    }
    else
    {
        /* WWDGRST ��־û������ */
    }

    /* ʹ��WWDGʱ�� */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /*
       ���ڿ��Ź���Ƶ���ã�
       ����ѡ��WWDG_Prescaler_8
       (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)
    */
    WWDG_SetPrescaler(WWDG_Prescaler);

    /*
     ���ô���ֵ��_ucWreg���û�������С��_ucWreg�Ҵ���0x40ʱˢ�¼���
     ����Ҫ�������ϵͳ��λ��
    */
    WWDG_SetWindowValue(_ucWreg);

    /*
     ʹ��WWDG�����ü�����
     ��������_ucTreg=127 8��Ƶʱ����ô���ʱ�����= ~780 us * 64 = 49.92 ms
     ���ڿ��Ź���ˢ��ʱ�����: ~780 * (127-80) = 36.6ms < ˢ�´��ڿ��Ź� < ~780 * 64 = 49.9ms
    */
    WWDG_Enable(_ucTreg);

    /* ���EWI�жϱ�־ */
    WWDG_ClearFlag();

    /* ʹ��EW�ж� */
    WWDG_EnableIT();

    /* ���� WWDG ��NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



/*
*********************************************************************************************************
*	�� �� ��: WWDG_IRQHandler
*	����˵��: ���ڿ��Ź��ж�ι��
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void WWDG_IRQHandler(void)
{
    WWDG_SetCounter(setWreg);
    WWDG_ClearFlag();
}


/*
*********************************************************************************************************
*	�� �� ��: bsp_InitIwdg_
*	����˵��: ���ڿ��Ź�����
*	��    �Σ�
*          IWDG_Prescaler : �������Ź���Ƶ	PCLK1 = 42MHz
*            IWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/1
*	     IWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/2
*	     IWDG_Prescaler_16: WWDG counter clock = (PCLK1/4096)/4
*	     IWDG_Prescaler_32: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_64: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_128: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_256: WWDG counter clock = (PCLK1/4096)/8
*          Reload_Cnt�����Ź�����ʱ�䣬[0,4096]->[0x00-0xFFF]
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void bsp_InitIwdg(void)
{
    //���Ź���ʼ������
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //����֮ǰҪ����ʹ�ܼĴ���д
    IWDG_SetPrescaler(IWDG_Prescaler_256);  //256��Ƶ һ������4.6ms
    IWDG_SetReload(Reload_Cnt); //�12λ [0,4096] 4096*4.6 = 18s
    IWDG_ReloadCounter(); //������������ֵ���ؿ��Ź�������
    IWDG_Enable();  //ʹ�ܿ��Ź�
}

/******************************************
ι��
*******************************************/
void bsp_Iwdg_Feed(void) //ι��
{
    IWDG_ReloadCounter(); //������������ֵ���ؿ��Ź�������
}


/***************************** (END OF FILE) *********************************/

