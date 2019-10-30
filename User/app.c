/*
*********************************************************************************************************
*
*	模块名称 : uCOS-II
*	文件名称 : app.c
*	版    本 : V1.0
*	说    明 :
                1. 共创建了如下几个任务，通过串口打印任务堆栈使用情况
*                   优先级   使用栈  剩余栈  百分比   任务名
*                    Prio     Used    Free    Per      Taskname
*                     63       17      111    13%     uC/OS-II Idle
*                     62       21      107    16%     uC/OS-II Stat
*                      0       34      222    13%     Start Task
*                      3       27      229    10%     COM
*
*                2. (1) 凡是用到 printf 函数的全部通过函数 App_Printf 实现。
*                   (2) App_Printf 函数做了信号量的互斥操作，解决资源共享问题。
*
*              注意事项：
*
*------------------------------------------------------------------------------------------------------
*	修改记录 :
*		版本号   日期         作者            说明
*                V1.0   2017-08-02   pigpeng    1. ST固件库到V3.6.1版本
*                                               2. BSP驱动包V1.2
*                                               3. uCOS-II版本V2.92.11
*                                               4. uC/CPU版本V1.30.02
*                                               5. uC/LIB版本V1.38.01
*
*	Copyright (C),
*
*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              任务堆栈
********************************************************************************************************
*/

/* 定义每个任务的堆栈空间，app_cfg.h文件中宏定义栈大小 */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
//static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskCOMStk[APP_TASK_COM_STK_SIZE];
static OS_STK AppTaskSENDStk[APP_TASK_SEND_STK_SIZE];
static OS_STK AppTaskRECVStk[APP_TASK_RECV_STK_SIZE];

/*
*******************************************************************************************************
*                              函数声明
*******************************************************************************************************
*/
static void AppTaskCreate(void);
static void AppTaskStart(void *p_arg);
//static void AppTaskUserIF(void *p_arg);
//static void DispTaskInfo(void);
static void AppTaskCom(void *p_arg);
static void AppTaskSend(void *p_arg);
static void AppTaskRecv(void *p_arg);

/*
*******************************************************************************************************
*                               变量
*******************************************************************************************************
*/
void app_rfReceiveCb(uint8_t *buf, uint8_t len, int8_t rssi);//RF 接收回调
void User_TimerISR(); //40ms定时中断回调
static uint16_t  BS_OffLine_Counter = 0;
static uint8_t datalen;
u8 gReceiveFlag = 0; //DIO0中断标志
//extern uint8_t  gRfBuffer[];

//result_t app_rfSendCb(struct framer *pframer, const uint8_t );

//static OS_EVENT *AppUserIFMbox; /* 定义一个邮箱， 这只是一个邮箱指针， OSMboxCreate函数会创建邮箱必需的资源 */

//static OS_EVENT *AppPrintfSemp;/*创建一个信号量用于打印信息*/

OS_EVENT *RF_Recv_Sem; /*创建一个信号量用于无线接收*/
OS_EVENT *RF_Send_Sem; /*创建一个信号量用于无线接收*/

//--------------------------------------------------//
uint8_t g_flag_test = 0;

struct FRAMER *framer = NULL;

uint8_t dnld_data_buff[LORALAN_FRAME_MAX_LEN];
uint8_t test_data[25] = {0x01, 0x01, 0x11, 0x00, 0x00, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15};

#ifdef EndDevisSensor
extern uint8_t up_data[];
void  Sensor_Sample();
const uint8_t datalen = 4;
#endif


