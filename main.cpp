/*
 * Copyright (c) 2020 Arm Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"
#include "uop_msb.h"
#include "rtos/ThisThread.h"
#include "NTPClient.h"
#include "azure_c_shared_utility/xlogging.h"
#include <cstring>
#include <string.h>
#include "date.hpp"
#include "sampling.h"

using namespace uop_msb;

extern void azureDemo();
extern NetworkInterface *_defaultSystemNetwork;

class sensordetect {
      
};

void LCD_update();

Thread nwrkThread;                                      //Thread network
Thread t1;												//writing to the SD card
Thread t2;												//communicating with the serial interface
Thread t3;												//communicating with the network
Thread t4;												//LCD REFRESH
Thread t5(osPriorityISR);					            //HIGHEST PRIORITY sample loop function

bool connect()
{
    LogInfo("Connecting to the network");

    _defaultSystemNetwork = NetworkInterface::get_default_instance();
    if (_defaultSystemNetwork == nullptr) {
        LogError("No network interface found");
        return false;
    }

    int ret = _defaultSystemNetwork->connect();
    if (ret != 0) {
        LogError("Connection error: %d", ret);
        return false;
    }
    LogInfo("Connection success, MAC: %s", _defaultSystemNetwork->get_mac_address());
    return true;
}

bool setTime()
{
    LogInfo("Getting time from the NTP server");

    NTPClient ntp(_defaultSystemNetwork);
    ntp.set_server("time.google.com", 123);
    time_t timestamp = ntp.get_timestamp();
    if (timestamp < 0) {
        LogError("Failed to get the current time, error: %ud", timestamp);
        return false;
    }
    LogInfo("Time: %s", ctime(&timestamp));
    set_time(timestamp);
    return true;
}

//MAIN CPP

int main() {

    UOP_MSB_TASK1  sensor; //Class for sensor 
    sensor.test();         //Task 1 test


    if (!connect()) return -1;

    if (!setTime()) return -1;

    // The two lines below will demonstrate the features on the MSB. See uop_msb.cpp for examples of how to use different aspects of the MSB
    // UOP_MSB_TEST board;  //Only uncomment for testing - DO NOT USE OTHERWISE
    // board.test();        //Only uncomment for testing - DO NOT USE OTHERWISE

    // Write fake data to Azure IoT Center. Don't forget to edit azure_cloud_credentials.h
    printf("You will need your own connection string in azure_cloud_credentials.h\n");
    LogInfo("Starting the Demo");
    azureDemo();
    LogInfo("The demo has ended");

    return 0;
}
