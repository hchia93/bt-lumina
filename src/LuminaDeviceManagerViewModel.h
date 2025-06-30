#pragma once
#include "LuminaDeviceManager.h"
#include <string>

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
    
    // UI helper methods
    void RenderDeviceDiscoveryTab();
    void RenderDeviceControlTab();
    void RenderDeviceList(const std::vector<Lumina::BluetoothDevice>& devices, const char* title);
    void RenderDeviceDetails(const Lumina::BluetoothDevice& device);
    void RenderDeviceActions(const Lumina::BluetoothDevice& device);
    void RenderScanControls();
    
    // UI event handlers
    void OnDeviceSelected(const std::string& deviceAddress);
    void OnConnectDevice(const std::string& deviceAddress);
    void OnDisconnectDevice(const std::string& deviceAddress);
    void OnRemoveDevice(const std::string& deviceAddress);
}; 