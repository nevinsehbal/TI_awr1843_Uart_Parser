// Nevin Sehbal Soyuslu 
// Shelectra

// AWR1843 UART Parser

#include <sys/types.h>
#include <inttypes.h>
#include <bits/stdc++.h>
#include <sys/stat.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <thread>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <fcntl.h> // Contains file controls like O_RDWR
#include <unistd.h> // write(), read(), close()
#include "AWR1843.hpp"

namespace awr1843
{ 
    AWR1843::AWR1843():
        deviceRadarData(-1),
        bytescounter(0),
        state(AWR1843_PARSE_STATE0_UNSYNC) {
        }
    AWR1843::~AWR1843()
    {
        closeConnection(&deviceRadarData, &oldtioRadarData);
        //close connections.
    }

    float AWR1843::read_four_bytes(unsigned char a, unsigned char b, unsigned char c, unsigned char d, int Endian){
        //NOTE THAT data our data is in Little Endian
        // 1 for Big Endian, 0 for Little Endian
        int value;
        if (Endian){value = (a << 24) | (b << 16) | (c << 8) | d;}
        else{value = (d << 24) | (c << 16) | (b << 8) | a;}
        return value;
        }

    float AWR1843::read_two_bytes(unsigned char a, unsigned char b, int Endian){
        //NOTE THAT data our data is in Little Endian
        // 1 for Big Endian, 0 for Little Endian
        int value;
        if (Endian){value = (a << 8) | b;}
        else{value = (b << 8) | a;}
        return value;
        }   
    int AWR1843::parseTlv(int idx, std::vector<uint8_t>vect){
        int ret=0;
        tlvbegin:
        std::cout<<"\nchecking tlv...\n";
        //std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx]<<" *****\n";
            Payload.tlvType=read_four_bytes(vect[idx],vect[idx+1],vect[idx+2],vect[idx+3],0);
            Payload.tlvLen = read_four_bytes(vect[idx+4],vect[idx+5],vect[idx+6],vect[idx+7],0);
            Payload.numofObj = read_two_bytes(vect[idx+8],vect[idx+9],0);
            //  std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx]<<" ";
            //  std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx+1]<<" ";
            //  std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx+2]<<" ";
            //  std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx+3]<<" \n";
            idx=idx+12;
            //std::cout<<Payload.tlvType<<"\n";
            if(Payload.tlvType==1){
                std::cout<<"\nType: Detected Objects\n";
                for(int i=0; i<Payload.numofObj; i++){
                    Payload.detobj.doppler=read_two_bytes(vect[idx],vect[idx+1],0);
                    // Payload.detobj.peakVal=read_two_bytes(vect[idx+2],vect[idx+3],0);
                    Payload.detobj.x=read_two_bytes(vect[idx+4],vect[idx+5],0);
                    Payload.detobj.y=read_two_bytes(vect[idx+6],vect[idx+7],0);
                    Payload.detobj.z=read_two_bytes(vect[idx+8],vect[idx+9],0);
                    idx=idx+10;
                    //std::cout<<Payload.detobj.doppler<<" "<<Payload.detobj.x<<"  ";
                    //std::cout<<Payload.detobj.y<<"  "<<Payload.detobj.z<<"  \n";

                    std::cout<<"Object #"<<(i+1)<<" \n";
                    std::cout<<"Velocity(m/s): "<<Payload.detobj.doppler<<std::endl;
                    // std::cout<<"Peak Value: "<<Payload.detobj.peakVal<<std::endl;
                    std::cout<<"X coordinate(m): "<<Payload.detobj.x*pow(10,-4)<<std::endl;
                    std::cout<<"Y coordinate(m): "<<Payload.detobj.y*pow(10,-4)<<std::endl;
                    std::cout<<"Z coordinate(m): "<<Payload.detobj.z*pow(10,-4)<<std::endl;
                    std::cout<<std::endl;
                }
                goto tlvbegin;
            }
            if(Payload.tlvType==2){
                std::cout<<"\nType: Cluster Objects\n";
                for(int i=0; i<Payload.numofObj; i++){
                    Payload.clusterout.xcenter=read_two_bytes(vect[idx],vect[idx+1],0);
                    Payload.clusterout.ycenter=read_two_bytes(vect[idx+2],vect[idx+3],0);
                    Payload.clusterout.xsize=read_two_bytes(vect[idx+4],vect[idx+5],0);
                    Payload.clusterout.ysize=read_two_bytes(vect[idx+6],vect[idx+7],0);
                    idx=idx+8;

                    std::cout<<"Object #"<<(i+1)<<" \n";
                    std::cout<<"X center(m): "<<Payload.clusterout.xcenter*pow(10,-4)<<std::endl;
                    std::cout<<"Y center(m) "<<Payload.clusterout.ycenter*pow(10,-4)<<std::endl;
                    std::cout<<"X size(m): "<<Payload.clusterout.xsize*pow(10,-4)<<std::endl;
                    std::cout<<"Y size(m): "<<Payload.clusterout.ysize*pow(10,-4)<<std::endl;
                    std::cout<<std::endl;
                }
                goto tlvbegin;
            }
            if(Payload.tlvType==3){
                std::cout<<"\nType: Tracking Output\n";
                for(int i=0; i<Payload.numofObj; i++){
                    Payload.trackout.xcenter=read_two_bytes(vect[idx],vect[idx+1],0);
                    Payload.trackout.ycenter=read_two_bytes(vect[idx+2],vect[idx+3],0);
                    Payload.trackout.xvelocity=read_two_bytes(vect[idx+4],vect[idx+5],0);
                    Payload.trackout.yvelocity=read_two_bytes(vect[idx+6],vect[idx+7],0);
                    Payload.trackout.xsize=read_two_bytes(vect[idx+8],vect[idx+9],0);
                    Payload.trackout.ysize=read_two_bytes(vect[idx+10],vect[idx+11],0);
                    idx=idx+12;

                    std::cout<<"Object #"<<(i+1)<<" \n";
                    std::cout<<"X center(m): "<<Payload.trackout.xcenter*pow(10,-4)<<std::endl;
                    std::cout<<"Y center(m): "<<Payload.trackout.ycenter*pow(10,-4)<<std::endl;
                    std::cout<<"X Velocity(m/s): "<<Payload.trackout.xvelocity*pow(10,-4)<<std::endl;
                    std::cout<<"Y Velocity(m/s): "<<Payload.trackout.yvelocity*pow(10,-4)<<std::endl;
                    std::cout<<"X size(m): "<<Payload.trackout.xsize*pow(10,-4)<<std::endl;
                    std::cout<<"Y size(m): "<<Payload.trackout.ysize*pow(10,-4)<<std::endl;
                    std::cout<<std::endl;     
                }
                goto tlvbegin;
            }
            if (Payload.tlvType == 4){
                std::cout<<"\nCurrent tlv is type-4 = NULL\n";
                idx=idx+66;
                goto tlvbegin;
            }
            if(vect[idx]==0x0F){
                while(vect[idx+1]==0x0F){
                    idx++;
                    //std::cout<<"\n";
                    //std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[idx+1]<<" ";
                }
                idx = idx+8;        
            }}
        
