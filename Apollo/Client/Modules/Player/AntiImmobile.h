#pragma once

class AntiImmobile : public Module
{
public:
    AntiImmobile(int keybind = 7, bool enabled = false) :
        Module("AntiImmobile", "Player", "Disables Immobile flag on servers", keybind, enabled)
    {

    }
};