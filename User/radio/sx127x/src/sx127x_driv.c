/*
*********************************************************************************************************
*
*	模块名称 : SPI接口SX127x模块驱动
*	文件名称 : sx127x_driv.c
*	版    本 : V1.0
*	说    明 :
*
*
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0
*
*	Copyright (C),
*
*********************************************************************************************************
*/

#include "../../bsp/bsp.h"
//#include "../../bsp/inc/bsp_spi_bus.h"
uint32_t  os_time_a;
uint32_t  os_time_b;

uint32_t  os_time_c;
uint32_t  os_time_d;
uint32_t  os_time_e;
uint32_t  os_time_f;

uint8_t Allocated_Chl_Tx;
uint8_t Allocated_Chl_Rx;
RF_SF_Set Allocated_Chl_SF;

/***********************************************************************************/
/******************************** variable declarations ****************************/
/***********************************************************************************/
#if 0
const unsigned char  gFreqCalTab[] =
{
    0x76, 0x93, 0x33, //474.3MHz 0
    0x76, 0xa0, 0x00, //474.5MHz 0
    //0x75,0x80,0x00,//470MHz 0
    //0x75,0xC0,0x00,//471MHz 1
    0x76, 0x00, 0x00, //472MHz 2
    0x76, 0x40, 0x00, //473MHz 3
    0x76, 0x80, 0x00, //474MHz 4
    0x76, 0xC0, 0x00, //475MHz 5
    0x77, 0x00, 0x00, //476MHz 6
    0x77, 0x40, 0x00, //477MHz 7
    0x77, 0x80, 0x00, //478MHz 8
    0x77, 0xC0, 0x00, //479MHz 9
    0x78, 0x00, 0x00, //480MHz 10
    0x78, 0x40, 0x00, //481MHz 11
    0x78, 0x80, 0x00, //482MHz 12
    0x78, 0xC0, 0x00, //483MHz 13
    0x79, 0x00, 0x00, //484MHz 14
    0x79, 0x40, 0x00, //485MHz 15
    0x79, 0x80, 0x00, //486MHz 16
    0x79, 0xC0, 0x00, //487MHz 17
    0x7A, 0x00, 0x00, //488MHz 18
    0x7A, 0x40, 0x00, //489MHz 19
    0x7A, 0x80, 0x00, //490MHz 20
    0x7A, 0xC0, 0x00, //491MHz 21
    0x7B, 0x00, 0x00, //492MHz 22
    0x7B, 0x40, 0x00, //493MHz 23
    0x7B, 0x80, 0x00, //494MHz 24
    0x7B, 0xC0, 0x00, //495MHz 25
    0x7C, 0x00, 0x00, //496MHz 26
    0x7C, 0x40, 0x00, //497MHz 27
    0x7C, 0x80, 0x00, //498MHz 28
    0x7C, 0xC0, 0x00, //499MHz 29
    //0x7d,0x0c,0xcb,//500.2MHz
    //0x7D,0x00,0x00,//500MHz 30
    0x7d, 0x20, 0x00, //500.5MHz 30
    0x7d, 0x60, 0x00, //501.5MHz 31
    // 0x7D,0x40,0x00,//501MHz 31
    0x7D, 0x80, 0x00, //502MHz 32
    0x7D, 0xC0, 0x00, //503MHz 33
    0x7E, 0x00, 0x00, //504MHz 34
    0x7E, 0x40, 0x00, //505MHz 35
    0x7E, 0x80, 0x00, //506MHz 36
    0x7E, 0xC0, 0x00, //507MHz 37
    0x7F, 0x00, 0x00, //508MHz 38
    0x7F, 0x40, 0x00, //509MHz 39
    0x7F, 0x80, 0x00, //510MHz 40
};
#endif

extern uint8_t  gRfBuffer[];
extern uint8_t  gRfLength;
extern int8_t   gRssi;
extern uint8_t  gRfCurChan;
extern uint8_t  gRfPreMode;
extern uint8_t  gRfPower;
extern RF_RECEIVE_CB rf_receiveCb;

extern uint8_t g_flag_test;

/***********************************************************************************/
/******************************* function definition *******************************/
/***********************************************************************************/

/*****************************************************************************
 * @brief   wait for timeout
 * @param   None.
 * @return  None.
 ****************************************************************************/
void rf_wait(uint16_t timeout)
{
    do
    {
        ;
    }
    while (timeout--);
}

