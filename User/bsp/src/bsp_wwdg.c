/*
*********************************************************************************************************
*
*	模块名称 : 窗口看门狗程序
*	文件名称 : bsp_wwdg.c
*	版    本 : V1.0
*	说    明 : WWDG例程。
*	修改记录 :
*		版本号   日期        作者     说明
*		V1.0
*
*	Copyright (C),
*********************************************************************************************************
*/

#include "bsp.h"


/*
*********************************************************************************************************
*	函 数 名: bsp_InitWwdg
*	功能说明: 窗口看门狗配置
*	形    参：
*             _ucTreg       : T[6:0],计数器值 	范围0x40 到 0x7F
*             _ucWreg       : W[6:0],窗口值     必须小于 0x80
*            WWDG_Prescaler : 窗口看门狗分频	PCLK1 = 42MHz
*            WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
*	     WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
*	     WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
*	     WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitWwdg(uint8_t _ucTreg, uint8_t _ucWreg, uint32_t WWDG_Prescaler)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    /* 检测系统是否从窗口看门狗复位中恢复 */
    if (RCC_GetFlagStatus(RCC_FLAG_WWDGRST) != RESET)
    {
        /* 清除复位标志 */
        RCC_ClearFlag();
    }
    else
    {
        /* WWDGRST 标志没有设置 */
    }

    /* 使能WWDG时钟 */
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    /*
       窗口看门狗分频设置：
       比如选择WWDG_Prescaler_8
       (PCLK1 (42MHz)/4096)/8 = 1281 Hz (~780 us)
    */
    WWDG_SetPrescaler(WWDG_Prescaler);

    /*
     设置窗口值是_ucWreg，用户必须在小于_ucWreg且大于0x40时刷新计数
     器，要不会造成系统复位。
    */
    WWDG_SetWindowValue(_ucWreg);

    /*
     使能WWDG，设置计数器
     比如设置_ucTreg=127 8分频时，那么溢出时间就是= ~780 us * 64 = 49.92 ms
     窗口看门狗的刷新时间段是: ~780 * (127-80) = 36.6ms < 刷新窗口看门狗 < ~780 * 64 = 49.9ms
    */
    WWDG_Enable(_ucTreg);

    /* 清除EWI中断标志 */
    WWDG_ClearFlag();

    /* 使能EW中断 */
    WWDG_EnableIT();

    /* 设置 WWDG 的NVIC */
    NVIC_InitStructure.NVIC_IRQChannel = WWDG_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}



/*
*********************************************************************************************************
*	函 数 名: WWDG_IRQHandler
*	功能说明: 窗口看门狗中断喂狗
*	形    参：无
*	返 回 值: 无
*********************************************************************************************************
*/
void WWDG_IRQHandler(void)
{
    WWDG_SetCounter(setWreg);
    WWDG_ClearFlag();
}


/*
*********************************************************************************************************
*	函 数 名: bsp_InitIwdg_
*	功能说明: 窗口看门狗配置
*	形    参：
*          IWDG_Prescaler : 独立看门狗分频	PCLK1 = 42MHz
*            IWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/1
*	     IWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/2
*	     IWDG_Prescaler_16: WWDG counter clock = (PCLK1/4096)/4
*	     IWDG_Prescaler_32: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_64: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_128: WWDG counter clock = (PCLK1/4096)/8
*	     IWDG_Prescaler_256: WWDG counter clock = (PCLK1/4096)/8
*          Reload_Cnt：看门狗重载时间，[0,4096]->[0x00-0xFFF]
*	返 回 值: 无
*********************************************************************************************************
*/
void bsp_InitIwdg(void)
{
    //看门狗初始化函数
    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable); //访问之前要首先使能寄存器写
    IWDG_SetPrescaler(IWDG_Prescaler_256);  //256分频 一个周期4.6ms
    IWDG_SetReload(Reload_Cnt); //最长12位 [0,4096] 4096*4.6 = 18s
    IWDG_ReloadCounter(); //按照上述重载值重载看门狗计数器
    IWDG_Enable();  //使能开门狗
}

/******************************************
喂狗
*******************************************/
void bsp_Iwdg_Feed(void) //喂狗
{
    IWDG_ReloadCounter(); //按照上述重载值重载看门狗计数器
}


/***************************** (END OF FILE) *********************************/

