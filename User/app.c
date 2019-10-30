/*
*********************************************************************************************************
*
*	ģ������ : uCOS-II
*	�ļ����� : app.c
*	��    �� : V1.0
*	˵    �� :
                1. �����������¼�������ͨ�����ڴ�ӡ�����ջʹ�����
*                   ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������
*                    Prio     Used    Free    Per      Taskname
*                     63       17      111    13%     uC/OS-II Idle
*                     62       21      107    16%     uC/OS-II Stat
*                      0       34      222    13%     Start Task
*                      3       27      229    10%     COM
*
*                2. (1) �����õ� printf ������ȫ��ͨ������ App_Printf ʵ�֡�
*                   (2) App_Printf ���������ź����Ļ�������������Դ�������⡣
*
*              ע�����
*
*------------------------------------------------------------------------------------------------------
*	�޸ļ�¼ :
*		�汾��   ����         ����            ˵��
*                V1.0   2017-08-02   pigpeng    1. ST�̼��⵽V3.6.1�汾
*                                               2. BSP������V1.2
*                                               3. uCOS-II�汾V2.92.11
*                                               4. uC/CPU�汾V1.30.02
*                                               5. uC/LIB�汾V1.38.01
*
*	Copyright (C),
*
*********************************************************************************************************
*/
#include "includes.h"


/*
********************************************************************************************************
*                              �����ջ
********************************************************************************************************
*/

/* ����ÿ������Ķ�ջ�ռ䣬app_cfg.h�ļ��к궨��ջ��С */
static OS_STK AppTaskStartStk[APP_TASK_START_STK_SIZE];
//static OS_STK AppTaskUserIFStk[APP_TASK_USER_IF_STK_SIZE];
static OS_STK AppTaskCOMStk[APP_TASK_COM_STK_SIZE];
static OS_STK AppTaskSENDStk[APP_TASK_SEND_STK_SIZE];
static OS_STK AppTaskRECVStk[APP_TASK_RECV_STK_SIZE];

/*
*******************************************************************************************************
*                              ��������
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
*                               ����
*******************************************************************************************************
*/
void app_rfReceiveCb(uint8_t *buf, uint8_t len, int8_t rssi);//RF ���ջص�
void User_TimerISR(); //40ms��ʱ�жϻص�
static uint16_t  BS_OffLine_Counter = 0;
static uint8_t datalen;
u8 gReceiveFlag = 0; //DIO0�жϱ�־
//extern uint8_t  gRfBuffer[];

//result_t app_rfSendCb(struct framer *pframer, const uint8_t );

//static OS_EVENT *AppUserIFMbox; /* ����һ�����䣬 ��ֻ��һ������ָ�룬 OSMboxCreate�����ᴴ������������Դ */

//static OS_EVENT *AppPrintfSemp;/*����һ���ź������ڴ�ӡ��Ϣ*/

OS_EVENT *RF_Recv_Sem; /*����һ���ź����������߽���*/
OS_EVENT *RF_Send_Sem; /*����һ���ź����������߽���*/

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
*	�� �� ��: Sensor_Sample
*	����˵��: �������ն�ģ�飬�����ص�����
*	��    ��: ��
*	�� �� ֵ: ��
*******************************************************************************************************
*/
#ifdef EndDevisSensor
void Sensor_Sample()
{
    static uint16_t Sensor_Samp_Counter = 0;
    Sensor_Samp_Counter++;
    if(Sensor_Samp_Counter == Sample_Cyc_S)
    {
        Read_sht30();      //��ȡSHT30����
        Convert_sht30();   //����ת������
        memcpy(test_data, up_data, 4);
        Sensor_Samp_Counter = 0;
    }
}
#endif

/*
*******************************************************************************************************
*	�� �� ��: app_rfReceiveCb
*	����˵��: sx127x���ջص�
*	��    ��: ��
*	�� �� ֵ: ��
*******************************************************************************************************
*/
void app_rfReceiveCb(uint8_t *buf, uint8_t len, int8_t rssi)
{
    gReceiveFlag = 1;
}

