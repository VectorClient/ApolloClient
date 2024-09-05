#pragma once

class Disabler : public Module
{
public:
    Disabler(int keybind = 7, bool enabled = true) :
        Module("Disabler", "Misc", "Disable certain anticheats.", keybind, enabled)
    {
        addEnum("Mode", "The mode of which the disabler will work", { "Flareon", "FakeTimer", "TestDuplicate", "TestInputFlag" }, &mode);
        addSlider("Timer Value", "No Description", &fakeTimer, 0.0, 3, 0.01);
        addBool("Send Packets", "Send packets to the server", &extraSecurity);
        addBool("Speed Strafe", "Disable strafe checks", &testing);
        addBool("Only Fly", "Run FakeTimer when fly is enabled", &onlyFly);
        addBool("Only PlayerAuth", "Cancelling only player auth input when you use FakeTimer mode", &onlyAuth);
        addBool("Debug", "Send message when you use FakeTimer mode", &debug);
        addBool("Display Mode", "Shows the mode for disabler", &showmode);
    }

    int mode = 0;
    int setTick = 0;
    int countPacket = 0;
    float fakeTimer = 0.4;
    bool extraSecurity = false;
    bool testing = false;
    bool onlyFly = true;
    bool onlyAuth = false;
    bool debug = false;
    bool showmode = true;
    bool isEnabledFly = false;
    bool ignore = false;

    Vector3<float> savedPos = Vector3<float>(0, 0, 0);
    Vector3<float> oldPos = Vector3<float>(0, 0, 0);

    void onEnabled() override {
        ClientInstance* instance = Address::getClientInstance();
        LoopbackPacketSender* sender = instance->getLoopbackPacketSender();

        if (!instance->getLocalPlayer())
        {
            return;
        }

        ChatUtils::sendMessage("Updating disabler...");
        ignore = false;
    }

    inline int randomInt(int a, int b) {
        int random = ((int)rand()) / (int)RAND_MAX;
        int diff = b - a;
        int r = random * diff;
        return a + r;
    }

    void onEvent(PacketEvent* event) override {
        ClientInstance* instance = Address::getClientInstance();
        LoopbackPacketSender* sender = instance->getLoopbackPacketSender();

        if (!instance->getLocalPlayer() && !instance->getMinecraftGame()->CanUseKeys)
        {
            return;
        }

        Player* player = instance->getLocalPlayer();

        if (mode != 1) Global::fakeTimer = -1;

        static bool startedUpdatingDisabler = false;

        static bool startedTickingDisabler = false;

        if (testing && getModuleByName("speed")->isEnabled()) {
            if (!player->isOnGround()) {
                startedTickingDisabler = false;
                TimeUtils::resetTime("TestingJump");
            }
            else {
                startedTickingDisabler = true;
            }

            if (startedTickingDisabler) {
                if (TimeUtils::hasTimeElapsed("TestingJump", 5500, true)) {
                    ChatUtils::sendDisablerMessage("Update your current collison state urgently");
                }
            }

            if (player->isOnGround()) {
                if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                    return;
                }
            }
        }

        switch (mode) {
        case 0: // Flareon
        {
            //player->getStateVector()->PrevPosition = player->getStateVector()->Position;

            if (event->Packet->getId() == PacketID::NetworkStackLatency) {
                if (TimeUtils::hasTimeElapsed("FlareonPacketRecieve", 2300, true) && !instance->getLocalPlayer()->isOnGround()) {
                    ChatUtils::sendDisablerMessage("Updated");
                    startedUpdatingDisabler = true;
                    TimeUtils::resetTime("StartedUpdating");
                    //return;
                }
            }

            if (TimeUtils::hasTimeElapsed("StartedUpdating", 1300, true) && startedUpdatingDisabler) {
                //ChatUtils::sendDisablerMessage("Updated Network");
                startedUpdatingDisabler = false;
            }

            if (event->Packet->getId() == PacketID::MobEquipment && getModuleByName("regen")->isEnabled()) {
                //ChatUtils::sendDisablerMessage("Updated Spoof");
            }

            if (event->Packet->getId() == PacketID::PlayerAuthInput && getModuleByName("fly")->isEnabled()) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (TimeUtils::hasTimeElapsed("FlareonAuthInputRecieve", 500, true)) {
                    int mInt = randomInt(-0.5, 0.5);
                    Vector3<float> pos = player->getStateVector()->Position;
                    Vector3<float> SillyPos = Vector3<float>(pos.x, pos.y + mInt, pos.z);
                    pkt->position = SillyPos;
                    //pkt->TicksAlive = rand() % 4;
                    ChatUtils::sendDisablerMessage("Updated Fly");
                    return;
                }
            }
            break;
        }
        case 1: // faketimer
            isEnabledFly = getModuleByName("HiveFly")->isEnabled();
            if (!isEnabledFly) Global::fakeTimer = fakeTimer;
            countPacket++;

