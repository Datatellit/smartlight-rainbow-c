#ifndef __GLOBAL_H
#define __GLOBAL_H

#include <stm8s.h> //Required for the stdint typedefs
#include "stdio.h"
#include "string.h"
#include "stm8s_conf.h"
#include "LightPublicDefine.h"

/* Exported types ------------------------------------------------------------*/
// Simple Direct Test
// Uncomment this line to work in Simple Direct Test Mode
//#define ENABLE_SDTM

// Include Sensors
/// Comment off line to disable sensor
//#define EN_SENSOR_ALS
//#define EN_SENSOR_MIC
//#define EN_SENSOR_PIR
//#define EN_SENSOR_DHT
//#define EN_SENSOR_PM25
//#define EN_SENSOR_MQ135
//#define EN_SENSOR_MQ2
//#define EN_SENSOR_MQ7

#define V1
#ifdef V1
#define NODEID_MAINDEVICE 1
#define IS_NOT_DEVICE_NODEID(nID)  ((nID < NODEID_MIN_LAMP || nID > NODEID_MAX_LAMP) && nID != NODEID_MAINDEVICE)
#endif

// Keep alive message interval, around 6 seconds
#define RTE_TM_KEEP_ALIVE               500    // about 5s (500 * 10ms)
#define MAX_RF_FAILED_TIME              8      // Reset RF module when reach max failed times of sending
#define MAX_RF_RESET_TIME               3      // Reset Node when reach max times of RF module consecutive reset

// Whether allow individual color control of ring
/// Uncomment this line only if h       ardware supports
//#define RING_INDIVIDUAL_COLOR

// Device (lamp) type
#define MAX_RING_NUM            3
typedef enum
{
  devtypUnknown = 0,
  // Color ring - Rainbow
  devtypCRing3,
  devtypCRing2,
  devtypCBar,
  devtypCFrame,
  devtypCWave,
  devtypCRing1 = 31,

  // White ring - Sunny
  devtypWRing3 = 32,
  devtypWRing2,
  devtypWBar,
  devtypWFrame,
  devtypWWave,
  devtypWSquare60,      // 60 * 60
  devtypWPanel120_30,   // 120 * 30
  devtypWBlackboard,    // Blackboard lamp
  devtypWRing1 = 95,

  // Color & Motion ring - Mirage
  devtypMRing3 = 96,
  devtypMRing2,
  devtypMBar,
  devtypMFrame,
  devtypMWave,
  devtypMRing1 = 127,

  devtypDummy = 255
} devicetype_t;

typedef struct
{
  UC State                    :1;           // Component state
  UC BR                       :7;           // Brightness of white [0..100]
  US CCT                      :16;          // CCT (warm or cold) [2700..6500]
  UC R                        :8;           // Brightness of red
  UC G                        :8;           // Brightness of green
  UC B                        :8;           // Brightness of blue
  UC W                        :8;           // Brightness of white
  UC L1                       :8;           // Length of thread 1
  UC L2                       :8;           // Length of thread 2
  UC L3                       :8;           // Length of thread 3
} Hue_t;

// Xlight Application Identification
#define XLA_VERSION               0x08
#define XLA_ORGANIZATION          "xlight.ca"               // Default value. Read from EEPROM

#if XLA_VERSION > 0x07
#define XLA_MIN_VER_REQUIREMENT   0x08
typedef struct
{
  // Static & status parameters
  UC version                  :8;           // Data version, other than 0xFF
  UC present                  :1;           // 0 - not present; 1 - present
  UC filter                   :4;
  UC mode                     :1;           // light mode(0-sunny 1-rgb)
  UC reserved0                :2;
  UC swTimes                  :4;           // On/Off times
  UC cntRFReset               :4;           // RF reset count
  Hue_t ring[MAX_RING_NUM];

  // Configurable parameters
  UC nodeID;                                // Node ID for this device
  UC subID;                                 // SubID
  UC NetworkID[6];
  UC rfChannel;                             // RF Channel: [0..127]
  UC rfPowerLevel             :2;           // RF Power Level 0..3
  UC rfDataRate               :2;           // RF Data Rate [0..2], 0 for 1Mbps, or 1 for 2Mbps, 2 for 250kbs
  UC rptTimes                 :2;           // Sending message max repeat times [0..3]
  UC enSDTM                   :1;           // Simple Direct Test Mode Flag
  UC hasSiblingMCU            :1;           // Whether sibling MCU presents
  UC type;                                  // Type of lamp
  US token;
  UC wattOption               :4;           // 0..15, ref to WATT regulation method
  UC enAutoPowerTest          :1;           // power auto test mode
  UC reserved1                :3;
  US senMap                   :16;          // Sensor Map
  US funcMap                  :16;          // Function Map
  UC alsLevel[2];
  UC pirLevel[2];
} Config_t;
#else
#define XLA_MIN_VER_REQUIREMENT   0x03
typedef struct
{
  UC version                  :8;           // Data version, other than 0xFF
  UC nodeID;                                // Node ID for this device
  UC NetworkID[6];
  UC present                  :1;           // 0 - not present; 1 - present
  UC enSDTM                   :1;           // Simple Direct Test Mode Flag
  UC rfDataRate               :2;           // RF Data Rate [0..2], 0 for 1Mbps, or 1 for 2Mbps, 2 for 250kbs
  UC filter                   :4;
  UC type;                                  // Type of lamp
  UC subID;                                 // SubID
  US token;
  Hue_t ring[MAX_RING_NUM];
  //char Organization[24];                    // Organization name
  //char ProductName[24];                     // Product name
  UC rfPowerLevel             :2;           // RF Power Level 0..3
  UC hasSiblingMCU            :1;           // Whether sibling MCU presents
  UC swTimes                  :3;           // On/Off times
  UC rptTimes                 :2;           // Sending message max repeat times [0..3]
  US senMap                   :16;          // Sensor Map
  US funcMap                  :16;          // Function Map
  UC alsLevel[2];
  UC pirLevel[2];
  UC cntRFReset               :4;           // RF reset count
  UC wattOption               :4;           // 0..15, ref to WATT regulation method
  UC rfChannel;                             // RF Channel: [0..127]
} Config_t;
#endif

