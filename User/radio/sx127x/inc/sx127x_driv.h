/*
*********************************************************************************************************
*
*	模块名称 : SPI接口sx127x模块
*	文件名称 : sx127x_dirv.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C),
*********************************************************************************************************
*/

#ifndef __sx127x_dirv_H__
#define __sx127x_dirv_H__

#ifdef __cplusplus
extern "C" {
#endif


//---------------------------------------------------------------------------------------------//
extern OS_EVENT *Sem_RF_Recv;
//-------------------------------------------------------------------------------------------//




/*==================================================================================================*/
/************************************************************************************
原board.h中的声明
************************************************************************************/
#define MCLK          8000000

/***********************************************************************************/
/*********************************** PULIC MACROS  *********************************/
/***********************************************************************************/
#define RFTIMEOUT            MCLK/400
#define RFTIMEOUT_LONG       MCLK

#define RF_BUFFER_LEN        200

/************************************************************************************
原hal.h中的声明
************************************************************************************/
//变量类型定义
typedef unsigned char	           BOOL;
//typedef unsigned char	           uint8_t;
//typedef signed   char	           int8_t;
//typedef unsigned short int	   uint16_t;
//typedef signed   short int      int16_t;
//typedef unsigned int	           uint32_t;
//typedef signed   int	           int32_t;

typedef enum
{
    sx127x_FAILED = 0,
    sx127x_SUCCESS = 1
} result_t;

//#ifndef FALSE
#define DUMMY_CHAR   0xFF    //定义一个虚假发送数据
//#endif

//Common values
#ifndef FALSE
#define FALSE 0
#endif

#ifndef TRUE
#define TRUE  1
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef HIGH
#define HIGH 1
#endif

#ifndef LOW
#define LOW  0
#endif

//原rf_internal.h中文件
/***********************************************************************************/
/*********************************** SX1278 MACROS  ********************************/
/***********************************************************************************/

/*******************************************************************
** RF State machine                                               **
*******************************************************************/
#define RF_STOP              0x01
#define RF_BUSY              0x02
#define RF_RX_DONE           0x04
#define RF_TX_DONE           0x08
#define RF_ERROR             0x10
#define RF_TIMEOUT           0x20

/*******************************************************************
** RF function return codes                                       **
*******************************************************************/
#define OK                   0x00
#define ERROR                0x01
#define RX_TIMEOUT           0x02
#define RX_RUNNING           0x03
#define TX_TIMEOUT           0x04
#define TX_RUNNING           0x05

/*******************************************************************
** SX1231 Operating modes definition                              **
*******************************************************************/
#define RF_SLEEP                         0x00
#define RF_STANDBY                       0x04
#define RF_SYNTHESIZER                   0x08
#define RF_TRANSMITTER                   0x0C
#define RF_RECEIVER                      0x10


/*!
 * SX1276 Internal registers Address
 */
#define REG_LR_FIFO                                 0x00
// Common settings
#define REG_LR_OPMODE                               0x01
//#define REG_LR_BANDSETTING                          0x04
#define REG_LR_FRFMSB                               0x06
#define REG_LR_FRFMID                               0x07
#define REG_LR_FRFLSB                               0x08
// Tx settings
#define REG_LR_PACONFIG                             0x09
#define REG_LR_PARAMP                               0x0A
#define REG_LR_OCP                                  0x0B
// Rx settings
#define REG_LR_LNA                                  0x0C
// LoRa registers
#define REG_LR_FIFOADDRPTR                          0x0D
#define REG_LR_FIFOTXBASEADDR                       0x0E
#define REG_LR_FIFORXBASEADDR                       0x0F
#define REG_LR_FIFORXCURRENTADDR                    0x10
#define REG_LR_IRQFLAGSMASK                         0x11
#define REG_LR_IRQFLAGS                             0x12
#define REG_LR_NBRXBYTES                            0x13
#define REG_LR_RXHEADERCNTVALUEMSB                  0x14
#define REG_LR_RXHEADERCNTVALUELSB                  0x15
#define REG_LR_RXPACKETCNTVALUEMSB                  0x16
#define REG_LR_RXPACKETCNTVALUELSB                  0x17
#define REG_LR_MODEMSTAT                            0x18
#define REG_LR_PKTSNRVALUE                          0x19
#define REG_LR_PKTRSSIVALUE                         0x1A
#define REG_LR_RSSIVALUE                            0x1B
#define REG_LR_HOPCHANNEL                           0x1C
#define REG_LR_MODEMCONFIG1                         0x1D
#define REG_LR_MODEMCONFIG2                         0x1E
#define REG_LR_SYMBTIMEOUTLSB                       0x1F
#define REG_LR_PREAMBLEMSB                          0x20
#define REG_LR_PREAMBLELSB                          0x21
#define REG_LR_PAYLOADLENGTH                        0x22
#define REG_LR_PAYLOADMAXLENGTH                     0x23
#define REG_LR_HOPPERIOD                            0x24
#define REG_LR_FIFORXBYTEADDR                       0x25
#define REG_LR_MODEMCONFIG3                         0x26
#define REG_LR_SYNWORD                              0x39

// end of documented register in datasheet
// I/O settings
#define REG_LR_DIOMAPPING1                          0x40
#define REG_LR_DIOMAPPING2                          0x41
// Version
#define REG_LR_VERSION                              0x42
// Additional settings
#define REG_LR_PLLHOP                               0x44
#define REG_LR_TCXO                                 0x4B
#define REG_LR_PADAC                                0x4D
#define REG_LR_FORMERTEMP                           0x5B
#define REG_LR_BITRATEFRAC                          0x5D
#define REG_LR_AGCREF                               0x61
#define REG_LR_AGCTHRESH1                           0x62
#define REG_LR_AGCTHRESH2                           0x63
#define REG_LR_AGCTHRESH3                           0x64


/*!
 * SX1276 LoRa bit control definition
 */

/*!
 * RegFifo
 */

/*!
 * RegOpMode
 */
#define RFLR_OPMODE_LONGRANGEMODE_MASK              0x7F
#define RFLR_OPMODE_LONGRANGEMODE_OFF               0x00 // Default
#define RFLR_OPMODE_LONGRANGEMODE_ON                0x80

#define RFLR_OPMODE_ACCESSSHAREDREG_MASK            0xBF
#define RFLR_OPMODE_ACCESSSHAREDREG_ENABLE          0x40
#define RFLR_OPMODE_ACCESSSHAREDREG_DISABLE         0x00 // Default

#define RFLR_OPMODE_FREQMODE_ACCESS_MASK            0xF7
#define RFLR_OPMODE_FREQMODE_ACCESS_LF              0x08 // Default
#define RFLR_OPMODE_FREQMODE_ACCESS_HF              0x00

#define RFLR_OPMODE_MASK                            0xF8
#define RFLR_OPMODE_SLEEP                           0x00
#define RFLR_OPMODE_STANDBY                         0x01 // Default
#define RFLR_OPMODE_SYNTHESIZER_TX                  0x02
#define RFLR_OPMODE_TRANSMITTER                     0x03
#define RFLR_OPMODE_SYNTHESIZER_RX                  0x04
#define RFLR_OPMODE_RECEIVER                        0x05
// LoRa specific modes
#define RFLR_OPMODE_RECEIVER_SINGLE                 0x06
#define RFLR_OPMODE_CAD                             0x07
#define REG_OPMODE_SYNWORD                          0x34

/*!
 * RegBandSetting
 */
#define RFLR_BANDSETTING_MASK                    0x3F
#define RFLR_BANDSETTING_AUTO                    0x00 // Default
#define RFLR_BANDSETTING_DIV_BY_1                0x40
#define RFLR_BANDSETTING_DIV_BY_2                0x80
#define RFLR_BANDSETTING_DIV_BY_6                0xC0


/*!
 * RegPaConfig
 */
#define RFLR_PACONFIG_PASELECT_MASK                 0x7F
#define RFLR_PACONFIG_PASELECT_PABOOST              0x80
#define RFLR_PACONFIG_PASELECT_RFO                  0x00 // Default

#define RFLR_PACONFIG_MAX_POWER_MASK                0x8F

#define RFLR_PACONFIG_OUTPUTPOWER_MASK              0xF0

/*!
 * RegPaRamp
 */
#define RFLR_PARAMP_TXBANDFORCE_MASK                0xEF
#define RFLR_PARAMP_TXBANDFORCE_BAND_SEL            0x10
#define RFLR_PARAMP_TXBANDFORCE_AUTO                0x00 // Default

#define RFLR_PARAMP_MASK                            0xF0
#define RFLR_PARAMP_3400_US                         0x00
#define RFLR_PARAMP_2000_US                         0x01
#define RFLR_PARAMP_1000_US                         0x02
#define RFLR_PARAMP_0500_US                         0x03
#define RFLR_PARAMP_0250_US                         0x04
#define RFLR_PARAMP_0125_US                         0x05
#define RFLR_PARAMP_0100_US                         0x06
#define RFLR_PARAMP_0062_US                         0x07
#define RFLR_PARAMP_0050_US                         0x08
#define RFLR_PARAMP_0040_US                         0x09 // Default
#define RFLR_PARAMP_0031_US                         0x0A
#define RFLR_PARAMP_0025_US                         0x0B
#define RFLR_PARAMP_0020_US                         0x0C
#define RFLR_PARAMP_0015_US                         0x0D
#define RFLR_PARAMP_0012_US                         0x0E
#define RFLR_PARAMP_0010_US                         0x0F

/*!
 * RegOcp
 */
#define RFLR_OCP_MASK                               0xDF
#define RFLR_OCP_ON                                 0x20 // Default
#define RFLR_OCP_OFF                                0x00

#define RFLR_OCP_TRIM_MASK                          0xE0
#define RFLR_OCP_TRIM_045_MA                        0x00
#define RFLR_OCP_TRIM_050_MA                        0x01
#define RFLR_OCP_TRIM_055_MA                        0x02
#define RFLR_OCP_TRIM_060_MA                        0x03
#define RFLR_OCP_TRIM_065_MA                        0x04
#define RFLR_OCP_TRIM_070_MA                        0x05
#define RFLR_OCP_TRIM_075_MA                        0x06
#define RFLR_OCP_TRIM_080_MA                        0x07
#define RFLR_OCP_TRIM_085_MA                        0x08
#define RFLR_OCP_TRIM_090_MA                        0x09
#define RFLR_OCP_TRIM_095_MA                        0x0A
#define RFLR_OCP_TRIM_100_MA                        0x0B  // Default
#define RFLR_OCP_TRIM_105_MA                        0x0C
#define RFLR_OCP_TRIM_110_MA                        0x0D
#define RFLR_OCP_TRIM_115_MA                        0x0E
#define RFLR_OCP_TRIM_120_MA                        0x0F
#define RFLR_OCP_TRIM_130_MA                        0x10
#define RFLR_OCP_TRIM_140_MA                        0x11
#define RFLR_OCP_TRIM_150_MA                        0x12
#define RFLR_OCP_TRIM_160_MA                        0x13
#define RFLR_OCP_TRIM_170_MA                        0x14
#define RFLR_OCP_TRIM_180_MA                        0x15
#define RFLR_OCP_TRIM_190_MA                        0x16
#define RFLR_OCP_TRIM_200_MA                        0x17
#define RFLR_OCP_TRIM_210_MA                        0x18
#define RFLR_OCP_TRIM_220_MA                        0x19
#define RFLR_OCP_TRIM_230_MA                        0x1A
#define RFLR_OCP_TRIM_240_MA                        0x1B

/*!
 * RegLna
 */
#define RFLR_LNA_GAIN_MASK                          0x1F
#define RFLR_LNA_GAIN_G1                            0x20 // Default
#define RFLR_LNA_GAIN_G2                            0x40
#define RFLR_LNA_GAIN_G3                            0x60
#define RFLR_LNA_GAIN_G4                            0x80
#define RFLR_LNA_GAIN_G5                            0xA0
#define RFLR_LNA_GAIN_G6                            0xC0

#define RFLR_LNA_BOOST_LF_MASK                      0xE7
#define RFLR_LNA_BOOST_LF_DEFAULT                   0x00 // Default
#define RFLR_LNA_BOOST_LF_GAIN                      0x08
#define RFLR_LNA_BOOST_LF_IP3                       0x10
#define RFLR_LNA_BOOST_LF_BOOST                     0x18

#define RFLR_LNA_RXBANDFORCE_MASK                   0xFB
#define RFLR_LNA_RXBANDFORCE_BAND_SEL               0x04
#define RFLR_LNA_RXBANDFORCE_AUTO                   0x00 // Default

#define RFLR_LNA_BOOST_HF_MASK                      0xFC
#define RFLR_LNA_BOOST_HF_OFF                       0x00 // Default
#define RFLR_LNA_BOOST_HF_ON                        0x03

/*!
 * RegFifoAddrPtr
 */
#define RFLR_FIFOADDRPTR                            0x00 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFOTXBASEADDR                         0x80 // Default

/*!
 * RegFifoTxBaseAddr
 */
#define RFLR_FIFORXBASEADDR                         0x00 // Default

/*!
 * RegFifoRxCurrentAddr (Read Only)
 */

/*!
 * RegIrqFlagsMask
 */
#define RFLR_IRQFLAGS_RXTIMEOUT_MASK                0x80
#define RFLR_IRQFLAGS_RXDONE_MASK                   0x40
#define RFLR_IRQFLAGS_PAYLOADCRCERROR_MASK          0x20
#define RFLR_IRQFLAGS_VALIDHEADER_MASK              0x10
#define RFLR_IRQFLAGS_TXDONE_MASK                   0x08
#define RFLR_IRQFLAGS_CADDONE_MASK                  0x04
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL_MASK       0x02
#define RFLR_IRQFLAGS_CADDETECTED_MASK              0x01

/*!
 * RegIrqFlags
 */
#define RFLR_IRQFLAGS_RXTIMEOUT                     0x80
#define RFLR_IRQFLAGS_RXDONE                        0x40
#define RFLR_IRQFLAGS_PAYLOADCRCERROR               0x20
#define RFLR_IRQFLAGS_VALIDHEADER                   0x10
#define RFLR_IRQFLAGS_TXDONE                        0x08
#define RFLR_IRQFLAGS_CADDONE                       0x04
#define RFLR_IRQFLAGS_FHSSCHANGEDCHANNEL            0x02
#define RFLR_IRQFLAGS_CADDETECTED                   0x01



/*!
 * RegFifoRxNbBytes (Read Only)    //
 */


/*!
* RegRxHeaderCntValueMsb (Read Only)    //
*/


/*!
* RegRxHeaderCntValueLsb (Read Only)    //
*/


/*!
 * RegRxPacketCntValueMsb (Read Only)    //
 */


/*!
* RegRxPacketCntValueLsb (Read Only)    //
*/


/*!
* RegModemStat (Read Only)    //
*/
#define RFLR_MODEMSTAT_RX_CR_MASK                   0x1F
#define RFLR_MODEMSTAT_MODEM_STATUS_MASK            0xE0

/*!
 * RegPktSnrValue (Read Only)    //
 */


/*!
* RegPktRssiValue (Read Only)    //
*/


/*!
 * RegRssiValue (Read Only)    //
 */


/*!
* RegModemConfig1
*/
#define RFLR_MODEMCONFIG1_BW_MASK                   0x0F

#define RFLR_MODEMCONFIG1_BW_7_81_KHZ               0x00
#define RFLR_MODEMCONFIG1_BW_10_41_KHZ              0x10
#define RFLR_MODEMCONFIG1_BW_15_62_KHZ              0x20
#define RFLR_MODEMCONFIG1_BW_20_83_KHZ              0x30
#define RFLR_MODEMCONFIG1_BW_31_25_KHZ              0x40
#define RFLR_MODEMCONFIG1_BW_41_66_KHZ              0x50
#define RFLR_MODEMCONFIG1_BW_62_50_KHZ              0x60
#define RFLR_MODEMCONFIG1_BW_125_KHZ                0x70 // Default
#define RFLR_MODEMCONFIG1_BW_250_KHZ                0x80
#define RFLR_MODEMCONFIG1_BW_500_KHZ                0x90

#define RFLR_MODEMCONFIG1_CODINGRATE_MASK           0xF1
#define RFLR_MODEMCONFIG1_CODINGRATE_4_5            0x02 //0x02 pigpeng 2019.5.18
#define RFLR_MODEMCONFIG1_CODINGRATE_4_6            0x04 // Default
#define RFLR_MODEMCONFIG1_CODINGRATE_4_7            0x06
#define RFLR_MODEMCONFIG1_CODINGRATE_4_8            0x08

#define RFLR_MODEMCONFIG1_IMPLICITHEADER_MASK       0xFE
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_ON         0x01
#define RFLR_MODEMCONFIG1_IMPLICITHEADER_OFF        0x00 // Default

/*!
* RegModemConfig2
*/
#define RFLR_MODEMCONFIG2_SF_MASK                   0x0F
#define RFLR_MODEMCONFIG2_SF_6                      0x60
#define RFLR_MODEMCONFIG2_SF_7                      0x70 // Default
#define RFLR_MODEMCONFIG2_SF_8                      0x80
#define RFLR_MODEMCONFIG2_SF_9                      0x90
#define RFLR_MODEMCONFIG2_SF_10                     0xA0
#define RFLR_MODEMCONFIG2_SF_11                     0xB0
#define RFLR_MODEMCONFIG2_SF_12                     0xC0

#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_MASK     0xF7
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_ON       0x08
#define RFLR_MODEMCONFIG2_TXCONTINUOUSMODE_OFF      0x00

#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_MASK         0xFB
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_ON           0x04
#define RFLR_MODEMCONFIG2_RXPAYLOADCRC_OFF          0x00 // Default

#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB_MASK       0xFC
#define RFLR_MODEMCONFIG2_SYMBTIMEOUTMSB            0x00 // Default


/*!
 * RegHopChannel (Read Only)
 */
#define RFLR_HOPCHANNEL_PLL_LOCK_TIMEOUT_MASK       0x7F
#define RFLR_HOPCHANNEL_PLL_LOCK_FAIL               0x80
#define RFLR_HOPCHANNEL_PLL_LOCK_SUCCEED            0x00 // Default

#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_MASK          0xBF
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_ON            0x40
#define RFLR_HOPCHANNEL_PAYLOAD_CRC16_OFF           0x00 // Default

#define RFLR_HOPCHANNEL_CHANNEL_MASK                0x3F


/*!
 * RegSymbTimeoutLsb
 */
#define RFLR_SYMBTIMEOUTLSB_SYMBTIMEOUT             0x64 // Default

/*!
 * RegPreambleLengthMsb
 */
#define RFLR_PREAMBLELENGTHMSB                      0x00 // Default

/*!
 * RegPreambleLengthLsb
 */
#define RFLR_PREAMBLELENGTHLSB                      0x08 // Default

/*!
 * RegPayloadLength
 */
#define RFLR_PAYLOADLENGTH                          0x0E // Default

/*!
 * RegPayloadMaxLength
 */
#define RFLR_PAYLOADMAXLENGTH                       0xFF // Default

/*!
 * RegHopPeriod
 */
#define RFLR_HOPPERIOD_FREQFOPPINGPERIOD            0x00 // Default


/*!
 * RegDioMapping1
 */
#define RFLR_DIOMAPPING1_DIO0_MASK                  0x3F
#define RFLR_DIOMAPPING1_DIO0_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO0_01                    0x40
#define RFLR_DIOMAPPING1_DIO0_10                    0x80
#define RFLR_DIOMAPPING1_DIO0_11                    0xC0

#define RFLR_DIOMAPPING1_DIO1_MASK                  0xCF
#define RFLR_DIOMAPPING1_DIO1_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO1_01                    0x10
#define RFLR_DIOMAPPING1_DIO1_10                    0x20
#define RFLR_DIOMAPPING1_DIO1_11                    0x30

#define RFLR_DIOMAPPING1_DIO2_MASK                  0xF3
#define RFLR_DIOMAPPING1_DIO2_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO2_01                    0x04
#define RFLR_DIOMAPPING1_DIO2_10                    0x08
#define RFLR_DIOMAPPING1_DIO2_11                    0x0C

#define RFLR_DIOMAPPING1_DIO3_MASK                  0xFC
#define RFLR_DIOMAPPING1_DIO3_00                    0x00  // Default
#define RFLR_DIOMAPPING1_DIO3_01                    0x01
#define RFLR_DIOMAPPING1_DIO3_10                    0x02
#define RFLR_DIOMAPPING1_DIO3_11                    0x03

/*!
 * RegDioMapping2
 */
#define RFLR_DIOMAPPING2_DIO4_MASK                  0x3F
#define RFLR_DIOMAPPING2_DIO4_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO4_01                    0x40
#define RFLR_DIOMAPPING2_DIO4_10                    0x80
#define RFLR_DIOMAPPING2_DIO4_11                    0xC0

#define RFLR_DIOMAPPING2_DIO5_MASK                  0xCF
#define RFLR_DIOMAPPING2_DIO5_00                    0x00  // Default
#define RFLR_DIOMAPPING2_DIO5_01                    0x10
#define RFLR_DIOMAPPING2_DIO5_10                    0x20
#define RFLR_DIOMAPPING2_DIO5_11                    0x30

#define RFLR_DIOMAPPING2_MAP_MASK                   0xFE
#define RFLR_DIOMAPPING2_MAP_PREAMBLEDETECT         0x01
#define RFLR_DIOMAPPING2_MAP_RSSI                   0x00  // Default

/*!
 * RegVersion (Read Only)
 */

/*!
 * RegAgcRef
 */

/*!
 * RegAgcThresh1
 */

/*!
 * RegAgcThresh2
 */

/*!
 * RegAgcThresh3
 */

/*!
 * RegFifoRxByteAddr (Read Only)
 */

/*!
 * RegPllHop
 */
#define RFLR_PLLHOP_FASTHOP_MASK                    0x7F
#define RFLR_PLLHOP_FASTHOP_ON                      0x80
#define RFLR_PLLHOP_FASTHOP_OFF                     0x00 // Default

/*!
 * RegTcxo
 */
#define RFLR_TCXO_TCXOINPUT_MASK                    0xEF
#define RFLR_TCXO_TCXOINPUT_ON                      0x10
#define RFLR_TCXO_TCXOINPUT_OFF                     0x00  // Default

/*!
 * RegPaDac
 */
#define RFLR_PADAC_20DBM_MASK                       0xF8
#define RFLR_PADAC_20DBM_ON                         0x07
#define RFLR_PADAC_20DBM_OFF                        0x04  // Default

/*!
 * RegPll
 */
#define RFLR_PLL_BANDWIDTH_MASK                     0x3F
#define RFLR_PLL_BANDWIDTH_75                       0x00
#define RFLR_PLL_BANDWIDTH_150                      0x40
#define RFLR_PLL_BANDWIDTH_225                      0x80
#define RFLR_PLL_BANDWIDTH_300                      0xC0  // Default

/*!
 * RegPllLowPn
 */
#define RFLR_PLLLOWPN_BANDWIDTH_MASK                0x3F
#define RFLR_PLLLOWPN_BANDWIDTH_75                  0x00
#define RFLR_PLLLOWPN_BANDWIDTH_150                 0x40
#define RFLR_PLLLOWPN_BANDWIDTH_225                 0x80
#define RFLR_PLLLOWPN_BANDWIDTH_300                 0xC0  // Default

/*!
 * RegModemConfig3
 */
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_MASK  0xF7
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_ON    0x08
#define RFLR_MODEMCONFIG3_LOWDATARATEOPTIMIZE_OFF   0x00 // Default

#define RFLR_MODEMCONFIG3_AGCAUTO_MASK              0xFB
#define RFLR_MODEMCONFIG3_AGCAUTO_ON                0x04 // Default 
#define RFLR_MODEMCONFIG3_AGCAUTO_OFF               0x00

/*!
 * RegFormerTemp
 */
/******************************************************************************************************/
/******************************************************************************************************/


/**********************************************原rf.h中文件***************************************************/
typedef enum
{
    RATE_537 = 0x0,    //SF-11 BAND-125K  CODE-1   -134.5
    RATE_1K = 0x01,    //SF-11 BAND-250K  CODE-1   -131.5
    RATE_2K = 0x02,    //SF-10 BAND-250K  CODE-1   -129
    RATE_3K = 0x03,    //SF-9 BAND-250K  CODE-1   -126
    RATE_6K = 0x04,    //SF-8 BAND-250K   CODE-1   -124
    RATE_10K = 0x05,   //SF-7 BAND-250K   CODE-1   -121
    RATE_18K = 0x06,   //SF-6 BAND-250K   CODE-1   -119
} rfRate_t;




typedef enum
{
    SF7      = 0x0,    //SF-11 BAND-125K  CODE-1   -134.5
    SF8      = 0x01,    //SF-11 BAND-250K  CODE-1   -131.5
    SF9      = 0x02,    //SF-10 BAND-250K  CODE-1   -129
    SF10     = 0x03,    //SF-9 BAND-250K  CODE-1   -126
    SF11     = 0x04,    //SF-8 BAND-250K   CODE-1   -124
    SF12     = 0x05,   //SF-7 BAND-250K   CODE-1   -121
    SF7_SF12 = 0x06,   //SF-6 BAND-250K   CODE-1   -119
} RF_SF_Set;

#if 0
struct sx127x
{
    struct radio_channel radio;                    // Super struct

    SX127X_FLAG          flag;                     // Flag param

    uint8_t                tx_buff[SX127X_BUFF_LEN]; // Tx buffer (save Tx data)
    uint8_t                rx_buff[SX127X_BUFF_LEN]; // Rx buffer (save Rx data)

    void                 *priv; // struct sx130x, for SX130x radio channel

    // --- Chip func(local) ---
    void (*select_cs)(void);    // Select CS func ptr, need to be init in bsp.c
    void (*deselect_cs)(void);  // Deselect CS func ptr, need to be init in bsp.c
    int (*control)(uint32 opt); // Control func, need to be init in bsp.c
    // ------------------------
};
#endif


#define FREQ_STEP                           (61.03515625) // Fstep = Fxosc/2^19
#define FREQ_LOWER_BOUND                    (470000000) //频率下界
#define FREQ_UPPER_BOUND                    (510000000) //频率下界

#define sx127x_FREQ_TX                      (470300000) // Start freq (Rx0)

#define sx127x_FREQ_RX                      (500000000) // Start freq (Rx0)

#define sx127x_RSSI_Offset_HF                (157) // CSMA 检测时 RSSI 校准，HF：高频 157
#define sx127x_RSSI_Offset_LF                (164) // 低频 164
#define sx127x_RSSI_THRD                     (130)
#define sx127x_RETRANS_MAX                   (3)



typedef void (*RF_RECEIVE_CB) (uint8_t *buf, uint8_t len, int8_t rssi);


extern uint8_t Allocated_Chl_Tx;
extern uint8_t Allocated_Chl_Rx;
extern RF_SF_Set Allocated_Chl_SF;



#if 1
//函数声明
void rf_wait(uint16_t timeout);
//void rf_setRfMode(uint8_t mode);
void rf_setRfMode(uint8_t mode, uint8_t preamb);
void rf_writeRegister(uint8_t address, uint16_t value);
uint16_t rf_readRegister(uint8_t address);
void rf_portInit(void);
void rf_sendByte(uint8_t byte);
uint8_t rf_receiveByte(void);
result_t rf_writeFifo(uint8_t *buffer, uint8_t size);
result_t rf_readFifo(uint8_t *buffer, uint8_t size);
result_t rf_setRate(rfRate_t rate);

//函数声明
uint8_t rf_getChannel(void);//get the rf channel.Return the register of rf chip working channel now.未定义
result_t rf_init(RF_RECEIVE_CB func);//Initialise the register of rf chip.Return SUCCESS or FAILED
result_t rf_start(void);//Start the crystal oscillator, and then the rf enter the Idle state.
result_t rf_stop(void);//Stop the rf crystal oscillator, and turn off the rf power.
//result_t rf_setChannel(uint8_t channel);//set the rf channel.
result_t rf_setPower(uint8_t power);//set the rf power. the typical value see the datasheet.
uint8_t rf_getPower(void);// Get the rf power.
result_t rf_send(uint8_t *buf, uint8_t len);// Write the data to fifo, and then turn on the tx.
void rf_receiveOff(void);//make 1278 can't receive anything
void rf_receiveOn(void);//Turn on the receive function of 1278
uint8_t *rf_getRfBuffer(void);//Get the address of g_rfBuffer
uint8_t rf_getRfLength(void);//Get the address of g_rfLength
int8_t rf_getRssi(void);//Get the address of g_rssi
void rf_clearRfBuffer(void);//Set every byte of g_rfBuffer to 0
result_t rf_setChannel(uint32_t Freq);
#endif


/*===================================================================================================*/

void sx127x_init(void);  //sx127x初始化

result_t Get_sx127xData(void);//sx127x读取接收数据

result_t rf_SFSet(RF_SF_Set sf_value);//设置sx127x SF 值

#ifdef __cplusplus
}
#endif

#endif

/***************************** (END OF FILE) *********************************/
