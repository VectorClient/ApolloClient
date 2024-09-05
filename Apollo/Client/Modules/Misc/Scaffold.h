#pragma once

class Scaffold : public Module
{
public:
    Scaffold(int keybind = 82, bool enabled = false) :
        Module("Scaffold", "Misc", "Builds blocks for you.", keybind, enabled)
    {
        addEnum("Rotations", "The rotation mode", { "Normal", "Moonwalk" }, &rotations);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addEnum("Switch", "How to hold the block", { "Hold", "Spoof", "ClientSpoof", "ServerHold" }, &holdStyle);
        addEnum("Swing", "The swing style for scaffold", { "Normal", "None" }, &Global::SwingType);
        addEnum("TowerMode", "The tower mode", { "Jump", "Velocity", "Timer", "Tozic" }, &towerMode);
        addEnum("Placement", "The placement mode", { "Normal", "Flareon", "Legit" }, &placement);
        addEnum("Counter", "The style of displaying the block counter", { "None", "Aeolus" }, &counter);
        addBool("Inventory Slot", "Use the inventory slot", &InvSlot);
        addBool("Flareon V2", "Places the placement in a speacific style", &FlareonV2);
        addBool("Aura Bypass", "Stops scaffold rotation and stops scaffold when killaura is hitting a target", &AuraBypass);
        addBool("Keep Y", "Keeps your y level when placing blocks", &Ylock);
        addSlider("PlacesPerTick", "how many blocks it can place in tick", &placesPerTick, 1, 10);
        addSlider("Distance", "The place distance (how far to place blocks)", &range, 3, 8);
        addSlider("Extend", "Distance your position to target block", &extend, 0, 10);
    }

private:
    //Settings
    int rotations = 0;
    int Side = 0;
    int holdStyle = 0;
    int towerMode = 0;
    int placement = 0;
    int counter = 1;
    float extend = 0;
    float range = 5.f;
    float placesPerTick = 1.f;
    bool InvSlot = true;
    bool FlareonV2 = false;
    bool AuraBypass = true;
    bool Ylock = false;

    //Other
    int prevSlot = 0;
    int blockSlot = 0;
    int placesPerTickCount = 0;
    float lockedY = 0;
    bool isHoldingSpace = false;
    bool foundBlock = false;
    Vector3<float> BlockPos;
    Vector3<float> ClickBlockPos;
    Vector3<float> BlockBelow;
public:

    bool canPlace(Vector3<float> pos) {
        return Address::getBlockSource()->getBlock(pos.floor().ToInt())->GetMaterialType() == MaterialType::Air;
    }