    int AWR1843::parserfunc(std::vector<uint8_t>&vect){   
        int size = vect.size();
        int idx=0;
        Header.version = read_four_bytes(vect[idx],vect[idx+1],vect[idx+2],vect[idx+3],0);
        Header.totalPacketLen = read_four_bytes(vect[idx+4],vect[idx+5],vect[idx+6],vect[idx+7],0);
        Header.platform = read_four_bytes(vect[idx+8],vect[idx+9],vect[idx+10],vect[idx+11],0);
        Header.frameNumber = read_four_bytes(vect[idx+12],vect[idx+13],vect[idx+14],vect[idx+15],0);
        Header.timeCpuCycles = read_four_bytes(vect[idx+16],vect[idx+17],vect[idx+18],vect[idx+19],0);
        Header.numDetectedObj = read_four_bytes(vect[idx+20],vect[idx+21],vect[idx+22],vect[idx+23],0);
        Header.numTLVs = read_four_bytes(vect[idx+24],vect[idx+25],vect[idx+26],vect[idx+27],0);
        Header.subFrameNumber = read_four_bytes(vect[idx+28],vect[idx+29],vect[idx+30],vect[idx+31],0);
        idx=idx+32; //beginning of the payload
        int err = parseTlv(idx,vect); 
        if (err==0){
            return 0;
        }
        //std::cout<<size<<" ";
        //for(int i=0; i<size; i++){
        //std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)vect[i]<<" ";
        //}
        return 0;
    }

