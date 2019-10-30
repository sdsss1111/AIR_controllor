/*
*********************************************************************************************************
*
*	ģ������ : SPI��������
*	�ļ����� : bsp_spi_bus.h
*	��    �� : V1.1
*	˵    �� : SPI���ߵײ��������ṩSPI���á��շ����ݡ����豸����SPI֧�֡�ͨ�����л���Ӳ��SPI�������ģ��
*	�޸ļ�¼ :
*		�汾��  ����        ����    ˵��
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
extern uint8_t g_flag_test;//���ԡ�����
extern u8 gReceiveFlag;//�����жϱ�־λ
extern const u8 TERMINAL_ID[4];

extern OS_EVENT *RF_Recv_Sem; /*����һ���ź����������߽���*/


uint8_t  gRfBuffer[RF_BUFFER_LEN];
uint8_t  gRfLength;
int8_t   gRssi = 0;
uint8_t  gRfCurChan = 11;
uint8_t  gRfPreMode = RF_STANDBY;
uint8_t  gRfPower = 0;
RF_RECEIVE_CB rf_receiveCb = NULL;

extern uint8_t  gRFRecv_test[80 + 1];


uint8_t g_spi_busy = 0;		/* SPI ���߹����־ */

/*
*********************************************************************************************************
*	�� �� ��: bsp_SPI_Init
*	����˵��: ����STM32�ڲ�SPIӲ���Ĺ���ģʽ�� �򻯿⺯�������ִ��Ч�ʡ� ������SPI�ӿڼ��л���
*	��    ��: _cr1 �Ĵ���ֵ
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#ifdef HARD_SPI		/* Ӳ��SPI */
static void bsp_SPI_Init(uint16_t _cr1)
{
    SPI_HARD->CR1 = ((SPI_HARD->CR1 & CR1_CLEAR_Mask) | _cr1);

    //SPI_Cmd(SPI_HARD, DISABLE);			/* �Ƚ�ֹSPI  */
    SPI_HARD->CR1 &= CR1_SPE_Reset;	/* Disable the selected SPI peripheral */

    //SPI_Cmd(SPI_HARD, ENABLE);			/* ʹ��SPI  */
    SPI_HARD->CR1 |= CR1_SPE_Set;	  /* Enable the selected SPI peripheral */
}
#endif

