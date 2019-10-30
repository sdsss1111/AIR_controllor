#include "stm32f10x.h"
#include "../inc/key.h"
#include "../inc/sys.h"
//#include "delay.h"
#include "../inc/lcd.h"
//#include "../inc/app_1278.h"
#include "bsp.h"

void Settemp_up(u16 Settemp)
{
    extern u8 up_buff[6];


    switch (Settemp)
    {
    case 16:
        up_buff[2] = 0x10;
        break;
    case 17:
        up_buff[2] = 0x11;
        break;
    case 18:
        up_buff[2] = 0x12;
        break;
    case 19:
        up_buff[2] = 0x13;
        break;
    case 20:
        up_buff[2] = 0x14;
        break;
    case 21:
        up_buff[2] = 0x15;
        break;
    case 22:
        up_buff[2] = 0x16;
        break;
    case 23:
        up_buff[2] = 0x17;
        break;
    case 24:
        up_buff[2] = 0x18;
        break;
    case 25:
        up_buff[2] = 0x19;
        break;
    case 26:
        up_buff[2] = 0x1a;
        break;
    case 27:
        up_buff[2] = 0x1b;
        break;
    case 28:
        up_buff[2] = 0x1c;
        break;
    case 29:
        up_buff[2] = 0x1d;
        break;
    case 30:
        up_buff[2] = 0x1e;
        break;
    }


}

void Gettemp_up(u16 Gettemp)
{
    switch (Gettemp)
    {
    case 16:
        up_buff[3] = 0x10;
        break;
    case 17:
        up_buff[3] = 0x11;
        break;
    case 18:
        up_buff[3] = 0x12;
        break;
    case 19:
        up_buff[3] = 0x13;
        break;
    case 20:
        up_buff[3] = 0x14;
        break;
    case 21:
        up_buff[3] = 0x15;
        break;
    case 22:
        up_buff[3] = 0x16;
        break;
    case 23:
        up_buff[3] = 0x17;
        break;
    case 24:
        up_buff[3] = 0x18;
        break;
    case 25:
        up_buff[3] = 0x19;
        break;
    case 26:
        up_buff[3] = 0x1a;
        break;
    case 27:
        up_buff[3] = 0x1b;
        break;
    case 28:
        up_buff[3] = 0x1c;
        break;
    case 29:
        up_buff[3] = 0x1d;
        break;
    case 30:
        up_buff[3] = 0x1e;
        break;
    }

}


void ChushiHuashedingwendu()
{
    extern u8 Ggewei, Sshiwei, shedingwenduzhi;
    Sshiwei = shedingwenduzhi / 10;
    Ggewei = shedingwenduzhi % 10;
    if(Ggewei == 0)
    {
        Sshineigewei0();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 1)
    {
        Sshineigewei1();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 2)
    {
        Sshineigewei2();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 3)
    {
        Sshineigewei3();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 4)
    {
        Sshineigewei4();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 5)
    {
        Sshineigewei5();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 6)
    {
        Sshineigewei6();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 7)
    {
        Sshineigewei7();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }
    if(Ggewei == 8)
    {
        Sshineigewei8();
        if(Sshiwei == 3)Sshineishiwei3();
        else Sshineishiwei2();
    }
    if(Ggewei == 9)
    {
        Sshineigewei9();
        if(Sshiwei == 2)
        {
            Sshineishiwei2();
        }
        else Sshineishiwei3();
    }

}

void wendujiaxianshi(u8 a)
{
  extern u8 shiwei,gewei;
                       //a += 1;
                Sshiwei = a / 10;
                Ggewei = a % 10;
                if(Ggewei == 0)
                {
                    Sshineigewei0();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 1)
                {
                    Sshineigewei1();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 2)
                {
                    Sshineigewei2();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 3)
                {
                    Sshineigewei3();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 4)
                {
                    Sshineigewei4();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 5)
                {
                    Sshineigewei5();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 6)
                {
                    Sshineigewei6();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3) Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 7)
                {
                    Sshineigewei7();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3) Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 8)
                {
                    Sshineigewei8();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)
                    {
                        Sshineishiwei3();
                    }
                    else Sshineishiwei1();
                }
                if(Ggewei == 9)
                {
                    Sshineigewei9();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3) Sshineishiwei3();
                    else Sshineishiwei1();
                }    

}


void wendujianxianshi(u8 a)
{
  extern u8 shiwei,gewei;
                  //a--;
                Sshiwei = a / 10;
                Ggewei = a % 10;
                if(Ggewei == 0)
                {
                    Sshineigewei0();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 1)
                {
                    Sshineigewei1();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 2)
                {
                    Sshineigewei2();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 3)
                {
                    Sshineigewei3();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 4)
                {
                    Sshineigewei4();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 5)
                {
                    Sshineigewei5();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 6)
                {
                    Sshineigewei6();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 7)
                {
                    Sshineigewei7();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 8)
                {
                    Sshineigewei8();
                    if(Sshiwei == 2)Sshineishiwei2();
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }
                if(Ggewei == 9)
                {
                    Sshineigewei9();
                    if(Sshiwei == 2)
                    {
                        Sshineishiwei2();
                    }
                    else if(Sshiwei == 3)Sshineishiwei3();
                    else Sshineishiwei1();
                }


}
void Getwendu(u8 a)
{
  extern u8 shiwei,gewei;
    shiwei = a / 10;
    gewei = a % 10;
    if(gewei == 0)
    {
        shineigewei0();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 1)
    {
        shineigewei1();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 2)
    {
        shineigewei2();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 3)
    {
        shineigewei3();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 4)
    {
        shineigewei4();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 5)
    {
        shineigewei5();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 6)
    {
        shineigewei6();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 7)
    {
        shineigewei7();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 8)
    {
        shineigewei8();
        if(shiwei == 2)shineishiwei2();
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }
    if(gewei == 9)
    {
        shineigewei9();
        if(shiwei == 2)
        {
            shineishiwei2();
        }
        else if(shiwei == 3) shineishiwei3();
        else shineishiwei1();
    }

}