/***************************************************************************
 * @brief   Sets the SX1278 operating mode
 * @param   mode - there are five modes: RF_TRANSMITTER, RF_RECEIVER,
                   RF_SYNTHESIZER, RF_STANDBY, RF_SLEEP
 * @return  None
 **************************************************************************/
void rf_setRfMode(uint8_t mode, uint8_t preamb)
{
    if (mode == RF_TRANSMITTER)
    {
        gRfPreMode = RF_TRANSMITTER;
    }
    else if (mode == RF_RECEIVER)
    {
        rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY);
        rf_writeRegister(REG_LR_PREAMBLEMSB, 0);
        rf_writeRegister(REG_LR_PREAMBLELSB, preamb);
        rf_writeRegister(REG_LR_PAYLOADLENGTH, 0);
        rf_writeRegister(REG_LR_IRQFLAGSMASK,
                         ~(RFLR_IRQFLAGS_RXDONE | RFLR_IRQFLAGS_PAYLOADCRCERROR));
        rf_writeRegister(REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_00);
        //        //TX_DIS();
        rf_writeRegister(REG_LR_FIFOADDRPTR, 0x00);
        rf_writeRegister(REG_LR_IRQFLAGS, 0xff);
        rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_RECEIVER);
        gRfPreMode = RF_RECEIVER;
    }
    else if (mode == RF_SYNTHESIZER)
    {
        gRfPreMode = RF_SYNTHESIZER;
    }
    else if (mode == RF_STANDBY)
    {
        rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY);
        gRfPreMode = RF_STANDBY;
    }
    else  // mode == RF_SLEEP
    {
        rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY);
        rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP);
        gRfPreMode = RF_SLEEP;
    }
}

/***************************************************************************
 * @brief   Writes the register value at the given address on the SX1278.本文件定义
 * @param   address - the address of the register
 *          value   - the value to be set
 * @return  None
 **************************************************************************/
void rf_writeRegister(uint8_t address, uint16_t value)
{
    address = address | 0x80;
    SPI_ENABLE();
    //    bsp_DelayUS(10000);//bsp_DelayMS(10);
    // delay_ms(10);
    SPI_1278_ReadWriteByte(address);
    SPI_1278_ReadWriteByte(value);
    SPI_DISABLE();
}

/***************************************************************************
 * @brief   Reads the register value at the given address on the SX1278
 * @param   address - the address of the register
 * @return  value
 **************************************************************************/
uint16_t rf_readRegister(uint8_t address)
{
    uint8_t value = 0;

    address = address & 0x7F;
    SPI_ENABLE();
    SPI_1278_ReadWriteByte(address);
    value = SPI_1278_ReadWriteByte(DUMMY_CHAR);
    SPI_DISABLE();

    return value;
}

#if 0
/*****************************************************************************
 * @brief   initialise the port of rf (1278).
 * @param   None.
 * @return  None.
 ****************************************************************************/
void rf_portInit(void)
{
    /* set SFD pin in */
    SX1278_PORT_INIT();
    /* spi port enable */
    HAL_SPI_INIT();
    //    /* enable the pin interrupt of fifop */
    //    DIO0_INT_INIT();
    //	  ENABLE_DIO0_INT();//初始化的时候改成ENABLE
    //    DISABLE_DIO0_INT();
}
#endif

/********************************************************************************
 * @brief   Sends a data to the transceiver through the SPI interface
 * @param   byte - the byte to be sent
 * @return  None
 *******************************************************************************/
void rf_sendByte(uint8_t byte)
{
    /* SPI burst mode not used in this implementation */
    rf_writeRegister(REG_LR_FIFO, byte);
}

/********************************************************************************
 * @brief   receives a data from the transceiver through the SPI interface
 * @param
 * @return  b
 *******************************************************************************/
uint8_t rf_receiveByte(void)
{
    uint8_t byte;

    /* SPI burst mode not used in this implementation */
    byte = rf_readRegister(REG_LR_FIFO);

    return  byte;
}

/***************************************************************************
 * @brief
 * @param
 * @return
 **************************************************************************/
result_t rf_writeFifo(uint8_t *buffer, uint8_t size)
{
    uint8_t i = 0;
    uint8_t address = REG_LR_FIFO | 0x80;

    SPI_ENABLE();
    SPI_1278_ReadWriteByte(address);
    for (i = 0; i < size; i++)
    {
        SPI_1278_ReadWriteByte(buffer[i]);
    }
    SPI_DISABLE();

    return sx127x_SUCCESS;
}

