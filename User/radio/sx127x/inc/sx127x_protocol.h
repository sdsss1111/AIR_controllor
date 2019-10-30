

#ifndef __sx127x_protocol_H__
#define __sx127x_protocol_H__

#ifdef __cplusplus
extern "C" {
#endif
//#include "../../bsp/bsp.h"

#if 1
#ifndef uint8
// ------ data type defintion ----
typedef	unsigned long long         uint64;
typedef signed   long long         sint64;
typedef	unsigned int	           uint32;
typedef signed   int               sint32;
typedef	unsigned short	           uint16;
typedef signed   short             sint16;
//typedef	unsigned short	USHORT;
typedef	unsigned char	           uint8;
typedef signed   char              sint8;
typedef float                      float32;
typedef double                     float64;

#ifndef bool
typedef	unsigned char	bool;
#endif
#endif


// --- type define ---
#define LORALAN_FRAME_TYPE_BEACON                           (0)  // Beacon
#define LORALAN_FRAME_TYPE_JOIN_REQ                         (3)  // JoinReq
#define LORALAN_FRAME_TYPE_JOIN_ACK                         (2)  // JoinAck
#define LORALAN_FRAME_TYPE_BROADCAST_GROUP_CONTROL          (1)  // BroadcastGroupControl
#define LORALAN_FRAME_TYPE_BROADCAST_GROUP_CONTROL_ACK      (6)  // BroadcastACK
#define LORALAN_FRAME_TYPE_SINGLE_UPGRADE                   (5)  // SingleUpgrade
#define LORALAN_FRAME_TYPE_SINGLE_UPGRADE_ACK               (10) // SingleUpgradeAck
#define LORALAN_FRAME_TYPE_SINGLE_UPGRADE_DATA              (9)  // SingleUpgradeData
#define LORALAN_FRAME_TYPE_SINGLE_UPGRADE_CONFIRM           (14) // SingleUpgradeConfirm
#define LORALAN_FRAME_TYPE_GROUP_UPGRADE                    (13) // GroupUpgrade
#define LORALAN_FRAME_TYPE_GROUP_UPGRADE_DATA               (17) // GroupUpgradeData
#define LORALAN_FRAME_TYPE_SYNC_REQ                         (7)  // SyncReq
#define LORALAN_FRAME_TYPE_SYNC_ACK                         (18) // SyncAck
#define LORALAN_FRAME_TYPE_EVENT_UP                         (21) // EventUp
#define LORALAN_FRAME_TYPE_EVENT_ACK                        (22) // EventAck

#define LORALAN_FRAME_TYPE_BRIDGE_DATA                      (25) // BridgeData
#define LORALAN_FRAME_TYPE_BRIDGE_ACK                       (26) // BridgeAck

#define LORALAN_NETWORK_CONNECT                             (27)
#define LORALAN_NETWORK_DISCONNECT                             (28)


// -------------------

// --- Frame length define ---
#define LORALAN_FRAME_LENGTH_BEACON                         (20) // Beacon frame length
#define LORALAN_FRAME_LENGTH_JOIN_REQ                       (13) // JoinReq frame length
#define LORALAN_FRAME_LENGTH_JOIN_ACK                       (12) // JoinAck frame length
#define LORALAN_FRAME_LENGTH_BCGC_ACK                       (7)  // BCGC_Ack frame length
#define LORALAN_FRAME_LENGTH_MIN_EVENT_UP                   (10) // Min frame length of EventUp
#define LORALAN_FRAME_LENGTH_EVENT_ACK                      (9)  // EventAck frame length
#define LORALAN_FRAME_LENGTH_SYNC_REQ                       (7)  // SyncReq frame length
#define LORALAN_FRAME_LENGTH_BRIDGE_ACK                     (8)  // BridgeAck frame length
// ---------------------------

#define LORALAN_FRAME_MAX_LEN                               (64)//(254) // Max = 254 frame length for LoRaLAN,modify 2019.8.11
#define LORALAN_FRAME_MIN_LEN                               (4)   // Min frame length for LoRaLAN

#define BRIDGE_DATA_HEAD                                    (7)

// --- Define "type" field of LORALAN_EVENT_TYPE ---
#define LORALAN_EVENT_TYPE_PWR_OFF                          (1) // The power supply of end_dev is cut off 
#define LORALAN_EVENT_TYPE_RESTART                          (2) // The end_dev restart

// For sensor
#define LORALAN_EVENT_TYPE_SENSOR_UPDATE                    (11) // The sensor update its value

// For bridge
#define LORALAN_EVENT_TYPE_BRIDGE_UPDATE                    (21) // Bridge data to Gateway
// -------------------------------------------------

/****
Global variable
****/
#define ENDDEV_ADDR                       0x0606
#define ENDDEV_COUNTER                    0x00


#endif


//------------  基站信道 SF 值枚举 -------------------//
typedef enum //-- Enumerate all the SF Value --//
{
    LoRa_SF7      = 0, //
    LoRa_SF8      = 1,
    LoRa_SF9      = 2,
    LoRa_SF10     = 3,
    LoRa_SF11     = 4,
    LoRa_SF12     = 5,
    LoRa_SF7_SF12 = 6,
    LoRa_SFk      = 7,
} GET_CHL_SF;

//------------  beacon framer struction -------------------//
#if 1
//-- Frame Control Field --//
typedef union
{
    uint8 value;
    struct
    {
        uint8 type        : 5;// sub type | main type
        uint8 version     : 2;// frame version
        uint8 isSecurity  : 1; //0- No, 1- yes
    } bits;
} FCF;


//------------ Beacon up/down Channel Field----------------//
typedef union
{
    uint8  up_dn_chl_num;
    struct
    {
        uint8  dn_chl    : 4; //down chlel number
        uint8  up_chl    : 4; //up chlel number
    } bits;
} CHL_NUM;


//-------------- Beacon Serve/Businss Identify Feild --------//
typedef union
{
    uint16  serv_or_busi;
    struct
    {
        uint16  chl_0   : 1; //0-business, 1- serve
        uint16  chl_1   : 1; //0-business, 1- serve
        uint16  chl_2   : 1; //0-business, 1- serve
        uint16  chl_3   : 1; //0-business, 1- serve
        uint16  chl_4   : 1; //0-business, 1- serve
        uint16  chl_5   : 1; //0-business, 1- serve
        uint16  chl_6   : 1; //0-business, 1- serve
        uint16  chl_7   : 1; //0-business, 1- serve
        uint16  chl_8   : 1; //0-business, 1- serve
        uint16  chl_9   : 1; //0-business, 1- serve
        uint16  chl_10  : 1; //0-business, 1- serve
        uint16  chl_11  : 1; //0-business, 1- serve
        uint16  chl_12  : 1; //0-business, 1- serve
        uint16  chl_13  : 1; //0-business, 1- serve
        uint16  chl_14  : 1; //0-business, 1- serve
        uint16  chl_15  : 1; //0-business, 1- serve
    } bits;
} SERV_OR_BUSI;

//------------------ channel sf value ---------------//
typedef union
{
    uint8  bitrate[8]; // Only use first 6 bytes
    uint64   LBitrate;
#pragma pack(8)
    struct
    {
        uint64 bitrate_chl_0        : 3; // Bitrate param
        uint64 bitrate_chl_1        : 3; // Bitrate param
        uint64 bitrate_chl_2        : 3; // Bitrate param
        uint64 bitrate_chl_3        : 3; // Bitrate param
        uint64 bitrate_chl_4        : 3; // Bitrate param
        uint64 bitrate_chl_5        : 3; // Bitrate param
        uint64 bitrate_chl_6        : 3; // Bitrate param
        uint64 bitrate_chl_7        : 3; // Bitrate param
        uint64 bitrate_chl_8        : 3; // Bitrate param
        uint64 bitrate_chl_9        : 3; // Bitrate param
        uint64 bitrate_chl_10       : 3; // Bitrate param
        uint64 bitrate_chl_11       : 3; // Bitrate param
        uint64 bitrate_chl_12       : 3; // Bitrate param
        uint64 bitrate_chl_13       : 3; // Bitrate param
        uint64 bitrate_chl_14       : 3; // Bitrate param
        uint64 bitrate_chl_15       : 3; // Bitrate param
        uint64 reserve              : 16;// 保留
    } bits;
#pragma pack()
} CHANNEL_SF;


//---- end device need to store channel information -----//
struct STORE_BS_INFOR
{
    //  FCF           fcf;
    //  uint8         len;
    //uint8 beacon_buff[18];

#pragma pack(1)
    struct
    {
        uint16         bs_addr; //base station address
        CHL_NUM        up_dn_chl_num;
        SERV_OR_BUSI   chl_idtf;
        CHANNEL_SF     chl_sf;
        uint32         bs_utc;
    } beacon_value;
#pragma pack()
    uint16         bs_rssi;
};
#endif //#if 1, beacon framer

//--------------- 数据传输用到的结构体 ----------------------//
#pragma pack(1)
struct FRAMER
{
    uint8 value[LORALAN_FRAME_MAX_LEN];
};
#pragma pack()

//------------  sync req framer struction -------------------//
#if 1
typedef union  //------ 终端同步请求帧结构 -----//
{
    uint8 value[3];// 实际只用到 5 个bytes
#pragma pack(1)
    struct
    {
        uint16   enddev_addr;
        uint8    counter;
    } payload;
#pragma pack()
} SYNC_REQ_VALUE;

#pragma pack(1)
struct ENDDEV_SYNCREQ
{

    FCF               fcf;
    uint8             len;
    SYNC_REQ_VALUE    sync_req_value;
    uint16            crc;
};
#pragma pack()
#endif //#if 1, sync req framer

//------------  Join req framer struction -------------------//
#if 1
#define End_Dev_DevNonce                (0x0000)
typedef enum  // --- enumerate the type of ENDDEV_PROPERTY ---//
{
    isAlarm_8Loop_and_isSocket_NoTemp       =  1, // 8回路控制器 或 无测温插座
    isAlarm_depnd_and_isSocket_Temp         =  2,// 独立报警 或 带测温插座
    isSwitch_Single_and_isPyroelectric      =  3,//单联开关 或 人体热释电
    isSwitch_Double_and_isTemp_Hum          =  4,//双联开关 或 温湿度
    isSwitch_Triple_and_isEvmPM             =  5,//三联开关 或 PM值
    isSwitch_Dimmer_and_isEvmCO             =  6,//智能调光 或 CO
    isLiveController_and_isEvmCO2           =  7,
    isInfrared_and_isEvmTVOC                =  8,
    isCurtain_and_isElectricalParameters    =  9,
    isAirController                         =  10,

} END_DEV_TYPE;

typedef union // --- Define the type of ENDDEV_PROPERTY ---//
{
    uint8  value;
    struct
    {
        uint8  isCollector     : 1; //0-no, 1-yes
        uint8  isController    : 1;//0-no, 1-yes
        uint8  reserved        : 2;//reserved
        uint8  subtype         : 4;//specific type: corresponding to the enumeration
    } bits;
} JOIN_REQ_OPT;


typedef union  //------ 终端接入请求帧 value 部分 -----//
{
    uint8 value[9];// 实际只用到 9 个bytes
    struct
    {
        uint16   enddev_addr;
        uint16   bs_addr;
        uint16   bs_rssi;
        uint16   devnonce;
    } payload;
} JOIN_REQ_VALUE;

#pragma pack(2)
struct ENDDEV_JOINREQ //------ 终端接入请求帧结构 -----//
{

    FCF               fcf;
    uint8             len;
    JOIN_REQ_VALUE    join_req_value;
    JOIN_REQ_OPT      join_req_opt;
    uint16             crc;
};
#pragma pack()
#endif //#if 1, join req framer


//------------  Join Ack framer struction -------------------//
typedef union    //join ack 信道信息
{
    uint8  chl_idl;
    struct
    {
        uint8  chl_num   : 4;
        uint8  chl_sf    : 4;
    } bits;
} JOIN_ACK_CHL_INFOR;

#pragma pack(1)
struct JOIN_ACK_INFOR
{
    uint16                       bs_addr;         //基站绝对地址
    uint16                       enddev_addr;     //终端绝对地址
    uint8                        enddev_rltv_addr;//终端相对地址
    JOIN_ACK_CHL_INFOR           chl_infor;       //分配与终端的信道信息
    uint16                       appnonce;        //option
    struct JOIN_ACK_INFOR       *next_bs;
};
#pragma pack()


//------------  EventUp framer struction -------------------//
typedef union
{
    uint8  value;
    struct
    {
        uint8  type      :  6; //type
        uint8  isAck     :  1; //0- no, 1- yes
        uint8  reserved  :  1; //预留
    } bits;
} EVENT_TYPE;

typedef union
{
    uint8  value[4];
#pragma pack(1)
    struct
    {
        uint8  enddev_rltv_addr;
        uint16 bs_addr;
        uint8  counter;
    } payload;
#pragma pack()
} EVENT_UP_HEAD;


struct  EVENTUP
{
    FCF             fcf;
    uint8           len;
    EVENT_UP_HEAD   eventup_head;
    EVENT_TYPE      eventype;
    uint8           lv2_len;
    uint8           lv2_data[LORALAN_FRAME_MAX_LEN];
};


//------------  BCGC framer struction -------------------//

//struct  BCGC
//{
//  FCF             fcf;
//  uint8           len;
//  uint16          bs_addr
//  EVENT_TYPE      eventype;
//  uint8           lv2_len;
//  uint8           lv2_data[LORALAN_FRAME_MAX_LEN];
//};

//------------  Download data framer struction -------------------//
typedef union
{
    uint8 vaule;
    struct
    {
        uint8  isAck     : 1;//0- no, 1- yes
        uint8  reserved  : 7;
    } bits;

} DN_DATA_TYPE;


struct  DNLD_DATA
{
    FCF               fcf;
    uint8             len;
    uint16            bs_addr;
    uint8             enddev_rltv_addr;
    uint8             counter;
    DN_DATA_TYPE      eventype;
    //uint8             lv2_len;
    uint8             lv2_data[LORALAN_FRAME_MAX_LEN];
};

#pragma pack(1)
struct BRAG_ACK
{
    FCF      fcf;
    uint8    len;
    uint8    enddev_rltv_addr;
    uint16   bs_addr;
    uint8    counter;
    //  uint16   crc;
};
#pragma pack()

#if 0
// --- LORALAN_EVENT_TYPE ---
typedef union
{
    uint8 value;
    struct
    {
        uint8 type              : 6; // See up define
        uint8 isAck             : 1; // 0 - No, 1 - yes
        uint8 reserved          : 1;
    } bits;
} LORALAN_EVENT_TYPE;
// ---------------------------


//struct EndDev_Tx
//{
//
//
//
//};





// --- Define LoRaLan frame struct ---
struct framer_loralan
{
    struct framer super;   // User framer as super struct
};
// -----------------------------------
#endif

extern uint8_t dnld_data_buff[];
//extern struct JOIN_ACK_INFOR* join_ack_infor;
extern uint8 FSM_LoRa_Type;
extern struct FRAMER *framer;
struct ENDDEV_SYNCREQ EndDev_Build_SyncReq(void);
struct ENDDEV_JOINREQ EndDev_Build_JoinReq(void);
struct  EVENTUP EndDev_EventUp_Data(const uint8_t *pBuff, const uint8_t length, const uint8_t type, const bool isACK);
struct BRAG_ACK EndDev_Build_BRGACK(void);

result_t Packet_Parse(const uint8 *RecvBuff, const uint8 RecvLength, const int8_t RecvRssi);

#ifdef __cplusplus
}
#endif
#endif

