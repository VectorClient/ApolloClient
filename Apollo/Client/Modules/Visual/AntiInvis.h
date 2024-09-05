#pragma once

class AntiInvis : public Module
{
public:
    AntiInvis(int keybind = Keys::NONE, bool enabled = false) :
        Module("AntiInvis", "Visual", "Shows invisable targets.", keybind, enabled)
    {
    }

public:
    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (!player)
        {
            return;
        }

        auto list = player->getLevel()->getRuntimeActorList();

        for (auto* actor : list) {
            if (actor != player) {
                actor->setInvisible(false);
            }
        }
    }
};