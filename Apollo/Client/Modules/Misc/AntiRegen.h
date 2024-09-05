#pragma once

class AntiRegen : public Module
{
public:
    AntiRegen(int keybind = Keys::NUM_0, bool enabled = false) :
        Module("Anti Regen", "Misc", "Cover up all redstones around you cuz of the fat nigga redstone eaters", keybind, enabled)
    {
        addSlider("Range", "How far from you are regen ores covered", &range, 1, 10);
        addSlider("Blocks", "Place Per Ticks", &pps, 1, 10);
    }

private:
    float range = 1.5;
    bool render = true;
    int slot = 0;

    int placedCountAtTick = 0;
    float pps = 1.f;


    Vector3<float> calcPos = Vector3 <float>{ 0, 0, 0 };
    int HotbarSlotPrev = -1;

public:
    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        HotbarSlotPrev = Address::getLocalPlayer()->getSupplies()->hotbarSlot; // This for us to set our slot to the current one
    }

    //Place
    bool canPlace(Vector3<float> pos) {
        return Address::getBlockSource()->getBlock(pos.floor().ToInt())->GetMaterialType() == MaterialType::Air;
    }

    bool findBlock() {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
                    if (previousSlot != i)
                        playerInventory->hotbarSlot = i;
                    return true;
                }
            }
        }
        return false;
    }

    bool buildBlockHive(Vector3<float> blockBelow) {
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
            Vector3<int> BlockPos = Vector3<int>(blockBelow.ToInt());
            if (foundCandidate && i == 1) {
                calcPos = blok.add(Vector3<int>(0.78f, 0.78f, 0.78f)).ToFloat();
                auto player = Address::getLocalPlayer();
                player->getLevel()->getHitResult()->BlockFace = i;
                player->getLevel()->getHitResult()->IBlockPos = BlockPos;
                player->getLevel()->getHitResult()->HitType = 0;
                player->getLevel()->getHitResult()->AbsoluteHitPos = BlockPos.ToFloat();
                Address::getLocalPlayer()->getGamemode()->buildBlock(blok, i, true);
                return true;
            }
        }
        return false;
    }

    std::vector<Actor*> targetList;
    void updateTargetList() {
        auto instance = Address::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();
        auto list = localPlayer->getLevel()->getRuntimeActorList();
        auto lpPos = localPlayer->getStateVector()->Position;
        for (Actor* actor : list) {
            if (actor == localPlayer || !actor->isAlive() || actor->isBot() || (getModuleByName("Teams")->isEnabled() && actor->isTeammate(localPlayer))) continue;
            float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
            if (dist <= range) {
                targetList.push_back(actor);
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        targetList.clear();
        placedCountAtTick = 0;
        static bool foundRedstone = false;
        auto player = Address::getLocalPlayer();
        if (player == nullptr) {
            return;
        }

        GameMode* gamemode = player->getGamemode();

        if (!gamemode || !player)
            return;

        PlayerInventory* supplies = player->getSupplies();

        Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();

        if (foundRedstone) {
            supplies->hotbarSlot = HotbarSlotPrev;
            foundRedstone = false;
        }

        HotbarSlotPrev = supplies->hotbarSlot;

        updateTargetList();
        if (!targetList.empty()) {
            Vector3<int> enemyBlockPos = targetList[0]->getAABBShape()->PosLower.ToInt();
            bool isRegenEnabled = getModuleByName("Regen")->isEnabled();

            for (int x = -range; x < range; x++) {
                for (int y = -range; y < range; y++) {
                    for (int z = -range; z < range; z++) {
                        Vector3<int> blockPos = Vector3<int>(enemyBlockPos.x + x, enemyBlockPos.y + y, enemyBlockPos.z + z);
                        if (playerBlockPos.distance(blockPos) >= range) continue;

                        BlockSource* source = Address::getBlockSource();

                        if (!source)
                            return;

                        Block* block = source->getBlock(blockPos);

                        if (block->GetBlockLegacy()->getBlockID() == 73 || block->GetBlockLegacy()->getBlockID() == 74)
                        {
                            if (Global::miningPosition != NULL) {
                                int dist = 0;
                                dist += abs(Global::miningPosition.x - blockPos.x);
                                dist += abs(Global::miningPosition.y - blockPos.y);
                                dist += abs(Global::miningPosition.z - blockPos.z);
                                if (isRegenEnabled && (blockPos == Global::miningPosition || dist <= 2)) continue;
                            }

                            static std::vector<Vector3<int>> posLists = {
                                Vector3<int>(0, -1, 0), Vector3<int>(0, 1, 0),
                                Vector3<int>(0, 0, -1), Vector3<int>(0, 0, 1),
                                Vector3<int>(-1, 0, 0), Vector3<int>(1, 0, 0),
                            };
                            for (int i = 0; i < posLists.size(); i++) {
                                if (canPlace(blockPos.add(posLists[i]).ToFloat())) {
                                    if (!findBlock()) return;
                                    if (buildBlockHive(blockPos.add(posLists[i]).ToFloat())) {
                                        foundRedstone = true;
                                    }
                                    else supplies->hotbarSlot = HotbarSlotPrev;
                                    placedCountAtTick++;
                                    if ((int)pps <= placedCountAtTick) {
                                        //supplies->hotbarSlot = HotbarSlotPrev;
                                        return;
                                    }
                                }
                                else
                                {
                                    continue;
                                }
                            }
                        }
                    }
                }
            }
        }
    }

    void onDisabled() override {
        if (!Address::getLocalPlayer()) {
            return;
        }

        Address::getLocalPlayer()->getSupplies()->hotbarSlot = HotbarSlotPrev;
    }
};
