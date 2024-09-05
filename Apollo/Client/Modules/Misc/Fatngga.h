#pragma once

class Fatngga : public Module
{
public:
	Fatngga(int keybind = Keys::NUM_0, bool enabled = false) :
		Module("Fat ngga", "Misc", "Kicks and does the funny to players", keybind, enabled)
	{
		addEnum("Mode", "Lmao", { "Morbidy A Beast" }, &style);
        addSlider("Radius", "How far around you is nuked", &radius, 1, 12);
        addBool("Ore Keeper", "Only nuke other tiles", &orekeeper);
	}

private:
	int style = 0;
    float radius = 3;
    bool orekeeper = false;
public:

    void onEvent(ActorBaseTickEvent* event) override
    {
        Player* player = Address::getLocalPlayer();
        GameMode* gamemode = player->getGamemode();

        if (!gamemode || !player)
            return;

        Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();

        for (int x = -radius; x <= radius; ++x)
        {
            for (int y = -radius; y <= radius; ++y)
            {
                for (int z = -radius; z <= radius; ++z)
                {
                    Vector3<int> blockPos = playerBlockPos.add(Vector3<int>(x, y, z));

                    BlockSource* source = Address::getBlockSource();

                    if (!source)
                        return;

                    Block* block = source->getBlock(blockPos);

                    if (orekeeper)
                    {
                        if (block->GetBlockLegacy()->getBlockID() != 73 || block->GetBlockLegacy()->getBlockID() != 74 || block->GetBlockLegacy()->getBlockID() != 56 || block->GetBlockLegacy()->getBlockID() != 129 || block->GetBlockLegacy()->getBlockID() != 14 || block->GetBlockLegacy()->getBlockID() != 15 || block->GetBlockLegacy()->getBlockID() != 21|| block->GetBlockLegacy()->getBlockID() != 16)
                        {

                        }
                        else continue;
                    }

                    if (blockPos.distance(playerBlockPos) <= radius)
                    {
                        if (blockPos.y <= -64)
                        {
                            if (block->GetBlockLegacy()->getBlockID() == 7)
                            {
                                continue;
                            }
                        }

                        if (block->GetMaterialType() != MaterialType::Lava && block->GetMaterialType() != MaterialType::Water)
                        {
                            BreakBlock(player, gamemode, blockPos, true);
                        }
                    }
                }
            }
        }
    }

    void BreakBlock(Player* player, GameMode* gamemode, Vector3<int> blockPos, bool transactions)
    {
        if (transactions) {
            /*auto pkt = MinecraftPackets::createPacket((int)PacketID::InventoryTransaction);
            auto* transac = reinterpret_cast<InventoryTransactionPacket*>(pkt.get());

            //auto cit = std::make_unique<ItemUseInventoryTransaction>();
            ItemUseInventoryTransaction* cit = reinterpret_cast<ItemUseInventoryTransaction*>(transac->mTransaction.get());
            cit->actionType = ItemUseInventoryTransaction::ActionType::Destroy;
            int slot = player->getSupplies()->hotbarSlot;
            cit->slot = slot;
            cit->itemInHand = *(NetworkItemStackDescriptor*)player->getSupplies()->inventory->getItem(slot);
            cit->blockPos = blockPos;
            cit->face = 1;
            cit->targetBlockRuntimeId = 0; // InfiniteFunnies.Enabled ? 3690217760 : 0
            cit->playerPos = player->getStateVector()->Position;*/
        }
        else {
            gamemode->destroyBlock(blockPos, 1);
        }
    }

    void onEvent(PacketEvent* event) override {
        if (event->Packet->getId() == PacketID::InventoryTransaction) {
            *event->cancelled = true;
            return;
        }
    }
};
