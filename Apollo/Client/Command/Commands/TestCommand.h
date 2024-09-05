#pragma once

class TestCommand : public Command
{
public:

    TestCommand() : Command("test", "Debugging command for the developers", { "test", "debug" })
    {}

    bool execute(std::vector<std::string> cmd) override
    {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;

        ItemStack* stack = inventory->getItem(playerInventory->hotbarSlot);

        ChatUtils::sendMessage(Utils::combine("Current Slot: ", playerInventory->hotbarSlot));

        if (stack->item != nullptr) {
            ChatUtils::sendMessage(stack->getItem()->name);
            ChatUtils::sendMessage(std::to_string(stack->getItem()->itemId));
            ChatUtils::sendMessage(stack->getHoverName());
            //ChatUtils::sendMessage(stack->getItem()->gettilenName());
        }

        //auto PlayerHealth = Address::getLocalPlayer()->getAttribute(AttributeID::Health).mCurrentValue;
        //ChatUtils::sendMessage(Utils::combine("Test: ", std::to_string(PlayerHealth).c_str()));


        //Block* block = Address::getBlockSource()->getBlock(Address::getLocalPlayer()->getLevel()->getHitResult()->IBlockPos);
        //ChatUtils::sendMessage("tile.unknown " + to_string(block->GetBlockLegacy()->getBlockID()));

        ChatUtils::sendMessage(Utils::combine("Layer: ", Global::LayerName));

        ChatUtils::sendMessage("Test Command");

        return false;
    }
};