/***************************************************************************
 * @brief
 * @param
 * @return
 **************************************************************************/
result_t rf_readFifo(uint8_t *buffer, uint8_t size)
{
    uint8_t i = 0;
    SPI_ENABLE();
    SPI_1278_ReadWriteByte(0);
    for (i = 0; i < size; i++)
    {
        buffer[i] = SPI_1278_ReadWriteByte(DUMMY_CHAR);
    }
    SPI_DISABLE();

    return sx127x_SUCCESS;
}

/***************************************************************************
 * @brief   Set the rf wireless rate.
 * @param   rate - RATE_537, precise 537 bps
 *                 RATE_1K,  precise 1074 bps
 *                 RATE_2K,  precise 1953 bps
 *                 RATE_3K,  precise 3515 bps
 *                 RATE_6K,  precise 6250 bps
 *                 RATE_10K, precise 10937 bps
 *                 RATE_18K, precise 18750 bps
 * @return  SUCCESS or FAILED
 **************************************************************************/
result_t rf_setRate(rfRate_t rate)
{
    switch (rate)
    {
    case RATE_537:     //SF-11 BAND-125K  CODE-1   -134.5
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_125_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_11
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_1K:    //SF-11 BAND-250K  CODE-1   -131.5    1074 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_11
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_2K:    //SF-10 BAND-250K  CODE-1   -129    1953 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_10
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_3K:    //SF-9 BAND-250K  CODE-1   -126   3515 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_9
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_6K:    //SF-8 BAND-250K   CODE-1   -124   6250 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_8
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_10K:     //SF-7 BAND-250K   CODE-1   -121  10937 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_7
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    case RATE_18K:    //SF-6 BAND-250K   CODE-1   -119  18750 bps
        rf_writeRegister(REG_LR_MODEMCONFIG1,
                         RFLR_MODEMCONFIG1_BW_250_KHZ +
                         RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                         RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_6
                         | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    default:
        break;
    }

    return sx127x_SUCCESS;
}


/***************************************************************************
 * @brief   Set the rf wireless rate.
 * @param   rate - 0,  SF7
 *                 1,  SF8
 *                 2,  SF9
 *                 3,  SF10
 *                 4,  SF11
 *                 5,  SF12
 *                 6,  SF7-SF12
 * @return  SUCCESS or FAILED
 **************************************************************************/
result_t rf_SFSet(RF_SF_Set sf_value)
{
    result_t  result = sx127x_SUCCESS;

    //  if((sf_value > 6)||(sf_value < 0))
    //    return sx127x_FAILED;


    DISABLE_DIO0_INT();
    //  rf_writeRegister(REG_LR_OPMODE, 0x80|RFLR_OPMODE_STANDBY);
    switch(sf_value)
    {
    case SF7:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_7 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    case SF8:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_8 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    case SF9:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_9 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    case SF10:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_10 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    case SF11:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_11 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    case SF12:
    {
        //    rf_writeRegister(REG_LR_MODEMCONFIG1,
        //	             RFLR_MODEMCONFIG1_BW_125_KHZ+
        //	             RFLR_MODEMCONFIG1_CODINGRATE_4_5+
        //	             RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_12 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        break;
    }
    default:
        break;
    }
    CLEAR_DIO0_INT();
    ENABLE_DIO0_INT();

    return result;

}

/*******************************************************************************
 * @brief   Get the value of rssi
 * @param   None
 * @return  The value of rssi
 ******************************************************************************/
#if 0
static result_t rf_CSMA_Check(void)
{
    int8_t current_snr;
    int8_t RSSI;
    result_t result = sx127x_SUCCESS;

    current_snr = rf_readRegister(REG_LR_PKTSNRVALUE);
    gRssi = rf_readRegister(REG_LR_RSSIVALUE);
    if(current_snr > 0)
        RSSI = (int8_t)(((float)16 / 15 * gRssi) - sx127x_RSSI_Offset_HF);
    else
        RSSI = (int8_t)(gRssi - sx127x_RSSI_Offset_HF + ((float)(current_snr * 0.25)));

    if(RSSI > sx127x_RSSI_THRD)
        return sx127x_FAILED;
    return result;
}
#endif

/***************************************************************************
 * @brief   Initialise the rf chip.
 * @param   func - the receive call back function
 * @return  SUCCESS or FAILED
 **************************************************************************/
result_t rf_init(RF_RECEIVE_CB func)
{
    // rf_portInit();
    //手动复位
    RESET_OUT_L();//先拉低
    bsp_DelayUS(200);
    //delay_us(200);//拉低持续200us，手册规定大于100us
    RESET_OUT_H();//再拉高
    bsp_DelayUS(10000);//bsp_DelayMS(10);
    //delay_ms(10);//拉高持续10ms，手册规定大于5ms

    /* init registers */
    rf_writeRegister(REG_LR_OPMODE, RFLR_OPMODE_SLEEP);
    rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_SLEEP);
    rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY);
    rf_writeRegister(REG_LR_PACONFIG, 0xff);
    rf_writeRegister(REG_LR_PADAC, RFLR_PADAC_20DBM_ON);
    rf_writeRegister(REG_LR_PARAMP, RFLR_PARAMP_1000_US);
    rf_writeRegister(REG_LR_OCP, 0x20 | RFLR_OCP_TRIM_240_MA);
    rf_writeRegister(REG_LR_PAYLOADLENGTH, 2);
    rf_writeRegister(REG_LR_MODEMCONFIG3, RFLR_MODEMCONFIG3_AGCAUTO_ON);
    // rf_setRate(RATE_3K);//SF-9 BAND-250K  CODE-1   -129
    rf_writeRegister(REG_LR_SYNWORD, REG_OPMODE_SYNWORD);

    rf_writeRegister(REG_LR_MODEMCONFIG1,
                     RFLR_MODEMCONFIG1_BW_125_KHZ +
                     RFLR_MODEMCONFIG1_CODINGRATE_4_5 +
                     RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF);
    rf_writeRegister(REG_LR_MODEMCONFIG2,
                     RFLR_MODEMCONFIG2_SF_7 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
    rf_writeRegister(REG_LR_PREAMBLELSB, 10);
    rf_writeRegister(REG_LR_PREAMBLEMSB, 0);
    /* register the receive call back function */
    rf_receiveCb = func;

    return sx127x_SUCCESS;
}

