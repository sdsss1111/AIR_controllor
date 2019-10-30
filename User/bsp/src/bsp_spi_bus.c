/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动
*	文件名称 : bsp_spi_bus.h
*	版    本 : V1.1
*	说    明 : SPI总线底层驱动。提供SPI配置、收发数据、多设备共享SPI支持。通过宏切换是硬件SPI还是软件模拟
*	修改记录 :
*		版本号  日期        作者    说明
*                v1.0
*
*	Copyright (C),
*
*********************************************************************************************************
*/

#include "bsp.h"

extern  uint32_t  os_time_d;
extern  uint32_t  os_time_e;
extern  uint32_t  os_time_f;
//---------------------------------------------------------------------------------------------------------------//
extern uint8_t g_flag_test;//测试。。。
extern u8 gReceiveFlag;//接收中断标志位
extern const u8 TERMINAL_ID[4];

extern OS_EVENT *RF_Recv_Sem; /*创建一个信号量用于无线接收*/


uint8_t  gRfBuffer[RF_BUFFER_LEN];
uint8_t  gRfLength;
int8_t   gRssi = 0;
uint8_t  gRfCurChan = 11;
uint8_t  gRfPreMode = RF_STANDBY;
uint8_t  gRfPower = 0;
RF_RECEIVE_CB rf_receiveCb = NULL;

extern uint8_t  gRFRecv_test[80 + 1];


uint8_t g_spi_busy = 0;		/* SPI 总线共享标志 */

/*
*********************************************************************************************************
*	函 数 名: bsp_SPI_Init
*	功能说明: 配置STM32内部SPI硬件的工作模式。 简化库函数，提高执行效率。 仅用于SPI接口间切换。
*	形    参: _cr1 寄存器值
*	返 回 值: 无
*********************************************************************************************************
*/
#ifdef HARD_SPI		/* 硬件SPI */
static void bsp_SPI_Init(uint16_t _cr1)
{
    SPI_HARD->CR1 = ((SPI_HARD->CR1 & CR1_CLEAR_Mask) | _cr1);

    //SPI_Cmd(SPI_HARD, DISABLE);			/* 先禁止SPI  */
    SPI_HARD->CR1 &= CR1_SPE_Reset;	/* Disable the selected SPI peripheral */

    //SPI_Cmd(SPI_HARD, ENABLE);			/* 使能SPI  */
    SPI_HARD->CR1 |= CR1_SPE_Set;	  /* Enable the selected SPI peripheral */
}
#endif

/*
*********************************************************************************************************
*	函 数 名: bsp_InitSPIBus
*	功能说明: 配置SPI总线。 只包括 SCK、 MOSI、 MISO口线的配置。不包括片选CS，也不包括外设芯片特有的INT、BUSY等
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void bsp_InitSPIBus(void)
{

#ifdef HARD_SPI		/* 硬件SPI */

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); /* 开启 SPI 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); /* 开启 SPI 时钟 */

    RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE); /* 使能 GPIO 时钟 */
    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//要先开时钟，再重映射；这句表示关闭jtag，使能swd。


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_SCK;
    GPIO_Init(PORT_SCK, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_MISO;
    GPIO_Init(PORT_MISO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* 配置 SPI引脚SCK、MISO 和 MOSI为复用推挽模式 */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_MOSI;
    GPIO_Init(PORT_MOSI, &GPIO_InitStructure);


    bsp_SPI_Init(SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_DataSize_8b
                 | SPI_CPOL_Low | SPI_CPHA_1Edge | SPI_NSS_Soft | SPI_BaudRatePrescaler_256 | SPI_FirstBit_MSB);

    /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    SPI_HARD->I2SCFGR &= SPI_Mode_Select; /* 选择SPI模式，不是I2S模式 */

    /*---------------------------- SPIx CRCPOLY Configuration --------------------*/
    /* Write to SPIx CRCPOLY */
    SPI_HARD->CRCPR = 7;	/* 一般不用 */

    SPI_Cmd(SPI_HARD, DISABLE);	/* 先禁止SPI  */

    SPI_Cmd(SPI_HARD, ENABLE);	/* 使能SPI  */
#endif
}


/*
*********************************************************************************************************
*	函 数 名: sx127x_ConfigGPIO
*	功能说明: 配置sx127x控制端口：PA4->CS; PB14->DIO0; PA11->RESET
*                 注意：PB14->DIO0 配置为外部中断，用于sx127x接收中断
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
static void sx127x_ConfigGPIO(void)
{
    /*
     sx127x GPIO: PA4->CS; PB14->DIO0; PA11->RESET
    */

    GPIO_InitTypeDef   GPIO_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* 使能GPIO 时钟 */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_CS | RCC_APB2Periph_AFIO, ENABLE);//sx127x ->CS
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_RESET| RCC_APB2Periph_AFIO, ENABLE);//sx127x ->RESET
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_DIO0 | RCC_APB2Periph_AFIO, ENABLE);//sx127x ->DIO0


    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE); //GPIO_Remap_SPI1
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//要先开时钟，再重映射；这句表示关闭jtag，使能swd。


    /* 配置 CS 口线为推挽输出模式 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_CS;
    GPIO_Init(sx127x_PORT_CS, &GPIO_InitStructure);

    /* 配置 RESET 口线为推挽输出模式 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_RESET;
    GPIO_Init(sx127x_PORT_RESET, &GPIO_InitStructure);

    /* 配置 DIO0 口线为下拉输入模式 */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_AF_PP
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_DIO0;
    GPIO_Init(sx127x_PORT_DIO0, &GPIO_InitStructure);



    /*  GPIO 作为外部中断时，做如下配置  */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //使能引脚复用 sx127x->DIO0作为中断
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); //将 GPIO 引脚和中断线连接

    /* 设置 DIO0 外部中断 */
    EXTI_InitStructure.EXTI_Line = sx127x_LINE_DIO0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   //中断事件
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;   //上升沿触发
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //根据EXTI_InitStruct中指定的参数初始化外设EXTI寄存器

    EXTI_ClearFlag(sx127x_LINE_DIO0);  //清除中断标志* 清中断标志*
    EXTI_ClearITPendingBit(sx127x_LINE_DIO0);  //清除中断标志* 清中断标志*
