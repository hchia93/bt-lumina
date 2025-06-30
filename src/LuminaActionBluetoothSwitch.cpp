#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.System.Threading.h>
#include "LuminaActionBluetoothSwitch.h"

bool LuminaActionBluetoothSwitch::GetIsBluetoothEnabled()
{
    return m_IsBluetoothEnabled.value_or(false);
}

void LuminaActionBluetoothSwitch::GetIsBluetoothEnabledAsync(std::function<void(bool)> callback)
{
    if (m_IsBluetoothEnabled.has_value())
    {
        if (callback)
        {
            callback(m_IsBluetoothEnabled.value());
        }
        return;
    }

    // Run the async operation on a background thread
    winrt::Windows::System::Threading::ThreadPool::RunAsync([this, callback](auto&&)
        {
            try
            {
                auto radios = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync().get();
                bool result = false;
                for (const auto& radio : radios)
                {
                    if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                    {
                        result = (radio.State() == winrt::Windows::Devices::Radios::RadioState::On);
                        break;
                    }
                }

                // Update the cached value and call the callback
                m_IsBluetoothEnabled = result;
                if (callback)
                {
                    callback(result);
                }
            }
            catch (...)
            {
                // Handle any exceptions and call callback with false
                m_IsBluetoothEnabled = false;
                if (callback)
                {
                    callback(false);
                }
            }
        });
}

void LuminaActionBluetoothSwitch::SetBluetoothEnabledAsync(bool enabled, std::function<void()> callback)
{
    // Run the async operation on a background thread
    winrt::Windows::System::Threading::ThreadPool::RunAsync([this, enabled, callback](auto&&)
        {
            try
            {
                auto radios = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync().get();
                for (const auto& radio : radios)
                {
                    if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                    {
                        if (enabled)
                        {
                            radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::On).get();
                        }
                        else
                        {
                            radio.SetStateAsync(winrt::Windows::Devices::Radios::RadioState::Off).get();
                        }
                        m_IsBluetoothEnabled = enabled;
                        break;
                    }
                }

                if (callback) callback();
            }
            catch (...)
            {
                // Handle any exceptions and still call the callback
                if (callback) callback();
            }
        });
}

void LuminaActionBluetoothSwitch::FetchBluetoothEnabledState()
{
    if (!m_Requesting && !m_IsBluetoothEnabled.has_value())
    {
        m_Requesting = true;
        GetIsBluetoothEnabledAsync([this](bool enabled)
            {
                OnStateReceived(enabled);
            });
    }
}

bool LuminaActionBluetoothSwitch::GetIsStateRequested() const
{
    return m_Requesting;
}

void LuminaActionBluetoothSwitch::OnStateReceived(bool enabled)
{
    m_IsBluetoothEnabled = enabled;
    m_Requesting = false;
}