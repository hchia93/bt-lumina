#pragma once
#include "LuminaDeviceManager.h"
#include <string>
#include <optional>
#include "LuminaDevicePropertyViewModel.h"
#include "LuminaHelper.h"
#include "LuminaActionBluetoothSwitch.h"
#include "LuminaActionDiscoverDevice.h"

// Forward declare the new class
class LuminaActionBluetoothSwitch;

class LuminaDeviceManagerViewModel
{
public:
    LuminaDeviceManagerViewModel();
    ~LuminaDeviceManagerViewModel();

    void Render();

private:
    LuminaDeviceManager m_DeviceManager;
    bool m_ShowDeviceDetails;
    std::string m_SelectedDeviceAddress;
    LuminaDevicePropertyViewModel m_PropertyViewModel;
    LuminaActionBluetoothSwitch m_BluetoothSwitch;
    LuminaActionDiscoverDevice m_DiscoverDevice;
    
    // UI helper methods
    void RenderDeviceDiscoveryTab();
    void RenderDeviceList(const std::vector<Lumina::BluetoothDevice>& devices, const char* title);
    void RenderDeviceDetails(const Lumina::BluetoothDevice& device);
    void RenderDeviceActions(const Lumina::BluetoothDevice& device);
    void RenderActionList();
    // UI event handlers
    void OnDeviceSelected(const std::string& deviceAddress);
    void OnConnectDevice(const std::string& deviceAddress);
    void OnDisconnectDevice(const std::string& deviceAddress);
    void OnRemoveDevice(const std::string& deviceAddress);
};