/*
*******************************************************************************************************
*	函 数 名: Sensor_Sample
*	功能说明: 传感器终端模块，采样回调函数
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
#ifdef EndDevisSensor
void Sensor_Sample()
{
    static uint16_t Sensor_Samp_Counter = 0;
    Sensor_Samp_Counter++;
    if(Sensor_Samp_Counter == Sample_Cyc_S)
    {
        Read_sht30();      //读取SHT30数据
        Convert_sht30();   //数据转换处理
        memcpy(test_data, up_data, 4);
        Sensor_Samp_Counter = 0;
    }
}
#endif

/*
*******************************************************************************************************
*	函 数 名: app_rfReceiveCb
*	功能说明: sx127x接收回调
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
void app_rfReceiveCb(uint8_t *buf, uint8_t len, int8_t rssi)
{
    gReceiveFlag = 1;
}

/*
*******************************************************************************************************
*	函 数 名: User_TimerISR
*	功能说明: 定时器中断回调，用于检测基站是否在线
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
void User_TimerISR()
{
    BS_OffLine_Counter++;
    if(BS_OffLine_Counter == 6000)
    {
        // BS_OffLine_Flag = 1;
        FSM_LoRa_Type = LORALAN_NETWORK_DISCONNECT;
        BS_OffLine_Counter = 0;
    }
}

/*
*******************************************************************************************************
*	函 数 名: main
*	功能说明: 标准c程序入口。
*	形    参: 无
*	返 回 值: 无
*******************************************************************************************************
*/
int main(void)
{
    INT8U  err;
    //static u8 up_buff[6] = {0x00,};

    OSInit(); /* 初始化"uC/OS-II"内核 */

    /* 创建一个启动任务（也就是主任务）。启动任务会创建所有的应用程序任务 */
    OSTaskCreateExt(AppTaskStart,	 /* 启动任务函数指针 */
                    (void *)0,		 /* 传递给任务的参数 */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],    /* 指向任务栈栈顶的指针 */
                    APP_TASK_START_PRIO,	   /* 任务的优先级，必须唯一，数字越低优先级越高 */
                    APP_TASK_START_PRIO,	   /* 任务ID，一般和任务优先级相同 */
                    (OS_STK *)&AppTaskStartStk[0], /* 指向任务栈栈底的指针。OS_STK_GROWTH 决定堆栈增长方向 */
                    APP_TASK_START_STK_SIZE,       /* 任务栈大小 */
                    (void *)0,	   /* 一块用户内存区的指针，用于任务控制块TCB的扩展功能
                                   （如任务切换时保存CPU浮点寄存器的数据）。一般不用，填0即可 */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* 任务选项字 */
    /*  定义如下：
        OS_TASK_OPT_STK_CHK  使能检测任务栈，统计任务栈已用的和未用的
        OS_TASK_OPT_STK_CLR  在创建任务时，清零任务栈
        OS_TASK_OPT_SAVE_FP  如果CPU有浮点寄存器，则在任务切换时保存浮点寄存器的内容
    */
    OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);/* 指定任务的名称，用于调试。这个函数是可选的 */

    OSTimeSet(0);/*ucosII的节拍计数器清0    节拍计数器是0-4294967295*/

    OSStart(); /* 启动多任务系统，控制权交给uC/OS-II */

    while(1)  /* 系统启动正常时，不会运行至此 */
    {
        //App_Printf("-- system failed start!-- \r\n"); //系统启动失败，打印信息
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskStart
*	功能说明: 这是一个启动任务，在多任务系统启动后，必须初始化滴答计数器(在BSP_Init中实现)
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
    (void)p_arg;/* 仅用于避免编译器告警，编译器不会产生任何目标代码 */
    //    INT8U err;

    bsp_Init();   /* BSP 初始化。 BSP = Board Support Package 板级支持包，可以理解为底层驱动。*/
    CPU_Init();
    BSP_Tick_Init(); /*BSP时钟初始化*/

    bsp_InitTimer();  /*软件定时器初始化*/
    bsp_StartTimer(0, 1); /* 启动定时器，1ms 定时*/
    bsp_StartHardTimer(2, 50, (void *)User_TimerISR);

#ifdef EndDevisSensor
    IIC_Init();
    bsp_StartHardTimer(3, 50, (void *)Sensor_Sample);

#endif


#ifdef EndDevisAircontroler
    ht1621_init();
    DS18B20_Init();
    lcd_init();
    lcd_clr();
    KEY_Init();
    GPIO_ResetBits(GPIOA, GPIO_Pin_8);
    GPIO_ResetBits(GPIOB, GPIO_Pin_13);
    GPIO_ResetBits(GPIOA, GPIO_Pin_9);
    ChushiHuashedingwendu();


    bsp_StartHardTimer(4, 40, (void *)KEY_CAOZUO);

    datalen = aircontroller_datalen;//
#endif

    FSM_LoRa_Type = LORALAN_NETWORK_DISCONNECT;
    rf_init(app_rfReceiveCb);      /*  sx127x 初始化  */
    rf_setChannel(sx127x_FREQ_RX);/* 设置为默认接收信道 sx127x_FREQ_RX */
    rf_receiveOn();

    AppTaskCreate();  /* 创建应用程序的任务 */

#if (OS_TASK_STAT_EN > 0) /* 检测CPU能力，统计模块初始化。该函数将检测最低CPU占有率 */
    OSStatInit();
#endif

    //   err = OSTaskDel(OS_PRIO_SELF); /* 任务启动完成，挂起 */
    //   if(err == OS_ERR_NONE)
    //     App_Printf("-- the start task is suspended!-- \r\n"); //系统启动失败，打印信息

}


