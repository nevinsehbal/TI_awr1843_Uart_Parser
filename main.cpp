
// AWR1843 UART Parser

#include <bits/stdc++.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <termios.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <inttypes.h>
#include <math.h>
#include <chrono>
#include <iomanip>
#include <unistd.h>
#include <errno.h>
#include "AWR1843.hpp"

awr1843::AWR1843 myRadar;

int32_t main() { 

    /* Initialize UART */

    while(true) {
        if(myRadar.initUart() == -1) {
            std::cout << "Retrying to connect..." << std::endl;
            sleep(1);
        }
        else {
            std::cout << std::endl << "Successfully connected to all devices." << std::endl;
            break;
        }
    }
    myRadar.cycleRadarRead();
    return 0; 
}