extern Config_t gConfig;
extern bool gIsConfigChanged;
extern bool gNeedSaveBackup;
extern bool gIsStatusChanged;
extern bool gResetRF;
extern bool gResetNode;
extern uint8_t _uniqueID[UNIQUE_ID_LEN];
// off delay tick
extern int32_t offdelaytick;


bool isIdentityEqual(const UC *pId1, const UC *pId2, UC nLen);
void GotNodeID();
void GotPresented();
void CCT2ColdWarm(uint32_t ucBright, uint32_t ucWarmCold);
bool SetDeviceOnOff(bool _sw, uint8_t _ring);
bool SetDeviceBrightness(uint8_t _br, uint8_t _ring);
bool SetDeviceCCT(uint16_t _cct, uint8_t _ring);
bool SetDeviceRGB(uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _br,uint8_t _ring);
bool SetDeviceStatus(bool _sw, uint8_t _br, uint16_t _cct, uint8_t _ring);
//bool SetDeviceHue(bool _sw, uint8_t _br, uint8_t _r, uint8_t _g, uint8_t _b, uint8_t _ring);
bool SetDeviceFilter(uint8_t _filter);
void ChangeBRByTime(uint8_t from,uint8_t to,uint32_t timetick);
void stopAllStateTimer();
void tmrProcess();
void idleProcess();
void ChangeDeviceBR(uint32_t _br, uint8_t _ring);
void ResetNodeToRegister();

bool SendMyMessage();

#define SECOND_UNIT  1
#define MINUTE_UNIT  2
#define HOUR_UNIT    3

// All rings or the first ring
#define DEVST_OnOff             gConfig.ring[0].State
#define DEVST_Bright            gConfig.ring[0].BR
#define DEVST_WarmCold          gConfig.ring[0].CCT
#define DEVST_R                 gConfig.ring[0].R
#define DEVST_G                 gConfig.ring[0].G
#define DEVST_B                 gConfig.ring[0].B
#define DEVST_W                 gConfig.ring[0].W

#define RING_ID_ALL             0
#define RING_ID_1               1
#define RING_ID_2               2
#define RING_ID_3               3

// Specific ring
#define RINGST_OnOff(rid)       gConfig.ring[(rid)].State
#define RINGST_Bright(rid)      gConfig.ring[(rid)].BR
#define RINGST_WarmCold(rid)    gConfig.ring[(rid)].CCT
#define RINGST_R(rid)           gConfig.ring[(rid)].R
#define RINGST_G(rid)           gConfig.ring[(rid)].G
#define RINGST_B(rid)           gConfig.ring[(rid)].B
#define RINGST_W(rid)           gConfig.ring[(rid)].W
#define RINGST_L1(rid)          gConfig.ring[(rid)].L1
#define RINGST_L2(rid)          gConfig.ring[(rid)].L2
#define RINGST_L3(rid)          gConfig.ring[(rid)].L3

#define IS_SUNNY(DevType)           ((DevType) >= devtypWRing3 && (DevType) <= devtypWRing1)
#define IS_RAINBOW(DevType)         ((DevType) >= devtypCRing3 && (DevType) <= devtypCRing1)
#define IS_MIRAGE(DevType)          ((DevType) >= devtypMRing3 && (DevType) <= devtypMRing1)
#define IS_VALID_REMOTE(DevType)    ((DevType) >= remotetypRFSimply && (DevType) <= remotetypRFEnhanced)

#endif /* __GLOBAL_H */
