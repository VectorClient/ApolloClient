#pragma once

class Fullbright : public Module
{
public:
    Fullbright(int keybind = Keys::NONE, bool enabled = false) :
        Module("Fullbright", "Visual", "Brightens your game (like night vision)", keybind, enabled)
    {
        
    }
};