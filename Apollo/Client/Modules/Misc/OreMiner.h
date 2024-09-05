#pragma once

class OreMiner : public Module
{
public:
    OreMiner(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("OreMiner", "Misc", "Nukes ores in a radius around you", keybind, enabled)
    {
        addEnum("Mode", "The mode for the delay", { "Milliseconds" }, &DelayMode);
        addEnum("Priority", "The priority for the ore", { "None", "Distance" }, &Priority);
        addSlider("Range", "How far around you is broken", &range, 1, 6);
        addSlider("Time MS", "The delay for breaking in millisecounds", &destroyMs, 1, 1000);
        //addSlider("Delay", "The delay between destroying blocs", &delay, 1, 20);
    }

private:
    int DelayMode = 0;
    int Priority = 0;
    float range = 5;
    bool render = true;

    Vector3<int> miningBlockPos;
    bool isRedstoneGettingDestroyed = false; // Is The RedstoneOre destroying?
    bool isExposed = false; // Is The RedstoneOre exposed?
    int RedstoneSide = 0; // The redstone ore's side.
    int previousSlot = 0; // The slot before destroying.

    int enabledticks;

    int tick = 0;
    int slot = 0;

    float delay = 12;
    bool haspickaxe = true;

    float destroyMs = 800; //you can use getDestroySpeed function instead
public:
    //Functions
    const static Vector2<float> CalcAngleRegen(Vector3<float> ths, Vector3<float> dst)
    {
        Vector3 diff = dst.submissive(ths);

        diff.y = diff.y / diff.magnitude();
        Vector2<float> angles;
        angles.x = asinf(diff.y) * -(180.0f / 3.1415927f);
        angles.y = (float)-atan2f(diff.x, diff.z) * (180.0f / 3.1415927f);

        return angles;
    }

    bool isAir(Vector3<float> pos) {
        return !Address::getBlockSource()->getBlock(pos.floor().ToInt())->GetBlockLegacy()->isSolid();
        //return true;
    }

    /*bool isRedstoneOreExposed(BlockSource* blockSource, const Vector3<int>& pos) {

        int exposedSides = 0;

        if (isAir(Vector3<float>(pos.x + 1, pos.y + 0, pos.z + 0))) exposedSides++;
        if (isAir(Vector3<float>(pos.x + -1, pos.y + 0, pos.z + 0))) exposedSides++;
        if (isAir(Vector3<float>(pos.x + 0, pos.y + 1, pos.z + 0))) exposedSides++;
        if (isAir(Vector3<float>(pos.x + 0, pos.y + -1, pos.z + 0))) exposedSides++;
        if (isAir(Vector3<float>(pos.x + 0, pos.y + 0, pos.z + 1))) exposedSides++;
        if (isAir(Vector3<float>(pos.x + 0, pos.y + 0, pos.z + -1))) exposedSides++;

        return exposedSides > 0;
    }*/

    bool isValidRedstoneBlock(Vector3<int> blockPos) {
        if (blockPos == NULL) return false;

        BlockSource* source = Address::getBlockSource();
        if (!source) return false;
        Block* block = source->getBlock(blockPos);

        //block exist check
        if (!block) return false;

        //block id check
        int blockId = block->GetBlockLegacy()->getBlockID();
        if (blockId != 56 && blockId != 129 && blockId != 14 && blockId != 15) return false;

        //distance check
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return false;
        if (player->getStateVector()->Position.distance(blockPos.ToFloat()) >= range) return false;

        isExposed = isRedstoneOreExposed(blockPos);
        if (isRedstoneOreExposed(blockPos)) {
            return true;
        }

        //exposed check
        //if (!isExposed(blockPos)) return false;
        //isExposed = isExposed(blockPos);

        return true;
    }

    int getBlockBreakFace(Vector3<float> bp) {
        if (isAir(bp.add(Vector3<float>(0, -1, 0)))) return 1;
        if (isAir(bp.add(Vector3<float>(0, 0, 1)))) return 2;
        if (isAir(bp.add(Vector3<float>(0, 0, -1)))) return 3;
        if (isAir(bp.add(Vector3<float>(1, 0, 0)))) return 4;
        if (isAir(bp.add(Vector3<float>(-1, 0, 0)))) return 5;
        if (isAir(bp.add(Vector3<float>(0, 1, 0)))) return 0;

        return -1;
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
            if (source->getBlock(blockPos.add(checklist[i]))->GetBlockLegacy()->GetMaterialType() == MaterialType::Air) {
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

    int findPickaxe() {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (stack->getItem()->isPickaxe()) {
                    return i;

                    return true;
                }
            }
        }

        return -1;
    }


    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;
        TimeUtils::resetTime("regenMs");
        TimeUtils::resetTime("regenDelay");
        isRedstoneGettingDestroyed = false;
    }

