#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Devices.Radios.h>
#include <winrt/Windows.System.Threading.h>
#include "LuminaActionBluetoothSwitch.h"

LuminaActionBluetoothSwitch::LuminaActionBluetoothSwitch()
{

}

bool LuminaActionBluetoothSwitch::GetIsBluetoothEnabled()
{
    return m_IsBluetoothEnabled.value_or(false);
}

void LuminaActionBluetoothSwitch::GetIsBluetoothEnabledAsync_Internal()
{
    m_Requested = true;
    auto op = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync();
    op.Completed([this](auto&& asyncOp, auto&& status)
        {
            winrt::Windows::Foundation::Collections::IVectorView<winrt::Windows::Devices::Radios::Radio> radios;
            switch (status)
            {
            case winrt::Windows::Foundation::AsyncStatus::Completed:
                radios = asyncOp.GetResults();
                for (const auto& radio : radios)
                {
                    if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                    {
                        m_IsBluetoothEnabled = (radio.State() == winrt::Windows::Devices::Radios::RadioState::On);
                        break;
                    }
                }
                m_Requested = false;
                break;
            case winrt::Windows::Foundation::AsyncStatus::Canceled:
                if (m_OnErrorMessageGenerated)
                {
                    m_OnErrorMessageGenerated("Bluetooth state query was canceled.");
                }
                m_Requested = false;
                break;
            case winrt::Windows::Foundation::AsyncStatus::Error:
                if (m_OnErrorMessageGenerated)
                {
                    m_OnErrorMessageGenerated("Bluetooth state query failed.");
                }
                m_Requested = false;
                break;
            case winrt::Windows::Foundation::AsyncStatus::Started:
                break;
            default:
                m_Requested = false;
                break;
            }
        });
}

void LuminaActionBluetoothSwitch::SetBluetoothEnabledAsync_Internal()
{
    m_Requested = true;
    auto op = winrt::Windows::Devices::Radios::Radio::GetRadiosAsync();
    op.Completed([this](auto&& asyncOp, auto&& status)
        {
            switch (status)
            {
            case winrt::Windows::Foundation::AsyncStatus::Completed:
                try
                {
                    auto radios = asyncOp.GetResults();
                    for (const auto& radio : radios)
                    {
                        if (radio.Kind() == winrt::Windows::Devices::Radios::RadioKind::Bluetooth)
                        {
                            auto setOp = radio.SetStateAsync(!m_IsBluetoothEnabled.value() ? winrt::Windows::Devices::Radios::RadioState::On : winrt::Windows::Devices::Radios::RadioState::Off);
                            setOp.Completed([this](auto&&, auto&& setStatus)
                                {
                                    switch (setStatus)
                                    {
                                    case winrt::Windows::Foundation::AsyncStatus::Completed:
                                        m_IsBluetoothEnabled.value() = !m_IsBluetoothEnabled.value();
                                        m_Requested = false;
                                        break;
                                    case winrt::Windows::Foundation::AsyncStatus::Canceled:
                                        if (m_OnErrorMessageGenerated)
                                        {
                                            m_OnErrorMessageGenerated("Bluetooth toggle was canceled.");
                                        }
                                        m_Requested = false;
                                        break;
                                    case winrt::Windows::Foundation::AsyncStatus::Error:
                                        if (m_OnErrorMessageGenerated)
                                        {
                                            m_OnErrorMessageGenerated("Bluetooth toggle failed.");
                                        }
                                        m_Requested = false;
                                        break;
                                    case winrt::Windows::Foundation::AsyncStatus::Started:
                                        break;
                                    default:
                                        m_Requested = false;
                                        break;
                                    }
                                });
                            return;
                        }
                    }
                }
                catch (...)
                {
                    m_Requested = false;
                }
                break;
            case winrt::Windows::Foundation::AsyncStatus::Canceled:
                if (m_OnErrorMessageGenerated)
                {
                    m_OnErrorMessageGenerated("Bluetooth state query was canceled.");
                }
                m_Requested = false;
                break;
            case winrt::Windows::Foundation::AsyncStatus::Error:
                if (m_OnErrorMessageGenerated)
                {
                    m_OnErrorMessageGenerated("Bluetooth state query failed.");
                }
                m_Requested = false;
                break;
            case winrt::Windows::Foundation::AsyncStatus::Started:
                break;
            default:
                m_Requested = false;
                break;
            }
        });
}


void LuminaActionBluetoothSwitch::RequestGetIsBluetoothEnabled()
{
    if (!m_Requested && !m_IsBluetoothEnabled.has_value())
    {
        GetIsBluetoothEnabledAsync_Internal();
    }
}

void LuminaActionBluetoothSwitch::RequestToogleBluetoothEnabled()
{
    if (!m_Requested && m_IsBluetoothEnabled.has_value())
    {
        SetBluetoothEnabledAsync_Internal();
    }
}

bool LuminaActionBluetoothSwitch::GetIsStateRequested() const
{
    return m_Requested;
}