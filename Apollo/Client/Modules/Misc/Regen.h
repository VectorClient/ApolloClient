#pragma once

class Regen : public Module
{
public:
    Regen(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("Regen", "Misc", "Mines redstones in the hive for you to regenerate.", keybind, enabled)
    {
        addEnum("Mode", "The mode for regen", { "Destroy Speed" }, &mode);
        addEnum("Uncover Mode", "The mode for uncover redstone", { "PathFinder", "New" }, &UncoverMode);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addSlider("Range", "How far around you find redstones", &range, 1, 14);
        addSlider("Destroy Speed", "The destroy speed for regen", &destroySpeed, 0, 100, 1);
        addBool("Uncover", "Uncover redstone ores if there are no exposed redstones", &uncover);
        addBool("Queue", "Queues redstone for you to mine later.", &queued); // TODO
        addBool("Debug", "Debug for regen.", &debug);
        addBool("Steal", "Steal the enemy's ores", &Global::StealOres);
        addBool("Always Steal", "Always steal ores", &alwaysSteal);
        addBool("Confuse", "Confuse stealer", &confuse);
        //addBool("Anti Confuse", "Prevent stealer confused", &antiConfuse); //TODO
        addBool("Anti Steal", "Stop mining If someone started destroying ur block before uncovering it", &antiSteal);
        addBool("Anti Cover", "Keep mining If someone covered redstone", &antiCover);
        addSlider("Compensation", "No description :P", &antiCoverCompensation, 20, 100, 1);
        addBool("Durability Exploit", "it gives you infinite durability", &InfDurability);
        addBool("Hotbar Only", "avoid invalid packet", &hotbarOnly);
        addEnum("Visual", "The mode for visualize", { "None", "Aeolus", "Lunaris" }, &renderMode);
    }

private:
    int mode = 0;
    int UncoverMode = 0;
    int Side = 0;
    float range = 5;
    float destroySpeed = 100;
    int renderMode = 1;
    bool queued = false;
    bool uncover = true;
    bool debug = false;
    bool InfDurability = false;
    bool hotbarOnly = true;
    bool alwaysSteal = false;
    bool antiSteal = false;
    bool antiCover = false;
    bool confuse = false;
    bool antiConfuse = false;
    bool isConfuserActivated = false;
    Vector3<int> lastConfusedPos = NULL;
    float antiCoverCompensation = 100;

    // Uneditable stuff
    bool isRedstoneGettingDestroyed = false; // Is The RedstoneOre destroying?
    bool isRedstoneGettingDestroyedServer = false; // Is The RedstoneOre destroying?
    bool isExposed = false; // Is The RedstoneOre exposed?
    int currentBlockFace = 0; // The redstone ore's side.
    int previousSlot = 0; // The slot before destroying.
    float absorption = 0;

    float currentDestroyPercentage = 0;

    bool canSteal = false;
    bool Covered = false;

    bool selectedTool = false;
    bool shouldSetBackSlot = false;
    int currentServerSlot = 0;
    int ToolSlot = 0;
public:
#pragma region Funtions
    const static Vector2<float> CalcAngle(Vector3<float> ths, Vector3<float> dst)
    {
        float deltaX = dst.x - ths.x;
        float deltaZ = dst.z - ths.z;
        float deltaY = dst.y - ths.y;
        float deltaXZ = hypot(deltaX, deltaZ);

        float yaw = atan2(-deltaX, deltaZ);

        float yawDegrees = yaw * (180 / PI);
        float pitch = atan2(deltaY, deltaXZ) * (180 / PI);

        return Vector2<float>(-pitch, yawDegrees);
    }

    bool isValidBlock(Vector3<int> blockPos, bool redstoneOnly, bool exposedOnly) {
        if (blockPos == NULL) return false;

        BlockSource* source = Address::getBlockSource();
        if (!source) return false;
        Block* block = source->getBlock(blockPos);

        //block exist check
        if (!block) return false;

        //block id check
        int blockId = block->GetBlockLegacy()->getBlockID();
        bool redstone = blockId == 73 || blockId == 74;
        if (redstoneOnly) {
            if (!redstone) return false;

            isExposed = isRedstoneOreExposed(blockPos);
        }
        else if (block->GetBlockLegacy()->GetMaterialType() == MaterialType::Air) return false;

        //distance check
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return false;
        if (player->getAABBShape()->PosLower.distance(blockPos.ToFloat()) >= range) return false;

        //exposed check
        bool isExposedBlock = isRedstoneOreExposed(blockPos);
        if (exposedOnly) {
            if (!isExposedBlock) return false;
        }

        //anti steal
        if (antiSteal && blockPos == Global::blacklistedBlockPos && !isExposedBlock) {
            std::string regenText = Utils::combine("Someone tried to steal your ore at Vec3I(", std::to_string((int)Global::miningPosition.x), ", ", std::to_string((int)Global::miningPosition.y), ", ", std::to_string((int)Global::miningPosition.y), ")");
            if (debug) ChatUtils::sendCustomMessage(regenText, "Regen");
            return false;
        }

        return true;
    }

    // Used for uncover
    Vector3<int> findPathToBlock(Vector3<int> blockPos, int searchRange) {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return NULL;

        static vector<Vector3<int>> blocks;

        if (blocks.empty()) {
            for (int x = -searchRange; x <= searchRange; x++) {
                for (int z = -searchRange; z <= searchRange; z++) {
                    for (int y = -searchRange; y <= searchRange; y++) {
                        if (abs(x) + abs(y) + abs(z) <= searchRange) blocks.push_back(Vector3<int>(x, y, z));
                    }
                }
            }
            sort(blocks.begin(), blocks.end(), [](Vector3<int> start, Vector3<int> end) {
                return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
                });
        }

        for (const Vector3<int>& offset : blocks) {
            Vector3<int> currentBlockPos = Vector3<int>(blockPos.x + offset.x, blockPos.y + offset.y, blockPos.z + offset.z);
            Block* currentBlock = Address::getBlockSource()->getBlock(currentBlockPos);
            MaterialType mtype = currentBlock->GetMaterialType();
            if (isRedstoneOreExposed(currentBlockPos)) return currentBlockPos;
        }
        return NULL;
    }

    Vector3<int> findAboveBlock(Vector3<int> blockPos, int searchRange) {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return NULL;

        Vector3<int> currentBlockPos = Vector3<int>(blockPos.x, blockPos.y + searchRange, blockPos.z);
        Block* currentBlock = Address::getBlockSource()->getBlock(currentBlockPos);
        MaterialType mtype = currentBlock->GetMaterialType();
        if (mtype == MaterialType::Air) return NULL;
        if (searchRange == 1 && !isRedstoneOreExposed(currentBlockPos)) return NULL;
        return currentBlockPos;
    }

    int getExposedBlockFace(Vector3<int> blockPos) {
        BlockSource* source = Address::getBlockSource();
        if (!source) return false;

        static std::vector<Vector3<int>> checklist = {
            Vector3<int>(0, 1, 0), Vector3<int>(0, -1, 0),
            Vector3<int>(0, 0, 1), Vector3<int>(0, 0, -1),
            Vector3<int>(1, 0, 0), Vector3<int>(-1, 0, 0),
        };
        for (int i = 0; i < checklist.size(); i++) {
            if (source->getBlock(blockPos.add(checklist[i]))->GetBlockLegacy()->GetMaterialType() == MaterialType::Air) return i;
        }
        return 1;
    }

    bool isRedstoneOreExposed(Vector3<int> blockPos) {
        BlockSource* source = Address::getBlockSource();
        if (!source) return false;
        static std::vector<Vector3<int>> checklist = {
            Vector3<int>(0, -1, 0), Vector3<int>(0, 1, 0),
            Vector3<int>(0, 0, -1), Vector3<int>(0, 0, 1),
            Vector3<int>(-1, 0, 0), Vector3<int>(1, 0, 0),
        };
        for (int i = 0; i < checklist.size(); i++) {
            bool isAir = source->getBlock(blockPos.add(checklist[i]))->GetBlockLegacy()->GetMaterialType() == MaterialType::Air;
            if (isAir) {
                return true;
            }
        }

        return false;
    }

    void startDestroyBlock(Vector3<int> blockPos, Vector3<int> targettingBlockPos) {
        auto player = Address::getLocalPlayer();
        if (!player) return;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode)
            return;

        BlockSource* source = Address::getBlockSource();
        if (!source) return;

        Block* block = source->getBlock(blockPos);
        ToolSlot = getBestTool(block);

        bool isDestroyedOut = false;

        Global::miningPosition = blockPos;
        Global::targettingBlockPos = targettingBlockPos;
        currentBlockFace = getExposedBlockFace(Global::miningPosition);

        player->getLevel()->getHitResult()->BlockFace = currentBlockFace;
        player->getLevel()->getHitResult()->IBlockPos = blockPos;
        player->getLevel()->getHitResult()->HitType = 0;
        player->getLevel()->getHitResult()->AbsoluteHitPos = blockPos.ToFloat();

        gamemode->startDestroyBlock(Global::miningPosition, currentBlockFace, isDestroyedOut);
        
        if (!selectedTool && ToolSlot != -1) {
            player->getSupplies()->hotbarSlot = ToolSlot;
        }

        currentDestroyPercentage = 0;
    }