/******************************************************************************
 * @brief   Start the crystal oscillator, and then the rf enter the Idle state.
 * @param   None
 * @return  SUCCESS if the crystal stable, else FAILED.
 ******************************************************************************/
result_t rf_start(void)
{
    result_t result = sx127x_SUCCESS;

    CLEAR_DIO0_INT();
    rf_setRfMode(RF_RECEIVER, 12);
    ENABLE_DIO0_INT();

    return result;
}

/**********************************************************************************
 * @brief   Stop the rf crystal oscillator, and turn off the rf power.
 * @param   None
 * @return  SUCCESS if rf enter the power down mode, else FAILED.
 *********************************************************************************/
result_t rf_stop(void)
{
    result_t result = sx127x_SUCCESS;

    rf_setRfMode(RF_SLEEP, 12);
    DISABLE_DIO0_INT();

    return result;
}

/*******************************************************************************
 * @brief   set the rf channel.
 * @param   channel -  the channel of rf, ranges from 0 to 40
 * @return  SUCCESS or FAILED
 ******************************************************************************/
result_t rf_setChannel(uint32_t Freq)
{
    //    uint8_t tempChannel = 0;
    result_t result = sx127x_SUCCESS;
    uint8_t testMsb = 0, testMid = 0, testLsb = 0;

    uint32_t freq;
    uint8_t  frfReg[3]; // Write buff

    freq      = (uint32_t)((double)Freq / (double)FREQ_STEP);
    frfReg[0] = (uint8_t)((freq >> 16) & 0xFF); // MSB
    frfReg[1] = (uint8_t)((freq >> 8) & 0xFF);  // Mid
    frfReg[2] = (uint8_t)(freq & 0xFF);         // LSB

    do
    {
        if ((Freq < FREQ_LOWER_BOUND) || (Freq > FREQ_UPPER_BOUND))
        {
            result = sx127x_FAILED;
            break;
        }
        //        tempChannel = channel;
        rf_setRfMode(RF_STANDBY, 12);
        rf_writeRegister(REG_LR_FRFMSB, frfReg[0]);  //set frequency
        rf_writeRegister(REG_LR_FRFMID, frfReg[1]);
        rf_writeRegister(REG_LR_FRFLSB, frfReg[2]);

        testMsb = rf_readRegister(REG_LR_FRFMSB);
        testMid = rf_readRegister(REG_LR_FRFMID);
        testLsb = rf_readRegister(REG_LR_FRFLSB);

        if (testMsb != frfReg[0])
        {
            result = sx127x_FAILED;
            break;
        }
        if (testMid != frfReg[1])
        {
            result = sx127x_FAILED;
            break;
        }
        if (testLsb != frfReg[2])
        {
            result = sx127x_FAILED;
            break;
        }
    }
    while (0);

    return result;
}


