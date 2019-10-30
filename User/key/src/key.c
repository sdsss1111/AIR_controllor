#include "stm32f10x.h"
#include "../inc/key.h"
#include "../inc/sys.h"
//#include "delay.h"
#include "../inc/lcd.h"
//#include "../inc/app_1278.h"
#include "bsp.h"

extern OS_EVENT *RF_Send_Sem; /*创建一个信号量用于无线接收*/
//extern u8 fengsuxunhuan;
//extern u8 moshixunhuan;
u8 key = 0;
u8 fengsuxunhuan = 0;
u8 moshixunhuan = 1;
u8 kaiguanxunhuan = 0;
u8 flag_kaiguan;
u8 up_guanji[1] = {0x00};
u8 up_kaiji[1] = {0x01};
u8 up_gaosu[1] = {0x02};
u8 up_zhongsu[1] = {0x03};
u8 up_disu[1] = {0x04};
u8 up_buff[6];
u8 temp;
u8 shedingwenduzhi = 26;
u8 shiwei;
u8 gewei;
u8 Sshiwei;
u8 Ggewei;
static u8 count = 0;
static u32 shifenzhongcount = 0;
u8 zidong_flag=0;
//按键初始化函数
void KEY_Init(void) //IO初始化
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin  = key_kaiguan_pin | key_wendujia_pin | key_fengsu_pin | key_wendujian_pin | key_moshi_pin ;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;           //按键设置成上拉输入
    GPIO_Init(key_port, &GPIO_InitStructure);              //初始化按键端口



    /**********************************控制初始化********************************************/
    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_13;             //控制阀门开关继电器
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;        //设置成复用输出
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);


    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;       //高中低三速继电器
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

}
//按键处理函数
//返回按键值
//mode:0,不支持连续按;1,支持连续按;
//0，没有任何按键按下
//1，KEY0按下
//2，KEY1按下
//3，KEY2按下
//4，KEY3按下 WK_UP
//注意此函数有响应优先级,KEY0>KEY1>KEY2>KEY3!!