    bool findBlock() {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;
        int slot = previousSlot;

        for (int i = 0; i < (InvSlot ? 36 : 9); i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
                    if (previousSlot != i) {
                        playerInventory->hotbarSlot = i;
                        blockSlot = i;

                        if (holdStyle == 3) {
                            Misc::SpoofSwitch(i);
                        }
                    }
                    return true;
                }
            }
        }

        return false; // false if we are using checks
    }

    Vector3<float> getBlockBelow(Player* player) {
        Vector3<float> blockBelow = player->getAABBShape()->PosLower;
        blockBelow.y -= 1.f;
        return blockBelow;
    }

    void adjustYCoordinate(const Vector3<float>& blockBelowReal) {
        if (floor(blockBelowReal.y) < floor(lockedY)) {
            lockedY = blockBelowReal.y + Address::getLocalPlayer()->getStateVector()->Velocity.y;
        }
    }

    Vector3<float> getExtendedPosition(const Vector3<float>& velocity, Vector3<float>& blockBelow, float extendValue) {
        Vector3<float> extendedBlock = blockBelow;
        Vector3<float> normalizedVelocity = velocity.Normalize();
        extendedBlock.x += normalizedVelocity.x * extendValue;
        extendedBlock.z += normalizedVelocity.z * extendValue;
        return extendedBlock;
    }

    Vector3<float> getNextBlock(const Vector3<float>& velocity, const Vector3<float>& blockBelow, float extendValue) {
        Vector3<float> nextBlock = blockBelow;
        if (abs(velocity.x) > abs(velocity.z)) {
            nextBlock.x += (velocity.x > 0 ? 1 : (velocity.x < 0 ? -1 : 0)) * extendValue;
        }
        else {
            nextBlock.z += (velocity.z > 0 ? 1 : (velocity.z < 0 ? -1 : 0)) * extendValue;
        }
        return nextBlock;
    }
    Vector2<float> getNormAngle(const Vector2<float>& angle) {
        float x = angle.x;
        float y = angle.y;
        while (x > 90.f)
            x -= 180.0f;
        while (x < -90.f)
            x += 180.0f;

        while (y > 180.0f)
            y -= 360.0f;
        while (y < -180.0f)
            y += 360.0f;
        return Vector2<float>(x, y);
    }

    bool buildBlock(Vector3<float> blockBelow) {
        BlockPos = Vector3<float>(blockBelow.ToInt().ToFloat().add(Vector3<float>(0.5f, 0, 0.5f)));
        Vector3<float> vel = Address::getLocalPlayer()->getStateVector()->Velocity;
        vel = vel.Normalize();  // Only use values from 0 - 1
        blockBelow = blockBelow.floor();

        if (Address::getBlockSource()->getBlock(Vector3<int>(blockBelow.ToInt()))->GetBlockLegacy()->getBlockID() == 0) {
            Vector3<int> blok(blockBelow.ToInt());

            // Find neighbour
            static std::vector<Vector3<int>*> checklist;
            if (checklist.empty()) {
                checklist.push_back(new Vector3<int>(0, -1, 0));
                checklist.push_back(new Vector3<int>(0, 1, 0));

                checklist.push_back(new Vector3<int>(0, 0, -1));
                checklist.push_back(new Vector3<int>(0, 0, 1));

                checklist.push_back(new Vector3<int>(-1, 0, 0));
                checklist.push_back(new Vector3<int>(1, 0, 0));
            }

            bool foundCandidate = false;
            int i = 0;
            for (auto current : checklist) {
                Vector3<int> calc = blok.submissive(*current);
                if (Address::getBlockSource()->getBlock(calc)->GetBlockLegacy()->getBlockID() != 0) {
                    // Found a solid block to click
                    foundCandidate = true;
                    blok = calc;
                    break;
                }
                i++;
            }

            ClickBlockPos = blok.ToFloat().add(Vector3<float>(0.5f, 0, 0.5f));
            if (foundCandidate) {
                if (1 <= placement) {
                    auto player = Address::getLocalPlayer();
                    player->getLevel()->getHitResult()->BlockFace = i;
                    player->getLevel()->getHitResult()->IBlockPos = BlockPos.ToInt();
                    player->getLevel()->getHitResult()->HitType = 0;
                    player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
                }
                Address::getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
                return true;
            }
        }
        return false;
    }

    bool predictBlock(Vector3<float> blockBelow) {
        Vector3<float> vel = Address::getLocalPlayer()->getStateVector()->Velocity;
        vel = vel.Normalize();  // Only use values from 0 - 1
        blockBelow = blockBelow.floor();

        static std::vector<Vector3<int>> checkBlocks;
        if (checkBlocks.empty()) {  // Only re sort if its empty
            for (int y = -range; y < range; y++) {
                for (int x = -range; x < range; x++) {
                    for (int z = -range; z < range; z++) {
                        checkBlocks.push_back(Vector3<int>(x, y, z));
                    }
                }
            }
            // https://www.mathsisfun.com/geometry/pythagoras-3d.html c2 = x2 + y2 + z2 funny
            std::sort(checkBlocks.begin(), checkBlocks.end(), [](Vector3<int> first, Vector3<int> last) {
                return sqrtf((float)(first.x * first.x) + (float)(first.y * first.y) + (float)(first.z * first.z)) < sqrtf((float)(last.x * last.x) + (float)(last.y * last.y) + (float)(last.z * last.z));
                });
        }

        for (const Vector3<int>& blockOffset : checkBlocks) {
            Vector3<int> currentBlock = Vector3<int>(blockBelow.ToInt()).add(blockOffset);
            BlockPos = Vector3<float>(currentBlock.ToInt().ToFloat().add(Vector3<float>(0.5f, 0, 0.5f)));

            // Normal tryScaffold after it sorts
            if (Address::getBlockSource()->getBlock(Vector3<int>(currentBlock))->GetBlockLegacy()->getBlockID() == 0) {
                Vector3<int> blok(currentBlock);

                // Find neighbour
                static std::vector<Vector3<int>*> checklist;
                if (checklist.empty()) {
                    checklist.push_back(new Vector3<int>(0, -1, 0));
                    checklist.push_back(new Vector3<int>(0, 1, 0));

                    checklist.push_back(new Vector3<int>(0, 0, -1));
                    checklist.push_back(new Vector3<int>(0, 0, 1));

                    checklist.push_back(new Vector3<int>(-1, 0, 0));
                    checklist.push_back(new Vector3<int>(1, 0, 0));
                }

                bool foundCandidate = false;
                int i = 0;
                for (auto current : checklist) {
                    Vector3<int> calc = blok.submissive(*current);
                    //bool Y = ((region->getBlock(calc)->blockLegacy))->material->isReplaceable;
                    if (Address::getBlockSource()->getBlock(calc)->GetBlockLegacy()->getBlockID() != 0) {
                        // Found a solid block to click
                        foundCandidate = true;
                        blok = calc;
                        break;
                    }
                    i++;
                }
                ClickBlockPos = blok.ToFloat().add(Vector3<float>(0.5f, 0, 0.5f));
                if (foundCandidate) {
                    if (1 <= placement) {
                        auto player = Address::getLocalPlayer();
                        player->getLevel()->getHitResult()->BlockFace = i;
                        player->getLevel()->getHitResult()->IBlockPos = BlockPos.ToInt();
                        player->getLevel()->getHitResult()->HitType = 0;
                        player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos;
                    }
                    Address::getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
                    return true;
                }
            }
        }
        return false;
    }

    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        Vector3<float> blockbelow = getBlockBelow(player);

        lockedY = blockbelow.y;
        prevSlot = Address::getLocalPlayer()->getSupplies()->hotbarSlot;

        BlockBelow = blockbelow;

        placesPerTickCount = 0;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        placesPerTickCount = 0;

        foundBlock = findBlock();
        if (!foundBlock) return;

        float speed = Address::getLocalPlayer()->getStateVector()->Velocity.magnitudexz();
        Vector3<float> velocity = Address::getLocalPlayer()->getStateVector()->Velocity.Normalize();
        Vector3<float> blockBelow = getBlockBelow(player);
        BlockBelow = blockBelow;
        Vector3<float> yLockedBlockBelow = Vector3<float>(blockBelow.x, lockedY, blockBelow.z);
        if (Ylock) blockBelow = yLockedBlockBelow;

        //Horizontal
        if (Ylock) {
            adjustYCoordinate(getBlockBelow(player));
            blockBelow = yLockedBlockBelow;
        }
        else lockedY = blockBelow.y;
        for (int i = 0; i < placesPerTick; i++) {
            //Place block below
            if (canPlace(blockBelow)) {
                if (predictBlock(blockBelow)) {
                    // Placed
                }
            }
            //Extend
            if (!isHoldingSpace) {
                if (placement == 1) {
                    for (int i = 1; i < extend; i++) {
                        Vector3<float> nextBlock = getNextBlock(velocity, blockBelow, i);
                        if (canPlace(nextBlock) && predictBlock(nextBlock)) {
                            // Placed
                            break;
                        }
                    }
                }
                else {
                    for (int i = 1; i < extend; i++) {
                        Vector3<float> nextBlock = getExtendedPosition(velocity, blockBelow, i);
                        if (canPlace(nextBlock) && predictBlock(nextBlock)) {
                            // Placed
                            break;
                        }
                    }
                }
            }
        }

        //Vertical
        if (Global::Keymap[VK_SPACE] && !MovementUtils::isMoving()) {
            Vector3<int> pos = Address::getLocalPlayer()->getStateVector()->Position.ToInt();
            lockedY = getBlockBelow(player).y;
            isHoldingSpace = true;
        }
        else {
            isHoldingSpace = false;
        }

        if (towerMode == 2 && !MovementUtils::isMoving()) {
            float speedOfTower = 0.7;
            bool hm = !player->isOnGround() && Global::Keymap[VK_SPACE];
            if (hm) {
                Address::getTimerClass()->setMainTimerSpeed(30.f);
            }
            else {
                Address::getTimerClass()->setMainTimerSpeed(20.f);
            }
        }

        if (towerMode == 1 && !MovementUtils::isMoving()) {
            float speedOfTower = 0.7;
            bool space = Global::Keymap[VK_SPACE];
            static bool oldSpace = false;
            if (space) {
                player->getStateVector()->Velocity.y = speedOfTower;
            }
            else if (oldSpace) {
                player->getStateVector()->Velocity.y = -5.f;
            }

            oldSpace = space;
        }
        
        if (towerMode == 3 && !MovementUtils::isMoving()) {
            float speedOfTower = 0.9;
            bool hm = !player->isOnGround() && Global::Keymap[VK_SPACE];
            if (hm) {
                Address::getTimerClass()->setMainTimerSpeed(11.f);
                player->getStateVector()->Velocity.y = speedOfTower;
            }
            else {
                Address::getTimerClass()->setMainTimerSpeed(20.f);
                player->getStateVector()->Velocity.y = -5.f;
            }
        }

        //Spoof
        if (holdStyle >= 1) {
            if (holdStyle == 2) {
                Misc::SpoofSwitch(blockSlot);
            }

            if (holdStyle == 3) {
                if (!TimeUtils::hasTimeElapsed("SpoofDelayScaffold", 30, false)) {
                    Misc::SpoofSwitch(prevSlot);
                }
                if (TimeUtils::hasTimeElapsed("SpoofDelayScaffold", 31, true)) {

                }
            }

            player->getSupplies()->hotbarSlot = prevSlot;
        }
    }

    void renderAeolusBlockAmountDisplay() {
        if (foundBlock) {
            Player* player = Address::getLocalPlayer();

            if (player == nullptr || !Address::canUseKeys())
                return;

            static EasingUtil inEase;

            inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 10.f / 10);

            float inScale = inEase.easeOutExpo();

            if (inEase.isPercentageMax())
                inScale = 1;

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);

            ImScaleUtil::ImScaleStart();

            GuiData* guidata = Address::getClientInstance()->getGuiData();

            float renderx = guidata->mcResolution.x / 2;
            float rendery = (guidata->mcResolution.y / 2);

            Vector2<float> textPos = Vector2<float>(renderx, rendery + 110.f);

            auto supplies = player->getSupplies();
            auto inv = supplies->inventory;

            int totalCount = 0;

            for (int i = 0; i < (InvSlot ? 36 : 9); i++) {
                ItemStack* stack = inv->getItem(i);
                if (stack->item != nullptr) {
                    if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
                        totalCount += (int)(*(uint8_t*)(((uintptr_t)stack) + 0x22));
                        if (totalCount != (int)(*(uint8_t*)(((uintptr_t)stack) + 0x22))) {
                            //inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                        }
                    }
                }
            }

            string text = "Blocks Left: " + to_string(totalCount);

            float textSize = 1.18;

            UIColor color = UIColor(255, 255, 255);

            if (totalCount <= 32) {
                color = UIColor(255, 0, 0);
            }

            float textWidth = ImRenderUtil::getTextWidth(&text, textSize);
            float textHeight = ImRenderUtil::getTextHeight(textSize);

            textPos.x -= textWidth / 2;

            Vector4<float> rectPos;
            rectPos.x = textPos.x - 6.f * textSize;
            rectPos.y = textPos.y - 2.f * textSize;
            rectPos.z = textPos.x + textWidth + 6.f * textSize;
            rectPos.w = textPos.y + textHeight + 2.f * textSize;

            ImRenderUtil::fillRectangle(rectPos, UIColor(0, 0, 0), 0.6f);
            ImRenderUtil::fillShadowRectangle(rectPos, UIColor(0, 0, 0), 0.4f, 60, 0);

            ImRenderUtil::drawText(textPos, &text, color, textSize, 1, true);

            ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(rectPos.getCenter().x, rectPos.getCenter().y));

            ImGui::PopFont();
        }
        
    }

    void onEvent(ImGuiRenderEvent* event) override {
        if (foundBlock) {
            Player* player = Address::getLocalPlayer();

            if (counter == 1) {
                renderAeolusBlockAmountDisplay();
            }
        }
    }

    void onEvent(PacketEvent* event) {
        auto player = Address::getLocalPlayer();

        if (!player || !player->getStateVector()) { return; }

        if (Global::isAttacking && AuraBypass) {
            return;
        }

        if (!MovementUtils::isMoving()) {
            if (!player->getMoveInputHandler()->jumping) {
                return;
            }
        }

        float speed = player->getStateVector()->Velocity.magnitudexz();
        Vector3<float> blockBelow = getBlockBelow(player);

        Vector2<float> angleNormal = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockPos.ToFloat());
        Vector2<float> angleMoonwalk = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockBelow.ToFloat());

        if (event->Packet->getId() == PacketID::InventoryTransaction && FlareonV2) {
            auto it = (InventoryTransactionPacket*)event->Packet;
            if (it->mTransaction->type == ComplexInventoryTransaction::Type::ItemUseTransaction)
            {
                ItemUseInventoryTransaction* transac = reinterpret_cast<ItemUseInventoryTransaction*>(it->mTransaction.get());
                if (transac->actionType == ItemUseInventoryTransaction::ActionType::Place)
                    transac->clickPos = Vector3<float>(0, 0, 0);
            }
        }

        //Spoof
        if (event->Packet->getId() == PacketID::MobEquipment && holdStyle == 2) {
            if (player->getSupplies()->hotbarSlot == prevSlot) {
                *event->cancelled = true;
            }
        }

        switch (rotations)
        {
        case 0: //Normal
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angleNormal.x;
                    pkt->rotation.y = angleNormal.y;
                    pkt->headYaw = angleNormal.y;
                }
            }
            break;
        case 1: //Moonwalk
            angleMoonwalk = getNormAngle(angleMoonwalk);

            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angleMoonwalk.x;
                    pkt->rotation.y = angleMoonwalk.y;
                    pkt->headYaw = angleMoonwalk.y;
                }
            }
            break;
        }
    }

    void onEvent(IntersectsTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        auto gm = player->getGamemode();

        if (!player || !player->getStateVector() || Side != 0) { return; }

        if (Global::isAttacking && AuraBypass) {
            return;
        }

        Vector2<float> angleNormal = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockPos.ToFloat());
        Vector2<float> angleMoonwalk = player->getStateVector()->Position.CalcAngle(player->getStateVector()->Position, BlockBelow.ToFloat());

        switch (rotations)
        {
        case 0: //Normal

            player->setPitch(angleNormal.x);
            player->setHeadYaw(angleNormal.y);
            player->setBodyRotation(angleNormal.y);
            break;
        case 1: //Moonwalk
            angleMoonwalk = getNormAngle(angleMoonwalk);
            player->setPitch(angleMoonwalk.x);
            player->setHeadYaw(angleMoonwalk.y);
            player->setBodyRotation(angleMoonwalk.y);
            break;
        }
    }



    void onDisabled() override {
        if (!Address::getClientInstance() || !Address::getLocalPlayer() || !Address::getClientInstance()->getTimerClass())
            return;

        //if(1 < holdStyle) Misc::SpoofSwitch(prevSlot);
        Address::getLocalPlayer()->getSupplies()->hotbarSlot = prevSlot;
    }

    std::string getModeName() override {
        return "";
    }
};