/*
*********************************************************************************************************
*	�� �� ��: bsp_InitSPIBus
*	����˵��: ����SPI���ߡ� ֻ���� SCK�� MOSI�� MISO���ߵ����á�������ƬѡCS��Ҳ����������оƬ���е�INT��BUSY��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void bsp_InitSPIBus(void)
{

#ifdef HARD_SPI		/* Ӳ��SPI */

    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); /* ���� SPI ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE); /* ���� SPI ʱ�� */

    RCC_APB2PeriphClockCmd(RCC_SCK | RCC_MOSI | RCC_MISO, ENABLE); /* ʹ�� GPIO ʱ�� */
    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//Ҫ�ȿ�ʱ�ӣ�����ӳ�䣻����ʾ�ر�jtag��ʹ��swd��


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_SCK;
    GPIO_Init(PORT_SCK, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_MISO;
    GPIO_Init(PORT_MISO, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; /* ���� SPI����SCK��MISO �� MOSIΪ��������ģʽ */
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = PIN_MOSI;
    GPIO_Init(PORT_MOSI, &GPIO_InitStructure);


    bsp_SPI_Init(SPI_Direction_2Lines_FullDuplex | SPI_Mode_Master | SPI_DataSize_8b
                 | SPI_CPOL_Low | SPI_CPHA_1Edge | SPI_NSS_Soft | SPI_BaudRatePrescaler_256 | SPI_FirstBit_MSB);

    /* Activate the SPI mode (Reset I2SMOD bit in I2SCFGR register) */
    SPI_HARD->I2SCFGR &= SPI_Mode_Select; /* ѡ��SPIģʽ������I2Sģʽ */

    /*---------------------------- SPIx CRCPOLY Configuration --------------------*/
    /* Write to SPIx CRCPOLY */
    SPI_HARD->CRCPR = 7;	/* һ�㲻�� */

    SPI_Cmd(SPI_HARD, DISABLE);	/* �Ƚ�ֹSPI  */

    SPI_Cmd(SPI_HARD, ENABLE);	/* ʹ��SPI  */
#endif
}


/*
*********************************************************************************************************
*	�� �� ��: sx127x_ConfigGPIO
*	����˵��: ����sx127x���ƶ˿ڣ�PA4->CS; PB14->DIO0; PA11->RESET
*                 ע�⣺PB14->DIO0 ����Ϊ�ⲿ�жϣ�����sx127x�����ж�
*	��    ��: ��
*	�� �� ֵ: ��
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

    /* ʹ��GPIO ʱ�� */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_CS | RCC_APB2Periph_AFIO, ENABLE);//sx127x ->CS
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_RESET| RCC_APB2Periph_AFIO, ENABLE);//sx127x ->RESET
    //    RCC_APB2PeriphClockCmd(sx127x_RCC_DIO0 | RCC_APB2Periph_AFIO, ENABLE);//sx127x ->DIO0


    GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE); //GPIO_Remap_SPI1
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);//Ҫ�ȿ�ʱ�ӣ�����ӳ�䣻����ʾ�ر�jtag��ʹ��swd��


    /* ���� CS ����Ϊ�������ģʽ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_CS;
    GPIO_Init(sx127x_PORT_CS, &GPIO_InitStructure);

    /* ���� RESET ����Ϊ�������ģʽ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_RESET;
    GPIO_Init(sx127x_PORT_RESET, &GPIO_InitStructure);

    /* ���� DIO0 ����Ϊ��������ģʽ */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;//GPIO_Mode_AF_PP
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
    GPIO_InitStructure.GPIO_Pin = sx127x_PIN_DIO0;
    GPIO_Init(sx127x_PORT_DIO0, &GPIO_InitStructure);



    /*  GPIO ��Ϊ�ⲿ�ж�ʱ������������  */
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE); //ʹ�����Ÿ��� sx127x->DIO0��Ϊ�ж�
    GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1); //�� GPIO ���ź��ж�������

    /* ���� DIO0 �ⲿ�ж� */
    EXTI_InitStructure.EXTI_Line = sx127x_LINE_DIO0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;   //�ж��¼�
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;   //�����ش���
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure); //����EXTI_InitStruct��ָ���Ĳ�����ʼ������EXTI�Ĵ���

    EXTI_ClearFlag(sx127x_LINE_DIO0);  //����жϱ�־* ���жϱ�־*
    EXTI_ClearITPendingBit(sx127x_LINE_DIO0);  //����жϱ�־* ���жϱ�־*
#if 1
    NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x0000); //�����ж��������λ��(������flash��)��ƫ��(ƫ��Ϊ0)
    /********************************************************************************************
     ** 4bit���ж����ȼ�����λ�ֳ�2��:�Ӹ�λ��ʼ��ǰ��Ķ�������ʽ���ȼ�������Ϊ�����ȼ�

     **����  �� �� �������
     ** 0      7     0:4	  ������ʽ���ȼ���16�������ȼ�
     ** 1      6	    1:3	  2������ʽ���ȼ���8�������ȼ�
     ** 2      5	    2:2	  4������ʽ���ȼ���4�������ȼ�
     ** 3      4	    3:1	  8������ʽ���ȼ���2�������ȼ�
     ** 4    3/2/1/0 4:0	  16������ʽ���ȼ����������ȼ�
     ********************************************************************************************/
    // NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0); //--- �� bsp.c �� bsp_Init() �������ж����ȼ��� */

    NVIC_InitStructure.NVIC_IRQChannel = sx127x_EXTI_IRQn_DIO0;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;	//��ռ���ȼ�0��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
#endif

}

