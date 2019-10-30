#include "../inc/lcd.h"
#include "../inc/sys.h"
#include "stm32f10x.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
//#include "delay.h"
#include "includes.h"
int bLcd_flag = 0;
int bLcd_timer = 0;

///////////////////////////////////////////////////驱动函数
/*
*	LCD 模式写入
*	入口:MODE :COM(命令模式) DAT(数据模式)
*	出口:void
*/
void write_mode(unsigned char MODE)	//写入模式,数据or命令
{
    GPIO_ResetBits(GPIOA, HT1621_WR);								//	RW = 0;
    bsp_DelayUS(10);
    GPIO_SetBits(GPIOA, HT1621_DATA);									//	DA = 1;
    GPIO_SetBits(GPIOA, HT1621_WR);									//	RW = 1;
    bsp_DelayUS(10);

    GPIO_ResetBits(GPIOA, HT1621_WR);								//	RW = 0;
    bsp_DelayUS(10);
    GPIO_ResetBits(GPIOA, HT1621_DATA);								//	DA = 0;
    GPIO_SetBits(GPIOA, HT1621_WR);									//	RW = 1;
    bsp_DelayUS(10);

    GPIO_ResetBits(GPIOA, HT1621_WR);								//	RW = 0;
    bsp_DelayUS(10);

    if (0 == MODE)
    {
        GPIO_ResetBits(GPIOA, HT1621_DATA);							//	DA = 0;
    }
    else
    {
        GPIO_SetBits(GPIOA, HT1621_DATA);								//	DA = 1;
    }
    bsp_DelayUS(10);
    GPIO_SetBits(GPIOA, HT1621_WR);									//	RW = 1;
    bsp_DelayUS(10);
}

/*
*	LCD 命令写入函数
*	入口:cbyte ,控制命令字
*	出口:void
*/
void write_command(unsigned char Cbyte)
{
    unsigned char i = 0;

    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);


        if ((Cbyte >> (7 - i)) & 0x01)
        {
            GPIO_SetBits(GPIOA, HT1621_DATA);
        }
        else
        {
            GPIO_ResetBits(GPIOA, HT1621_DATA);
        }
        bsp_DelayUS(10);
        GPIO_SetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
    }
    GPIO_ResetBits(GPIOA, HT1621_WR);
    bsp_DelayUS(10);
    GPIO_ResetBits(GPIOA, HT1621_DATA);
    GPIO_SetBits(GPIOA, HT1621_WR);
    bsp_DelayUS(10);
}

/*
*	LCD 地址写入函数
*	入口:cbyte,地址
*	出口:void
*/
void write_address(unsigned char Abyte)
{
    unsigned char i = 0;
    Abyte = Abyte << 2;

    for (i = 0; i < 6; i++)
    {
        GPIO_ResetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
        if ((Abyte >> (7 - i)) & 0x01)
        {
            GPIO_SetBits(GPIOA, HT1621_DATA);
        }
        else
        {
            GPIO_ResetBits(GPIOA, HT1621_DATA);
        }
        bsp_DelayUS(10);
        GPIO_SetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
    }
}

/*
*	LCD 数据写入函数
*	入口:Dbyte,数据
*	出口:void
*/
void write_data_8bit(unsigned char Dbyte)
{
    int i = 0;

    for (i = 0; i < 8; i++)
    {
        GPIO_ResetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
        if ((Dbyte >> (7 - i)) & 0x01)
        {
            GPIO_SetBits(GPIOA, HT1621_DATA);
        }
        else
        {
            GPIO_ResetBits(GPIOA, HT1621_DATA);
        }
        bsp_DelayUS(10);
        GPIO_SetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
    }
}

void write_data_4bit(unsigned char Dbyte)
{
    int i = 0;

    for (i = 0; i < 4; i++)
    {
        GPIO_ResetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
        if ((Dbyte >> (3 - i)) & 0x01)
        {
            GPIO_SetBits(GPIOA, HT1621_DATA);
        }
        else
        {
            GPIO_ResetBits(GPIOA, HT1621_DATA);
        }
        bsp_DelayUS(10);
        GPIO_SetBits(GPIOA, HT1621_WR);
        bsp_DelayUS(10);
    }
}

///////////////////////////////////////////////////接口函数
void ht1621_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;// declare the structure
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    //////////////////////////////////////////////////////
    //配置PIN13-PA3,PIN15-PA5,PIN15-PA6,PIN15-PA7,
    //memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Pin = HT1621_CS | HT1621_WR | HT1621_DATA;//HT1621_CS | HT1621_WR | HT1621_DATA | HT1621_IRQ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //配置PIN19-PB1
    //memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//HT1621_RD | GPIO_Pin_7 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_SetBits(GPIOB, GPIO_Pin_0);

}