            if (countPacket >= 20 / Global::fakeTimer || (!isEnabledFly && onlyFly)) {
                if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                    countPacket = 0;
                    auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                    if (setTick == 0) setTick = pkt->TicksAlive;
                    else setTick++;
                    pkt->TicksAlive = setTick;
                    Global::canChange = true;
                    if (isEnabledFly && onlyFly) {
                        pkt->rotation.x = 0;
                        pkt->rotation.y = 0;
                        pkt->headYaw = 0;
                    }
                    if (Global::stopFly) {
                        Global::canFly = 0;
                        Global::stopFly = false;
                    }
                    if (onlyFly && !isEnabledFly && debug) ChatUtils::sendDisablerMessage("Updated"); // Sended!
                } else if (isEnabledFly && onlyFly) {
                    *event->cancelled = true;
                }
            } else {
                if (onlyAuth) {
                    if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                        *event->cancelled = true;
                    }
                } else {
                    *event->cancelled = true;
                }
            }
            break;
        case 2: //Duplicate
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                if (ignore) ignore = false;
                else {
                    auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                    if (setTick == 0) setTick = pkt->TicksAlive;
                    else setTick++;
                    savedPos = pkt->position;
                    Vector3<float> newPos = savedPos.average(oldPos);
                    if (newPos.distance(oldPos) > 3) {
                        oldPos = savedPos;
                        newPos = savedPos;
                    }
                    pkt->TicksAlive = setTick;
                    pkt->position = newPos;
                    if (debug) ChatUtils::sendDisablerMessage("Pos Normal: " + to_string(pkt->position.x) + ", " + to_string(pkt->position.z));
                    setTick++;

                    std::shared_ptr<Packet> packet = MinecraftPackets::createPacket((int)PacketID::PlayerAuthInput);
                    auto* dupePkt = reinterpret_cast<PlayerAuthInputPacket*>(packet.get());
                    dupePkt = pkt;
                    dupePkt->TicksAlive = setTick;
                    dupePkt->position = savedPos;
                    ignore = true;
                    Address::getClientInstance()->getLoopbackPacketSender()->sendToServer(dupePkt);
                    if (debug) ChatUtils::sendDisablerMessage("Pos Duplicate: " + to_string(dupePkt->position.x) + ", " + to_string(dupePkt->position.z));
                    oldPos = savedPos;
                }
            }
            break;
        case 3: //InputFlags
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                __int64 flags = pkt->mInputData;
                if (getModuleByName("fly")->isEnabled()) {
                    removeFlagFromName(flags, InputData::HandledTeleport);
                } else {
                    addFlagFromName(flags, InputData::HandledTeleport);
                }
                std::vector<std::string> flagNames = getFlagNames(flags);
                std::string output = "";
                for (const auto& name : flagNames) {
                    output += name + ", ";
                }
                if (debug) ChatUtils::sendDisablerMessage("InputFlags: " + output);
                pkt->mInputData = flags;
            }
            break;
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        ClientInstance* instance = Address::getClientInstance();
        LoopbackPacketSender* sender = instance->getLoopbackPacketSender();

        if (!instance->getLocalPlayer() && !instance->getMinecraftGame()->CanUseKeys)
        {
            return;
        }

        switch (mode) {
        case 0: // Flareon
            if (extraSecurity) {
                std::shared_ptr<Packet> packet = MinecraftPackets::createPacket((int)PacketID::NetworkStackLatency);
                auto* pkt = reinterpret_cast<NetworkStackLatencyPacket*>(packet.get());
                pkt->mCreateTime = randomInt(1, 15);
                if (TimeUtils::hasTimeElapsed("FlareonPacket", 10000, true)) {
                    Address::getClientInstance()->getLoopbackPacketSender()->sendToServer(pkt);
                    return;
                }
            }
            break;
        }
    }

    void onEvent(RenderContextEvent* event) override {
        if (Address::getLocalPlayer() == nullptr)
            return;

        Player* ent = Address::getLocalPlayer();

        //ChatUtils::sendMessage(Utils::combine("Layer: ", Global::LayerName));
    }

    void addFlagFromName(__int64& inputFlags, InputData flag) {
        inputFlags |= static_cast<__int64>(flag);
    }

    void removeFlagFromName(__int64& inputFlags, InputData flag) {
        inputFlags &= ~static_cast<__int64>(flag);
    }

    std::vector<std::string> getFlagNames(__int64 flags) {
        std::unordered_map<__int64, std::string> flagNames = {
            {1ll << 0, "Ascend"},
            {1ll << 1, "Descend"},
            {1ll << 2, "NorthJump"},
            {1ll << 3, "JumpDown"},
            {1ll << 4, "SprintDown"},
            {1ll << 5, "ChangeHeight"},
            {1ll << 6, "Jumping"},
            {1ll << 7, "AutoJumpingInWater"},
            {1ll << 8, "Sneaking"},
            {1ll << 9, "SneakDown"},
            {1ll << 10, "Up"},
            {1ll << 11, "Down"},
            {1ll << 12, "Left"},
            {1ll << 13, "Right"},
            {1ll << 14, "UpLeft"},
            {1ll << 15, "UpRight"},
            {1ll << 16, "WantUp"},
            {1ll << 17, "WantDown"},
            {1ll << 18, "WantDownSlow"},
            {1ll << 19, "WantUpSlow"},
            {1ll << 20, "Sprinting"},
            {1ll << 21, "AscendBlock"},
            {1ll << 22, "DescendBlock"},
            {1ll << 23, "SneakToggleDown"},
            {1ll << 24, "PersistSneak"},
            {1ll << 25, "StartSprinting"},
            {1ll << 26, "StopSprinting"},
            {1ll << 27, "StartSneaking"},
            {1ll << 28, "StopSneaking"},
            {1ll << 29, "StartSwimming"},
            {1ll << 30, "StopSwimming"},
            {1ll << 31, "StartJumping"},
            {1ll << 32, "StartGliding"},
            {1ll << 33, "StopGliding"},
            {1ll << 34, "PerformItemInteraction"},
            {1ll << 35, "PerformBlockActions"},
            {1ll << 36, "PerformItemStackRequest"},
            {1ll << 37, "HandledTeleport"},
            {1ll << 38, "Emoting"},
            {1ll << 39, "MissedSwing"},
            {1ll << 40, "StartCrawling"},
            {1ll << 41, "StopCrawling"},
            {1ll << 42, "StartFlying"},
            {1ll << 43, "StopFlying"},
            {1ll << 44, "ReceivedServerData"},
            {1ll << 45, "InClientPredictedInVehicle"},
            {1ll << 46, "PaddlingLeft"},
            {1ll << 47, "PaddlingRight"},
        };

        std::vector<std::string> activeFlags;

        for (int i = 0; i < 64; ++i) {
            __int64 bit = 1ll << i;
            if ((flags & bit) != 0) {
                auto it = flagNames.find(bit);
                if (it != flagNames.end()) {
                    activeFlags.push_back(it->second);
                }
            }
        }

        return activeFlags;
    }
};