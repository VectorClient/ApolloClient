#pragma once

class Glint : public Module
{
public:
    Glint(int keybind = Keys::NONE, bool enabled = false) :
        Module("Glint", "Visual", "Edits the enchantment glint", keybind, enabled)
    {

    }

    void onEvent(ActorBaseTickEvent* event) {
        // when low on health we don't have this yet
        auto localPlayer = Address::getLocalPlayer();

        if (!localPlayer || !Address::canUseKeys()) return;

        PlayerInventory* playerInventory = localPlayer->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                stack->getItem()->setIsGlint(true);
            }
        }
    }
};