    void stopDestroyBlock(Vector3<int> blockPos) {
        if (blockPos == NULL) return;

        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player) return;

        BlockSource* source = Address::getBlockSource();
        if (!source) return;

        Block* block = source->getBlock(blockPos);

        if (!block) return;

        gamemode->stopDestroyBlock(blockPos);
    }

    float getDestroySpeed(int slot, Block* block, float destroySpeedDivisor = 1.0f) {
        auto player = Address::getLocalPlayer();
        if (!player) return -1;

        int currentSlot = player->getSupplies()->hotbarSlot;
        player->getSupplies()->hotbarSlot = slot;
        float destroySpeed = player->getGamemode()->getDestroyRate(*block);

        player->getSupplies()->hotbarSlot = currentSlot;

        return destroySpeed / destroySpeedDivisor;
    }

    int getBestTool(Block* block) {
        auto player = Address::getLocalPlayer();
        if (!player) return -1;

        int bestSlot = -1;
        float bestSpeed = 0;

        for (int i = 0; i < 36; i++)
        {
            if (hotbarOnly && 8 < i) continue;
            auto itemStack = player->getSupplies()->inventory->getItem(i);
            if (!itemStack || !itemStack->item) continue;

            float speed = getDestroySpeed(i, block, 1.0f);

            if (speed > bestSpeed)
            {
                bestSpeed = speed;
                bestSlot = i;
            }
        }

        return bestSlot;
    }

    void resetStatus() {
        stopDestroyBlock(Global::miningPosition);
        Global::miningPosition = NULL;
        Global::targettingBlockPos = NULL;
        Global::StopStealing = false;
        isRedstoneGettingDestroyed = false;
        Global::shouldAttack = true;
        selectedTool = false;
        currentDestroyPercentage = 0;
    }

    void setBackSlot() {
        auto player = Address::getLocalPlayer();
        PlayerInventory* supplies = player->getSupplies();
        supplies->hotbarSlot = getInvalidSlot();
    }

    int getInvalidSlot() {
        auto player = Address::getLocalPlayer();
        PlayerInventory* supplies = player->getSupplies();
        for (int i = 0; i < 8; i++) {
            int i2 = 8 - i;
            if (i2 != supplies->hotbarSlot && i2 != currentServerSlot && i2 != ToolSlot && i2 != previousSlot) return i2;
        }
        return 0;
    }