/*
*********************************************************************************************************
*	函 数 名: AppTaskCreate
*	功能说明: 创建应用任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    INT8U      err;

    /* 创建邮箱(MBOX) */
    //    AppUserIFMbox = OSMboxCreate((void *)0);
    //    AppPrintfSemp = OSSemCreate(1);	  /* 创建一个信号量 实现信号量互斥 */
    RF_Recv_Sem = OSSemCreate(0);       /* 创建一个信号量 用于无线接收标识*/
    RF_Send_Sem = OSSemCreate(0);       /* 创建一个信号量 用于无线接收标识*/

#if 0
    /* 创建AppTaskUserIF任务 */
    OSTaskCreateExt(AppTaskUserIF,
                    (void *)0,
                    (OS_STK *)&AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE - 1],
                    APP_TASK_USER_IF_PRIO,
                    APP_TASK_USER_IF_PRIO,
                    (OS_STK *)&AppTaskUserIFStk[0],
                    APP_TASK_USER_IF_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskNameSet(APP_TASK_USER_IF_PRIO, APP_TASK_USER_IF_NAME, &err);
#endif

#if 1
    /* 创建AppTaskLED任务 */
    OSTaskCreateExt(AppTaskSend,
                    (void *)0,
                    (OS_STK *)&AppTaskSENDStk[APP_TASK_SEND_STK_SIZE - 1],
                    APP_TASK_SEND_PRIO,
                    APP_TASK_SEND_PRIO,
                    (OS_STK *)&AppTaskSENDStk[0],
                    APP_TASK_SEND_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskNameSet(APP_TASK_SEND_PRIO, APP_TASK_SEND_NAME, &err);
#endif

    /* 创建AppTaskCom任务 */
    OSTaskCreateExt(AppTaskCom,
                    (void *)0,
                    (OS_STK *)&AppTaskCOMStk[APP_TASK_COM_STK_SIZE - 1],
                    APP_TASK_COM_PRIO,
                    APP_TASK_COM_PRIO,
                    (OS_STK *)&AppTaskCOMStk[0],
                    APP_TASK_COM_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskNameSet(APP_TASK_COM_PRIO, APP_TASK_COM_NAME, &err);

#if 1
    /* 创建AppTaskKeyScan任务 */
    OSTaskCreateExt(AppTaskRecv,
                    (void *)0,
                    (OS_STK *)&AppTaskRECVStk[APP_TASK_RECV_STK_SIZE - 1],
                    APP_TASK_RECV_PRIO,
                    APP_TASK_RECV_PRIO,
                    (OS_STK *)&AppTaskRECVStk[0],
                    APP_TASK_RECV_STK_SIZE,
                    (void *)0,
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR);

    OSTaskNameSet(APP_TASK_RECV_PRIO, APP_TASK_RECV_NAME, &err);
#endif

    //    err = OSTaskSuspend(OS_PRIO_SELF); /* 任务启动完成，挂起 */
    //    if(err == OS_ERR_NONE)
    //      App_Printf("-- the AppTaskCreate is suspended!-- \r\n"); /* 打印信息 */

}


/*
*********************************************************************************************************
*	函 数 名: AppTaskSend
*	功能说明: RF发送任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskRecv(void *p_arg)
{
    (void)p_arg;
    INT8U err;
    uint8_t recvlen;
    result_t rf_recv_result;
    result_t CMD_result = sx127x_FAILED;

    while (1) /* 任务主体，必须是一个死循环 */
    {

        OSSemPend(RF_Recv_Sem, 0, &err); /* RF 接收任务等待接收信号量 */

        rf_recv_result = Get_sx127xData();//读取 RF 数据

        if(rf_recv_result == sx127x_SUCCESS)
        {
            if((gRfBuffer[1] != 0))
                Packet_Parse(gRfBuffer, gRfLength, gRssi);
            //            dnld_data_buff[0] = 1;
            //            dnld_data_buff[1] = 1;
            CMD_result =  prase_aircontrol(dnld_data_buff);
            //  memset(dnld_data_buff,0,LORALAN_FRAME_MAX_LEN);

            if((gRfBuffer[0] == LORALAN_FRAME_TYPE_BEACON) && (gRfBuffer[1] != 0))
            {
                BS_OffLine_Counter = 0;
                recvlen = LORALAN_FRAME_LENGTH_BEACON;
                //App_Printf("-- receive the beacon --\r\n");
            }
            if(gRfBuffer[0] == LORALAN_FRAME_TYPE_JOIN_ACK)
            {
                BS_OffLine_Counter = 0;
                recvlen = LORALAN_FRAME_LENGTH_JOIN_ACK;
                // App_Printf("-- receive the join ack --\r\n");
            }
            if(gRfBuffer[0] == LORALAN_FRAME_TYPE_EVENT_ACK)
            {
                BS_OffLine_Counter = 0;
                recvlen = LORALAN_FRAME_LENGTH_EVENT_ACK;
                //App_Printf("-- receive the event ack --\r\n");
            }
            if(gRfBuffer[0] == LORALAN_FRAME_TYPE_BRIDGE_DATA)
            {
                BS_OffLine_Counter = 0;
                recvlen = 100;
                // App_Printf("-- receive the birdge data --\r\n");
            }

            if((gRfBuffer[1] != 0))
            {
                for(uint8_t i = 0; i < recvlen; i++) //RF_BUFFER_LEN
                {
                    // App_Printf(" %d ", gRfBuffer[i]);
                }
                //App_Printf("\r\n ");
            }

        }
        else
        {
            // App_Printf("%d", &err, "\r\n");
            //App_Printf("--  receiving the data is failed! --\r\n");
        }

        OSTimeDlyHMSM(0, 0, 0, 50);
    }
}