/*
****************************************************************************
 * @brief   1278��дһ���ֽ�
 * @param   TxData Ҫд���һ���ֽ�����
 * @return  ����һ���ֽڵ�����
 ***************************************************************************
*/
u8 SPI_1278_ReadWriteByte(u8 TxData)
{
    u8 retry = 0;
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) //���ָ����SPI��־λ�������:���ͻ���ձ�־λ
    {
        retry++;
        if(retry > 200)return 0;
    }
    SPI_I2S_SendData(SPI1, TxData); //ͨ������SPIx����һ������
    retry = 0;

    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) //���ָ����SPI��־λ�������:���ܻ���ǿձ�־λ
    {
        retry++;
        if(retry > 200)return 0;
    }
    return SPI_I2S_ReceiveData(SPI1); //����ͨ��SPIx������յ�����
}

#if 0
/*
*********************************************************************************************************
*	�� �� ��: bsp_SpiBusEnter
*	����˵��: ռ��SPI����
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ��æ  1��ʾæ
*********************************************************************************************************
*/
void bsp_SpiBusEnter(void)
{
    g_spi_busy = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SpiBusExit
*	����˵��: �ͷ�ռ�õ�SPI����
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ��æ  1��ʾæ
*********************************************************************************************************
*/
void bsp_SpiBusExit(void)
{
    g_spi_busy = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_SpiBusBusy
*	����˵��: �ж�SPI����æ�������Ǽ������SPIоƬ��Ƭѡ�ź��Ƿ�Ϊ1
*	��    ��: ��
*	�� �� ֵ: 0 ��ʾ��æ  1��ʾæ
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
 * @param   sx127x��ʼ��
 * @return  ����һ���ֽڵ�����
 ***************************************************************************
*/
void bsp_sx127xIF_init(void)//sx127x SPI �ӿڳ�ʼ��
{

    bsp_InitSPIBus();
    sx127x_ConfigGPIO();
}


/*
****************************************************************************
 * @brief   sx127x_��ȡ��������
 * @param
 * @return  ����һ���ֽڵ�����
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
        // memset(&gRfBuffer,0,RF_BUFFER_LEN);//������ݽ��ջ�����gRfBuffer,������һ�����ݽ���;
        rf_clearRfBuffer();  //������ݽ��ջ�����gRfBuffer,������һ�����ݽ���;
        result = sx127x_FAILED;
    }
    rf_receiveOn();
    CLEAR_DIO0_INT();
    ENABLE_DIO0_INT();
    return result;
}


/*-------------------------------------------- DIO0 �жϴ���------------------------------------------------------*/
/*
****************************************************************************
 * @brief   sx127x �����ж�
 * @param
 * @return  �׳��ź���
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
    EXTI_ClearITPendingBit(sx127x_LINE_DIO0);//���LINE14�ϵ��жϱ�־λ
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
        rf_receiveCb(gRfBuffer, gRfLength, gRssi);//�ص������ý��ձ�־λ
        //       gReceiveFlag=1;
        //       rf_stop();
    }
    else
    {
        gReceiveFlag = 0;
        memset(gRfBuffer, 0, RF_BUFFER_LEN); //������ݽ��ջ�����gRfBuffer,������һ�����ݽ���
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
        App_Printf("-- RF_Recv_Sem is OS_ERR_NONE! --\r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ
        break;
    case OS_ERR_SEM_OVF:
        App_Printf("-- RF_Recv_Sem is OS_ERR_SEM_OVF! --\r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ
        break;
    case OS_ERR_EVENT_TYPE:
        App_Printf("-- RF_Recv_Sem is OS_ERR_EVENT_TYPE!  --\r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ
        break;
    case OS_ERR_PEVENT_NULL:
        App_Printf("-- RF_Recv_Sem is OS_ERR_PEVENT_NULL!  --\r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ
        break;
    default:
        break;
    }
#endif
}
#endif //#if 1 EXTI15_10_IRQHandler
/***************************** (END OF FILE) *********************************/
