#pragma once
#include <string>

class LuminaErrorMessageInfo
{
public:
    void Show(const std::string& message);
    void Hide();
    void Render();
    bool IsVisible() const;
private:
    bool m_Visible = false;
    std::string m_Message;
};