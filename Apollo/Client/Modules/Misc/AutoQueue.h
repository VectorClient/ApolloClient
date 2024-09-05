#pragma once

class AutoQueue : public Module
{
public:
    AutoQueue(int keybind = Keys::NONE, bool enabled = false) :
        Module("AutoQueue", "Misc", "Hits every entity around you.", keybind, enabled)
    {
        addBool("Queued On Game End", "Queues on game end", &Global::shouldQueueOnGameEnd);
        addBool("Queued On Death", "Queues on death", &Global::shouldQueueOnDeath);
    }

public:
    void onEvent(ActorBaseTickEvent* event) override {
        // when low on health we don't have this yet
        auto player = Address::getLocalPlayer();

        if (!player || !Address::canUseKeys()) return;

        if (Global::shouldQueue) {
            std::shared_ptr<Packet> packet = MinecraftPackets::createPacket(77);
            auto* command_packet = reinterpret_cast<CommandRequestPacket*>(packet.get());
            command_packet->Command = "/q " + Global::LastGame;

            command_packet->Origin.mType = CommandOriginType::Player;

            command_packet->InternalSource = true;
            Address::getClientInstance()->getLoopbackPacketSender()->sendToServer(command_packet);
            Global::shouldQueue = false;
        }
    }
};