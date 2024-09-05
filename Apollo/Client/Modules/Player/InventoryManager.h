#pragma once

class ItemStatus {
public:
	ItemStatus(ItemStack* item, int slot, int containerSlot, bool isArmor, int armorSlot, bool isHotbar) {
		itemstack = item;
		itemslot = slot;
		containerslot = containerSlot;
		isarmor = isArmor;
		armorslot = armorSlot;
		ishotbar = isHotbar;
	}
	ItemStack* itemstack = nullptr;
	int itemslot = 0;
	int containerslot = 0;
	bool isarmor = false;
	int armorslot = -1;
	bool ishotbar = false;
};

class InventoryManager : public Module
{
public:
	InventoryManager(int keybind = Keys::NONE, bool enabled = false) :
		Module("InventoryManager", "Player", "Manages ur inventory", keybind, enabled)
	{
		addEnum("Mode", "When to manage", { "Inventory" }, &mode);
		addEnum("Drop", "Drop useless items automatically", { "None", "EquipmentOnly", "All" }, &dropUselessItems);
		addEnum("Tools", "Where should tools move", { "None", "Hotbar", "Inventory" }, &toolsMode);
		addBool("AutoArmor", "Equip best armor automatically", &autoArmor);
		addBool("SortSword", "Sort sword automatically", &sortSword);
		addBool("KeepBoomBox", "Do not drop boomboxes", &keepBoomBox);
		addBool("KeepEmberSword", "Do not drop ember sword", &keepEmberSword);
		addBool("KeepSpells", "Do not drop spell books", &keepSpells);
		addSlider("Delay", "Interval of manage inventory", &delay, 0, 500);
	}

private:
	bool isInvOpen = false;

	bool sortSword = true;
	bool autoArmor = true;
	bool keepBoomBox = true;
	bool keepEmberSword = true;
	bool keepSpells = true;
	int dropUselessItems = 0;
	int toolsMode = 0;
	int mode = 0;
	float delay = 0.f;
public:
	void onEvent(LayerEvent* event) override {
		if (UILayer::Is(event->GuiLayer, { UILayer::Ingame_InventoryScreen }))
		{
			isInvOpen = true;
		}
		else {
			isInvOpen = false;
		}
	}

