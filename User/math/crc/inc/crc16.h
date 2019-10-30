/*
********************************************************************************
                                 SEASON_RTOS

                     (c) Copyright 2015; Season's work
         All rights reserved.  Protected by international copyright laws

File        :  crc16.h
By          :  Season
Version     :  V0.1.0
Createdate  :  2015-09-27
----------------------------- Liscensing terms ---------------------------------

********************************************************************************
*/

#ifndef CRC16_H
#define CRC16_H


/****
Include
****/
#ifndef  uint8
#define  uint8 uint8_t
#endif

#ifndef  uint16
#define uint16  uint16_t
#endif

#ifndef  uint32
#define uint32  uint32_t
#endif

/****
Defines
****/

/****
Global variable
****/

/****
Global func
****/
extern  uint16 crc16(uint8 *pBuff, uint32 len);
/******************************************************************************/
#endif
