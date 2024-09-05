#pragma once

class RegenOld : public Module
{
public:
    RegenOld(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("RegenOld", "Misc", "Mines redstones in the hive for you to regenerate.", keybind, enabled)
    {
        addEnum("Mode", "The mode for the delay", { "Milliseconds", "Destroy Speed" }, &DelayMode);
        addEnum("Uncover Mode", "The mode for uncover redstone", { "PathFinder", "New" }, &UncoverMode);
        addEnum("Side", "The side for the rotations", { "Client", "Server" }, &Side);
        addSlider("Range", "How far around you find redstones", &range, 1, 10);

        // Milliseconds
        addSlider("Time MS", "The delay for breaking in millisecounds", &destroyMs, 1, 600, 1);
        addSlider("Other MS", "The delay for breaking blocks that sorround the redstone in millisecounds", &blockDestroyMs, 1, 600, 1);

        // Destroy Speed
        addSlider("Destroy Speed", "The destroy speed for regen", &destroySpeed, 0, 1, 1); // 
        addSlider("Other Destroy Speed", "The delay for breaking blocks that sorround the redstone in destroy speed", &blockDestroySpeed, 0, 1, 1);

        // Steal
        addBool("Insta Break Steal", "Insta breaks the stolen block and not wait for any delay", &InstaBreakSteal);
        addBool("Steal", "Steal the enemy's ores", &Global::StealOres);
        addBool("Always Steal", "Always steal the enemy's ores despite you being on full absorption", &AlwaysSteal);
        addEnum("Steal Mode", "The mode for the steal delay", { "Milliseconds", "Destroy Speed" }, &StealProcessMode);
        addSlider("Steal Speed", "The steal destroy speed", &StealDestroySpeed, 0, 1);
        addSlider("Steal MS", "The steal ms speed", &StealDestroyMs, 400, 600);

        // Legit Mine
        addBool("Legit Mine", "Sets your mining style to legit", &LegitMine);
        addBool("Legit Multiplier", "weither to have a custom legit mining speed", &LowMultiplier);
        addSlider("Low Multiplier", "How much to delay the breaking on Legit Mine (Legit Multiplier)", &LowValue, 0, 1);

        addBool("Exposed Only", "Only mine redstone ores that aren't covered.", &exposedOnly);
        addBool("Queue", "Queues redstone for you to mine later.", &queued);
        //addBool("Ignore Covered", "Ignore redstone if it's covered.", &IgnoreCovered);
        addBool("Debug", "Debug for regen.", &debug);
        addBool("Durability Exploit", "Infinite Durability for your pickaxe", &Global::DurabilityExploit);
        addBool("Render", "Render the redstone counter", &render);
    }

private:
    int DelayMode = 0;
    int UncoverMode = 0;
    int Side = 0;
    float range = 5;
    bool render = true;
    bool exposedOnly = true;
    bool queued = false;
    bool debug = false;
    bool AlwaysSteal = false;
    bool LegitMine = true;
    bool LowMultiplier = false;
    float LowValue = 0.75;
    bool InstaBreakSteal = false;
    int StealProcessMode = 0;
    float StealDestroySpeed = 0.75;
    float StealDestroyMs = 490;

    // Uneditable stuff
    Vector3<int> miningBlockPos;
    bool isRedstoneGettingDestroyed = false; // Is The RedstoneOre destroying?
    bool isExposed = false; // Is The RedstoneOre exposed?
    int RedstoneSide = 0; // The redstone ore's side.
    int previousSlot = 0; // The slot before destroying.
    int miningTicks = 0;
    float absorption = 0;

    float destroyMs = 550; // you can use getDestroySpeed function instead
    float blockDestroyMs = 550; // you can use getDestroySpeed function instead
    float currentDestroyMs = 0;

    float destroySpeed = 1; // you can use getDestroySpeed function instead
    float blockDestroySpeed = 1; // you can use getDestroySpeed function instead
    float currentDestroySpeed = 0;

    int targetIndex = 0;
    bool Covered = false;

    bool hasPacketSent = false;
    bool shouldSetBackSlot = false;
    bool hasSelectedSlot = false;
    int currentPacketSlot = 0;
public:
    //Functions
    const static Vector2<float> CalcAngleRegen(Vector3<float> ths, Vector3<float> dst)
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

    bool isAir(Vector3<float> pos) {
        return !Address::getBlockSource()->getBlock(pos.floor().ToInt())->GetBlockLegacy()->isSolid();
    }

    bool isValidBlock(Vector3<int> blockPos, bool isRedstone, bool exposedOnly) {
        if (blockPos == NULL) return false;

        BlockSource* source = Address::getBlockSource();
        if (!source) return false;
        Block* block = source->getBlock(blockPos);

        //block exist check
        if (!block) return false;

        //block id check
        int blockId = block->GetBlockLegacy()->getBlockID();
        bool redstone = blockId == 73 || blockId == 74;
        if (isRedstone) {
            if (!redstone) return false;

            isExposed = isRedstoneOreExposed(blockPos);
        }
        else if (block->GetBlockLegacy()->GetMaterialType() == MaterialType::Air) return false;

        //distance check
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return false;
        if (player->getStateVector()->Position.distance(blockPos.ToFloat()) >= range) return false;

        if (exposedOnly && (UncoverMode == 0 || redstone)) {
            if (!isRedstoneOreExposed(blockPos)) return false;
        }

        return true;
    }

    int getBlockBreakFace(Vector3<float> bp) {
        if (isAir(bp.add(Vector3<float>(0, -1, 0)))) return 1;
        if (isAir(bp.add(Vector3<float>(0, 0, 1)))) return 2;
        if (isAir(bp.add(Vector3<float>(0, 0, -1)))) return 3;
        if (isAir(bp.add(Vector3<float>(1, 0, 0)))) return 4;
        if (isAir(bp.add(Vector3<float>(-1, 0, 0)))) return 5;
        if (isAir(bp.add(Vector3<float>(0, 1, 0)))) return 0;

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
            MaterialType mtype = source->getBlock(blockPos.add(checklist[i]))->GetBlockLegacy()->GetMaterialType();
            if (mtype == MaterialType::Air || mtype == MaterialType::ReplaceablePlant) {
                return true;
            }
        }

        return false;
    }

