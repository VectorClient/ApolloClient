#pragma once

class StreamerMode : public Module
{
public:
    StreamerMode(int keybind = Keys::NONE, bool enabled = false) :
        Module("StreamerMode", "Visual", "Hides your nametag so you don't get stream sniped.", keybind, enabled)
    {
    }

public:
    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (!player)
        {
            return;
        }
        
        std::string silly = "Apollo User";
        player->setNametag(&silly);
    }
};