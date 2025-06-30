#pragma once
#include <vector>
#include <string>
#include <chrono>
#include <functional>
#include <winrt/Windows.Devices.Enumeration.h>
#include "LuminaDeviceManager.h" // For Lumina::BluetoothDevice

class LuminaActionDiscoverDevice {
public:
    LuminaActionDiscoverDevice();
    ~LuminaActionDiscoverDevice();

    void StartScan();
    void StopScan();
    bool GetIsScanning() const;
    void ScanAsync();
    void Tick();
    void StartScanWithGate();
    bool IsScanInProgress() const;
    float GetScanProgress() const;
    void UpdateScanState();
    void SetOnBeginScanCooldown(const std::function<void()>& cb) { m_OnBeginScanCooldown = cb; }
    void SetOnCompleteScanCooldown(const std::function<void()>& cb) { m_OnCompleteScanCooldown = cb; }
    bool CanRenderDeviceList() const { return m_CanRenderDeviceList; }

    // Set callback to report discovered devices
    void SetOnDevicesDiscovered(const std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)>& cb) { m_OnDevicesDiscovered = cb; }

private:
    bool m_IsScanning;
    int m_ScanTimeoutSeconds;
    std::chrono::steady_clock::time_point m_LastScanTime;
    bool m_ScanInProgress = false;
    std::chrono::steady_clock::time_point m_ScanStartTime;
    float m_ScanCooldown = 0.5f;
    std::function<void()> m_OnBeginScanCooldown;
    std::function<void()> m_OnCompleteScanCooldown;
    bool m_CanRenderDeviceList = true;
    bool m_IsShuttingDown = false;
    std::function<void(const std::vector<winrt::Windows::Devices::Enumeration::DeviceInformation>&)> m_OnDevicesDiscovered;
}; 