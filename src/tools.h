#pragma once

#include "pico/stdlib.h"
//#include "Button2.h" moved to Button2.cpp
//#include "stdio.h"

//#define SPORT_TYPES_MAX 24 // = NUMBER_MAX_IDX

struct field {
    int32_t value;
    bool available;
    bool onceAvailable;
    uint32_t nextMillis;
    uint16_t sportInterval; // msec
    uint8_t sportDeviceId;
    uint16_t sportFieldId;
} ;



enum fieldIdx {     // Internal Id for the measurements stored in oXs and that can be sent (some in a different format/field)
      LATITUDE =0,  //  GPS special format
      LONGITUDE,    //  GPS special format
      GROUNDSPEED , //  GPS cm/s
      HEADING,      //  GPS 0.01 degree
      ALTITUDE ,    //  GPS cm

      NUMSAT ,      //  5 GPS no unit   
      GPS_DATE ,    // GPS special format AAMMJJFF
      GPS_TIME ,    // GPS special format HHMMSS00
      GPS_PDOP ,    // GPS no unit
      GPS_HOME_BEARING, // GPS degree

      GPS_HOME_DISTANCE, // 10 GPS  in m
      MVOLT,        // volt1   in mVolt
      CURRENT,  // volt2 must be in seq for voltage.cpp in mA (mV)
      RESERVE1, // volt3 must be in seq for voltage.cpp in mV
      RESERVE2, // volt4 must be in seq for voltage.cpp in mV
      
      CAPACITY,    // based on current (volt2) in mAh
      TEMP1,       // = Volt3 but saved as temp in degree
      TEMP2,       // = Volt4 but saved as temp in degree
      VSPEED,      // baro       in cm/s
      RELATIVEALT , // baro      in cm
      
      PITCH,       // 20 imu        in degree 
      ROLL,        // imu           in degree
      YAW ,        // not used to save data  in degree
      RPM ,        // RPM sensor    in Herzt
      NUMBER_MAX_IDX, // used to count the number of entries  24
};


uint32_t millis() ;

uint32_t micros();

void waitUs(uint32_t delayUs);

bool __no_inline_not_in_flash_func(get_bootsel_button)();

typedef struct
{
    uint8_t type;
    int32_t data;
} queue_entry_t;

void sent2Core0( uint8_t fieldType, int32_t value);

void enlapsedTime(uint8_t idx);
void startTimerUs(uint8_t idx); 
void getTimerUs(uint8_t idx);