/*
*******************************************************************************************************
*	�� �� ��: User_TimerISR
*	����˵��: ��ʱ���жϻص������ڼ���վ�Ƿ�����
*	��    ��: ��
*	�� �� ֵ: ��
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
*	�� �� ��: main
*	����˵��: ��׼c������ڡ�
*	��    ��: ��
*	�� �� ֵ: ��
*******************************************************************************************************
*/
int main(void)
{
    INT8U  err;
    //static u8 up_buff[6] = {0x00,};

    OSInit(); /* ��ʼ��"uC/OS-II"�ں� */

    /* ����һ����������Ҳ���������񣩡���������ᴴ�����е�Ӧ�ó������� */
    OSTaskCreateExt(AppTaskStart,	 /* ����������ָ�� */
                    (void *)0,		 /* ���ݸ�����Ĳ��� */
                    (OS_STK *)&AppTaskStartStk[APP_TASK_START_STK_SIZE - 1],    /* ָ������ջջ����ָ�� */
                    APP_TASK_START_PRIO,	   /* ��������ȼ�������Ψһ������Խ�����ȼ�Խ�� */
                    APP_TASK_START_PRIO,	   /* ����ID��һ����������ȼ���ͬ */
                    (OS_STK *)&AppTaskStartStk[0], /* ָ������ջջ�׵�ָ�롣OS_STK_GROWTH ������ջ�������� */
                    APP_TASK_START_STK_SIZE,       /* ����ջ��С */
                    (void *)0,	   /* һ���û��ڴ�����ָ�룬����������ƿ�TCB����չ����
                                   ���������л�ʱ����CPU����Ĵ��������ݣ���һ�㲻�ã���0���� */
                    OS_TASK_OPT_STK_CHK | OS_TASK_OPT_STK_CLR); /* ����ѡ���� */
    /*  �������£�
        OS_TASK_OPT_STK_CHK  ʹ�ܼ������ջ��ͳ������ջ���õĺ�δ�õ�
        OS_TASK_OPT_STK_CLR  �ڴ�������ʱ����������ջ
        OS_TASK_OPT_SAVE_FP  ���CPU�и���Ĵ��������������л�ʱ���渡��Ĵ���������
    */
    OSTaskNameSet(APP_TASK_START_PRIO, APP_TASK_START_NAME, &err);/* ָ����������ƣ����ڵ��ԡ���������ǿ�ѡ�� */

    OSTimeSet(0);/*ucosII�Ľ��ļ�������0    ���ļ�������0-4294967295*/

    OSStart(); /* ����������ϵͳ������Ȩ����uC/OS-II */

    while(1)  /* ϵͳ��������ʱ�������������� */
    {
        //App_Printf("-- system failed start!-- \r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ
    }
}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskStart
*	����˵��: ����һ�����������ڶ�����ϵͳ�����󣬱����ʼ���δ������(��BSP_Init��ʵ��)
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 0
*********************************************************************************************************
*/
static void AppTaskStart(void *p_arg)
{
    (void)p_arg;/* �����ڱ���������澯����������������κ�Ŀ����� */
    //    INT8U err;

    bsp_Init();   /* BSP ��ʼ���� BSP = Board Support Package �弶֧�ְ����������Ϊ�ײ�������*/
    CPU_Init();
    BSP_Tick_Init(); /*BSPʱ�ӳ�ʼ��*/

    bsp_InitTimer();  /*�����ʱ����ʼ��*/
    bsp_StartTimer(0, 1); /* ������ʱ����1ms ��ʱ*/
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
    rf_init(app_rfReceiveCb);      /*  sx127x ��ʼ��  */
    rf_setChannel(sx127x_FREQ_RX);/* ����ΪĬ�Ͻ����ŵ� sx127x_FREQ_RX */
    rf_receiveOn();

    AppTaskCreate();  /* ����Ӧ�ó�������� */

#if (OS_TASK_STAT_EN > 0) /* ���CPU������ͳ��ģ���ʼ�����ú�����������CPUռ���� */
    OSStatInit();
#endif

    //   err = OSTaskDel(OS_PRIO_SELF); /* ����������ɣ����� */
    //   if(err == OS_ERR_NONE)
    //     App_Printf("-- the start task is suspended!-- \r\n"); //ϵͳ����ʧ�ܣ���ӡ��Ϣ

}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskCreate
*	����˵��: ����Ӧ������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void AppTaskCreate (void)
{
    INT8U      err;

    /* ��������(MBOX) */
    //    AppUserIFMbox = OSMboxCreate((void *)0);
    //    AppPrintfSemp = OSSemCreate(1);	  /* ����һ���ź��� ʵ���ź������� */
    RF_Recv_Sem = OSSemCreate(0);       /* ����һ���ź��� �������߽��ձ�ʶ*/
    RF_Send_Sem = OSSemCreate(0);       /* ����һ���ź��� �������߽��ձ�ʶ*/

#if 0
    /* ����AppTaskUserIF���� */
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
    /* ����AppTaskLED���� */
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

    /* ����AppTaskCom���� */
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
    /* ����AppTaskKeyScan���� */
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

    //    err = OSTaskSuspend(OS_PRIO_SELF); /* ����������ɣ����� */
    //    if(err == OS_ERR_NONE)
    //      App_Printf("-- the AppTaskCreate is suspended!-- \r\n"); /* ��ӡ��Ϣ */

}


