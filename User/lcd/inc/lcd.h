#ifndef __LCD__
#define __LCD__
#include "../inc/sys.h"
//HT1621
#define HT1621_CS		GPIO_Pin_12
#define HT1621_WR		GPIO_Pin_11
#define HT1621_DATA		GPIO_Pin_10
//#define HT1621_IRQ		GPIO_Pin_7
//#define HT1621_RD		GPIO_Pin_1
#define LCD_TIME		1500	//15s背光熄灭

#define DAT 1	//数据
#define COMMAND 0	//命令
/*********************************************************************************/
#define ADR1_shuzi3 write_1621(3,0x0F);write_1621(2,0x09);// 消除L    0
#define ADR1_shuzi2 write_1621(3,0x07);write_1621(2,0x0D);
#define ADR1_shuzi9 write_1621(0x09,0x0F);write_1621(0x08,0x0B);write_1621(0x01,0x00);

/****************************************************************************************/








/*
*       温度      水分      湿度
*       0xAF      0xF5      0x5F    |     0
*       0xA0      0x05      0x50    |     1
*       0xCB      0xB6      0x3D    |     2
*       0xE9      0x97      0x79    |     3
*       0xE4      0x47      0x72    |     4
*       0x6D      0xD3      0x6B    |     5
*       0x6F      0xF3      0x6F    |     6
*       0xA8      0x85      0x51    |     7
*       0xEF      0xF7      0x7F    |     8
*       0xED      0xD7      0x7B    |     9
*
*       +0x10    +0x08      +0x80
*/

typedef enum
{
    Temperature,	//温度
    Water,		//水分
    Humidity,	//湿度
} lcd_type;

extern int bLcd_flag;
extern int bLcd_timer;

///////////////////////////////////////////////////驱动函数
void write_mode(unsigned char MODE);
void write_command(unsigned char Cbyte);
void write_address(unsigned char Abyte);
void write_data_8bit(unsigned char Dbyte);
void write_data_4bit(unsigned char Dbyte);

//内部使用
void write_addr_dat_n(unsigned char _addr, unsigned char _dat, unsigned char n);

///////////////////////////////////////////////////
//接口函数
void ht1621_init(void); //必须先初始化主控制器连接ht1621的GPIO
void lcd_init(void);    //初始化lcd
void lcd_clr(void);     //清屏
void lcd_all(void);     //全显
void display_off();		//显示OFF

//温度
void temperature_all(void);    //温度全显
void temperature_clr(void);    //温度清屏
void temperature_display(const char *_val);    //显示温度

//水分
void water_all(void);    //水分全显
void water_clr(void);    //水分清屏
void water_display(const char *_val);    //显示水分

//湿度
void humidity_all(void);    //湿度全显
void humidity_clr(void);    //湿度清屏
void humidity_display(const char *_val);    //显示湿度

/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/
void Ht1621Wr_Data1(uint8_t Data, uint8_t cnt);
void Ht1621WrOneData(uint8_t Addr, uint8_t Data);
/**+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++*/

void write_1621(unsigned char addr, unsigned char dat);
void write_1621t(unsigned char addr, unsigned char addr2, unsigned char dat, unsigned char dat2);
void fengsu_gaosu(void);
void fengsu_zhongsu(void);
void fengsu_disu(void);
void fengsu(void);
void shedingwendu(void);
void moshi(void);
void moshi_lengfeng(void);
void moshi_nuanfeng(void);
void shineiwendu(void);
void qingmoshi(void);
void shineigewei9(void);
void shineigewei7(void);
void shineigewei8(void);
void shineigewei6(void);
void shineigewei5(void);
void shineigewei4(void);
void shineigewei3(void);
void shineigewei2(void);
void shineigewei1(void);
void shineigewei0(void);
void shineishiwei3(void);
void shineishiwei2(void);
void shineishiwei1(void);
void Sshineigewei9(void);
void Sshineigewei7(void);
void Sshineigewei8(void);
void Sshineigewei6(void);
void Sshineigewei5(void);
void Sshineigewei4(void);
void Sshineigewei3(void);
void Sshineigewei2(void);
void Sshineigewei1(void);
void Sshineigewei0(void);
void Sshineishiwei3(void);
void Sshineishiwei2(void);
void Sshineishiwei1(void);
#endif //__LCD__

