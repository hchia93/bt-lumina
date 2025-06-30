#pragma once
#include <string>
#include "LuminaDeviceManager.h"

class LuminaDevicePropertyViewModel
{
public:
    LuminaDevicePropertyViewModel();
    void Show(const std::string& deviceAddress);
    void Hide();
    void Render(LuminaDeviceManager& deviceManager);
    bool IsVisible() const;
private:
    std::string m_DeviceAddress;
    bool m_Visible;
};