/*
*	LCD 初始化，对lcd自身做初始化设置
*	入口:void
*	出口:void
*/
void lcd_init(void)
{
    //////////////////////////////////////////////////////
    GPIO_SetBits(GPIOA, HT1621_CS);
    GPIO_SetBits(GPIOA, HT1621_WR);
    GPIO_SetBits(GPIOA, HT1621_DATA);
    bsp_DelayUS(500);

    //////////////////////////////////////////////////////
    GPIO_ResetBits(GPIOA, HT1621_CS);        //CS = 0;
    bsp_DelayUS(10);
    write_mode(COMMAND);    //命令模式
    write_command(0x01);	//Enable System
    write_command(0x03);	//Enable Bias
    write_command(0x04);	//Disable Timer
    write_command(0x05);	//Disable WDT
    write_command(0x08);	//Tone OFF
    write_command(0x18);	//on-chip RC震荡
    write_command(0x29);	//1/4Duty 1/3Bias
    write_command(0x80);	//Disable IRQ
    write_command(0x40);	//Tone Frequency 4kHZ
    write_command(0xE3);	//Normal Mode

    GPIO_SetBits(GPIOA, HT1621_CS);  //CS = 1;

}




/*****************+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*****************************************************/
/********************************************************
函数名称：void Ht1621WrOneData(uint8_t Addr,uint8_t Data)
功能描述: HT1621在指定地址写入数据函数
全局变量：无
参数说明：Addr为写入初始地址，Data为写入数据
返回说明：无
说 明：因为HT1621的数据位4位，所以实际写入数据为参数的后4位
********************************************************/
void Ht1621WrOneData(uint8_t Addr, uint8_t Data)
{
    GPIO_ResetBits(GPIOA, HT1621_CS);
    Ht1621Wr_Data1(0xa0, 3);      //写入数据标志101
    Ht1621Wr_Data1(Addr << 2, 6); //写入地址数据
    Ht1621Wr_Data1(Data << 4, 4); //写入数据
    GPIO_SetBits(GPIOA, HT1621_CS);
    //SysTick_Delay_us(HtDelay);
}


/******************************************************
写数据函数,cnt为传送数据位数,数据传送为低位在前
*******************************************************/
void Ht1621Wr_Data1(uint8_t Data, uint8_t cnt)
{
    uint8_t i;
    for (i = 0; i < cnt; i++)
    {
        GPIO_ResetBits(GPIOA, HT1621_WR);
        //SysTick_Delay_us(HtDelay);
        if (Data & 0x80)
            GPIO_SetBits(GPIOA, HT1621_DATA);
        else
            GPIO_ResetBits(GPIOA, HT1621_DATA);
        // SysTick_Delay_us(HtDelay);
        GPIO_SetBits(GPIOA, HT1621_WR);
        // SysTick_Delay_us(HtDelay);
        Data <<= 1;
    }
}












/*****************+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*****************************************************/

/*
*	LCD 清屏函数
*	入口:void
*	出口:void
*/
void lcd_clr(void)
{
    write_addr_dat_n(0x0, 0x00, 15);
}

/*
*	LCD 全显示函数
*	入口:void
*	出口:void
*/
void lcd_all(void)
{
    write_addr_dat_n(0x0, 0xff, 15);
}

void write_addr_dat_n(unsigned char _addr, unsigned char _dat, unsigned char n)
{
    unsigned char i = 0;

    GPIO_ResetBits(GPIOA, HT1621_CS);								// CS = 0;
    write_mode(DAT);
    write_address(_addr);

    for (i = 0; i < n; i++)
    {
        write_data_8bit(_dat);
    }
    GPIO_SetBits(GPIOA, HT1621_CS);									//CS = 1;
}




void write_1621(unsigned char addr, unsigned char dat)
{
    GPIO_ResetBits(GPIOA, HT1621_CS);
    write_mode(DAT);
    write_address( addr);
    write_data_8bit( dat);
    GPIO_SetBits(GPIOA, HT1621_CS);
    bsp_DelayUS(5);
}
void write_1621t(unsigned char addr, unsigned char addr2, unsigned char dat, unsigned char dat2)
{
    GPIO_ResetBits(GPIOA, HT1621_CS);
    write_mode(DAT);
    write_address( addr);
    write_data_8bit( dat);
    GPIO_SetBits(GPIOA, HT1621_CS);
    GPIO_ResetBits(GPIOA, HT1621_CS);
    write_mode(DAT);
    write_address( addr2);
    write_data_8bit( dat2);
    GPIO_SetBits(GPIOA, HT1621_CS);
}

void fengsu_gaosu()//显示高速
{
    //write_1621(0x01,0x08);
    //lcd_clr();
    Ht1621WrOneData(0x02, 0x0C);
}

