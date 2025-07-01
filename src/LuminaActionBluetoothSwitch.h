#pragma once
#include <winrt/Windows.Devices.Radios.h>
#include <optional>
#include <atomic>
#include <functional>

class LuminaActionBluetoothSwitch
{
public:
    LuminaActionBluetoothSwitch();
    LuminaActionBluetoothSwitch(const LuminaActionBluetoothSwitch&) = delete;
    LuminaActionBluetoothSwitch& operator=(const LuminaActionBluetoothSwitch&) = delete;

    // Polling from cache.
    bool GetIsBluetoothEnabled();
    bool GetIsStateRequested() const;

    // State management methods
    void RequestGetIsBluetoothEnabled();
    void RequestToogleBluetoothEnabled();

    void HandleOnErrorMessage(const std::function<void(const std::string&)>& callback) { m_OnErrorMessageGenerated = callback; }

private:
    // tracks async state
    std::atomic<bool> m_Requested = false;

    std::optional<bool> m_IsBluetoothEnabled;
    
    std::function<void(const std::string&)> m_OnErrorMessageGenerated;

    void GetIsBluetoothEnabledAsync_Internal();
    void SetBluetoothEnabledAsync_Internal();
};