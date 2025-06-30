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

    auto op = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync();
    op.Completed([this, callback](auto&& asyncOp, auto&& status) {
        bool result = false;
        if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
        {
            try
            {
                auto radios = asyncOp.GetResults();
                for (const auto& radio : radios)
                {
                    if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                    {
                        result = (radio.State() == winrt::Windows::Devices::Radios::RadioState::On);
                        break;
                    }
                }
            }
            catch (...)
            {
                // handle error
            }
        }
        m_IsBluetoothEnabled = result;
        if (callback) callback(result);
    });
}

void LuminaActionBluetoothSwitch::SetBluetoothEnabledAsync(bool enabled, std::function<void()> callback)
{
    auto op = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync();
    op.Completed([this, enabled, callback](auto&& asyncOp, auto&& status) {
        if (status == winrt::Windows::Foundation::AsyncStatus::Completed)
        {
            try
            {
                auto radios = asyncOp.GetResults();
                for (const auto& radio : radios)
                {
                    if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                    {
                        auto setOp = radio.SetStateAsync(enabled ? winrt::Windows::Devices::Radios::RadioState::On : winrt::Windows::Devices::Radios::RadioState::Off);
                        setOp.Completed([this, enabled, callback](auto&&, auto&& setStatus) {
                            if (setStatus == winrt::Windows::Foundation::AsyncStatus::Completed)
                            {
                                m_IsBluetoothEnabled = enabled;
                            }
                            if (callback) callback();
                        });
                        return;
                    }
                }
            }
            catch (...)
            {
                // handle error
                if (callback) callback();
            }
        }
        else
        {
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