/***********************************************************************************
 * @brief   Set the rf power, ranges from 0 to 20 dB.
 * @param   power - from 0 to 20.
 * @return  SUCCESS or FAILED
 **********************************************************************************/
result_t rf_setPower(uint8_t power)
{
    result_t result = sx127x_SUCCESS;
    uint8_t temp = 0;

    gRfPower = power;
    if (power < 15)
    {
        rf_writeRegister(REG_LR_PADAC, RFLR_PADAC_20DBM_OFF);
        temp = power | 0x70;
        rf_writeRegister(REG_LR_PACONFIG, temp);
    }
    else if (power < 18)
    {
        rf_writeRegister(REG_LR_PADAC, RFLR_PADAC_20DBM_OFF);
        temp = power - 2;
        temp = temp | 0xf0;
        rf_writeRegister(REG_LR_PACONFIG, temp);
    }
    else if (power < 21)
    {
        rf_writeRegister(REG_LR_PADAC, RFLR_PADAC_20DBM_ON);
        temp = power - 5;
        temp = temp | 0xf0;
        rf_writeRegister(REG_LR_PACONFIG, temp);
    }
    else
    {
        result = sx127x_FAILED;
    }

    return result;
}

/********************************************************************************
 * @brief   Get the rf power.
 * @param   None.
 * @return  the working power of rf chip
 *******************************************************************************/
uint8_t rf_getPower(void)
{
    return gRfPower;
}


/*********************************************************************************
 * @brief   Write the data to fifo, and then turn on the tx.
 * @param   buf - the content that should be sent.
 *          len - the length of the content
 * @return  SUCCESS if the data have been sent, else FAILED.
 ********************************************************************************/
result_t rf_send(uint8_t *buf, uint8_t len)
{
    uint32_t timeout = RFTIMEOUT_LONG;
    result_t result = sx127x_SUCCESS;
    uint8_t i = 0;
    //    uint8_t retrans = 0;

    //#define DUG
#if 0
#ifdef DUG
    for(uint8_t i = 0; i < len; i++)
        App_Printf(" %d", *(buf + i));
#endif
#endif


    timeout = RFTIMEOUT_LONG;
    if ((len > 254) || (len == 0))
    {
        return sx127x_FAILED;
    }
    if (buf == NULL)
    {
        return sx127x_FAILED;
    }

#if 0
    do
    {
        if(rf_CSMA_Check() != sx127x_SUCCESS)
        {
            bsp_DelayUS(OSTimeGet() % 2000);
            retrans++;
        }
        else
            break;
        if(retrans > sx127x_RETRANS_MAX)
            return sx127x_FAILED;
    }
    while(1);
#endif

    if((FSM_LoRa_Type == LORALAN_FRAME_TYPE_SYNC_REQ) || (FSM_LoRa_Type == LORALAN_FRAME_TYPE_JOIN_REQ))
    {
        rf_setChannel(sx127x_FREQ_TX);
#ifdef DEBUG_ERR
        // App_Printf("-- connecting Tx send parameters:" " %d ", sx127x_FREQ_TX, "\r\n");
#endif
    }
    else
    {
        rf_writeRegister(REG_LR_PREAMBLELSB, 0x0c);
        rf_writeRegister(REG_LR_PREAMBLEMSB, 0);
        rf_setChannel(sx127x_FREQ_TX + Allocated_Chl_Tx * 200000); //将终端设置位基站分配信道
        rf_SFSet(Allocated_Chl_SF);
#ifdef DEBUG_ERR
        //   App_Printf("-- eventup download parameters- freq:" " %d ", sx127x_FREQ_TX + Allocated_Chl_Tx * 200000, "\r\n");
        // App_Printf("-- eventup download parameters- sf:" " %d ", Allocated_Chl_SF, "\r\n");
#endif
    }

    DISABLE_DIO0_INT();
    rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_STANDBY);
    rf_writeRegister(REG_LR_PAYLOADLENGTH, len);
    //    rf_writeRegister(REG_LR_MODEMCONFIG2,
    //		     RFLR_MODEMCONFIG2_SF_7|RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);

    rf_writeRegister(REG_LR_IRQFLAGS, 0xff);
    rf_writeRegister(REG_LR_IRQFLAGSMASK, ~(RFLR_IRQFLAGS_TXDONE));
    rf_writeRegister(REG_LR_DIOMAPPING1, RFLR_DIOMAPPING1_DIO0_01);
    rf_writeRegister(REG_LR_FIFOADDRPTR, 0x80);
    for (i = 0; i < len; i++)
    {
        rf_sendByte(*(buf + i));
    }
    os_time_a = OSTimeGet();
    rf_writeRegister(REG_LR_OPMODE, 0x80 | RFLR_OPMODE_TRANSMITTER);
    /* Wait for Packet sent */

    timeout = RFTIMEOUT_LONG;
    while(!(DIO0_IS_1()))
    {
        timeout--;
        //        rf_receiveOn();
        if (!(timeout))
        {
            rf_receiveOn();
            result = sx127x_FAILED;
            break;
        }
        else
        {
            // os_time_b = OSTimeGet();
            rf_receiveOn();
            // os_time_c = OSTimeGet();
            break;
        }
    }
    return result;
    //    CLEAR_DIO0_INT();
    //    ENABLE_DIO0_INT();
    //    rf_receiveOn();

}

