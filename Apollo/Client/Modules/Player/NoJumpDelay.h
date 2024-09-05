#pragma once

class NoJumpDelay : public Module
{
public:
    NoJumpDelay(int keybind = Keys::NONE, bool enabled = false) :
        Module("No Jump Delay", "Player", "Disables the jump delay", keybind, enabled)
    {
    }

    void onEnabled() override {
        if (!Address::getLocalPlayer())
            return;

        Player* player = Address::getLocalPlayer();

        player->getJumpControl()->noJumpDelay = true;
    }

    void onEvent(RenderContextEvent* event) override
    {
        if (!Address::getLocalPlayer())
            return;

        Player* player = Address::getLocalPlayer();

        player->getJumpControl()->noJumpDelay = true;
    }

    void onDisabled() override {
        if (!Address::getLocalPlayer())
            return;

        Player* player = Address::getLocalPlayer();

        player->getJumpControl()->noJumpDelay = false;
    }
};