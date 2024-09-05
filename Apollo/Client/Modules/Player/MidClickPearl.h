#pragma once

class MidClickPearl : public Module
{
public:
    MidClickPearl(int keybind = Keys::NONE, bool enabled = false) :
        Module("MidClickPearl", "Player", "Automatically ender pearls when you press the middle click button", keybind, enabled)
    {

    }

    void onEvent(RenderContextEvent* event) {
        auto localPlayer = Address::getLocalPlayer();

        if (!localPlayer || !Address::canUseKeys()) return;

        static bool Clicked = false;

        if (Utils::middleClick) {
            Clicked = true;
            Utils::middleClick = false;
        }

        if (Clicked) {
            Clicked = false;

            PlayerInventory* playerInventory = localPlayer->getSupplies();
            Inventory* inventory = playerInventory->inventory;
            auto previousSlot = playerInventory->hotbarSlot;

            for (int n = 0; n < 36; n++) {
                ItemStack* stack = inventory->getItem(n);
                if (stack->item != nullptr) {
                    std::string ItemName = stack->getItem()->name;
                    if (ItemName.find("ender_pearl") != std::string::npos) {
                        if (previousSlot != n) {
                            playerInventory->hotbarSlot = n;
                            localPlayer->getGamemode()->baseUseItem(*stack);
                            playerInventory->hotbarSlot = previousSlot;
                        }
                    }
                    else {
                        ChatUtils::sendMessage("No pearl!");
                        playerInventory->hotbarSlot = previousSlot;
                    }
                }
            }
        }
    }
};