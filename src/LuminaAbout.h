#pragma once

class LuminaAbout
{
public:
    void Show();
    void Hide();
    void Render();
    bool IsVisible() const;

private:
    bool m_Visible = false;
};