    /*
    * Parses UART package. 
    *
    * Method outputs:
    *  -1: Received package is not valid
    *   0: Package not received yet
    *   1: Successfully received package
    */
    int32_t AWR1843::parseRadarPackage(uint8_t c)
    {
        int32_t ret = 0;
        int idxBuffer;
        //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)c << " ";
        switch (state) {
            case AWR1843_PARSE_STATE0_UNSYNC:
                {if(c == 0x02){
                    state=magic01;}
                }break;
            case magic01:
                {if(c == 0x01){state=magic04;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic04:
                {if(c == 0x04){state=magic03;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic03:
               { if(c == 0x03){state=magic06;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic06:
                {if(c == 0x06){state=magic05;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic05:
                {if(c == 0x05){state=magic08;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic08:
                {if(c == 0x08){state=magic07;}
                else {state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;
            case magic07:
                {if(c== 0x07){
                packetstart:
                std::cout<<"\nI've found magic key! Packet info:\n";
                packetvec.clear();
                int i=0;
                bytescounter=0;
                state=AWR1843_PARSE_STATE1_GOT_SYNC;}
                else{state = AWR1843_PARSE_STATE0_UNSYNC;}
                }break;

            case AWR1843_PARSE_STATE1_GOT_SYNC:
                {//std::cout<<bytescounter;
                //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)c << " ";
                packetvec.push_back(c);
                //std::cout<<std::hex << std::setfill('0') << std::setw(2)<<(int)packetvec[bytescounter]<<" ";
                bytescounter++;
                
                if(packetvec[bytescounter-1]== 0x07){
                if(packetvec[bytescounter-2]==0x08){
                if(packetvec[bytescounter-3]==0x05){
                if(packetvec[bytescounter-4]==0x06){
                if(packetvec[bytescounter-5]==0x03){
                if(packetvec[bytescounter-6]==0x04){
                if(packetvec[bytescounter-7]==0x01){
                if(packetvec[bytescounter-8]==0x02){
                    int packetend = parserfunc(packetvec); // calling parser function
                    if (packetend == 0){
                    std::cout<<"packet end\n\n";
                    goto packetstart; // allows iteration
                }}}}}}}}}}
                break;     
        }
       //unreachable up to here
        return ret;
    }

     /*
    * Inits UART device Port. 
    * Method outputs:
    *  -1: Fail.
    *   0: Succeess.
    */
    int32_t AWR1843::initDevicePort(int32_t* serial_port, char* deviceLocation, struct termios* oldtio, struct termios* newtio)
    {
        *serial_port = open(deviceLocation, O_RDWR | O_NOCTTY); 
        //std::cout<<*serial_port;

        if (*serial_port < 0) {
            std::cout<<"Error %i from open: %s\n", errno, strerror(errno); 
        }
        tcgetattr(*serial_port, oldtio); // save current port settings
        //configuration setup for the serial port
        bzero(newtio, sizeof(*newtio));
        newtio->c_cflag = BAUDRATE | CS8 | CLOCAL | CREAD; //set Control Modes (c_cflags) of tty
        newtio->c_iflag = IGNPAR; //set Input Modes (c_iflag) of tty
        newtio->c_oflag = 0; //set Output Modes (c_oflag) of tty
        newtio->c_lflag = 0; //set Local Modes (c_lflag) of tty
        newtio->c_cc[VTIME]    = 1;   /* inter-character timer unused */
        newtio->c_cc[VMIN]     = 1;   /* blocking read until that amount of chars received */
        tcflush(*serial_port, TCIFLUSH);
        if (tcsetattr(*serial_port, TCSANOW, newtio) != 0) {
            return -1; //error message 
        }
        else {return 0;}
    }
   
    /*
    * Inits UART device. 
    *
    * Method outputs:
    *  -1: Fail.
    *   0: Succsess.
    */
    int32_t AWR1843::initUart(){
    if (deviceRadarData < 0) {
            if(initDevicePort(&deviceRadarData, const_cast<char *>(RADAR_DATA), &oldtioRadarData, &newtioRadarData) == -1) {
                std::cout << "No radar data port connection." << std::endl;
                return -1;
            }
            else {
                std::cout << "Successfully connected to radar data port." << std::endl;
    }}
        return 0;
    }

    /*
    * Reads and parses UART package from the radar device.
    *
    * Method outputs:
    *  -1: Package error
    *   1: Success
    */
     int32_t AWR1843::cycleRadarRead()
    {
        uint8_t temp_byte = 0;
        //std::cout<<deviceRadarData;
        /* Read altitude information from radar */
        while(true) {
            
            bytes_read = read(deviceRadarData, &temp_byte, 1);
            //std::cout << std::hex << std::setfill('0') << std::setw(2) << (int)temp_byte << " ";
            
            if(bytes_read == 0) {
                return -1;
            }
            else {
                int32_t return_parse = parseRadarPackage(temp_byte);

                if (return_parse == 1) { // If the package was successfully received
    }}}}

   /*
    * Closes connection.
    */
    void AWR1843::closeConnection(int32_t* serial_port,  struct termios* oldtio)
    {
        tcsetattr(*serial_port, TCSANOW, oldtio);
        close(*serial_port);
    }}