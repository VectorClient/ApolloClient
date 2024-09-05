#pragma once

class TestModule : public Module
{
public:
    TestModule(int keybind = 70, bool enabled = false) :
        Module("TestModule", "Motion", "a module for development purposes", keybind, enabled)
    {
        // a1 flight purposes
        // a2 soon for disabler shit
        // a3 soon for speed shit
        // a4 soon for funny packets :D
        addEnum("Interact", "(developer testing stuff)", { "a1", "spoof3", "a3", "gay"}, &mode);
        addBool("Test", "Keep Vel", &Testa);
        addSlider("Test", "The fly duh", &Global::testModuleFly, -5, 0);
        addSlider("Boost", "The boost duh", &speed, 0, 30);
        addSlider("Timer", "The timer duh", &timer, 1, 25);
        addSlider("Glider", "The glide nigga", &glide, -1, 0);
    }

private:
    int mode = 0;
    float speed = 0.7;
    float timer = 7.3f;
    float glide = -0.03f; // Make it glide a bit or u homeless and getting flagged
    
    bool Testa = true;

    bool ShouldFly = false; // We using bools cuz we are homeless
public:
    void onEnabled() override {
        if (!Address::getLocalPlayer()) return;

        Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20.f);
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        if (!Address::getLocalPlayer())
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        if (mode == 2) {
            if (player->getHurtTime() > 1 && Testa) {
                state->Velocity = glide;
            }
        }

        if (mode == 3) {
            ChatUtils::sendMessage(to_string(state->Velocity.y));
        }
    }

    void onEvent(PacketEvent* event) override {
        if (!Address::getLocalPlayer())
            return;

        if (event->Packet->getId() == PacketID::MobEquipment && mode == 1) {
            auto* pkt = reinterpret_cast<MobEquipmentPacket*>(event->Packet);
            ChatUtils::sendCustomMessage(Utils::combine("mSlot", std::to_string(pkt->mSlot)), "Packet");
            ChatUtils::sendCustomMessage(Utils::combine("mSelectedSlot", std::to_string(pkt->mSelectedSlot)), "Packet");
            ChatUtils::sendCustomMessage(Utils::combine("mContainerID", std::to_string((int)pkt->mContainerId)), "Packet");
            ChatUtils::sendCustomMessage(Utils::combine("mSlotByte", std::to_string((int)pkt->mSlotByte)), "Packet");
            ChatUtils::sendCustomMessage(Utils::combine("mSelectedSlotByte", std::to_string((int)pkt->mSelectedSlotByte)), "Packet");
            ChatUtils::sendCustomMessage(Utils::combine("mContainerIDByte", std::to_string((int)pkt->mContainerIdByte)), "Packet");
        }

        //if (strcmp(event->Packet->getTypeName().getText(), "PlayerAuthInputPacket") != 0 && strcmp(event->Packet->getTypeName().getText(), "SubChunkRequestPacket") != 0) {
            //Address::getLocalPlayer()->displayClientMessage(Utils::combine(GRAY, "Packet -> ", RESET, event->Packet->getTypeName().getText()));
        //}
    }

    void onDisabled() override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
    }
};