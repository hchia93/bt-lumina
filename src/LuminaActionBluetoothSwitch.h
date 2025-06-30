#pragma once
#include <winrt/Windows.Devices.Radios.h>
#include <optional>
#include <functional>

class LuminaActionBluetoothSwitch
{
public:

    // Polling from cache.
    bool GetIsBluetoothEnabled();
    
    // State management methods
    void FetchBluetoothEnabledState();
    bool GetIsStateRequested() const;

    // Async calls to get/set Bluetooth state
    void SetBluetoothEnabledAsync(bool enabled, std::function<void()> callback = nullptr);

private:
    std::optional<bool> m_IsBluetoothEnabled;
    
    // Fetch Bluetooth state
    bool m_Requesting = false;
    void GetIsBluetoothEnabledAsync(std::function<void(bool)> callback);
    void OnStateReceived(bool enabled);
};