/*
*********************************************************************************************************
*	函 数 名: AppTaskSend
*	功能说明: RF发送任务
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskSend(void *p_arg)
{
    (void)p_arg;
    INT8U err;
    //DispTaskInfo();  /* 打印当前系统任务信息 */
#if 1
    //    uint8_t  local_RFSend[LORALAN_FRAME_MAX_LEN];
    //    uint8_t  Local_BuffLen;
    FSM_LoRa_Type = LORALAN_NETWORK_DISCONNECT;
    //     FSM_LoRa_Type = LORALAN_FRAME_TYPE_JOIN_REQ;


    while(1)
    {
        if( FSM_LoRa_Type == LORALAN_NETWORK_DISCONNECT) //未组网
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_SYNC_REQ; //发生sync_req

        else if( FSM_LoRa_Type == LORALAN_NETWORK_CONNECT) //组网成功，可以发送数据
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_BRIDGE_DATA;//发送数据
        else
            ;

        switch(FSM_LoRa_Type)
        {
        case LORALAN_FRAME_TYPE_SYNC_REQ:
        {
            struct ENDDEV_SYNCREQ EndDev_SyncReq = EndDev_Build_SyncReq();

            framer = (struct FRAMER *)&EndDev_SyncReq;
            framer->value[5] = EndDev_SyncReq.crc & 0xff; //crc
            framer->value[6] = (EndDev_SyncReq.crc >> 8) & 0xff;
            //        rf_setChannel(sx127x_FREQ_TX);
            break;
        }
        case LORALAN_FRAME_TYPE_JOIN_REQ:
        {
            struct ENDDEV_JOINREQ EndDev_JoinReq = EndDev_Build_JoinReq();

            framer = (struct FRAMER *)&EndDev_JoinReq;
            framer->value[11] = EndDev_JoinReq.crc & 0xff; //crc
            framer->value[12] = (EndDev_JoinReq.crc >> 8) & 0xff;
            //        rf_setChannel(sx127x_FREQ_TX);
            break;
        }
        case LORALAN_FRAME_TYPE_BRIDGE_DATA:
        {
            //         framer =  NULL;
            //          OSSemPend(RF_Send_Sem, 0, &err); /* RF 接收任务等待接收信号量 */

            struct EVENTUP EventUp_Data = EndDev_EventUp_Data(test_data, datalen, 21, 1);
            framer = (struct FRAMER *)&EventUp_Data;
            //       framer->value[(framer->value[1])-1] = i;
            uint16_t crc_result = crc16((uint8_t *)&framer->value[2], (framer->value[1]) - 2); // CRC
            framer->value[(framer->value[1])] = crc_result & 0xff;
            framer->value[(framer->value[1]) + 1] = ((crc_result >> 8) & 0xFF);

            //            uint32_t os_time = OSTimeGet();
            //            test_data[0] = ((os_time & 0xff) % 0x01);
            //            test_data[1] = ((os_time & 0xff) % 0x05);
            //            //       test_data[2] = ((os_time&0xff)%0x09)+0x10;
            //            test_data[2] = i;
            //            test_data[3] = ((os_time & 0xff) % 0x04);
            //            test_data[4] = ((os_time & 0xff) % 0x01);
            //            i++;
            //            if(i > 30) i = 17;

            break;
        }
        case LORALAN_FRAME_TYPE_BRIDGE_ACK:
        {
            struct BRAG_ACK Brige_Ack = EndDev_Build_BRGACK();
            framer = (struct FRAMER *)&Brige_Ack;
            uint16_t crc_result = crc16((uint8_t *)&framer->value[2], (framer->value[1]) - 2); // CRC
            framer->value[(framer->value[1])] = crc_result & 0xff;
            framer->value[(framer->value[1]) + 1] = ((crc_result >> 8) & 0xFF);
            FSM_LoRa_Type = LORALAN_NETWORK_CONNECT;
            break;
        }
        default:
        {
            framer =  NULL;
            break;
        }

        }


        if (framer != NULL)
        {

            if(rf_send((uint8_t *)&framer->value, framer->value[1] + 2) == sx127x_SUCCESS)
            {
#ifdef DEBUG_ERR
                //           App_Printf("--send by --\r\n");
                //           App_Printf("------------------------------------\r\n");
                if(FSM_LoRa_Type == LORALAN_FRAME_TYPE_SYNC_REQ)
                    //  App_Printf("--end device send sync req--\r\n");
                    if(FSM_LoRa_Type == LORALAN_FRAME_TYPE_JOIN_REQ)
                        // App_Printf("--end device send join req--\r\n");
                        if(FSM_LoRa_Type == LORALAN_FRAME_TYPE_BRIDGE_ACK)
                            //  App_Printf("--end device send brige ack--\r\n");
                            for(uint8_t i = 0; i < (framer->value[1] + 2); i++) //RF_BUFFER_LEN
                            {
                                // App_Printf(" %d ", framer->value[i]);
                            }
                // App_Printf("\r\n ");
#endif
            }

        }
        else
        {
            // App_Printf("------------------------------------\r\n");
            // App_Printf("the length of local_RFSend cannot be NULL! \r\n");
            rf_receiveOn();
        }
#endif

        if((FSM_LoRa_Type == LORALAN_NETWORK_CONNECT) || (FSM_LoRa_Type == LORALAN_FRAME_TYPE_BRIDGE_DATA))
            OSTimeDlyHMSM(0, 0, 15, 0);
        //  if(FSM_LoRa_Type != LORALAN_NETWORK_DISCONNECT)
        //if(FSM_LoRa_Type == LORALAN_FRAME_TYPE_BRIDGE_DATA)
        else
            OSTimeDlyHMSM(0, 0, 1, 0);
    }

}