#pragma endregion

    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;
        resetStatus();
        currentServerSlot = player->getSupplies()->hotbarSlot;
    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player)
            return;

        BlockSource* source = Address::getBlockSource();
        if (!source) return;

        if (player->getAttribute(AttributeId::Health) == nullptr) return;
        absorption = player->getAbsorption();

        Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();
        canSteal = isValidBlock(Global::stealingBlockPos, true, false);
        PlayerInventory* supplies = player->getSupplies();
        Inventory* inventory = supplies->inventory;
        bool isOnGround = player->isOnGround();
        bool isItDestroyed = false;

        //Return if extra health is full
        if (10 <= absorption && (!alwaysSteal || !Global::StealOres || !canSteal) && !queued) {
            if (isConfuserActivated) {
                gamemode->stopDestroyBlock(lastConfusedPos);
                isConfuserActivated = false;
            }
            if (Global::miningPosition != NULL || selectedTool) {
                supplies->hotbarSlot = currentServerSlot;
                shouldSetBackSlot = true;
            }
            if(shouldSetBackSlot) setBackSlot();
            resetStatus();
            return;
        }

        // Get the block
        Block* block = source->getBlock(Global::miningPosition);
        // Get the mining block ID
        int blockId = block->GetBlockLegacy()->getBlockID();
        bool isRedstone = blockId == 73 || blockId == 74;
        bool noExposed = true;
        bool ignoreCovered = false;
        if ((Global::StealOres && canSteal && Global::miningPosition == Global::stealingBlockPos) || (UncoverMode == 1 && !isRedstone)) noExposed = false;
        if (antiCover && (antiCoverCompensation / 100) <= currentDestroyPercentage) {
            noExposed = false;
            ignoreCovered = true;
        }

        bool shouldQueue = queued && 10 <= absorption;

        if (isValidBlock(Global::miningPosition, !uncover, noExposed) && !Global::StopStealing) { // If mining
            int toolSlot = getBestTool(block);
            currentDestroyPercentage += getDestroySpeed(toolSlot, block);
            ToolSlot = toolSlot;

            bool isExposed = isRedstoneOreExposed(Global::miningPosition);

            if (!shouldQueue) {
                if (selectedTool) {
                    previousSlot = supplies->hotbarSlot;
                }
            }

            if (isRedstone) Covered = false;
            else Covered = true;

            if ((!isRedstone || isExposed || ignoreCovered) && (destroySpeed - 23 / 100) <= currentDestroyPercentage && !shouldQueue) {
                isRedstoneGettingDestroyedServer = true;
            }
            else {
                isRedstoneGettingDestroyedServer = false;
            }

            if ((!isRedstone || isExposed || ignoreCovered) && (destroySpeed / 100) <= currentDestroyPercentage && !shouldQueue) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                if (toolSlot != -1) {
                    if (InfDurability) supplies->hotbarSlot = getInvalidSlot();
                    else supplies->hotbarSlot = toolSlot;
                    gamemode->destroyBlock(Global::miningPosition, currentBlockFace);
                    std::string regenText = Utils::combine("Ore mined at Vec3I(", std::to_string((int)Global::miningPosition.x), ", ", std::to_string((int)Global::miningPosition.y), ", ", std::to_string((int)Global::miningPosition.y), ")");

                    if (Global::StealOres && canSteal && Global::miningPosition == Global::stealingBlockPos) 
                        regenText = Utils::combine("Stole ore at Vec3I(", std::to_string((int)Global::miningPosition.x), ", ", std::to_string((int)Global::miningPosition.y), ", ", std::to_string((int)Global::miningPosition.y), ")");

                    if (debug) ChatUtils::sendCustomMessage(regenText, "Regen");
                }
                gamemode->stopDestroyBlock(Global::miningPosition);
                supplies->hotbarSlot = currentServerSlot;
                shouldSetBackSlot = true;
                setBackSlot();
                selectedTool = false;
                return;
            }
            else {
                isRedstoneGettingDestroyed = false;
                Global::shouldAttack = true;
            }
        }
        else //find new block
        {
            if (selectedTool) {
                shouldSetBackSlot = true;
                setBackSlot();
                resetStatus();
                return;
            }
            else shouldSetBackSlot = false;
            resetStatus();
            previousSlot = supplies->hotbarSlot;
            ItemStack* stack = supplies->inventory->getItem(supplies->hotbarSlot);
            if (stack != nullptr && stack->isBlockType()) return;
            if (Global::StealOres && canSteal) {
                startDestroyBlock(Global::stealingBlockPos, Global::stealingBlockPos);
                return;
            }
            static vector<Vector3<int>> blocks;
            vector<Vector3<int>> unExposedRedstones;
            vector<Vector3<int>> exposedRedstones;

            if (blocks.empty()) {
                for (int x = -range; x <= range; x++) {
                    for (int z = -range; z <= range; z++) {
                        for (int y = -range; y <= range; y++) {
                            blocks.push_back(Vector3<int>(x, y, z));
                        }
                    }
                }
                sort(blocks.begin(), blocks.end(), [](Vector3<int> start, Vector3<int> end) {
                    return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
                    });
            }

            for (const Vector3<int>& offset : blocks) {
                Vector3<int> blockPos = Vector3<int>(playerBlockPos.x + offset.x, playerBlockPos.y + offset.y, playerBlockPos.z + offset.z);

                if (isValidBlock(blockPos, true, false))
                {
                    if (isRedstoneOreExposed(blockPos)) exposedRedstones.push_back(blockPos);
                    else unExposedRedstones.push_back(blockPos);
                }
                else continue;
            }
            if (!exposedRedstones.empty()) {
                // Confuser
                if (confuse) {
                    if (isConfuserActivated) {
                        gamemode->stopDestroyBlock(lastConfusedPos);
                        isConfuserActivated = false;
                    }
                    else if (!unExposedRedstones.empty()) {
                        bool out = false;
                        Vector3<int> confusePos = unExposedRedstones[0].add(Vector3<int>(0, -1, 0));
                        gamemode->startDestroyBlock(confusePos, 0, out);
                        lastConfusedPos = confusePos;
                        isConfuserActivated = true;
                        return;
                    }
                }

                for (const Vector3<int>& blockPos : exposedRedstones) {
                    startDestroyBlock(blockPos, blockPos);
                    return;
                }
            }
            else if (uncover && !unExposedRedstones.empty()) {
                switch (UncoverMode) {
                case 0: // PathFind
                    for (int i = 1; i <= 3; i++) {
                        for (const Vector3<int>& blockPos : unExposedRedstones) {
                            auto foundBlock = findPathToBlock(blockPos, i);
                            if (foundBlock == NULL) continue;
                            startDestroyBlock(foundBlock, blockPos);
                            return;
                        }
                    }
                    break;
                case 1: // New
                    for (int i = 1; i <= 2; i++) {
                        for (const Vector3<int>& blockPos : unExposedRedstones) {
                            auto foundBlock = findAboveBlock(blockPos, i);
                            if (foundBlock == NULL) continue;
                            startDestroyBlock(foundBlock, blockPos);
                            return;
                        }
                    }
                    break;
                }
            }
        }

        if (shouldSetBackSlot) {
            setBackSlot();
        }
    }

    void onEvent(IntersectsTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr || !isRedstoneGettingDestroyed || Global::miningPosition == NULL) {
            return;
        }
        Vector2<float> angle = CalcAngle(player->getStateVector()->Position, Global::miningPosition.ToFloat());

        if (Side == 0) {
            player->setPitch(angle.x);
            player->setHeadYaw(angle.y);
            player->setBodyRotation(angle.y);
        }

        player->getLevel()->getHitResult()->BlockFace = currentBlockFace;
        player->getLevel()->getHitResult()->IBlockPos = Global::miningPosition;
        player->getLevel()->getHitResult()->HitType = 0;
        player->getLevel()->getHitResult()->AbsoluteHitPos = Global::miningPosition.ToFloat();
    }

    void onEvent(PacketEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        // Spoof
        if (event->Packet->getId() == PacketID::MobEquipment) {
            auto* pkt = reinterpret_cast<MobEquipmentPacket*>(event->Packet);
            bool shouldCancel = false;
            ItemStack* stack = player->getSupplies()->inventory->getItem(pkt->mSlot);
            if (shouldSetBackSlot) {
                if (pkt->mSlot != previousSlot) {
                    player->getSupplies()->hotbarSlot = previousSlot;
                    shouldSetBackSlot = false;
                }
            }
            else if (Global::miningPosition != NULL) { // If mining
                if (pkt->mSlot == ToolSlot && !selectedTool) {
                    player->getSupplies()->hotbarSlot = previousSlot;
                    selectedTool = true;
                }
                else if (stack != nullptr && stack->isBlockType()) {
                    resetStatus();
                    selectedTool = false;
                    //ChatUtils::sendCustomMessage("Stopped mining", "Regen");
                }
                else shouldCancel = true;
            }
            if (shouldCancel) *event->cancelled = true;
            else currentServerSlot = pkt->mSlot;
        }

        if (!isRedstoneGettingDestroyed || Global::miningPosition == NULL) return;

        Vector2<float> angle = CalcAngle(player->getStateVector()->Position, Global::miningPosition.ToFloat());

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            auto* pkt = (PlayerAuthInputPacket*)event->Packet;
            if (pkt) {
                pkt->rotation.x = angle.x;
                pkt->rotation.y = angle.y;
                pkt->headYaw = angle.y;
            }
        }
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto instance = Address::getClientInstance();
        auto player = instance->getLocalPlayer();
        if (player == nullptr || renderMode == 0) { //  || !render || miningBlockPos == NULL || !instance->getMinecraftGame()->getCanUseKeys()
            return;
        }

        static EasingUtil inEase;

