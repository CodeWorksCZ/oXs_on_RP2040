#pragma once

//#include <Arduino.h>
#include "config_basic.h"
//#include "oXs_config_advanced.h"
//#include "oXs_config_macros.h"

// from the UBlox6 document, the largest payout we receive i the NAV-SVINFO and the payload size
// is calculated as 8 + 12*numCh.  numCh in the case of a Glonass receiver is 28.
#define UBLOX_PAYLOAD_SIZE 344 // 344 is the absolute max size 
#define UBLOX_BUFFER_SIZE 50 // but the message that we read should not exceed about 50 bytes

// UBX support
//typedef struct { // not used I think
//    uint8_t preamble1;
//    uint8_t preamble2;
//    uint8_t msg_class;
//    uint8_t msg_id;
//    uint16_t length;
//} ubx_header;

typedef struct {
    uint32_t time;              // GPS msToW
    int32_t longitude;                 // in degree with 7 decimals
    int32_t latitude;                  // in degree with 7 decimals 
    int32_t altitude_ellipsoid;        // in mm
    int32_t altitude_msl;              // in mm
    uint32_t horizontal_accuracy;
    uint32_t vertical_accuracy;        // in mm
} ubx_nav_posllh;

typedef struct {
    uint32_t time;              // GPS msToW
    uint8_t fix_type;
    uint8_t fix_status;
    uint8_t differential_status;
    uint8_t res;
    uint32_t time_to_first_fix;
    uint32_t uptime;            // milliseconds
} ubx_nav_status;

typedef struct {
    uint32_t time;
    int32_t time_nsec;
    int16_t week;
    uint8_t fix_type;
    uint8_t fix_status;
    int32_t ecef_x;
    int32_t ecef_y;
    int32_t ecef_z;
    uint32_t position_accuracy_3d;
    int32_t ecef_x_velocity;
    int32_t ecef_y_velocity;
    int32_t ecef_z_velocity;
    uint32_t speed_accuracy;
    uint16_t position_DOP;
    uint8_t res;
    uint8_t satellites;
    uint32_t res2;
} ubx_nav_solution;

typedef struct {
    uint32_t time;              // GPS msToW
    int32_t ned_north;
    int32_t ned_east;
    int32_t ned_down;
    uint32_t speed_3d;
    uint32_t speed_2d;
    int32_t heading_2d;
    uint32_t speed_accuracy;
    uint32_t heading_accuracy;
} ubx_nav_velned;

typedef struct {
    uint8_t chn;                // Channel number, 255 for SVx not assigned to channel
    uint8_t svid;               // Satellite ID
    uint8_t flags;              // Bitmask
    uint8_t quality;            // Bitfield
    uint8_t cno;                // Carrier to Noise Ratio (Signal Strength) // dbHz, 0-55.
    uint8_t elev;               // Elevation in integer degrees
    int16_t azim;               // Azimuth in integer degrees
    int32_t prRes;              // Pseudo range residual in centimetres
} ubx_nav_svinfo_channel;

typedef struct {
    uint32_t time;              // GPS Millisecond time of week
    uint8_t numCh;              // Number of channels
    uint8_t globalFlags;        // Bitmask, Chip hardware generation 0:Antaris, 1:u-blox 5, 2:u-blox 6
    uint16_t reserved2;         // Reserved
    ubx_nav_svinfo_channel channel[16];         // 16 satellites * 12 byte
} ubx_nav_svinfo;

// GPS codes that could be used
#define    PREAMBLE1  0xb5
#define    PREAMBLE2  0x62
#define    CLASS_NAV  0x01
#define    CLASS_ACK  0x05
#define    CLASS_CFG  0x06
#define    MSG_ACK_NACK  0x00
#define    MSG_ACK_ACK  0x01
#define    MSG_POSLLH  0x2
#define    MSG_STATUS  0x3
#define    MSG_SOL  0x6
#define    MSG_VELNED  0x12
#define    MSG_SVINFO  0x30
#define    MSG_CFG_PRT  0x00
#define    MSG_CFG_RATE  0x08
#define    MSG_CFG_SET_RATE  0x01
#define    MSG_CFG_NAV_SETTINGS  0x24

#define    FIX_NONE  0
#define    FIX_DEAD_RECKONING  1
#define    FIX_2D  2
#define    FIX_3D  3
#define    FIX_GPS_DEAD_RECKONING  4
#define    FIX_TIME  5

#define    NAV_STATUS_FIX_VALID  1


class GPS {
public:
    // **********************
    // GPS data being read
    // **********************
    int32_t GPS_lon;               // longitude in degree with 7 decimals, (neg for S)
    bool    GPS_lonAvailable = false ; 
    int32_t GPS_lat;               // latitude   in degree with 7 decimals, (neg for ?)
    bool    GPS_latAvailable = false ;

    int32_t GPS_altitude;              // altitude in mm
    bool    GPS_altitudeAvailable = false ;
    uint16_t GPS_speed_3d;                 // speed in cm/s
    bool    GPS_speed_3dAvailable = false;
    uint16_t GPS_speed_2d;                 // speed in cm/s
    bool    GPS_speed_2dAvailable = false ;
    uint32_t GPS_ground_course ;     // degrees with 5 decimals
    bool    GPS_ground_courseAvailable = false ;

    uint8_t GPS_numSat;
    uint8_t GPS_fix_type;
    uint16_t GPS_hdop = 9999;           // Compute GPS quality signal
    uint16_t GPS_packetCount = 0;

    // *********** GPS calculated data
    int16_t GPS_distance ;   // distance from home (first location) in m
    int16_t GPS_heading ;          // heading from home (in Rad)
    int32_t GPS_home_lat ;         // position of home in degre with 7 decimals
    int32_t GPS_home_lon ;         // position of home in degre with 7 decimals
    float GPS_scale ;     // scaling factor to calculate the distance depending on latitude
    int16_t GPS_bearing ;          // bearing from home in degrees

    bool GPS_fix = false ; // true if gps data are available.
    bool new_position = false ;
    bool new_speed = false ;
      
    // Receive buffer
    union {
        ubx_nav_posllh posllh;
        ubx_nav_status status;
        ubx_nav_solution solution;
        ubx_nav_velned velned;
        ubx_nav_svinfo svinfo;
        uint8_t bytes[UBLOX_BUFFER_SIZE];
    } _buffer;



    explicit GPS(void);
    void setupGps() ;
    void readGps();
    bool UBLOX_parse_gps(void) ;
    bool gpsNewFrameUBLOX(uint8_t data) ;
  
private:
    uint16_t gpsDataErrors;

    uint8_t _ck_a;// Packet checksum accumulators
    uint8_t _ck_b;// Packet checksum accumulators
    bool _skip_packet = false ;
    uint8_t _step;
    uint8_t _class;
    uint16_t _payload_length;
    uint16_t _payload_counter;
    uint8_t _msg_id; //used to identify the message type when reading the gps, is used also when buffer is parsed 

    #if defined( A_LOCATOR_IS_CONNECTED)  && ( A_LOCATOR_IS_CONNECTED == YES)
    uint32_t GPS_last_fix_millis ; // time when last fix has been received (used by lora locator) 
    int32_t GPS_last_fix_lon ;     // last lon when a fix has been received
    int32_t GPS_last_fix_lat ;     // last lat when a fix has been received
    #endif

};