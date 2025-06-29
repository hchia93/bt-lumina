#pragma once

#include "LuminaDeviceManagerTab.h"
#include "LuminaAbout.h"

class LuminaMainWindow
{
public:
	void Render();
	void ApplyImGuiStyle();

private:

	LuminaDeviceManagerTab m_DeviceManager;
	LuminaAbout m_About;
};