#if 1
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000); //设置中断向量表的位置(设置在flash中)和偏移(偏移为0)
    /********************************************************************************************
     ** 4bit的中断优先级控制位分成2组:从高位开始，前面的定义抢先式优先级，后面为子优先级

     **分组  编 号 分配情况
     ** 0      7     0:4	  无抢先式优先级，16个子优先级
     ** 1      6	    1:3	  2个抢先式优先级，8个子优先级
     ** 2      5	    2:2	  4个抢先式优先级，4个子优先级
     ** 3      4	    3:1	  8个抢先式优先级，2个子优先级
     ** 4    3/2/1/0 4:0	  16个抢先式优先级，无子优先级
     ********************************************************************************************/
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //--- 在 bsp.c 中 bsp_Init() 中配置中断优先级组 */

    NVIC_InitStructure.NVIC_IRQChannel = sx127x_EXTI_IRQn_DIO0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//抢占优先级0，
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

}

/*
****************************************************************************
 * @brief   1278读写一个字节
 * @param   TxData 要写入的一个字节数据
 * @return  返回一个字节的数据
 ***************************************************************************
*/
u8 SPI_1278_ReadWriteByte(u8 TxData)
{
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //检查指定的SPI标志位设置与否:发送缓存空标志位
    {
        retry++;
        if(retry > 200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData); //通过外设SPIx发送一个数据
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //检查指定的SPI标志位设置与否:接受缓存非空标志位
    {
        retry++;
        if(retry > 200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //返回通过SPIx最近接收的数据
}

#if 0
/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusEnter
*	功能说明: 占用SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusEnter(void)
{
    g_spi_busy = 1;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusExit
*	功能说明: 释放占用的SPI总线
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
void bsp_SpiBusExit(void)
{
    g_spi_busy = 0;
}

/*
*********************************************************************************************************
*	函 数 名: bsp_SpiBusBusy
*	功能说明: 判断SPI总线忙。方法是检测其他SPI芯片的片选信号是否为1
*	形    参: 无
*	返 回 值: 0 表示不忙  1表示忙
*********************************************************************************************************
*/
uint8_t bsp_SpiBusBusy(void)
{
    return g_spi_busy;
}
#endif

/*
****************************************************************************
 * @brief   sx127x_init
 * @param   sx127x初始化
 * @return  返回一个字节的数据
 ***************************************************************************
*/
void bsp_sx127xIF_init(void)//sx127x SPI 接口初始化
{

    bsp_InitSPIBus();
    sx127x_ConfigGPIO();
}


/*
****************************************************************************
 * @brief   sx127x_读取接收数据
 * @param
 * @return  返回一个字节的数据
 ***************************************************************************
*/
result_t Get_sx127xData(void)
{
#if 1
    result_t result = sx127x_SUCCESS;
    uint8_t crcConfig = 0;
    uint8_t crcTemp = 0;
    uint8_t csize = 0;
#endif
    DISABLE_DIO0_INT();
    do
    {
        crcConfig = rf_readRegister(REG_LR_HOPCHANNEL);
        if (crcConfig & RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON)
        {
            crcTemp = rf_readRegister(REG_LR_IRQFLAGS);
            if (crcTemp & RFLR_IRQFLAGS_PAYLOADCRCERROR)
            {
                result = sx127x_FAILED;
                //App_Printf("-- fifo crc error--\r\n");
                break;
            }
        }
        gRssi = rf_readRegister(REG_LR_PKTRSSIVALUE);
        gRssi = gRssi - 157;
        csize = rf_readRegister(REG_LR_NBRXBYTES);
        if (csize == 0)
        {
            result = sx127x_FAILED;
            // App_Printf("-- data size error--\r\n");
            break;
        }
        gRfLength = csize;
        rf_writeRegister(REG_LR_FIFOADDRPTR, 0x00);
        rf_readFifo(gRfBuffer, csize);
    }
    while(0);

    /* clear the receive flag of sx1278 */
    rf_writeRegister(REG_LR_IRQFLAGS, 0xff);
    if(result == sx127x_FAILED)
    {
        // memset(&gRfBuffer,0,RF_BUFFER_LEN);//清除数据接收缓冲区gRfBuffer,用于下一次数据接收;
        rf_clearRfBuffer();  //清除数据接收缓冲区gRfBuffer,用于下一次数据接收;
        result = sx127x_FAILED;
    }
    rf_receiveOn();
    CLEAR_DIO0_INT();
    ENABLE_DIO0_INT();
    return result;
}


/*-------------------------------------------- DIO0 中断处理------------------------------------------------------*/
/*
****************************************************************************
 * @brief   sx127x 接收中断
 * @param
 * @return  抛出信号量
 ***************************************************************************
*/
/***************** Port 0 interrupt service routine ***************/
#if 1
void EXTI1_IRQHandler(void)
{
#if 0
    result_t result = sx127x_SUCCESS;
    uint8_t crcConfig = 0;
    uint8_t crcTemp = 0;
    uint8_t csize = 0;
#endif
    INT8U err;
    EXTI_ClearITPendingBit(sx127x_LINE_DIO0);//清除LINE14上的中断标志位
    //    g_flag_test = 1;

#if 0

    do
    {
        crcConfig = rf_readRegister(REG_LR_HOPCHANNEL);
        if (crcConfig & RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON)
        {
            crcTemp = rf_readRegister(REG_LR_IRQFLAGS);
            if (crcTemp & RFLR_IRQFLAGS_PAYLOADCRCERROR)
            {
                result = sx127x_FAILED;
                break;
            }
        }
        gRssi = rf_readRegister(REG_LR_PKTRSSIVALUE);
        gRssi = gRssi - 157;
        csize = rf_readRegister(REG_LR_NBRXBYTES);
        if (csize == 0)
        {
            result = sx127x_FAILED;
            break;
        }
        gRfLength = csize;
        rf_writeRegister(REG_LR_FIFOADDRPTR, 0x00);
        rf_readFifo(gRfBuffer, csize);
    }
    while(0);

    /* clear the receive flag of sx1278 */
    rf_writeRegister(REG_LR_IRQFLAGS, 0xff);
    if(result == sx127x_SUCCESS)
    {
        rf_receiveCb(gRfBuffer, gRfLength, gRssi);//回调函数置接收标志位
        //       gReceiveFlag=1;
        //       rf_stop();
    }
    else
    {
        gReceiveFlag = 0;
        memset(gRfBuffer, 0, RF_BUFFER_LEN); //清除数据接收缓冲区gRfBuffer,用于下一次数据接收
        //rf_receiveOn();
    }
    //    }
    //    else
    //    {
    rf_receiveOn();
    //    }
#endif

    err = OSSemPost(RF_Recv_Sem);
#if 0
    switch(err)
    {
    case OS_ERR_NONE:
        App_Printf("-- RF_Recv_Sem is OS_ERR_NONE! --\r\n"); //系统启动失败，打印信息
        break;
    case OS_ERR_SEM_OVF:
        App_Printf("-- RF_Recv_Sem is OS_ERR_SEM_OVF! --\r\n"); //系统启动失败，打印信息
        break;
    case OS_ERR_EVENT_TYPE:
        App_Printf("-- RF_Recv_Sem is OS_ERR_EVENT_TYPE!  --\r\n"); //系统启动失败，打印信息
        break;
    case OS_ERR_PEVENT_NULL:
        App_Printf("-- RF_Recv_Sem is OS_ERR_PEVENT_NULL!  --\r\n"); //系统启动失败，打印信息
        break;
    default:
        break;
    }
#endif
}
#endif //#if 1 EXTI15_10_IRQHandler
/***************************** (END OF FILE) *********************************/
