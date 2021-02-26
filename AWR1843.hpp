
#ifndef AWR1843_HPP
#define AWR1843_HPP

////////////////////////////////////////////////////////////////////////////////////////////////////////
//                                                                                                    //
//    Data Format for AWR1843 Log Configuration                                                       //
//    =====================================================================================           //
//    Information per message:                                                                        //
//    Packet: Frame Header(40 bytes) + TLVs(type:1,2,3,4 number:N) + Package end(0F0F0F.. format)     //
//                                                                                                    //
//    1)Frame Header (40 bytes)                                                                       //
//        1.1) Header synchronization: 8 bytes (uint64_t ,'0201040306050807')                         //
//        1.2) Header version: 4 bytes (uint32_t)                                                     //
//        1.3) Total Packet Length: 4 bytes (uint32_t)                                                //
//        1.4) Header Platform: 4 bytes (uint32_t)                                                    //
//        1.5) Header Frame Number: 4 bytes (uint32_t)                                                //
//        1.6) Header Time Cpu Cycles 4 bytes (uint32_t)                                              //
//        1.7) Total # Detected Objects: 4 bytes (uint32_t)                                           //
//        1.8) Total # TLVs: 4 bytes (uint32_t)                                                       //
//        1.9) Total # Subframes: 4 bytes (uint32_t)                                                  //
//                                                                                                    //
//    2) #N TLVs (N times iterating packet for each TLV)                                              //
//        2.1) TLV Type: 4 bytes (uint32_t)                                                           //
//              Types --> 1:DetectedObj, 2:ClusterOutput, 3:TrackingOutput, 4:BlankOutput             //
//        2.2) TLV Length: 4 bytes (uint32_t) (Sum of 2.3 + 2.4 + 2.5)                                //
//        2.3) # Objects in the TLV: 2 bytes (uint16_t)                                               //
//        2.4) Unknown data: 2 bytes (uint16_t, '0700'), probably Q7 format                           //
//        2.5) Output points: Type varying bytes                                                      //
//              for type-1, DetectedObj     (#obj*10) bytes (uint16_t)                                //
//                  type-2, ClusterOutput   (#obj*8) bytes (uint16_t)                                 //
//                  type-3, TrackingOutput  (#obj*12) bytes (uint16_t)                                //
//                  type-4, independent from #objects: 64 bytes (uint64_t)                            //
//                                                                                                    //
//    3) Package End: (Total Packet Length-[Part_1+Part_2]) bytes (uint8_t)                           //
//        Format: ('0F0F0F0F.....')                                                                   //
//                                                                                                    //
//        Note: These values are little endian.                                                       //
////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <sys/types.h>
#include <inttypes.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <thread>
#include <errno.h>
#include <termios.h>
#include <fcntl.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <iomanip>

#define AWR1843_BOOST

/* UART Definitions */
    static const char RADAR_DATA[]          = "/dev/ttyACM1";
    static const char LOG_FILE_LOCATION[]   = "/home/shelectra/Desktop/log";

#define BAUDRATE                B921600

/**
 * Local functions in support of the shell awr1843.
 */
namespace awr1843
{
    /* Class Definition */
    class AWR1843
    {
        private:

            std::vector<uint8_t> packetvec;
            int bytescounter;

            enum PARSE_STATE {
                AWR1843_PARSE_STATE0_UNSYNC = 0,
                magic02=1,
                magic01=2,
                magic04=3,
                magic03=4,
                magic06=5,
                magic05=6,
                magic08=7,
                magic07=8,
                AWR1843_PARSE_STATE1_GOT_SYNC = 9,
            };

            struct Header{
                int version;
                int totalPacketLen;
                int platform;
                int frameNumber;
                int timeCpuCycles;
                int numDetectedObj;
                int numTLVs;
                int subFrameNumber;
            } Header;

            struct detectedObjects{
                    float doppler;
                    float peakVal;
                    float x;
                    float y;
                    float z;
                }; 
                struct clusterOutput{
                    float xcenter;
                    float ycenter;
                    float xsize;
                    float ysize;
                }; 
                struct trackingOutput{
                    float xcenter;
                    float ycenter;
                    float xvelocity;
                    float yvelocity;
                    float xsize;
                    float ysize;
                };

            struct Tlv{
                int tlvType;
                int tlvLen;
                int numofObj;
                detectedObjects detobj;
                clusterOutput clusterout;
                trackingOutput trackout;
                
            } Payload;

            /* Definitions */
            enum PARSE_STATE state;
            int32_t             bytes_read;       
            struct termios      oldtioRadarData;
            struct termios      newtioRadarData; 
            int32_t             deviceRadarData;

            /* Functions */
            int32_t             initUart();
            int32_t             initDevicePort(int32_t* device, char* deviceLocation, struct termios* oldtio, struct termios* newtio);
            int32_t             cycleRadarRead();
            int32_t             parseRadarPackage(uint8_t c);
            int                 parserfunc(std::vector<uint8_t>&vect);
            int                 parseTlv(int index, std::vector<uint8_t>vect);
            float               read_two_bytes(unsigned char a, unsigned char b, int Endian);
            float               read_four_bytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int Endian);


            int32_t disp_Package(uint8_t &array);
            void                closeConnection(int32_t* serial_port,  struct termios* oldtio);



        public:
            AWR1843();
            ~AWR1843();
     
            int32_t             initUart();
            int32_t             cycleRadarRead();
            int32_t             writeCsvLine();
            void                closeConnection(int32_t* serial_port,  struct termios* oldtio);
            int32_t disp_Package(uint8_t &array);

    };
}

#endif