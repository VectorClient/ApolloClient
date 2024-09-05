#pragma once

class FastBreak : public Module
{
public:
    FastBreak(int keybind = Keys::NONE, bool enabled = false) :
        Module("FastBreak", "Misc", "Breaks blocks much more faster", keybind, enabled)
    {
        addSlider("Speed", "The speed of the breaking", &Global::BreakSpeed, 1, 50);
    }
};