/*
*********************************************************************************************************
*	�� �� ��: AppTaskSend
*	����˵��: RF��������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
*********************************************************************************************************
*/
static void AppTaskRecv(void *p_arg)
{
    (void)p_arg;
    INT8U err;
    uint8_t recvlen;
    result_t rf_recv_result;
    result_t CMD_result = sx127x_FAILED;

    while (1) /* �������壬������һ����ѭ�� */
    {

        OSSemPend(RF_Recv_Sem, 0, &err); /* RF ��������ȴ������ź��� */

        rf_recv_result = Get_sx127xData();//��ȡ RF ����

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
*	�� �� ��: AppTaskSend
*	����˵��: RF��������
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
*********************************************************************************************************
*/
static void AppTaskSend(void *p_arg)
{
    (void)p_arg;
    INT8U err;
    //DispTaskInfo();  /* ��ӡ��ǰϵͳ������Ϣ */
#if 1
    //    uint8_t  local_RFSend[LORALAN_FRAME_MAX_LEN];
    //    uint8_t  Local_BuffLen;
    FSM_LoRa_Type = LORALAN_NETWORK_DISCONNECT;
    //     FSM_LoRa_Type = LORALAN_FRAME_TYPE_JOIN_REQ;


    while(1)
    {
        if( FSM_LoRa_Type == LORALAN_NETWORK_DISCONNECT) //δ����
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_SYNC_REQ; //����sync_req

        else if( FSM_LoRa_Type == LORALAN_NETWORK_CONNECT) //�����ɹ������Է�������
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_BRIDGE_DATA;//��������
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
            //          OSSemPend(RF_Send_Sem, 0, &err); /* RF ��������ȴ������ź��� */

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
*	�� �� ��: AppTaskCom
*	����˵��: ϵͳ����ָʾ
*	��    ��: p_arg ���ڴ���������ʱ���ݵ��β�
*	�� �� ֵ: ��
	�� �� ��: 3
*********************************************************************************************************
*/
static void AppTaskCom(void *p_arg)
{
    (void)p_arg;

    while(1)
    {
        //   bsp_LedToggle(1);//bsp_LedOff(1);//bsp_LedToggle(4);
        //        App_Printf("------------------------------------\r\n");
        //        App_Printf("��������-AppTaskCom-������...\r\n");
#ifdef USER_WDG
        //bsp_InitWwdg(setTreg, setWreg, WWDG_Prescaler_8);
        bsp_Iwdg_Feed();
#endif
        OSTimeDlyHMSM(0, 0, 1, 0);
    }

}



/*======================================= ����Ϊ���õ� printf �ӿں���  ============================================*/

/*
*********************************************************************************************************
*	�� �� ��: App_Printf
*	����˵��: �̰߳�ȫ��printf��ʽ
*	��    ��: ͬprintf�Ĳ�����
*                 ��C�У����޷��г����ݺ���������ʵ�ε����ͺ���Ŀʱ,������ʡ�Ժ�ָ��������
*	�� �� ֵ: ��
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
*	�� �� ��: DispTaskInfo
*	����˵��: ��uCOS-II������Ϣ��ӡ������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
#if 0
static void DispTaskInfo(void)
{
    OS_TCB      *ptcb;	        /* ����һ��������ƿ�ָ��, TCB = TASK CONTROL BLOCK */
    OS_STK_DATA stk;	        /* ���ڶ�ȡ����ջʹ����� */

    ptcb    = &OSTCBTbl[0];		/* ָ���1��������ƿ�(TCB) */

    /* ��ӡ���� */
    //App_Printf("==================================================\r\n");
    // App_Printf("  ���ȼ�   ʹ��ջ  ʣ��ջ  �ٷֱ�   ������\r\n");
    // App_Printf("  Prio     Used    Free    Per      Taskname\r\n");
    // App_Printf("==================================================\r\n");

    OSTimeDly(10);

    /* ����������ƿ��б�(TCB list)����ӡ���е���������ȼ������� */
    while (ptcb != NULL)
    {
        /*
        	ptcb->OSTCBPrio : ������ƿ��б������������ȼ�
        	ptcb->OSTCBTaskName : ������ƿ��б�����������ƣ���Ҫ�ڴ��������ʱ��
        	����OSTaskNameSet()ָ���������ƣ����磺
        	OSTaskNameSet(APP_TASK_USER_IF_PRIO, "User I/F", &err);
        */
        OSTaskStkChk(ptcb->OSTCBPrio, &stk);	/* �������ջ���ÿռ� */
        App_Printf("   %2d    %5d    %5d    %02d%%     %s\r\n", ptcb->OSTCBPrio,
                   stk.OSUsed, stk.OSFree, (stk.OSUsed * 100) / (stk.OSUsed + stk.OSFree),
                   ptcb->OSTCBTaskName);
        ptcb = ptcb->OSTCBPrev;		            /* ָ����һ��������ƿ� */
    }
}
#endif

/************************************* (END OF FILE) *****************************************/