	void onEvent(ContainerTickEvent* event) override
	{
		auto player = Address::getLocalPlayer();
		if (player == nullptr) return;
		ContainerScreenController* controller = event->Controller;
		if (!controller)
			return;
		if (!isInvOpen) return;
		if (!TimeUtils::hasTimeElapsed("invManagerDelay", delay, true)) return;

		std::vector<ItemStatus> allItems;
		std::vector<int> dropSlots;

		//Add all items to list
		int slotCount = 0;
		// Hotbar
		for (int i = 0; i < 9; i++) {
			ItemStack* inventoryStack = controller->_getItemStack(Containers::Hotbar, i);
			if (inventoryStack->item != nullptr) {
				bool isArmor = false;
				int armorSlot = -1;
				if (inventoryStack->getItem()->isHelmet()) {
					isArmor = true;
					armorSlot = 0;
				}
				else if (inventoryStack->getItem()->isChestplate()) {
					isArmor = true;
					armorSlot = 1;
				}
				else if (inventoryStack->getItem()->isLeggings()) {
					isArmor = true;
					armorSlot = 2;
				}
				else if (inventoryStack->getItem()->isBoots()) {
					isArmor = true;
					armorSlot = 3;
				}
				allItems.push_back(ItemStatus(inventoryStack, slotCount, i, isArmor, armorSlot, true));
			}
			slotCount++;
		}
		//Inventory
		for (int i = 0; i < 27; i++) {
			ItemStack* inventoryStack = controller->_getItemStack(Containers::Inventory, i);
			if (inventoryStack->item != nullptr) {
				bool isArmor = false;
				int armorSlot = -1;
				if (inventoryStack->getItem()->isHelmet()) {
					isArmor = true;
					armorSlot = 0;
				}
				else if (inventoryStack->getItem()->isChestplate()) {
					isArmor = true;
					armorSlot = 1;
				}
				else if (inventoryStack->getItem()->isLeggings()) {
					isArmor = true;
					armorSlot = 2;
				}
				else if (inventoryStack->getItem()->isBoots()) {
					isArmor = true;
					armorSlot = 3;
				}
				allItems.push_back(ItemStatus(inventoryStack, slotCount, i, isArmor, armorSlot, false));
			}
			slotCount++;
		}

		//AutoArmor
		if (!allItems.empty() && autoArmor) {
			for (int i = 0; i < 4; i++) {
				ItemStack* stack = controller->_getItemStack(Containers::Armor, i);
				float currentArmorValue = 0;
				bool isHotbar = false;
				int betterArmorSlot = -1;
				if (stack->item != nullptr) {
					currentArmorValue = stack->getItem()->getArmorValueFromName();
				}
				for (auto itemStatus : allItems) {
					if (itemStatus.isarmor && itemStatus.armorslot == i) {
						float currentInventoryArmorValue = itemStatus.itemstack->getItem()->getArmorValueFromName();
						if (currentInventoryArmorValue > currentArmorValue) {
							currentArmorValue = currentInventoryArmorValue;
							betterArmorSlot = itemStatus.containerslot;
							isHotbar = itemStatus.ishotbar;
						}
						else {
							dropSlots.push_back(itemStatus.itemslot);
						}
					}
				}

				//Equip armor
				if (betterArmorSlot != -1 && allItems.size() != 36) {
					if (controller->_getItemStack(Containers::Armor, i)->item != nullptr) {
						controller->shiftItems(Containers::Armor, i);
					}
					if (isHotbar) {
						controller->shiftItems(Containers::Hotbar, betterArmorSlot);
					}
					else {
						controller->shiftItems(Containers::Inventory, betterArmorSlot);
					}
					return;
				}
			}
		}

		//Sort Sword
		if (!allItems.empty() && sortSword) {
			ItemStack* stack = controller->_getItemStack(Containers::Hotbar, 0);
			float currentSwordValue = 0;
			bool isHotbar = false;
			int betterSwordSlot = -1;
			if (stack->item != nullptr) {
				currentSwordValue = stack->getItem()->getSwordValueFromName() + stack->getEnchantValue(9);
			}
			for (auto itemStatus : allItems) {
				if (itemStatus.itemstack->getItem()->isSword()) {
					if (itemStatus.itemstack->getItem()->getSwordValueFromName() + itemStatus.itemstack->getEnchantValue(9) > currentSwordValue) {
						betterSwordSlot = itemStatus.containerslot;
						isHotbar = itemStatus.ishotbar;
					}
					else {
						if (itemStatus.itemslot != 0) {
							if (!(keepEmberSword && itemStatus.itemstack->getItem()->nameContains("golden_sword"))) dropSlots.push_back(itemStatus.itemslot);
						}
					}
				}
			}

			//Move Sword
			if (betterSwordSlot != -1 && allItems.size() != 36) {
				if (controller->_getItemStack(Containers::Hotbar, 0)->item != nullptr) {
					controller->shiftItems(Containers::Hotbar, 0);
				}
				if (isHotbar) {
					controller->shiftItems(Containers::Hotbar, betterSwordSlot);
				}
				else {
					controller->shiftItems(Containers::Inventory, betterSwordSlot);
				}
				return;
			}
		}

		//Find useless items
		if (!allItems.empty() && dropUselessItems >= 2)
			for (auto itemStatus : allItems) {
				if (isUselessItem(itemStatus.itemstack)) dropSlots.push_back(itemStatus.itemslot);
			}

		//Move Tools
		if (1 <= toolsMode) {
			if (toolsMode == 1) { //Hotbar
				for (auto itemStatus : allItems) {
					if (!itemStatus.ishotbar && (itemStatus.itemstack->getItem()->isPickaxe() || itemStatus.itemstack->getItem()->isAxe())) {
						controller->shiftItems(Containers::Inventory, itemStatus.containerslot);
					}
				}
			}
			else if (toolsMode == 2) { //Inventory
				for (auto itemStatus : allItems) {
					if (itemStatus.ishotbar && (itemStatus.itemstack->getItem()->isPickaxe() || itemStatus.itemstack->getItem()->isAxe())) {
						controller->shiftItems(Containers::Hotbar, itemStatus.containerslot);
					}
				}
			}
		}

		//Drop useless items
		if (!dropSlots.empty() && dropUselessItems >= 1)
		{
			for (int i = 0; i < dropSlots.size(); i++) {
				player->getSupplies()->inventory->dropSlot(dropSlots[i], false, true, false);
				if (0.f < delay) return;
			}
		}
	}

	bool isUselessItem(ItemStack* stack) {
		if (stack->getItem() == nullptr) return true;

		//Tool
		if (stack->getItem()->isPickaxe() || stack->getItem()->isAxe()) return false;

		//Sword
		if (stack->getItem()->isSword()) return false;

		// Spell Book
		if (stack->getItem()->nameContains("enchanted_book")) {
			if (keepSpells) {
				return false;
			}
		}

		//Blocks
		if (stack->isBlockType()) {
			if (stack->getItem()->isBoomBox()) {
				if (keepBoomBox) return false;
			}
			else return false;
		}

		//Armor
		if (stack->getItem()->isHelmet() || stack->getItem()->isChestplate() || stack->getItem()->isLeggings() || stack->getItem()->isBoots()) return false;

		//Food
		if (stack->getItem()->isFood()) return false;

		return true;
	}

	std::string getModeName() override {
		return "";
	}
};