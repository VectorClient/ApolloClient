#pragma once

class PacketMine : public Module
{
public:
    PacketMine(int keybind = Keys::NONE, bool enabled = false) :
        Module("PacketMine", "Misc", "Mines the blocks nigger", keybind, enabled)
    {
    }

    Vector3<int> BlockPos = Vector3<int>(0, 0, 0);
    int BlockSide = 0;
    bool ShouldDestroy = false;
    int fuckYou = -1;

    void onEvent(PacketEvent* event) override {
        if (Address::getLocalPlayer() == nullptr)
            return;

        if (!ShouldDestroy) return;

        PlayerAuthInputPacket* packet = nullptr;

        Vector2<float> angle = Address::getLocalPlayer()->getStateVector()->Position.CalcAngle(Address::getLocalPlayer()->getStateVector()->Position, BlockPos.ToFloat());

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }

        if (TimeUtils::hasTimeElapsed("PacketMine", 480, false)) {
            if (event->Packet->getId() == PacketID::PlayerAuthInput) {
                auto* pkt = reinterpret_cast<PlayerAuthInputPacket*>(event->Packet);
                if (pkt) {
                    pkt->rotation.x = angle.x;
                    pkt->rotation.y = angle.y;
                    pkt->headYaw = angle.y;
                }
            }
        }
    }

    bool findBestTool(Block* block) {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        bool isAxe = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Wood;
        bool isDirt = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Dirt;
        bool isStone = block->GetBlockLegacy()->GetMaterialType() == MaterialType::Stone;

        for (int i = 0; i < 36; i++) {
            ItemStack* stack = inventory->getItem(i);
            if (stack->item != nullptr) {
                if (isAxe) {
                    if (stack->getItem()->isAxe()) {
                        if (previousSlot != i) {
                            //playerInventory->hotbarSlot = i;
                            fuckYou = i;
                        }
                        return true;
                    }
                }
                if (isDirt) {
                    if (stack->getItem()->nameContains("shovel")) {
                        if (previousSlot != i) {
                            //playerInventory->hotbarSlot = i;
                            fuckYou = i;
                        }
                        return true;
                    }
                }
                else {
                    if (stack->getItem()->isPickaxe()) {
                        if (previousSlot != i) {
                            //playerInventory->hotbarSlot = i;
                            fuckYou = i;
                        }
                        return true;
                    }
                }
            }
        }
    }

    bool SpoofSwitch(int slot) {
        auto player = Address::getLocalPlayer();
        if (!player) return false;
        auto itemStack = player->getSupplies()->inventory->getItem(slot);
        if (!itemStack)
        {
            return false;
        }


        auto mep = MinecraftPackets::createPacket((int)PacketID::MobEquipment);
        auto* pkt = reinterpret_cast<MobEquipmentPacket*>(mep.get());
        //auto* pkt = reinterpret_cast<MobEquipmentPacket*>(mep.get());

        //pkt->mRuntimeId = player->getComponent<RuntimeIDComponent>()->runtimeID;
        pkt->mSlot = slot;
        pkt->mSelectedSlot = slot;
        pkt->mContainerId = 0;
        pkt->mSlotByte = slot;
        pkt->mSelectedSlotByte = slot;
        pkt->mContainerIdByte = 0;

        Address::getClientInstance()->getLoopbackPacketSender()->sendToServer(pkt);

        return true;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (!player || !Address::canUseKeys()) return;

        HitResult* HitResult = player->getLevel()->getHitResult();
        Block* block = Address::getBlockSource()->getBlock(HitResult->IBlockPos);

        bool isDestroyed = false;

        static bool startDestroying = false;

        if (Utils::leftClick) {
            if (block->GetBlockLegacy()->getBlockID() != 0) {
                //player->getGamemode()->startDestroyBlock(BlockPos, BlockSide, isDestroyed);
                BlockPos = HitResult->IBlockPos;
                BlockSide = HitResult->BlockFace;
                TimeUtils::resetTime("PacketMine");
                ShouldDestroy = true;
            }
        }

        if (startDestroying) {
            player->getGamemode()->startDestroyBlock(BlockPos, BlockSide, isDestroyed);
            startDestroying = false;
        }

        if (ShouldDestroy) {
            startDestroying = true;
            findBestTool(Address::getBlockSource()->getBlock(BlockPos));
            if (TimeUtils::hasTimeElapsed("PacketMine", 490, true)) {
                int Slot = player->getSupplies()->hotbarSlot;

                player->getGamemode()->destroyBlock(BlockPos, BlockSide);

                player->getSupplies()->hotbarSlot = Slot;

                ShouldDestroy = false;
            }
            else {
                SpoofSwitch(fuckYou);
            }
        }
    }
};