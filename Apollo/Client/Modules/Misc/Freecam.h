#pragma once

class Freecam : public Module
{
public:
    Freecam(int keybind = Keys::NONE, bool enabled = false) :
        Module("Freecam", "Misc", "Look around without editing your game", keybind, enabled)
    {}

    Vector3<float> freecamPos;
    Vector3<float> freecamRotation;

    void onEnabled() override {
        Player* player = Address::getLocalPlayer();

        if (!player)
            return;

        freecamPos = player->getStateVector()->Position;
        Global::freecamPosition = freecamPos;
    }

    void onEvent(PacketEvent* event) override
    {
        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            *event->cancelled = true;
        }
    }
};