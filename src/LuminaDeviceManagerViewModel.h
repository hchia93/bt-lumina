#pragma once
#include "LuminaDeviceManager.h"
#include <string>
#include <optional>
#include "LuminaDevicePropertyViewModel.h"
#include "LuminaHelper.h"
#include "LuminaActionBluetoothSwitch.h"
#include "LuminaActionDiscoverDevice.h"
#include "LuminaErrorMessageInfo.h"

class LuminaDeviceManagerViewModel
{
public:
    LuminaDeviceManagerViewModel();

    void Render();
    void RaiseErrorMessage(const std::string& message);

private:
    LuminaDeviceManager m_DeviceManager;

    LuminaActionBluetoothSwitch m_ActionBluetoothSwitch;
    LuminaActionDiscoverDevice m_ActionDiscoverDevice;

    bool m_ShowDeviceDetails;
    std::string m_SelectedDeviceAddress;
    LuminaDevicePropertyViewModel m_PropertyViewModel;

    LuminaErrorMessageInfo m_ErrorMessageInfo;

    void BindActionOnDeviceDiscovered();

    // UI helper methods
    void RenderDeviceTable();
    void RenderDeviceEntry(const Lumina::BluetoothDevice& devices);
    void RenderDeviceDetails(const Lumina::BluetoothDevice& device);
    void RenderDeviceActions(const Lumina::BluetoothDevice& device);
    void RenderActionList();
    // UI event handlers
    void OnDeviceSelected(const std::string& deviceAddress);
    void OnConnectDevice(const std::string& deviceAddress);
    void OnDisconnectDevice(const std::string& deviceAddress);
    void OnRemoveDevice(const std::string& deviceAddress);
};