    void onEvent(RenderContextEvent* event) override
    {
        //Avoid hive anticheat checks
        if (!TimeUtils::hasTimeElapsed("regenDelay", 50, false)) {
            return;
        }

        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        GameMode* gamemode = player->getGamemode();
        if (!gamemode || !player)
            return;

        BlockSource* source = Address::getBlockSource();
        if (!source) return;

        Vector3<int> playerBlockPos = player->getStateVector()->Position.ToInt();
        PlayerInventory* supplies = player->getSupplies();
        bool isItDestroyed = false;
        //Global::testString = std::to_string(supplies->inventory->getItem(supplies->hotbarSlot)->item->itemId);

        if (isValidRedstoneBlock(miningBlockPos)) { //If mining
            if (TimeUtils::hasTimeElapsed("regenMs", destroyMs - 50, false)) {
                isRedstoneGettingDestroyed = true;
            }
            else
            {
                isRedstoneGettingDestroyed = false;
            }

            if (TimeUtils::hasTimeElapsed("regenMs", destroyMs, true)) {

                Block* block = source->getBlock(miningBlockPos);

                previousSlot = supplies->hotbarSlot;
                gamemode->destroyBlock(miningBlockPos, RedstoneSide);
                gamemode->stopDestroyBlock(miningBlockPos);
                supplies->hotbarSlot = previousSlot;
                TimeUtils::resetTime("regenDelay");
            }
            else {
                Misc::SpoofSwitch(findPickaxe());
            }
        }
        else //find new block
        {
            stopBreakingBlock(miningBlockPos);
            miningBlockPos = NULL;
            isRedstoneGettingDestroyed = false;
            TimeUtils::resetTime("regenMs");
            vector<Vector3<int>> blocks;

            if (Priority == 0) {
                for (int x = -range; x <= range; x++) {
                    for (int y = -range; y <= range; y++) {
                        for (int z = -range; z <= range; z++) {
                            Vector3<int> blockPos = Vector3<int>(playerBlockPos.x + x, playerBlockPos.y + y, playerBlockPos.z + z);

                            Block* block = source->getBlock(blockPos);

                            if (isValidRedstoneBlock(blockPos) && isExposed)
                            {
                                miningBlockPos = blockPos;

                                RedstoneSide = getBlockBreakFace(blockPos.ToFloat());
                                if (getBlockBreakFace(blockPos.ToFloat()) == -1) continue;

                                gamemode->startDestroyBlock(blockPos, RedstoneSide, isItDestroyed);
                                return;
                            }
                            else if (!isValidRedstoneBlock(blockPos) && !isExposed) {

                            }
                            else continue;
                        }
                    }
                }
            }

            if (Priority == 1) {
                if (blocks.empty()) {
                    for (int x = -range; x <= range; x++) {
                        for (int z = -range; z <= range; z++) {
                            for (int y = -range; y <= range; y++) {
                                if ((player->getStateVector()->Position).distance(player->getStateVector()->Position.add(Vector3<float>(x, y, z))) < range)
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

                    Block* block = source->getBlock(blockPos);

                    if (isValidRedstoneBlock(blockPos) && isExposed)
                    {
                        miningBlockPos = blockPos;

                        RedstoneSide = getBlockBreakFace(blockPos.ToFloat());
                        if (getBlockBreakFace(blockPos.ToFloat()) == -1) continue;

                        gamemode->startDestroyBlock(blockPos, RedstoneSide, isItDestroyed);
                        return;
                    }
                    else if (!isValidRedstoneBlock(blockPos) && !isExposed) {

                    }
                    else continue;
                }
            }
        }

        if (isRedstoneGettingDestroyed) {
            Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

            Global::shouldAttack = false;
        }
        else {
            Global::shouldAttack = true;
        }
    }

    void onEvent(IntersectsTickEvent* event) override {

        auto player = Address::getLocalPlayer();
        if (player == nullptr || !isRedstoneGettingDestroyed || miningBlockPos == NULL) {
            return;
        }
        Vector2<float> angle = CalcAngleRegen(player->getStateVector()->Position, miningBlockPos.ToFloat());

        player->setPitch(angle.x);
        player->setHeadYaw(angle.y);
        player->setBodyRotation(angle.y);

        player->getLevel()->getHitResult()->BlockFace = RedstoneSide;
        player->getLevel()->getHitResult()->IBlockPos = miningBlockPos;
        player->getLevel()->getHitResult()->HitType = 0;
        player->getLevel()->getHitResult()->AbsoluteHitPos = miningBlockPos.ToFloat();
    }

    void onEvent(PacketEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr || !isRedstoneGettingDestroyed || miningBlockPos == NULL) {
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

    void onDisabled() override {
        stopBreakingBlock(miningBlockPos);
        //TimeUtils::resetTime("regenMs");
    }

    std::string getModeName() override {
        return std::string("");
    }
};