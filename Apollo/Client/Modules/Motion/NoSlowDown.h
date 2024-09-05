#pragma once

class NoSlowDown : public Module
{
public:
    NoSlowDown(int keybind = Keys::NONE, bool enabled = false) :
        Module("NoSlowdown", "Motion", "Prevents you from slowing down.", keybind, enabled)
    {

    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        if (Address::getLocalPlayer() == nullptr)
            return;

        Address::getLocalPlayer()->getComponent<BlockMovementSlowdownMultiplierComponent>()->blockMovementSlowdownMultiplier = Vector3<float>(0.f, 0.f, 0.f);
    }
};