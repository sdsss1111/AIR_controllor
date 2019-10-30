
#include "../../../bsp/bsp.h"


uint8_t FSM_LoRa_Type; // 定义终端状态机标志

/*-- 需存储的基站信息 --*/
static struct STORE_BS_INFOR *store_bs_infor = NULL;//存储基站信道信息
static struct JOIN_ACK_INFOR *join_ack_infor = NULL;
static struct DNLD_DATA *dnld_data;
static uint8_t Allocated_rltv_addr;
static uint8_t EndDev_TxBuff[LORALAN_FRAME_MAX_LEN];
//struct FRAMER* framer = NULL; // 数据发包帧



/*
*********************************************************************************************************
*	函 数 名: EndDev_Build_SyncReq
*	功能说明: 终端设备组织 SyncReq 帧，用于请求 基站 Beacon
*
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 1
struct ENDDEV_SYNCREQ EndDev_Build_SyncReq(void)
{

    struct ENDDEV_SYNCREQ pFramer;

    //---------------- TLV: 数据帧格式 ------------------------//
    // T: type
    pFramer.fcf.value = LORALAN_FRAME_TYPE_SYNC_REQ;

    // L: length
    pFramer.len = (LORALAN_FRAME_LENGTH_SYNC_REQ - 2); // Setup Length

    // V: value
    pFramer.sync_req_value.payload.enddev_addr = ENDDEV_ADDR; // end device address 2bytes
    pFramer.sync_req_value.payload.counter = ENDDEV_COUNTER; //counter  1byte

    uint16 crc_result = crc16((uint8_t *)&pFramer.sync_req_value.value, LORALAN_FRAME_LENGTH_SYNC_REQ - 4); // crc 2bytes
    pFramer.crc = crc_result;//((uint16_t)(crc_result & 0xFF))|(uint16_t)((crc_result >> 8) & 0xFF);

    //  pFramer.payload.crc[1] = ((crc_result >> 8) & 0xFF);
    // -------------------------

    return pFramer;
}
#endif


/*
*********************************************************************************************************
*	函 数 名: EndDev_Build_JoinReq
*	功能说明: 终端设备组织 JoinReq 帧，用于请求 基站 Beacon
*
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 1
struct ENDDEV_JOINREQ EndDev_Build_JoinReq(void)
{

    struct ENDDEV_JOINREQ pFramer;
    // END_DEV_TYPE end_dev_type_property;

    //---------------- TLV: 数据帧格式 ------------------------//
    // T: type
    pFramer.fcf.value = LORALAN_FRAME_TYPE_JOIN_REQ;

    // L: length
    pFramer.len = (LORALAN_FRAME_LENGTH_JOIN_REQ - 2); // Setup Length

    // V: value
    pFramer.join_req_value.payload.enddev_addr = ENDDEV_ADDR; // end device addr
    pFramer.join_req_value.payload.bs_addr = store_bs_infor->beacon_value.bs_addr;// base station addr
    pFramer.join_req_value.payload.bs_rssi = store_bs_infor->bs_rssi; //recv the bs rssi
    pFramer.join_req_value.payload.devnonce = End_Dev_DevNonce; //devnonce

    pFramer.join_req_opt.bits.isCollector = 1;     //bit define the end device type
    pFramer.join_req_opt.bits.isController = 1;
    pFramer.join_req_opt.bits.reserved = 0;
    pFramer.join_req_opt.bits.subtype = isAlarm_depnd_and_isSocket_Temp;
    //pFramer.join_req_opt.value = 0x23;//isAirController

    pFramer.join_req_value.value[8] = pFramer.join_req_opt.value; // 标识节点属性

    uint16 crc_result = crc16((uint8_t *)&pFramer.join_req_value.value, LORALAN_FRAME_LENGTH_JOIN_REQ - 4); // CRC
    pFramer.crc = crc_result;
    //pFramer.crc[1] = (uint8_t)((crc_result >> 8) & 0xFF);
    // -------------------------

#ifdef DEBUG
    //printk("LoRaLAN_Build_Beacon: beaconFrame_counter %d!\r\n", pGateway->beaconFrame_counter);
#endif
    return pFramer;
}
#endif


/*
*********************************************************************************************************
*	函 数 名: EventUP_Data
*	功能说明: 终端设备组织 EventUP 帧，用于发送数据到基站
*
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
struct  EVENTUP EndDev_EventUp_Data(const uint8_t *pBuff, const uint8_t length, const uint8_t type, const bool isACK)
{
    struct  EVENTUP pEventUp;

    pEventUp.fcf.value = LORALAN_FRAME_TYPE_EVENT_UP;
    pEventUp.len = length + 8;

    pEventUp.eventup_head.payload.enddev_rltv_addr = EndDev_TxBuff[4];//join_ack_infor->enddev_rltv_addr;
    pEventUp.eventup_head.payload.bs_addr = (uint16)(EndDev_TxBuff[0] | (EndDev_TxBuff[1] << 8 & 0xff00)); //join_ack_infor->bs_addr;
    pEventUp.eventup_head.payload.counter = 0;

    pEventUp.eventype.bits.type = type;
    pEventUp.eventype.bits.isAck = 1;
    pEventUp.eventype.bits.reserved = 0;
    //  pEventUp.eventype.value = 0x21;

    pEventUp.lv2_len = length;
    memcpy(pEventUp.lv2_data, pBuff, length);

    return pEventUp;

}

/*
*********************************************************************************************************
*	函 数 名: EndDev_Build_BRGACK
*	功能说明: 终端设备组织 BRIGE_ACK 帧，用于请求 基站 Beacon
*
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
struct BRAG_ACK EndDev_Build_BRGACK(void)
{
    struct BRAG_ACK pBrigAck;

    pBrigAck.fcf.value = LORALAN_FRAME_TYPE_BRIDGE_ACK;
    pBrigAck.len = LORALAN_FRAME_LENGTH_BRIDGE_ACK - 2;
    pBrigAck.enddev_rltv_addr = dnld_data->enddev_rltv_addr;
    pBrigAck.bs_addr = dnld_data->bs_addr;
    pBrigAck.counter = 0x00;

    return pBrigAck;
}

/*
*********************************************************************************************************
*	函 数 名: EndDev_Build_SyncReq
*	功能说明: 终端设备组织 SyncReq 帧，用于请求 基站 Beacon
*
*	形    参: 无
*	返 回 值: 无
*********************************************************************************************************
*/
#if 1
result_t Packet_Parse(const uint8 *RecvBuff, const uint8_t RecvLength, const int8_t RecvRssi)
{
    FCF fcf;

    //  uint8_t temp_chl_sf[6];
    result_t Parse_Flag = sx127x_SUCCESS;

    if(RecvLength == NULL)
    {
#ifdef DEBUG_ERR
        //App_Printf("-- the length cannot be NULL! --\r\n");
#endif
        return sx127x_FAILED;
    }
    if(RecvLength > LORALAN_FRAME_MAX_LEN || RecvLength < LORALAN_FRAME_MIN_LEN)
    {
#ifdef DEBUG_ERR
        //App_Printf("-- the length is crossing! --\r\n");
#endif
        return sx127x_FAILED;
    }

    uint16_t crc_check = crc16((uint8 *)(RecvBuff + 2), RecvLength - 4);
    uint16_t crc_recv  = ((uint16_t)(*(RecvBuff + (RecvLength - 2)))) | ((uint16_t)(*(RecvBuff + (RecvLength - 1)) << 8));

    if(crc_check != crc_recv)
    {
#ifdef DEBUG_ERR
        //App_Printf("-- Packet Parse: CRC check error! --\r\n");
#endif
        return sx127x_FAILED;
    }

    fcf.value = *(RecvBuff);

    switch(fcf.value)
    {
    case LORALAN_FRAME_TYPE_BEACON:
    {
        //    if(store_bs_infor->beacon_value.bs_addr == (((uint16_t)(*(RecvBuff+2))&0xff))|(uint16_t)(*(RecvBuff+3)<<8)&0xff00)
        //      return sx127x_SUCCESS;
        //转换为 STORE_BS_INFOR 结构体类型
        store_bs_infor = (struct STORE_BS_INFOR *)(RecvBuff + 2);

        //计算 基站 UTC
        store_bs_infor->beacon_value.bs_utc = ((uint32_t)(*(RecvBuff + 16) << 24) & 0xff000000) | ((uint32_t)(*(RecvBuff + 15) << 16) & 0xff0000)\
                                              | ((uint32_t)(*(RecvBuff + 14) << 8) & 0xff00) | ((uint32_t)(*(RecvBuff + 13)) & 0xff);
        //接收基站 RSSI
        store_bs_infor->bs_rssi = (uint16_t)((0x00 << 8) | (RecvRssi));
        if(FSM_LoRa_Type == LORALAN_FRAME_TYPE_SYNC_REQ)
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_JOIN_REQ;//置位 JOIN_REQ 状态
        else
            FSM_LoRa_Type = LORALAN_NETWORK_CONNECT;
        break;
    }
    case LORALAN_FRAME_TYPE_JOIN_ACK:
    {
        if((*(RecvBuff + 4) | *(RecvBuff + 5) << 8) != ENDDEV_ADDR)
            return sx127x_FAILED;
        memcpy(&EndDev_TxBuff[0], (RecvBuff + 2), RecvLength);
        join_ack_infor = (struct JOIN_ACK_INFOR *)(RecvBuff + 2);
        join_ack_infor->enddev_rltv_addr = *(RecvBuff + 6);
        Allocated_rltv_addr = join_ack_infor->enddev_rltv_addr;//基站分配的相对地址
        Allocated_Chl_Tx = join_ack_infor->chl_infor.bits.chl_num;
        Allocated_Chl_SF = (RF_SF_Set)join_ack_infor->chl_infor.bits.chl_sf;
        Allocated_Chl_Rx = Allocated_Chl_Tx;
        rf_setChannel(sx127x_FREQ_TX + join_ack_infor->chl_infor.bits.chl_num * 200000); //将终端设置位基站分配信道
        rf_SFSet((RF_SF_Set)join_ack_infor->chl_infor.bits.chl_sf);

        FSM_LoRa_Type = LORALAN_NETWORK_CONNECT;
        break;
    }
    case LORALAN_FRAME_TYPE_EVENT_ACK:
    {
        FSM_LoRa_Type = LORALAN_NETWORK_CONNECT;//LORALAN_FRAME_TYPE_BRIDGE_DATA;
#define DEBUG
#ifdef DEBUG
        //App_Printf("-- recv envet ack parameters:" " %d ", RecvRssi, "\r\n");
#endif
        break;
    }
    case LORALAN_FRAME_TYPE_BRIDGE_DATA:
    {
        if((*(RecvBuff + 4)) != Allocated_rltv_addr)
            return sx127x_FAILED;
        dnld_data = (struct DNLD_DATA *)(RecvBuff);
        memcpy(&dnld_data_buff[0], dnld_data->lv2_data, dnld_data->len - 7); //dnld_data->lv2_len);
        if(dnld_data->eventype.bits.isAck == 1)
        {
            struct BRAG_ACK Brige_Ack = EndDev_Build_BRGACK();
            framer = (struct FRAMER *)&Brige_Ack;
            uint16_t crc_result = crc16((uint8_t *)&framer->value[2], (framer->value[1]) - 2); // CRC
            framer->value[(framer->value[1])] = crc_result & 0xff;
            framer->value[(framer->value[1]) + 1] = ((crc_result >> 8) & 0xFF);
            FSM_LoRa_Type = LORALAN_NETWORK_CONNECT;
            if(rf_send((uint8_t *)&framer->value, framer->value[1] + 2) == sx127x_SUCCESS)
                break;//rf_receiveOn();
        }
        memset(dnld_data, 0, LORALAN_FRAME_MAX_LEN + BRIDGE_DATA_HEAD);
        //  FSM_LoRa_Type = LORALAN_FRAME_TYPE_BRIDGE_ACK;
        break;
    }

    default:
    {
        if(*RecvBuff != 0x01)
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_BRIDGE_DATA;
        else
            FSM_LoRa_Type = LORALAN_FRAME_TYPE_BRIDGE_DATA;
        break;
    }


    }

    return Parse_Flag;
}
#endif













