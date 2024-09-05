#pragma once

class AutoClicker : public Module
{
public:
    AutoClicker(int keybind = Keys::NONE, bool enabled = false) :
        Module("Auto Clicker", "Combat", "a automatical clicker", keybind, enabled)
    {
        addBool("Right", "Right click", &rightClick);
        addBool("Left", "Left click", &leftClick);
        addSlider("APS", "attack per second", &APS, 0, 20);
    }
     
    bool leftClick = true;
    bool rightClick = false;
    float APS = 10;

    void onEvent(ImGuiRenderEvent* event) override {
        if (!Address::canUseKeys())
            return;

        if (TimeUtils::hasTimeElapsed("autoClickerTimer", 1000 / APS, true)) {
            if (Utils::leftClick && leftClick) {
                POINT p;
                if (GetCursorPos(&p))
                {
                    mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
                    mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
                }
            }

            if (Utils::rightClick && rightClick) {
                POINT p;
                if (GetCursorPos(&p))
                {
                    mouse_event(MOUSEEVENTF_RIGHTDOWN, p.x, p.y, 0, 0);
                    mouse_event(MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
                }
            }
        }
    }
};