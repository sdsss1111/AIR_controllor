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

///////////////////////////////////////////////////��������
/*
*	LCD ģʽд��
*	���:MODE :COM(����ģʽ) DAT(����ģʽ)
*	����:void
*/
void write_mode(unsigned char MODE)	//д��ģʽ,����or����
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
*	LCD ����д�뺯��
*	���:cbyte ,����������
*	����:void
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
*	LCD ��ַд�뺯��
*	���:cbyte,��ַ
*	����:void
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
*	LCD ����д�뺯��
*	���:Dbyte,����
*	����:void
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

///////////////////////////////////////////////////�ӿں���
void ht1621_init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;// declare the structure
    //GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    // RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE);
    //////////////////////////////////////////////////////
    //����PIN13-PA3,PIN15-PA5,PIN15-PA6,PIN15-PA7,
    //memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Pin = HT1621_CS | HT1621_WR | HT1621_DATA;//HT1621_CS | HT1621_WR | HT1621_DATA | HT1621_IRQ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    //����PIN19-PB1
    //memset(&GPIO_InitStructure, 0, sizeof(GPIO_InitTypeDef));

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;//HT1621_RD | GPIO_Pin_7 | GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    //GPIO_SetBits(GPIOB, GPIO_Pin_0);

}

/*
*	LCD ��ʼ������lcd��������ʼ������
*	���:void
*	����:void
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
    write_mode(COMMAND);    //����ģʽ
    write_command(0x01);	//Enable System
    write_command(0x03);	//Enable Bias
    write_command(0x04);	//Disable Timer
    write_command(0x05);	//Disable WDT
    write_command(0x08);	//Tone OFF
    write_command(0x18);	//on-chip RC��
    write_command(0x29);	//1/4Duty 1/3Bias
    write_command(0x80);	//Disable IRQ
    write_command(0x40);	//Tone Frequency 4kHZ
    write_command(0xE3);	//Normal Mode

    GPIO_SetBits(GPIOA, HT1621_CS);  //CS = 1;

}




/*****************+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*****************************************************/
/********************************************************
�������ƣ�void Ht1621WrOneData(uint8_t Addr,uint8_t Data)
��������: HT1621��ָ����ַд�����ݺ���
ȫ�ֱ�������
����˵����AddrΪд���ʼ��ַ��DataΪд������
����˵������
˵ ������ΪHT1621������λ4λ������ʵ��д������Ϊ�����ĺ�4λ
********************************************************/
void Ht1621WrOneData(uint8_t Addr, uint8_t Data)
{
    GPIO_ResetBits(GPIOA, HT1621_CS);
    Ht1621Wr_Data1(0xa0, 3);      //д�����ݱ�־101
    Ht1621Wr_Data1(Addr << 2, 6); //д���ַ����
    Ht1621Wr_Data1(Data << 4, 4); //д������
    GPIO_SetBits(GPIOA, HT1621_CS);
    //SysTick_Delay_us(HtDelay);
}


/******************************************************
д���ݺ���,cntΪ��������λ��,���ݴ���Ϊ��λ��ǰ
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
*	LCD ��������
*	���:void
*	����:void
*/
void lcd_clr(void)
{
    write_addr_dat_n(0x0, 0x00, 15);
}

/*
*	LCD ȫ��ʾ����
*	���:void
*	����:void
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

void fengsu_gaosu()//��ʾ����
{
    //write_1621(0x01,0x08);
    //lcd_clr();
    Ht1621WrOneData(0x02, 0x0C);
}

void fengsu_zhongsu()//��ʾ����
{
    Ht1621WrOneData(0x02, 0x0A);
}
void fengsu_disu()//��ʾ����
{
    Ht1621WrOneData(0x02, 0x09);
}
void shedingwendu()//��ʾ�趨�¶�
{
    Ht1621WrOneData(0x06, 0x01);
}
void shineiwendu()//��ʾ�����¶�
{
    Ht1621WrOneData(0x0A, 0x01);
}

void moshi_lengfeng()//��ʾ���
{
    Ht1621WrOneData(0x00, 0X0C);
}
void moshi_nuanfeng()//��ʾů��
{
    Ht1621WrOneData(0x00, 0X0A);
}
void qingmoshi()//���ģʽ
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