#pragma region logic
        float maxDestroyProgress = destroySpeed / 100;
        float progress = (currentDestroyPercentage / maxDestroyProgress) * 140;

        if (progress >= 140) {
            progress = 140;
        }

        // Set a UIColor for the render background
        UIColor RenderColor = UIColor(212, 173, 48); // Yellow Color for mining

        // The rendering text
        std::string RenderText = "Queued";

        if (renderMode == 1) {
            // If the Block is not a redstone ore
            if (Covered) {
                RenderColor = UIColor(193, 54, 52); // Set the color to red (covered)
                RenderText = "Covered";
            }
            else {
                if (Global::StealOres && canSteal && Global::miningPosition == Global::stealingBlockPos) {
                    RenderColor = UIColor(140, 50, 170); // Set the color to pruple (stealing)
                    RenderText = "Stealing";
                }
                else if (absorption >= 10) {
                    RenderColor = UIColor(201, 62, 220); // Set the color to pruple (queued)
                    RenderText = "Queued";
                }
                else {
                    RenderColor = UIColor(212, 173, 48); // Set the color to yellow (mining)
                    RenderText = "Mining";
                }
            }
        }

        if (renderMode == 2) {
            float valueN = (currentDestroyPercentage / maxDestroyProgress) * 100;
            std::ostringstream oss;
            oss << std::fixed << std::setprecision(1) << (valueN);
            std::string value = oss.str() + "%";
            if (Covered) {
                if (absorption >= 10) {
                    RenderText = "Queued Cover";
                }
                else {
                    RenderText = "Cover " + value;
                }
            }
            else {
                if (Global::StealOres && canSteal && Global::miningPosition == Global::stealingBlockPos) {
                    RenderText = "Stealing " + value;
                }
                else {
                    if (absorption >= 10) {
                        RenderText = "Queued";
                    }
                    else {
                        RenderText = value;

                        if (valueN >= 98) {
                            RenderText = "Mined";
                        }
                    }
                }
            }
        }

        float RenderX = instance->getGuiData()->mcResolution.x / 2; // The X Position (center)
        float RenderY = (instance->getGuiData()->mcResolution.y / 2) + 90; // The Y Position (center)

        Vector2<float> RenderPos(RenderX, RenderY);

        static float animatedProgress = 0;
        animatedProgress = Math::animate(progress, animatedProgress, ImRenderUtil::getDeltaTime() * 15);
