#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Radios.h>
#include "LuminaActionBluetoothSwitch.h"

bool LuminaActionBluetoothSwitch::IsBluetoothEnabled() const
{
    if (m_IsShuttingDown)
    {
        return false;
    }
    try
    {
        auto radios = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync().get();
        for (const auto& radio : radios)
        {
            if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
            {
                return radio.State() == winrt::Windows::Devices::Radios::RadioState::On;
            }
        }
    }
    catch (const winrt::hresult_canceled&)
    {
        // Operation was canceled, this is expected during shutdown
    }
    catch (...)
    {
        // Fallback or error
    }
    return false;
}

void LuminaActionBluetoothSwitch::SetBluetoothEnabled(bool enabled)
{
    if (m_IsShuttingDown)
    {
        return;
    }
    try
    {
        auto radios = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync().get();
        for (const auto& radio : radios)
        {
            if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
            {
                try
                {
                    if (enabled)
                    {
                        radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::On).get();
                    }
                    else
                    {
                        radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::Off).get();
                    }
                }
                catch (const winrt::hresult_canceled&)
                {
                    // Operation was canceled, this is expected during shutdown
                }
                catch (...)
                {
                    // Handle any errors during radio state setting
                }
                break;
            }
        }
    }
    catch (const winrt::hresult_canceled&)
    {
        // Operation was canceled, this is expected during shutdown
    }
    catch (...)
    {
        // Fallback or error
    }
}