void fengsu_zhongsu()//显示中速
{
    Ht1621WrOneData(0x02, 0x0A);
}
void fengsu_disu()//显示低速
{
    Ht1621WrOneData(0x02, 0x09);
}
void shedingwendu()//显示设定温度
{
    Ht1621WrOneData(0x06, 0x01);
}
void shineiwendu()//显示室内温度
{
    Ht1621WrOneData(0x0A, 0x01);
}

void moshi_lengfeng()//显示冷风
{
    Ht1621WrOneData(0x00, 0X0C);
}
void moshi_nuanfeng()//显示暖风
{
    Ht1621WrOneData(0x00, 0X0A);
}
void qingmoshi()//清楚模式
{
    Ht1621WrOneData(0x0, 0X00);
}
void shineishiwei1()//
{
    Ht1621WrOneData(0x08, 0x0A);
    Ht1621WrOneData(0x07, 0x00);
}
void shineishiwei2()//
{
    Ht1621WrOneData(8, 0x07);
    Ht1621WrOneData(7, 0x0D);
    //write_1621(0x07,0x07);
    //		write_1621(0x07,0X0A);
    //	write_1621(0x06,0X0F);


}
void shineishiwei3()//
{
    Ht1621WrOneData(0x08, 0x0E);
    Ht1621WrOneData(7, 9);

}
void Sshineishiwei3()//
{
    Ht1621WrOneData(0x04, 0x0E);
    Ht1621WrOneData(3, 9);

}
void Sshineishiwei2()//
{
    Ht1621WrOneData(4, 6);
    Ht1621WrOneData(3, 13);
}
void Sshineishiwei1()//
{
    Ht1621WrOneData(4, 10);
    Ht1621WrOneData(3, 0);
}

void shineigewei0()
{
    Ht1621WrOneData(0x0A, 0x0B);
    Ht1621WrOneData(0x09, 0x0F);

}
void shineigewei1()//
{
    Ht1621WrOneData(10, 11);
    Ht1621WrOneData(9, 0);

}
void shineigewei2()//
{
    Ht1621WrOneData(0x0A, 0x07);
    Ht1621WrOneData(0x09, 0x0D);

}
void shineigewei3()//
{
    Ht1621WrOneData(0x0A, 0x0F);
    Ht1621WrOneData(0x09, 0x09);

}

void shineigewei4()//
{
    Ht1621WrOneData(0x0A, 0x0F);
    Ht1621WrOneData(0x09, 0x02);

}
void shineigewei5()//
{
    Ht1621WrOneData(0x0A, 0x0D);
    Ht1621WrOneData(0x09, 0x0B);

}
void shineigewei6()//
{
    Ht1621WrOneData(0x0A, 0x0D);
    Ht1621WrOneData(0x09, 0x0F);

}
void shineigewei7()//
{
    Ht1621WrOneData(10, 11);
    Ht1621WrOneData(9, 1);

}
void shineigewei8()//
{
    Ht1621WrOneData(0x0A, 0x0F);
    Ht1621WrOneData(0x09, 0x0F);

}
void shineigewei9()//
{
    Ht1621WrOneData(0x0A, 0x0F);
    Ht1621WrOneData(0x09, 0x0B);

}
void Sshineigewei0()
{
    Ht1621WrOneData(0x05, 0x0F);
    Ht1621WrOneData(0x06, 0x0B);


}
void Sshineigewei1()//
{
    Ht1621WrOneData(6, 11);
    Ht1621WrOneData(5, 0);
}
void Sshineigewei2()//
{
    Ht1621WrOneData(6, 7);
    Ht1621WrOneData(5, 13);
    


}
void Sshineigewei3()//
{
    Ht1621WrOneData(0x06, 0x0F);
    Ht1621WrOneData(0x05, 0x09);

}

void Sshineigewei4()//
{
    Ht1621WrOneData(0x06, 0x0F);
    Ht1621WrOneData(0x05, 0x02);

}
void Sshineigewei5()//
{
    Ht1621WrOneData(0x06, 0x0D);
    Ht1621WrOneData(0x05, 0x0B);

}
void Sshineigewei6()//
{

    Ht1621WrOneData(0x05, 0x0F);
    Ht1621WrOneData(0x06, 0x0D);

}
void Sshineigewei7()//
{
    Ht1621WrOneData(6, 11);
    Ht1621WrOneData(5, 1);

}
void Sshineigewei8()//
{
    Ht1621WrOneData(0x06, 0x0F);
    Ht1621WrOneData(0x05, 0x0F);

}
void Sshineigewei9()//
{
    Ht1621WrOneData(0x06, 0x0F);
    Ht1621WrOneData(0x05, 0x0B);
}






