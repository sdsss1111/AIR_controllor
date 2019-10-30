/*
*********************************************************************************************************
*                                            EXAMPLE CODE
*
*               This file is provided as an example on how to use Micrium products.
*
*               Please feel free to use any application code labeled as 'EXAMPLE CODE' in
*               your application products.  Example code may be used as is, in whole or in
*               part, or may be used as a reference only. This file can be modified as
*               required to meet the end-product requirements.
*
*               Please help us continue to provide the Embedded community with the finest
*               software available.  Your honesty is greatly appreciated.
*
*               You can find our product's user manual, API reference, release notes and
*               more information at https://doc.micrium.com.
*               You can contact us at www.micrium.com.
*********************************************************************************************************
*/

/*
*********************************************************************************************************
*
*	模块名称 : uCOS-II的应用配置
*	文件名称 : app_cfg.c
*	版    本 : V1.0
*	说    明 : ucos-ii的应用配置
*--------------------------------------------------------------------------------------------------------
*	修改记录 :
*		版本号  日期        作者     说明
*		V1.0
*
*	Copyright (C):
*
*********************************************************************************************************
*/

#ifndef  APP_CFG_MODULE_PRESENT
#define  APP_CFG_MODULE_PRESENT


/*
*********************************************************************************************************
*                                              TASKS NAMES
*********************************************************************************************************
*/

#define  APP_TASK_START_NAME                          "Start Task"
#define  APP_TASK_SEND_NAME                           "SX127X SEND"
//#define  APP_TASK_USER_IF_NAME                      "User Interface"
#define  APP_TASK_COM_NAME                            "COM"
#define  APP_TASK_RECV_NAME                           "SX127X_RECV"


/*
*********************************************************************************************************
*                                            TASK PRIORITIES
*********************************************************************************************************
*/
/* 启动任务 */
#define  APP_TASK_START_PRIO              7
//#define  APP_TASK_USER_IF_PRIO            2
#define  APP_TASK_COM_PRIO		  6
#define  APP_TASK_RECV_PRIO		  4
#define  APP_TASK_SEND_PRIO               5 //(OS_LOWEST_PRIO - 3)

/*
*********************************************************************************************************
*                                            TASK STACK SIZES
*                             Size of the task stacks (# of OS_STK entries)
*********************************************************************************************************
*/

#define  APP_TASK_START_STK_SIZE                         256
#define  APP_TASK_SEND_STK_SIZE                          256
#define  APP_TASK_RECV_STK_SIZE                          512
#define  APP_TASK_COM_STK_SIZE			         64

#define  OS_CPU_EXCEPT_STK_SIZE                          256

/*
*********************************************************************************************************
*                                     TRACE / DEBUG CONFIGURATION
*********************************************************************************************************
*/

#ifndef  TRACE_LEVEL_OFF
#define  TRACE_LEVEL_OFF                        0u
#endif

#ifndef  TRACE_LEVEL_INFO
#define  TRACE_LEVEL_INFO                       1u
#endif

#ifndef  TRACE_LEVEL_DBG
#define  TRACE_LEVEL_DBG                        2u
#endif

#include <cpu.h>
void  App_SerPrintf  (CPU_CHAR *format, ...);

#if (APP_CFG_SERIAL_EN == DEF_ENABLED)
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_DBG
#else
#define  APP_TRACE_LEVEL                        TRACE_LEVEL_OFF
#endif
#define  APP_TRACE                              App_SerPrintf

#define  APP_TRACE_INFO(x)               ((APP_TRACE_LEVEL >= TRACE_LEVEL_INFO)  ? (void)(APP_TRACE x) : (void)0)
#define  APP_TRACE_DBG(x)                ((APP_TRACE_LEVEL >= TRACE_LEVEL_DBG)   ? (void)(APP_TRACE x) : (void)0)

#endif

/***************************** (END OF FILE) *********************************/