#pragma endregion

#pragma region Aeolus
        if (renderMode == 1) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[4]);
            Vector2<float> TextPos(RenderX, RenderY - 5);
            float TextLength = ImRenderUtil::getTextWidth(&RenderText, 1);
            TextPos.x -= TextLength / 2;

            (instance->getMinecraftGame()->CanUseKeys && Global::miningPosition != NULL && (renderMode > 0) && currentDestroyPercentage <= maxDestroyProgress * 0.8) ?
                inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 10.f / 10) // Increase the animation
                : inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10); // Decrease the animation

            float inScale = inEase.easeOutExpo();

            if (inEase.isPercentageMax())
                inScale = 1;

            ImScaleUtil::ImScaleStart();

            if (maxDestroyProgress * 0.9 <= currentDestroyPercentage) {
                //inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
            }

            // The rendering rectangle
            Vector4<float> RenderRect(RenderPos.x - 70, RenderPos.y - 8, RenderPos.x + 70, RenderPos.y + 15);
            Vector4<float> AnimatedRenderRect(RenderRect.x, RenderRect.y, RenderRect.x + animatedProgress, RenderRect.w);

            // Draw a gray background
            ImRenderUtil::fillRectangle(RenderRect, UIColor(33, 33, 33), 1.f, 9.f);
            ImRenderUtil::fillShadowRectangle(RenderRect, UIColor(33, 33, 33), 1, 40, 0, 9.f);

            // Draw the color of the thing regen is doing currently to the redstone from (mining, queued, covering, uncovering, jewing & covored)
            ImRenderUtil::fillRectangle(AnimatedRenderRect, RenderColor, 1.f, 9.f);
            ImRenderUtil::fillShadowRectangle(AnimatedRenderRect, RenderColor, 1, 40, 0, 9.f);

            // Draw the current thing regen is doing to the redstone from (mining, queued, covering, uncovering, jewing & covored)
            ImRenderUtil::drawText(TextPos, &RenderText, UIColor(255, 255, 255), 1, 1.f, true);

            ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(RenderRect.getCenter().x, RenderRect.getCenter().y));
            ImGui::PopFont();
        }
