#pragma once

#include "LuminaLayoutHelper.h"
#include "LuminaDeviceManagerTab.h"
#include "LuminaAbout.h"

class LuminaMainWindow
{
public:
	void Render();

private:
	LuminaAbout m_About;
	LuminaDeviceManagerTab m_DeviceManager;

	// State for Device Control tab
	float brightness = 0.5f;
	float color[3] = {1.0f, 1.0f, 1.0f};
	// State for Settings tab
	bool auto_connect = false;
	int scan_timeout = 10;

	bool bShowAbout = false;
};