u8 KEY_Scan(u8 mode)
{

    static u8 key_up = 1; //按键按松开标志
    if(mode)key_up = 1; //支持连按
    if(key_up && (key_kaiguan == 0 || key_wendujia == 0 || key_fengsu == 0 || key_wendujian == 0 || key_moshi == 0))
    {
        bsp_DelayUS(50000);//去抖动
        key_up = 0;
        if(key_kaiguan == 0)return fanhuikey_kaiguan;
        else if(key_wendujia == 0)return fanhuikey_wendujia;
        else if(key_fengsu == 0)return fanhuikey_fengsu;
        else if(key_wendujian == 0)return fanhuikey_wendujian;
        else if(key_moshi == 0)return fanhuikey_moshi;

    }
    else if(key_kaiguan == 1 && key_wendujia == 1 && key_fengsu == 1 && key_wendujian == 1 && key_moshi == 1) key_up = 1;
    return 0;// 无按键按下
}
void KEY_CAOZUO()
{
    u8 shiwei, gewei, temp;
    u16 wendu, temperature;
    u8 nuanfeng_flag;
    u8 lengfeng_flag;
    temperature = DS18B20_Get_Temp();
    wendu = temperature - 30;
    //wendu = temperature;
    if(wendu < 0)
    {

        wendu = -wendu;					//如果温度为负值，则取反，一般用不到
    }
    else
        temp = wendu / 10;
    //temp=22;
    Getwendu(temp);
  


    key = KEY_Scan(0);	    //得到键值

    switch(key)
    {
    case fanhuikey_kaiguan:  	  //控制开关
    {
        GPIO_SetBits(GPIOB, GPIO_Pin_0);
        count = 0;
        kaiguanxunhuan = !kaiguanxunhuan;
        if(kaiguanxunhuan == 0)   //关机
        {
            flag_kaiguan = 0;
            GPIO_ResetBits(GPIOB, GPIO_Pin_0);
            lcd_clr();
            shineiwendu();
            CLOUSE;
  

            up_buff[0] = 0x00;
      

            switch(fengsuxunhuan % 3)
            {
            case 0:
                up_buff[4] = 0x01;
                break;
            case 1:
                up_buff[4] = 0x03;
                break;
            case 2:
                up_buff[4] = 0x02;
                break;
            }
            if(moshixunhuan == 1)
            {
                up_buff[1] = 0x02;
            }
            else
            {
                up_buff[1] = 0x01;
            }
            Settemp_up(shedingwenduzhi);
            Gettemp_up(temp);
           
            memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff)
            OSSemPost(RF_Send_Sem);

        }
        if(kaiguanxunhuan == 1)     //开机低速运行
        {
           // if(nuanfeng_flag == 0)
          //  {
                flag_kaiguan = 1;
                shineiwendu();
                ChushiHuashedingwendu();
                up_buff[0] = 0x01;

                fengsu_disu();
                if(zidong_flag==1){
                zidong_flag=0;
                LOW_SPEED;              //低速继电器跳
                }
                up_buff[4] = 0x01;

                if(moshixunhuan == 1)
                {
                    moshi_nuanfeng();
                    up_buff[1] = 0x02;
                }
                else
                {
                    moshi_lengfeng();
                    up_buff[1] = 0x01;
                }


                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                

                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);

           // }
        }
        break;
    }

    case fanhuikey_fengsu:  	   //控制风速
    {

        if(flag_kaiguan == 1)
        { 
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            count = 0;
            fengsuxunhuan++;
            up_buff[0] = 0x01;
            if(fengsuxunhuan % 3 == 1)   //高速
            {   
              if(zidong_flag==1){
                zidong_flag=0;
                HIGH_SPEED;//高速继电器跳
              }
                fengsu_gaosu();
                up_buff[4] = 0x03;
                if(moshixunhuan == 1)
                {
                    up_buff[1] = 0x02;
                }
                else
                {
                    up_buff[1] = 0x01;
                }
                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);
            }
            if(fengsuxunhuan % 3 == 2)
            {
              if(zidong_flag==1){
                zidong_flag=0;
                MID_SPEED;     //中速继电器跳
              }
                fengsu_zhongsu();
                up_buff[4] = 0x02;
                if(moshixunhuan == 1)
                {
                    up_buff[1] = 0x02;
                }
                else
                {
                    up_buff[1] = 0x01;
                }
                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);
            }
            if(fengsuxunhuan % 3 == 0)
            {
              if(zidong_flag==1){
                zidong_flag=0;
                LOW_SPEED;            //低速继电器跳
              }
                fengsu_disu();
                up_buff[4] = 0x01;
                if(moshixunhuan == 1)
                {
                    up_buff[1] = 0x02;
                }
                else
                {
                    up_buff[1] = 0x01;
                }
                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);
            }
        }
        break;
    }
    case fanhuikey_wendujian:  	//同时控制温度减
    {
        up_buff[1] = 0x01;
        if(flag_kaiguan == 1)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            count = 0;
           
            if(shedingwenduzhi != 16)
            {
              shedingwenduzhi--;
              wendujianxianshi(shedingwenduzhi);//控制温度减少显示
             

            }
            else
            {
                Sshineishiwei1();
                Sshineigewei6();
               
            }
             Settemp_up(shedingwenduzhi);
              Gettemp_up(temp);
          
            switch(fengsuxunhuan % 3)
            {
            case 0:
                up_buff[4] = 0x01;
                break;
            case 1:
                up_buff[4] = 0x03;
                break;
            case 2:
                up_buff[4] = 0x02;
                break;
            }
            memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
            OSSemPost(RF_Send_Sem);

        }
        break;
    }
    case fanhuikey_wendujia:  	//控制温度加
    {

        up_buff[1] = 0x01;
        if(flag_kaiguan == 1)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            count = 0;
         
            if(shedingwenduzhi != 30)
            {
              shedingwenduzhi++;
              wendujiaxianshi(shedingwenduzhi);         //控制温度加显示
             
            }
            else
            {
                
                Sshineishiwei3();
                Sshineigewei0();
               

            }
             Settemp_up(shedingwenduzhi);
              Gettemp_up(temp);
           
            switch(fengsuxunhuan % 3)
            {
            case 0:
                up_buff[4] = 0x01;
                break;
            case 1:
                up_buff[4] = 0x03;
                break;
            case 2:
                up_buff[4] = 0x02;
                break;
            }

            memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
            OSSemPost(RF_Send_Sem);




        }
        break;
    }
    case fanhuikey_moshi:  	//同时控制模式
    {
        up_buff[0] = 0x01;
        if(flag_kaiguan == 1)
        {
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            count = 0;

            moshixunhuan = !moshixunhuan;
            if(moshixunhuan == 1)
            {
//                if(temp - shedingwenduzhi > 1)
//                {
//                    nuanfeng_flag = 1;
//                }
                moshi_nuanfeng();
                up_buff[1] = 0x02;
                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                
                switch(fengsuxunhuan % 3)
                {
                case 0:
                    up_buff[4] = 0x01;
                    break;
                case 1:
                    up_buff[4] = 0x03;
                    break;
                case 2:
                    up_buff[4] = 0x02;
                    break;
                }
                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);
            }
            if(moshixunhuan == 0)
            {
                moshi_lengfeng();
                up_buff[1] = 0x01;
                Settemp_up(shedingwenduzhi);
                Gettemp_up(temp);
                
                switch(fengsuxunhuan % 3)
                {
                case 0:
                    up_buff[4] = 0x01;
                    break;
                case 1:
                    up_buff[4] = 0x03;
                    break;
                case 2:
                    up_buff[4] = 0x02;
                    break;
                }
                memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
                OSSemPost(RF_Send_Sem);
            }
        }
        break;
    }
    default:
            {
                count++;
                if(count >= 250)
                {
                    count = 250;
                    GPIO_ResetBits(GPIOB, GPIO_Pin_0);
        
                }
        
            }
        break;


    }