#pragma endregion

#pragma region Lunaris
        if (renderMode == 2) {
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            Vector2<float> TextPos(RenderX, RenderY - 5);
            float TextLength = ImRenderUtil::getTextWidth(&RenderText, 1);
            TextPos.x -= TextLength / 2;

            (instance->getMinecraftGame()->CanUseKeys&& Global::miningPosition != NULL && (renderMode > 0)) ?
                inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 10.f / 10) // Increase the animation
                : inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10); // Decrease the animation

            float inScale = inEase.easeOutExpo();

            if (inEase.isPercentageMax())
                inScale = 1;

            ImScaleUtil::ImScaleStart();

            Vector4<float> RenderRect(RenderPos.x - 70, RenderPos.y - 6, RenderPos.x + 70, RenderPos.y + 13);
            Vector4<float> AnimatedRenderRect(RenderRect.x, RenderRect.y, RenderRect.x + animatedProgress, RenderRect.w);

            ImRenderUtil::fillRectangle(RenderRect, UIColor(0, 0, 0), 0.3f * inScale, 9.f);
            ImRenderUtil::fillShadowRectangle(RenderRect, UIColor(0, 0, 0), 0.8f * inScale, 40, 0, 9.f);

            ImRenderUtil::fillRectangle(AnimatedRenderRect, ColorUtils::getClientColor(1.9, 1, 1, 1), 1.f * inScale, 9.f);
            ImRenderUtil::fillShadowRectangle(AnimatedRenderRect, ColorUtils::getClientColor(1.9, 1, 1, 400), 1 * inScale, 40, 0, 9.f);

            ImRenderUtil::drawText(TextPos, &RenderText, UIColor(255, 255, 255), 1, 1.f, false);

            ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(RenderRect.getCenter().x, RenderRect.getCenter().y));
            ImGui::PopFont();
        }
#pragma endregion
    }

    void onDisabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        stopDestroyBlock(Global::miningPosition);
        if (Global::miningPosition != NULL) {
            player->getSupplies()->hotbarSlot = currentServerSlot;
        }
        Global::shouldAttack = true;
    }

    std::string getModeName() override {
        return " " + std::to_string((int)destroySpeed) + std::string("%");
    }
};
