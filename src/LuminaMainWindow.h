#pragma once

#include "LuminaDeviceManagerViewModel.h"
#include "LuminaAbout.h"

class LuminaMainWindow
{
public:
    void Render();
    void ApplyImGuiStyle();

private:

    LuminaDeviceManagerViewModel m_DeviceManager;
    LuminaAbout m_About;
};