#pragma once

class Misc {
public:
    static bool SpoofSwitch(int slot) {
        auto player = Address::getLocalPlayer();
        if (!player) return false;
        auto itemStack = player->getSupplies()->inventory->getItem(slot);
        if (!itemStack)
        {
            return false;
        }


        auto mep = MinecraftPackets::createPacket((int)PacketID::MobEquipment);
        auto* pkt = reinterpret_cast<MobEquipmentPacket*>(mep.get());

        pkt->mSlot = slot;
        pkt->mSelectedSlot = slot;
        pkt->mContainerId = 0;
        pkt->mSlotByte = slot;
        pkt->mSelectedSlotByte = slot;
        pkt->mContainerIdByte = 0;

        Address::getClientInstance()->getLoopbackPacketSender()->sendToServer(pkt);

        return true;
    }

    static inline float getDestroySpeed(int slot, Block* block, float destroySpeedDivisor = 1.0f) {
        auto player = Address::getLocalPlayer();
        if (!player) return -1;

        int currentSlot = player->getSupplies()->hotbarSlot;
        player->getSupplies()->hotbarSlot = slot;
        float destroySpeed = player->getGamemode()->getDestroyRate(*block);

        player->getSupplies()->hotbarSlot = currentSlot;

        return destroySpeed / destroySpeedDivisor;
    }

    static inline int getBestBreakingTool(Block* block) {
        auto player = Address::getLocalPlayer();
        if (!player) return 0;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode) return 0;

        PlayerInventory* supplies = player->getSupplies();
        int previousSlot = supplies->hotbarSlot;

        int slot = 0;
        float fastestSpeed = 0;

        for (int i = 0; i < 36; i++)
        {
            auto itemStack = player->getSupplies()->inventory->getItem(i);
            if (!itemStack || !itemStack->item) continue;
            supplies->hotbarSlot = i;
            float speed = gamemode->getDestroyRate(*block);

            if (fastestSpeed < speed)
            {
                fastestSpeed = speed;
                slot = i;
            }
        }

        supplies->hotbarSlot = previousSlot;
        return slot;
    }
};