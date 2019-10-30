/*
*********************************************************************************************************
*
*	模块名称 : SPI总线驱动
*	文件名称 : bsp_spi_bus.h
*	版    本 : V1.0
*	说    明 : 头文件
*
*	Copyright (C),
*
*********************************************************************************************************
*/

#ifndef __BSP_SPI_BUS_H
#define __BSP_SPI_BUS_H




extern uint8_t  gRfBuffer[];
extern uint8_t  gRfLength;
extern int8_t   gRssi;
extern uint8_t  gRfCurChan;
//extern uint8_t  gRfPreMode = RF_STANDBY;
//extern uint8_t  gRfPower = 0;
//extern RF_RECEIVE_CB rf_receiveCb;

//#define SOFT_SPI		/* 定义此行表示使用GPIO模拟SPI接口 */
#define HARD_SPI		/* 定义此行表示使用CPU的硬件SPI接口 */

/*
	【SPI时钟最快是2分频，不支持不分频】
	如果是SPI1，2分频时SCK时钟 = 42M，4分频时SCK时钟 = 21M
	如果是SPI3, 2分频时SCK时钟 = 21M
*/
#define SPI_SPEED_42M		SPI_BaudRatePrescaler_2
#define SPI_SPEED_21M		SPI_BaudRatePrescaler_4
#define SPI_SPEED_5_2M		SPI_BaudRatePrescaler_8
#define SPI_SPEED_2_6M		SPI_BaudRatePrescaler_16
#define SPI_SPEED_1_3M		SPI_BaudRatePrescaler_32
#define SPI_SPEED_0_6M		SPI_BaudRatePrescaler_64

/*
	SPI1口线分配
	PA5/SPI1_SCK
	PA6/SPI1_MISO
	PA7/SPI1_MOSI
        PA4/SPI1_CS


	SPI1的时钟源是 APB2Periph
*/

/* 定义SPI总线的 GPIO端口 */
#define RCC_SCK 	RCC_APB2Periph_GPIOB
#define PORT_SCK	GPIOB
#define PIN_SCK		GPIO_Pin_3

#define RCC_MISO 	RCC_APB2Periph_GPIOB
#define PORT_MISO	GPIOB
#define PIN_MISO	GPIO_Pin_4

#define RCC_MOSI 	RCC_APB2Periph_GPIOB
#define PORT_MOSI	GPIOB
#define PIN_MOSI	GPIO_Pin_5

//#define GPIO_Remap_SWJ_JTAGDisable  ((uint32_t)0x00300200)  /*!< JTAG-DP Disabled and SW-DP Enabled*/

/*
	sx127x控制口线分配
	PA4/SPI1_CS
        PA11/sx127x RESET
        PB14/sx127x DIO0 配置为外部中断，用于接收中断
*/

/* sx127x片选 GPIO端口  */
#define sx127x_RCC_CS 			RCC_APB2Periph_GPIOA
#define sx127x_PORT_CS			GPIOA
#define sx127x_PIN_CS			GPIO_Pin_15
/*  sx127x CS control  */
#define SPI_ENABLE()			sx127x_PORT_CS->BRR = sx127x_PIN_CS
#define SPI_DISABLE()			sx127x_PORT_CS->BSRR = sx127x_PIN_CS
//#define sx127x_CS_0()			sx127x_PORT_CS->BRR = sx127x_PIN_CS
//#define sx127x_CS_1()			sx127x_PORT_CS->BSRR = sx127x_PIN_CS

/* sx127x TX控制 GPIO端口
#define sx127x_RCC_TX 			RCC_APB2Periph_GPIOA
#define sx127x_PORT_TX			GPIOA
#define sx127x_PIN_TX			GPIO_Pin_9
//The SX1278 TX/RX control //
#define TX_DIS()                        do{ GPIO_SetBits(sx127x_PORT_TX,sx127x_PIN_TX);}  while(0)
#define TX_EN()                         do{ GPIO_ResetBits(sx127x_PORT_TX,sx127x_PIN_TX);}while(0)
*/

/* sx127x DIO0端口  */
#define sx127x_RCC_DIO0 		RCC_APB2Periph_GPIOA
#define sx127x_PORT_DIO0		GPIOA
#define sx127x_PIN_DIO0			GPIO_Pin_1
/*  sx127x DIO0  */
#define DIO0_IS_1()			(!!(sx127x_PORT_DIO0->IDR & sx127x_PIN_DIO0))
#define sx127x_LINE_DIO0                EXTI_Line1
#define sx127x_EXTI_IRQn_DIO0           EXTI1_IRQn

/* sx127x RESET电源复位引脚 端口  */
#define sx127x_RCC_RESET		RCC_APB2Periph_GPIOA
#define sx127x_PORT_RESET		GPIOA
#define sx127x_PIN_RESET		GPIO_Pin_0
/*  sx127x RESET control  */
#define RESET_OUT_H()			sx127x_PORT_RESET->BSRR = sx127x_PIN_RESET
#define RESET_OUT_L()			sx127x_PORT_RESET->BRR = sx127x_PIN_RESET


#ifdef HARD_SPI         /*硬件SPI*/
#define SPI_HARD	SPI1
#define RCC_SPI		RCC_APB2Periph_SPI1

/* SPI or I2S mode selection masks */
#define SPI_Mode_Select      ((uint16_t)0xF7FF)
#define I2S_Mode_Select      ((uint16_t)0x0800)

/* SPI registers Masks */
#define CR1_CLEAR_Mask       ((uint16_t)0x3040)
#define I2SCFGR_CLEAR_Mask   ((uint16_t)0xF040)

/* SPI SPE mask */
#define CR1_SPE_Set          ((uint16_t)0x0040)
#define CR1_SPE_Reset        ((uint16_t)0xFFBF)
#endif


/*------------------------------------------------------------------------------------------------*/
/*  使能/失能 DIO0 外部中断  */
#define ENABLE_DIO0_INT()               do{ NVIC_EnableIRQ(sx127x_EXTI_IRQn_DIO0);  }while(0)
#define DISABLE_DIO0_INT()              do{ NVIC_DisableIRQ(sx127x_EXTI_IRQn_DIO0); }while(0)

#define CLEAR_DIO0_INT()                do{ EXTI_ClearFlag(sx127x_LINE_DIO0);\
                                            EXTI_ClearITPendingBit(sx127x_LINE_DIO0);}while(0)


uint8_t bsp_SpiBusBusy(void);

u8 SPI_1278_ReadWriteByte(u8 TxData);


//void bsp_SPI_Init(uint16_t _cr1);

void bsp_SpiBusEnter(void);
void bsp_SpiBusExit(void);
uint8_t bsp_SpiBusBusy(void);


void bsp_sx127xIF_init(void);

#endif

/***************************** (END OF FILE) *********************************/