/*******************************************************************************
 * @brief   Turn of the receive function
 * @param   None
 * @return  None
 ******************************************************************************/
void rf_receiveOff(void)
{
    rf_setRfMode(RF_SLEEP, 12);
    DISABLE_DIO0_INT();
}

/*******************************************************************************
 * @brief   Turn on the receive function
 * @param   None
 * @return  None
 ******************************************************************************/
void rf_receiveOn(void)
{
    CLEAR_DIO0_INT();

    //os_time_d = OSTimeGet();

    if((FSM_LoRa_Type == LORALAN_NETWORK_DISCONNECT) || (FSM_LoRa_Type == LORALAN_FRAME_TYPE_SYNC_REQ) || (FSM_LoRa_Type == LORALAN_FRAME_TYPE_JOIN_REQ))
    {
        rf_setChannel(sx127x_FREQ_RX);//+2000000
        rf_writeRegister(REG_LR_MODEMCONFIG2,
                         RFLR_MODEMCONFIG2_SF_9 | RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON);
        rf_setRfMode(RF_RECEIVER, 12);
        //#ifdef DEBUG_ERR
        //      App_Printf("-- connecting Rx send parameters:" " %d ", sx127x_FREQ_RX, "\r\n");
        //#endif
    }
    else
    {

        //      rf_writeRegister(REG_LR_PREAMBLELSB,20);
        //      rf_writeRegister(REG_LR_PREAMBLEMSB,0);
        rf_setChannel(sx127x_FREQ_RX + Allocated_Chl_Rx * 1000000); //将终端设置位基站分配信道
        rf_SFSet(Allocated_Chl_SF);
        rf_setRfMode(RF_RECEIVER, 12);
        //#ifdef DEBUG_ERR
        //      App_Printf("-- eventup upload parameters- freq:" " %d ", sx127x_FREQ_RX+Allocated_Chl_Rx*1000000, "\r\n");
        //      App_Printf("-- eventup upload parameters- sf:" " %d ", Allocated_Chl_SF, "\r\n");
        //#endif
    }

    //    rf_setChannel(sx127x_FREQ_RX);//设置为默认接收信道 RF_PUBLIC_CHANNEL
    // os_time_e = OSTimeGet();

    // os_time_f = OSTimeGet();
    ENABLE_DIO0_INT();
}

/*******************************************************************************
 * @brief   Get the address of gRfBuffer
 * @param   None
 * @return  The address of gRfBuffer
 ******************************************************************************/
uint8_t *rf_getRfBuffer(void)
{
    return gRfBuffer;
}

/*******************************************************************************
 * @brief   Get the length
 * @param   None
 * @return  The address of g_rfLength
 ******************************************************************************/
uint8_t rf_getRfLength(void)
{
    return gRfLength;
}

/*******************************************************************************
 * @brief   Get the value of rssi
 * @param   None
 * @return  The value of rssi
 ******************************************************************************/
int8_t rf_getRssi(void)
{

    return gRssi;
}


/*******************************************************************************
 * @brief   Set every byte of gRfBuffer to 0
 * @param   None
 * @return  None
 ******************************************************************************/
void rf_clearRfBuffer(void)
{
    uint8_t i = 0;
    for (i = 0; i < RF_BUFFER_LEN; i++)
    {
        gRfBuffer[i] = 0;
    }
}



/***************************** (END OF FILE) *********************************/
