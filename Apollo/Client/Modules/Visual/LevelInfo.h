#pragma once

class LevelInfo : public Module
{
public:
    LevelInfo(int keybind = Keys::NONE, bool enabled = false) :
        Module("LevelInfo", "Visual", "Displays information about level", keybind, enabled)
    {
        addEnum("Style", "The style of the Level Info", { "Vanilla", }, &style);
        addBool("FPS", "Displays your fps", &showFps);
        addBool("Ping", "Displays your ping", &showPing);
        addBool("BPS", "Displays your bps", &showBPS);
        addBool("XYZ", "Displays your xyz pos", &showXYZ);
        addBool("SpeedSpells", "Displays your speed spells count", &showSpeedSpells);
        addBool("HealthSpells", "Displays your health spells count", &showHealthSpells);
        //addBool("ItemInfo", "Displays the item info for the item ur selecting", &showItemInfo);
        addBool("BlockInfo", "Displays the pointing block info", &showBlockInfo);
        addBool("Pearls", "Displays your pearl count", &showPearls);
        addBool("Arrows", "Displays your arrows count", &showArrows);
    }

    int style = 0;
    bool showFps = true;
    bool showPing = true;
    bool showBPS = true;
    bool showXYZ = false;
    bool showSpeedSpells = false;
    bool showHealthSpells = false;
    bool showItemInfo = false;
    bool showBlockInfo = false;
    bool showPearls = false;
    bool showArrows = false;

    int ping = 0;
    int totalPearlCount = 0;
    int totalArrowCount = 0;
    int totalHealthSpellCount = 0;
    int totalSpeedSpellCount = 0;

    void onEvent(IntersectsTickEvent* event) override {
        auto* player = Address::getLocalPlayer();

        if (player == nullptr)
            return;

        PlayerInventory* supplies = player->getSupplies();
        auto n = supplies->hotbarSlot;
        Inventory* inv = supplies->inventory;
        ItemStack* stack = inv->getItem(n);

        int arrow = 0;
        int pearl = 0;
        int healthSpells = 0;
        int speedSpells = 0;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inv->getItem(i);
            if (stack->item != nullptr) {
                if (stack->getItem()->nameContains("ender_pearl")) {
                    pearl += stack->count;
                }

                if (stack->getItem()->nameContains("arrow")) {
                    arrow += stack->count;
                }

                if (stack->customNameContains("Spell of Life")) {
                    healthSpells += stack->count;
                }
                
                if (stack->customNameContains("Spell of Swiftness")) {
                    speedSpells += stack->count;
                }
            }
        }

        totalArrowCount = arrow;
        totalPearlCount = pearl;
        totalHealthSpellCount = healthSpells;
        totalSpeedSpellCount = speedSpells;
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (!player || !Address::canUseKeys()) return;

        Vector3<float> pos = player->getStateVector()->Position;

        std::ostringstream oss;
        oss.precision(2);
        oss << std::fixed << player->getHorizontalSpeed();

        static int FramePerSecond = 0;

        if (TimeUtils::hasTimeElapsed("FpsCounter", 500, true)) {
            FramePerSecond = ImGui::GetIO().Framerate;
        }

        int index = 0;

        if (showFps) {
            drawLevelInfo(Utils::combine("FPS: ", FramePerSecond), index++);
        }

        if (showPing) {
            drawLevelInfo(Utils::combine("Ping: ", ping, "ms"), index++);
        }
        
        if (showBPS) {
            drawLevelInfo(Utils::combine("BPS: ", oss.str().c_str()), index++);
        }

        if (showXYZ) {
            drawLevelInfo(Utils::combine("XYZ: ", pos.x, ", ", pos.y, ", ", pos.z), index++);
        }

        if (showPearls) {
            drawLevelInfo(Utils::combine("Pearls: ", totalPearlCount), index++);
        }

        if (showArrows) {
            drawLevelInfo(Utils::combine("Arrows: ", totalArrowCount), index++);
        }

        if (showHealthSpells) {
            drawLevelInfo(Utils::combine("Health spells: ", totalHealthSpellCount), index++);
        }

        if (showSpeedSpells) {
            drawLevelInfo(Utils::combine("Speed spells: ", totalSpeedSpellCount), index++);
        }

        if (showBlockInfo) {
            //drawLevelInfo(BlockIDLevelInfo, index++);
        }
    }

    void onEvent(PacketEvent* event) override {
        if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector != nullptr && Address::getLocalPlayer() != nullptr) {
            if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->JoinedIp == "") { // Checks if User is in a server
                ping = 0; // Sets the ping to 0 if user is in a world to avoid crashing
            }
            else {
                ping = event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->getPing();

            }
        }
        //ping = event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->getPing();
    }

    void drawLevelInfo(std::string tileDescr, int index)
    {
        if (style == 0) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            Vector2<float> tilePos = Vector2<float>(5, (Address::getClientInstance()->getGuiData()->mcResolution.y - 25) - (index * 19.f));

            ImRenderUtil::drawText(tilePos, &tileDescr, UIColor(255, 255, 255), 1.18f, 0.9f, true);

            float width = ImRenderUtil::getTextWidth(&tileDescr, 1);

            if (size.x < width)
                size.x = width;

            location = tilePos;
            size.y = (index + 1) * 10;
            ImGui::PopFont();
        }
    }
};