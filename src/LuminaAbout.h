#pragma once

class LuminaAbout
{
public:
    void Show();
    void Close();
    bool CanShow();

    void Render();

private:
    bool m_IsDisplayed = false;
};