    void stopBreakingBlock(Vector3<int> blockPos) {
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

    void stopMining() {
        stopBreakingBlock(miningBlockPos);
        miningBlockPos = NULL;
        Global::miningPosition = NULL;
        isRedstoneGettingDestroyed = false;
        Global::shouldAttack = true;
        TimeUtils::resetTime("extraHealthMs");
        hasPacketSent = false;
        hasSelectedSlot = false;
        miningTicks = 0;
        ChatUtils::sendCustomMessage("Stopped mining", "Regen");
    }

    bool findBestTool(Block* block) {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        bool isAxe = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Wood;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (isAxe) {
                    if (stack->getItem()->isAxe()) {
                        if (previousSlot != i) {
                            playerInventory->hotbarSlot = i;
                            currentPacketSlot = i;
                        }
                        return true;
                    }
                }
                else {
                    if (stack->getItem()->isPickaxe()) {
                        if (previousSlot != i) {
                            playerInventory->hotbarSlot = i;
                            currentPacketSlot = i;
                        }
                        return true;
                    }
                }
            }
        }

        return false;
    }

    void setBackSlot() {
        auto player = Address::getLocalPlayer();
        PlayerInventory* supplies = player->getSupplies();
        if (previousSlot == currentPacketSlot) shouldSetBackSlot = false;

        if (supplies->hotbarSlot == previousSlot) {
            supplies->hotbarSlot = currentPacketSlot;
        }
        else supplies->hotbarSlot = previousSlot;
    }

    float calculatePercentage(float currentMs, float destroyMs) {
        return (currentMs / destroyMs) * 100.0f;
    }

    Vector3<int> findPathToBlock(Vector3<int> blockPos, int searchRange) {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return NULL;
        }

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
            if (mtype != MaterialType::Stone && mtype != MaterialType::Wood && mtype != MaterialType::Dirt && mtype != MaterialType::Sand && mtype != MaterialType::StoneDecoration) continue;
            if (isRedstoneOreExposed(currentBlockPos)) return currentBlockPos;
        }
        return NULL;
    }

    Vector3<int> findAboveBlock(Vector3<int> blockPos, int searchRange) {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return NULL;
        }

        Vector3<int> currentBlockPos = Vector3<int>(blockPos.x, blockPos.y + searchRange, blockPos.z);
        Block* currentBlock = Address::getBlockSource()->getBlock(currentBlockPos);
        MaterialType mtype = currentBlock->GetMaterialType();
        if (mtype == MaterialType::Air) return NULL;
        if (mtype != MaterialType::Stone && mtype != MaterialType::Wood && mtype != MaterialType::Dirt && mtype != MaterialType::Sand && mtype != MaterialType::StoneDecoration) return NULL;
        if (searchRange == 1 && !isRedstoneOreExposed(currentBlockPos)) return NULL;
        return currentBlockPos;
    }


    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;
        TimeUtils::resetTime("extraHealthMs");
        isRedstoneGettingDestroyed = false;
        shouldSetBackSlot = false;
    }

    float getDestroySpeed(int slot, Block* block, float destroySpeedDivisor = 1.0f) {
        auto player = Address::getLocalPlayer();
        if (!player) return -1;

        int currentSlot = player->getSupplies()->hotbarSlot;
        player->getSupplies()->hotbarSlot = slot;
        bool onGround = player->isOnGround();
        player->setIsOnGround(true);
        float destroySpeed = player->getGamemode()->getDestroyRate(*block);
        player->setIsOnGround(onGround);

        player->getSupplies()->hotbarSlot = currentSlot;

        return destroySpeed / destroySpeedDivisor;
    }

    int getBestBreakingTool(Block* block) {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        bool isAxe = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Wood;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (isAxe) {
                    if (stack->getItem()->isAxe()) {
                        return i;
                    }
                }
                else {
                    if (stack->getItem()->isPickaxe()) {
                        return i;
                    }
                }
            }
        }

        return 1;
    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player)
            return;

        BlockSource* source = Address::getBlockSource();
        if (!source) return;

        if (player->getAttribute(AttributeId::Health) == nullptr) {
            return;
        }

        absorption = player->getAbsorption();

        Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();
        PlayerInventory* supplies = player->getSupplies();
        Inventory* inventory = supplies->inventory;

        bool isItDestroyed = false;

        bool outBool = false;

        if (Global::StealOres && Global::stealingBlockPos == NULL) {
            TimeUtils::resetTime("StealMSTime");
        }

        if (Global::StealOres && Global::stealingBlockPos != NULL && Global::stealingBlockPos != Global::miningPosition) {
            if (!AlwaysSteal && 10 <= absorption && Global::stealingBlockPos != NULL) {
                Global::stealingBlockPos = NULL;
                return;
            }

            gamemode->startDestroyBlock(Global::stealingBlockPos, 1, outBool);

            Block* stealingBlock = Address::getBlockSource()->getBlock(Global::stealingBlockPos);

            if (outBool) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                Global::miningPosition = Global::stealingBlockPos;
                miningBlockPos = Global::stealingBlockPos;

                gamemode->destroyBlock(Global::stealingBlockPos, 1);

                std::string regenText = Utils::combine("Stolen ore at Vec3I(", std::to_string((int)miningBlockPos.x), ", ", std::to_string((int)miningBlockPos.y), ", ", std::to_string((int)miningBlockPos.y), ")");
                if (debug) {
                    ChatUtils::sendCustomMessage(regenText, "Regen");
                }
                Global::stealingBlockPos = NULL;
            }

            if (StealProcessMode == 1) {
                TimeUtils::resetTime("StealMSTime");
            }

            if (TimeUtils::hasTimeElapsed("StealMSTime", StealDestroyMs, true) && StealProcessMode == 0) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                Global::miningPosition = Global::stealingBlockPos;
                miningBlockPos = Global::stealingBlockPos;

                gamemode->destroyBlock(Global::stealingBlockPos, 1);

                std::string regenText = Utils::combine("Stolen ore at Vec3I(", std::to_string((int)miningBlockPos.x), ", ", std::to_string((int)miningBlockPos.y), ", ", std::to_string((int)miningBlockPos.y), ")");
                if (debug) {
                    ChatUtils::sendCustomMessage(regenText, "Regen");
                }
                Global::stealingBlockPos = NULL;
            }

            float destroySpeed = getDestroySpeed(getBestBreakingTool(stealingBlock), stealingBlock, 1.0f);

            if (destroySpeed == StealDestroySpeed && StealProcessMode == 1) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                Global::miningPosition = Global::stealingBlockPos;
                miningBlockPos = Global::stealingBlockPos;

                gamemode->destroyBlock(Global::stealingBlockPos, RedstoneSide);

                std::string regenText = Utils::combine("Insta mined ore at Vec3I(", std::to_string((int)miningBlockPos.x), ", ", std::to_string((int)miningBlockPos.y), ", ", std::to_string((int)miningBlockPos.y), ")");
                if (debug) {
                    ChatUtils::sendCustomMessage(regenText, "Regen");
                }
                Global::stealingBlockPos = NULL;
            }
        }

        Block* cuhBlock = Address::getBlockSource()->getBlock(Global::miningPosition);

        float DestroySpeed = getDestroySpeed(getBestBreakingTool(cuhBlock), cuhBlock, destroySpeed);

        //Return if extra health is full
        if (10 <= absorption && !queued) {
            stopBreakingBlock(miningBlockPos);
            if (miningBlockPos != NULL) {
                shouldSetBackSlot = true;
                miningBlockPos = NULL;
            }
            Global::miningPosition = NULL;
            isRedstoneGettingDestroyed = false;
            Global::shouldAttack = true;
            hasPacketSent = false;
            hasSelectedSlot = false;
            TimeUtils::resetTime("extraHealthMs");
            miningTicks = 0;
            if (shouldSetBackSlot) setBackSlot();
            return;
        }

        bool shouldQueue = queued && 10 <= absorption;

        if (isValidBlock(miningBlockPos, exposedOnly, true)) { // If mining
            bool isOnGround = player->isOnGround();
            if (!isOnGround && LegitMine) {
                if (LowMultiplier) {
                    currentDestroyMs += 39.5f * LowValue;
                }
                else {
                    currentDestroyMs += 39.5f;
                }
            }

            Block* block = source->getBlock(miningBlockPos);
            if (!shouldQueue) {
                if (hasPacketSent) {
                    if (!hasSelectedSlot) {
                        supplies->hotbarSlot = previousSlot;
                        hasSelectedSlot = true;
                    }
                }
                else {
                    findBestTool(block);
                }
            }

            // Get the mining block ID
            int blockId = source->getBlock(miningBlockPos)->GetBlockLegacy()->getBlockID();

            if (blockId != 73 && blockId != 74) {
                Covered = true;
            }
            else if (blockId == 73 || blockId == 74) {
                Covered = false;
            }

            if (TimeUtils::hasTimeElapsed("extraHealthMs", currentDestroyMs, true) && !shouldQueue) {
                isRedstoneGettingDestroyed = true;
                Global::shouldAttack = false;

                if (findBestTool(block)) {
                    gamemode->destroyBlock(miningBlockPos, RedstoneSide);
                    std::string regenText = Utils::combine("Ore mined at Vec3I(", std::to_string((int)miningBlockPos.x), ", ", std::to_string((int)miningBlockPos.y), ", ", std::to_string((int)miningBlockPos.y), ")");
                    if (debug) {
                        ChatUtils::sendCustomMessage(regenText, "Regen");
                    }
                    hasSelectedSlot = false;
                }
                gamemode->stopDestroyBlock(miningBlockPos);
                shouldSetBackSlot = true;
            }
            else {
                miningTicks++;
                isRedstoneGettingDestroyed = false;
                Global::shouldAttack = true;
            }
        }
        else if (hasSelectedSlot) { //didn't mine redstone
            supplies->hotbarSlot = currentPacketSlot;
            shouldSetBackSlot = true;
            hasSelectedSlot = false;
        }
        else //find new block
        {
            stopBreakingBlock(miningBlockPos);
            miningBlockPos = NULL;
            Global::miningPosition = NULL;
            isRedstoneGettingDestroyed = false;
            Global::shouldAttack = true;
            TimeUtils::resetTime("extraHealthMs");
            previousSlot = supplies->hotbarSlot;
            hasPacketSent = false;
            hasSelectedSlot = false;
            miningTicks = 0;
            shouldSetBackSlot = false;
            ItemStack* stack = supplies->inventory->getItem(supplies->hotbarSlot);
            if (stack != nullptr && stack->isBlockType()) return;
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
                for (const Vector3<int>& blockPos : exposedRedstones) {
                    miningBlockPos = blockPos;
                    Global::miningPosition = blockPos;

                    RedstoneSide = getBlockBreakFace(miningBlockPos.ToFloat());

                    gamemode->startDestroyBlock(miningBlockPos, RedstoneSide, isItDestroyed);
                    currentDestroyMs = destroyMs; // Set the currentDestroyMs to destroyMs
                    currentDestroySpeed = destroySpeed; // Set the currentDestroySpeed to destroySpeed
                    return;
                }
            }
            else if (!exposedOnly && !unExposedRedstones.empty()) {
                switch (UncoverMode) {
                case 0: // PathFind
                    for (int i = 1; i <= 3; i++) {
                        for (const Vector3<int>& blockPos : unExposedRedstones) {
                            auto foundBlock = findPathToBlock(blockPos, i);
                            if (foundBlock == NULL) continue;
                            miningBlockPos = foundBlock;
                            Global::miningPosition = blockPos;

                            RedstoneSide = getBlockBreakFace(miningBlockPos.ToFloat());

                            gamemode->startDestroyBlock(miningBlockPos, RedstoneSide, isItDestroyed);
                            currentDestroyMs = blockDestroyMs; // Set the currentDestroyMs to blockDestroyMs
                            currentDestroySpeed = blockDestroySpeed; // Set the currentDestroySpeed to blockDestroySpeed
                            return;
                        }
                    }
                    break;
                case 1: // New
                    for (int i = 1; i <= 2; i++) {
                        for (const Vector3<int>& blockPos : unExposedRedstones) {
                            auto foundBlock = findAboveBlock(blockPos, i);
                            if (foundBlock == NULL) continue;
                            miningBlockPos = foundBlock;
                            Global::miningPosition = blockPos;

                            RedstoneSide = getBlockBreakFace(miningBlockPos.ToFloat());

                            gamemode->startDestroyBlock(miningBlockPos, RedstoneSide, isItDestroyed);
                            currentDestroyMs = blockDestroyMs; // Set the currentDestroyMs to blockDestroyMs
                            currentDestroySpeed = blockDestroySpeed; // Set the currentDestroySpeed to blockDestroySpeed
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
        if (player == nullptr || !isRedstoneGettingDestroyed || miningBlockPos == NULL) {
            return;
        }
        Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

        if (Side == 0) {
            player->setPitch(angle.x);
            player->setHeadYaw(angle.y);
            player->setBodyRotation(angle.y);
        }

        player->getLevel()->getHitResult()->BlockFace = RedstoneSide;
        player->getLevel()->getHitResult()->IBlockPos = miningBlockPos;
        player->getLevel()->getHitResult()->HitType = 0;
        player->getLevel()->getHitResult()->AbsoluteHitPos = miningBlockPos.ToFloat();
    }

    void onEvent(PacketEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        //Spoof
        if (event->Packet->getId() == PacketID::MobEquipment) {
            auto* pkt = reinterpret_cast<MobEquipmentPacket*>(event->Packet);
            if (pkt->mSlot == previousSlot) {
                if (shouldSetBackSlot || miningBlockPos == NULL) {
                    shouldSetBackSlot = false;
                }
                else {
                    ItemStack* stack = player->getSupplies()->inventory->getItem(pkt->mSlot);
                    if (stack == nullptr || !stack->isBlockType()) {
                        *event->cancelled = true;
                    }
                    else if (miningBlockPos != NULL) {
                        stopMining();
                    }
                }
            }
            else if (pkt->mSlot == currentPacketSlot) {
                hasPacketSent = true;
            }
            else if (miningBlockPos != NULL) {
                stopMining();
            }
        }

        if (!isRedstoneGettingDestroyed || miningBlockPos == NULL) {
            return;
        }

        Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

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
        if (player == nullptr || !render) { //  || !render || miningBlockPos == NULL || !instance->getMinecraftGame()->getCanUseKeys()
            return;
        }

        static EasingUtil inEase;

        (instance->getMinecraftGame()->CanUseKeys && miningBlockPos != NULL && render && !TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 200, false)) ?
            inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 10.f / 10) // Increase the animation
            : inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10); // Decrease the animation

        float inScale = inEase.easeOutExpo();

        if (inEase.isPercentageMax())
            inScale = 1;

        ImScaleUtil::ImScaleStart();

        // Set a UIColor for the render background
        UIColor RenderColor = UIColor(212, 173, 48); // Yellow Color for mining

        float RenderX = instance->getGuiData()->mcResolution.x / 2; // The X Position (center)
        float RenderY = (instance->getGuiData()->mcResolution.y / 2) + 90; // The Y Position (center)

        Vector2<float> RenderPos(RenderX, RenderY);

        // The rendering text
        std::string RenderText = "Queued";

#pragma region RenderColor & RenderText
        if (!isExposed) {
            // If the Block is not a redstone ore
            if (Covered) {
                RenderColor = UIColor(193, 54, 52); // Set the color to red (covered)
                RenderText = "Covered";

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
            else {
                if (absorption >= 10) {
                    RenderColor = UIColor(201, 62, 220); // Set the color to pruple (queued)
                    RenderText = "Queued";
                }
                else {
                    RenderColor = UIColor(212, 173, 48); // Set the color to yellow (mining)
                    RenderText = "Mining";
                }

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
        }
        else {
            // If the Block is not a redstone ore
            if (Covered) {
                RenderColor = UIColor(193, 54, 52); // Set the color to red (covered)
                RenderText = "Covered";

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
            else {
                if (absorption >= 10) {
                    RenderColor = UIColor(201, 62, 220); // Set the color to pruple (queued)
                    RenderText = "Queued";
                }
                else {
                    RenderColor = UIColor(212, 173, 48); // Set the color to yellow (mining)
                    RenderText = "Mining";
                }

                if (TimeUtils::hasTimeElapsed("extraHealthMs", destroyMs - 90, false)) {
                    inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 10.f / 10);
                    //ChatUtils::sendMessage("Uncovering ore");
                }
            }
        }
#pragma endregion

        Vector2<float> TextPos(RenderX, RenderY - 5);
        float TextLength = ImRenderUtil::getTextWidth(&RenderText, 1);
        TextPos.x -= TextLength / 2;

        // The rendering rectangle
        Vector4<float> RenderRect(RenderPos.x - 70, RenderPos.y - 8, RenderPos.x + 70, RenderPos.y + 15);
        Vector4<float> AnimatedRenderRect(RenderRect.x, RenderRect.y, RenderRect.z - 147, RenderRect.w);

        // Draw a gray background
        ImRenderUtil::fillRectangle(RenderRect, UIColor(33, 33, 33), 1.f, 9.f);
        ImRenderUtil::fillShadowRectangle(RenderRect, UIColor(33, 33, 33), 1, 40, 0, 9.f);

        // Draw the color of the thing regen is doing currently to the redstone from (mining, queued, covering, uncovering, jewing & covored)
        ImRenderUtil::fillRectangle(AnimatedRenderRect, RenderColor, 1.f, 9.f);
        ImRenderUtil::fillShadowRectangle(AnimatedRenderRect, RenderColor, 1, 40, 0, 9.f);

        // Draw the current thing regen is doing to the redstone from (mining, queued, covering, uncovering, jewing & covored)
        ImRenderUtil::drawText(TextPos, &RenderText, UIColor(255, 255, 255), 1, 1.f, true);

        ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(RenderRect.getCenter().x, RenderRect.getCenter().y));
        //Vector4<float> rectSmooth = Vector4<float>(RenderRect.x, rect.y, (rect.z - 120) + (cool * 4.4), rect.w);
    }

    void onDisabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return;
        }
        stopBreakingBlock(miningBlockPos);
        if (miningBlockPos != NULL) {
            player->getSupplies()->hotbarSlot = currentPacketSlot;
        }
        Global::shouldAttack = true;
    }

    std::string getModeName() override {
        return std::string("");
    }
};