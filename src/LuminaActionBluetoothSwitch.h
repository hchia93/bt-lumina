#pragma once
#include <winrt/Windows.Devices.Radios.h>

class LuminaActionBluetoothSwitch
{
public:
    LuminaActionBluetoothSwitch() = default;
    ~LuminaActionBluetoothSwitch() = default;

    bool IsBluetoothEnabled() const;
    void SetBluetoothEnabled(bool enabled);

private:
    bool m_IsShuttingDown = false;
};