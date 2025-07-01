#pragma once
#include <string>

namespace Lumina
{
    struct BluetoothDevice
    {
        std::string name;
        std::string address;
        bool isConnected;
        bool isPaired;
        int signalStrength; // RSSI value. Not available in this API
        std::string deviceType; // Ideally should be enum after knowing all possible device type
    };
}