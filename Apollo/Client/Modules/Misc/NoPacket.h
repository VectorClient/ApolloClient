#pragma once

class NoPacket : public Module // this wont be fun to keep clean
{
public:
    NoPacket(int keybind = Keys::NONE, bool enabled = false) :
        Module("NoPacket", "Misc", "Stop sending all outgoing packets", keybind, enabled)
    {}

    void onEvent(PacketEvent* event) override
    {
        *event->cancelled = true;
    }
};