//    if(nuanfeng_flag == 1)
//    {
//        CLOUSE;
//        if(shedingwenduzhi > temp)
//        {
//            nuanfeng_flag = 0;
//        }
//
//    }



    //    shifenzhongcount++;
    //    if(shifenzhongcount == 600000)
    //    {
    //        shifenzhongcount = 0;
    //        memcpy(test_data, up_buff, aircontroller_datalen); //sizeof(up_buff));
    //        OSSemPost(RF_Send_Sem);
    //
    //    }

/*******************************************设定温度暖风自动停止***************************************************/

    if((moshixunhuan==1)&&(temp-shedingwenduzhi>=2))
    {
        CLOUSE;
    }
    else
    {
      if(temp<=shedingwenduzhi){
      zidong_flag=1;
      if(flag_kaiguan ==1)
      {
         switch(fengsuxunhuan % 3)
            {
            case 0:
                fengsu_disu();
                LOW_SPEED;
                break;
            case 1:
                fengsu_gaosu();
                HIGH_SPEED;
                break;
            case 2:
                fengsu_zhongsu();
                MID_SPEED;
                break;
            }
        
      }
    }
    }
    
/*******************************************设定温度暖风自动停止***************************************************/
/*******************************************设定温度冷风自动停止***************************************************/

    if((moshixunhuan==0)&&(shedingwenduzhi-temp>=2))
    {
        CLOUSE;
    }
    else
    {
      if(shedingwenduzhi<=temp){
      zidong_flag=1;
      if(flag_kaiguan ==1)
      {
         switch(fengsuxunhuan % 3)
            {
            case 0:
                fengsu_disu();
                LOW_SPEED;
                break;
            case 1:
                fengsu_gaosu();
                HIGH_SPEED;
                break;
            case 2:
                fengsu_zhongsu();
                MID_SPEED;
                break;
            }
      }
      }
      
    }
    
/*******************************************设定温度冷风自动停止***************************************************/
}





enum CTL_RUN
{
    on  = 1,
    off = 0,

};
enum CTL_MODE
{
    lengfeng  = 1,
    naunfeng = 2,
};
enum CTL_SPEED
{
    gaosu    = 3,
    zhongsu  = 2,
    disu     = 1,
};

enum CTL_TEMP
{
    set_16  = 0x10,
    set_17  = 0x11,
    set_18  = 0x12,
    set_19  = 0x13,
    set_20  = 0x14,
    set_21  = 0x15,
    set_22  = 0x16,
    set_23  = 0x17,
    set_24  = 0x18,
    set_25  = 0x19,
    set_26  = 0x1a,
    set_27  = 0x1b,
    set_28  = 0x1c,
    set_29  = 0x1d,
    set_30  = 0x1e,

};

//typedef union
//{
//  uint8_t data;
//  struct
//  {
//    uint8_t on_off   : 1; //1:on, 0:off
//    uint8_t mode     : 2;
//    uint8_t reserved : 5;
//
//  }bit;
//
//}air_state;


enum AirCMD_Type
{
    CTL_NONE = 0,//on_off_state;//;air_state data
    CTL_MODE = 1,//work_state;
    CTL_SPEED = 2,//set_temp;
    CTL_TEMP = 3,//cloud_v;
    CTL_RUN = 4,//cloud_dri;
};

struct AirControler_Receive
{
    uint8_t type;
    uint8_t value;
};







