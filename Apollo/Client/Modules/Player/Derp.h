#pragma once

class Derp : public Module
{
public:
    Derp(int keybind = Keys::NONE, bool enabled = false) :
        Module("Derp", "Player", "Spins the player", keybind, enabled)
    {
        addEnum("Side", "The side that the rotations should be sent to", { "Client", "Server", "Both" }, &side);
        addBool("Pitch", "Changes the player's pitch", &pitch);
        addBool("HeadYaw", "Changes the player's headyaw", &headyaw);
    }

private:
    int side = 0;
    bool pitch = false;
    bool headyaw = false;

    float yaw = 0;
    float speed = 2;
public:

    void onEvent(IntersectsTickEvent* event) override
    {
        Player* player = Address::getLocalPlayer();

        if (!player) { return; }

        // Increase the yaw acording to the speed

        yaw++;

        if (yaw >= 360 / speed) {
            yaw = 0;
        }

        if (side == 0 || side == 2) {
            if (pitch) {
                player->setPitch(80); // ClientSided pitch (makes you look down)
            }

            player->setBodyRotation(yaw * speed); // ClientSided Body rotations
            if (headyaw) {
                player->setHeadYaw(yaw * speed); // Clientsided head rotations
            }
        }
    }

    void onEvent(PacketEvent* event) override
    {
        PlayerAuthInputPacket* packet = nullptr;

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }

        if (side == 1 || side == 2) {
            if (packet != nullptr) {
                if (pitch) {
                    packet->rotation.x = 80;
                }
                if (headyaw) {
                    packet->headYaw = yaw; // ServerSided headyaw rotation
                }
                packet->rotation.y = yaw; // ServerSided body rotation
            }
        }
    }
};