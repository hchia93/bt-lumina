#pragma once
#include "LuminaDeviceManager.h"
#include <string>

class LuminaDeviceManagerTab
{
public:
    LuminaDeviceManagerTab();
    ~LuminaDeviceManagerTab();

    void Render();

private:
    LuminaDeviceManager deviceManager;
    
    // UI state variables
    bool showDeviceDetails;
    std::string selectedDeviceAddress;
    char deviceNameBuffer[256];
    char deviceAddressBuffer[256];
    char deviceTypeBuffer[256];
    
    // UI helper methods
    void RenderDeviceDiscoveryTab();
    void RenderDeviceControlTab();
    void RenderDeviceList(const std::vector<BluetoothDevice>& devices, const char* title);
    void RenderDeviceDetails(const BluetoothDevice& device);
    void RenderDeviceActions(const BluetoothDevice& device);
    void RenderScanControls();
    void RenderDeviceForm();
    
    // UI event handlers
    void OnDeviceSelected(const std::string& deviceAddress);
    void OnConnectDevice(const std::string& deviceAddress);
    void OnDisconnectDevice(const std::string& deviceAddress);
    void OnRemoveDevice(const std::string& deviceAddress);
    void OnAddDevice();
}; 