/*
*********************************************************************************************************
*	函 数 名: AppTaskCom
*	功能说明: 系统运行指示
*	形    参: p_arg 是在创建该任务时传递的形参
*	返 回 值: 无
	优 先 级: 3
*********************************************************************************************************
*/
static void AppTaskCom(void *p_arg)
{
    (void)p_arg;

    while(1)
    {
        //   bsp_LedToggle(1);//bsp_LedOff(1);//bsp_LedToggle(4);
        //        App_Printf("------------------------------------\r\n");
        //        App_Printf("测试任务-AppTaskCom-运行中...\r\n");
#ifdef USER_WDG
        //bsp_InitWwdg(setTreg, setWreg, WWDG_Prescaler_8);
        bsp_Iwdg_Feed();
#endif
        OSTimeDlyHMSM(0, 0, 1, 0);
    }

}



/*======================================= 以下为调用的 printf 接口函数  ============================================*/

/*
*********************************************************************************************************
*	函 数 名: App_Printf
*	功能说明: 线程安全的printf方式
*	形    参: 同printf的参数。
*                 在C中，当无法列出传递函数的所有实参的类型和数目时,可以用省略号指定参数表
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
void App_Printf (CPU_CHAR *format, ...)
{
    CPU_CHAR  buf_str[80 + 1];
    va_list   v_args;
    CPU_INT08U  os_err;


    va_start(v_args, format);
    (void)vsnprintf((char *)&buf_str[0],
                    (size_t      ) sizeof(buf_str),
                    (char const *) format,
                    v_args);
    va_end(v_args);

    OSSemPend((OS_EVENT *)AppPrintfSemp,
              (INT32U     )0,
              (INT8U *)&os_err);

    printf("%s", buf_str);

    os_err = OSSemPost((OS_EVENT *)AppPrintfSemp);
}
#endif

/*
*********************************************************************************************************
*	函 数 名: DispTaskInfo
*	功能说明: 将uCOS-II任务信息打印到串口
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 0
static void DispTaskInfo(void)
{
    OS_TCB      *ptcb;	        /* 定义一个任务控制块指针, TCB = TASK CONTROL BLOCK */
    OS_STK_DATA stk;	        /* 用于读取任务栈使用情况 */

    ptcb    = &OSTCBTbl[0];		/* 指向第1个任务控制块(TCB) */

    /* 打印标题 */
    //App_Printf("==================================================\r\n");
    // App_Printf("  优先级   使用栈  剩余栈  百分比   任务名\r\n");
    // App_Printf("  Prio     Used    Free    Per      Taskname\r\n");
    // App_Printf("==================================================\r\n");

    OSTimeDly(10);

    /* 遍历任务控制块列表(TCB list)，打印所有的任务的优先级和名称 */
    while (ptcb != NULL)
    {
        /*
        	ptcb->OSTCBPrio : 任务控制块中保存的任务的优先级
        	ptcb->OSTCBTaskName : 任务控制块中保存的任务名称，需要在创建任务的时候
        	调用OSTaskNameSet()指定任务名称，比如：
        	OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
        */
        OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* 获得任务栈已用空间 */
        App_Printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio,
                   stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
                   ptcb->OSTCBTaskName);
        ptcb = ptcb->OSTCBPrev;		            /* 指向上一个任务控制块 */
    }
}
#endif

/************************************* (END OF FILE) *****************************************/