result_t prase_aircontrol(uint8_t *tempbuff)//struct AirControler_Receive *temp
{
    result_t result = sx127x_FAILED;
    // uint8_t tempbuff[5] = {0x04, 0x01, 0x03, 0x04, 0x05};
    struct AirControler_Receive *Recv_Cmd = (struct AirControler_Receive *) tempbuff;

    switch(Recv_Cmd->type)
    {
    case CTL_RUN:
    {
        if(Recv_Cmd->value == on)
        {
          kaiguanxunhuan=1;
            flag_kaiguan = 1;
            GPIO_SetBits(GPIOB, GPIO_Pin_0);
            shineiwendu();
            ChushiHuashedingwendu();
            
            up_buff[0] = 0x01;


            fengsu_disu();
            LOW_SPEED;                  //低速继电器跳
            GPIO_ResetBits(GPIOB, GPIO_Pin_13);
            GPIO_SetBits(GPIOA, GPIO_Pin_8);
            GPIO_SetBits(GPIOA, GPIO_Pin_9);
            up_buff[4] = 0x01;

            if(moshixunhuan == 1)
            {
                moshi_nuanfeng();
                // up_buff[1] = 0x02;
            }
            else
            {
                moshi_lengfeng();
                // up_buff[1] = 0x01;
            }
            result = sx127x_SUCCESS;
        }
        else
        {
            flag_kaiguan = 0;
            kaiguanxunhuan=0;
            GPIO_ResetBits(GPIOB, GPIO_Pin_0);
            lcd_clr();
            shineiwendu();
            GPIO_SetBits(GPIOB, GPIO_Pin_13);
            GPIO_ResetBits(GPIOA, GPIO_Pin_8);
            GPIO_ResetBits(GPIOA, GPIO_Pin_9);
            up_buff[0] = 0x00;
            result = sx127x_SUCCESS;
        }
        Recv_Cmd->type=5;
        break;

    }
    case CTL_MODE:
    {
        if(Recv_Cmd->value == naunfeng)
        {
            moshi_nuanfeng();
            up_buff[1] = 0x02;
        }
        else
        {
            moshi_lengfeng();
            up_buff[1] = 0x01;
        }
        Recv_Cmd->type=5;
        break;
    }
    case CTL_SPEED:
    {
        switch(Recv_Cmd->value)
        {
        case gaosu:
        {
          fengsuxunhuan=1;
            HIGH_SPEED;
            fengsu_gaosu();
            up_buff[4] = 0x03;
            
            break;
        }
        case zhongsu:
        {
          fengsuxunhuan=2;
            MID_SPEED;

            fengsu_zhongsu();
            up_buff[4] = 0x02;


            break;
        }
        case disu:
        {
          fengsuxunhuan=0;
            LOW_SPEED;
             fengsu_disu();
            up_buff[4] = 0x01;
           
            break;
        }
        }
        Recv_Cmd->type=5;
        break;
    }
    case CTL_TEMP:
    {
        switch (Recv_Cmd->value)
        {
        case set_16:
            Sshineishiwei1();
            Sshineigewei6();
            up_buff[2] = 0x10;
            break;
        case set_17:
            Sshineishiwei1();
            Sshineigewei7();
            up_buff[2] = 0x11;
            break;
        case set_18:
            Sshineishiwei1();
            Sshineigewei8();
            up_buff[2] = 0x12;
            break;
        case set_19:
            Sshineishiwei1();
            Sshineigewei9();
            up_buff[2] = 0x13;
            break;
        case set_20:
            Sshineishiwei2();
            Sshineigewei0();
            up_buff[2] = 0x14;
            break;
        case set_21:
            Sshineishiwei2();
            Sshineigewei1();
            up_buff[2] = 0x15;
            break;
        case set_22:
            Sshineishiwei2();
            Sshineigewei2();
            up_buff[2] = 0x16;
            break;
        case set_23:
            Sshineishiwei2();
            Sshineigewei3();
            up_buff[2] = 0x17;
            break;
        case set_24:
            Sshineishiwei2();
            Sshineigewei4();
            up_buff[2] = 0x18;
            break;
        case set_25:
            Sshineishiwei2();
            Sshineigewei5();
            up_buff[2] = 0x19;
            break;
        case set_26:
            Sshineishiwei2();
            Sshineigewei6();
            up_buff[2] = 0x1a;
            break;
        case set_27:
            Sshineishiwei2();
            Sshineigewei7();
            up_buff[2] = 0x1b;
            break;
        case set_28:
            Sshineishiwei2();
            Sshineigewei8();
            up_buff[2] = 0x1c;
            break;
        case set_29:
            Sshineishiwei2();
            Sshineigewei9();
            up_buff[2] = 0x1d;
            break;
        case set_30:
            Sshineishiwei3();
            Sshineigewei0();
            up_buff[2] = 0x1e;
            break;
        }
        Recv_Cmd->type=5;
        break;
    }


    default:
        break;


    }



    memcpy(test_data, up_buff, sizeof(up_buff));
    OSSemPost(RF_Send_Sem);  // 此处添加打开代码
}

