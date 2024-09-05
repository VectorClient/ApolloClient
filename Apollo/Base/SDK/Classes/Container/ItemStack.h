#pragma once

class ItemStack
{
public:
	Item* getItem()
	{
		return *this->item;
	}
	bool isBlockType()
	{
		return *(uintptr_t**)((uintptr_t)(this) + 0x18) != nullptr;
	}

	int32_t getEnchantValue(EnchantType enchantType) {
		using getEnchantsLevel_t = int32_t(__cdecl*)(EnchantType, ItemStack*);
		static auto getEnchantsLevel = reinterpret_cast<getEnchantsLevel_t>(Memory::findSig(xorstr_("48 89 5c 24 ?? 48 89 74 24 ?? 57 48 83 ec ?? 48 8b f2 0f b6 d9")));
		return getEnchantsLevel(enchantType, this);
	}

	std::string getHoverName() {
		std::string name;
		static uintptr_t func = Memory::getXref((uintptr_t)Memory::findSig("E8 ? ? ? ? 48 8D ? ? ? ? ? 48 8B ? E8 ? ? ? ? 48 8B ? ? ? 48 83 FA ? 0F 82"));
		static auto _ItemStackBase_getName = (std::string * (__fastcall*)(ItemStack*, std::string*))func;
		_ItemStackBase_getName(this, &name);
		return name;
	}

	bool customNameContains(const std::string& itemName) {
		if ((getHoverName()).find(itemName) != std::string::npos) return true;
		return false;
	}

	inline int32_t getEnchantValue(int8_t enchantType) {
		return this->getEnchantValue(static_cast<EnchantType>(enchantType)); 
	}

	float getArmorValueWithEnchants() {
		if (!this->item || !(*this->item)->isArmor())
			return 0;

		return (float)(((*this->item)->getArmorValue() + ((this->getEnchantValue(0) * 1.5f         // Protection
			+ this->getEnchantValue(5) * 0.4f       // Thorns
			+ this->getEnchantValue(3) * 0.4f       // Blast Protection
			+ this->getEnchantValue(1) * 0.4f       // Fire Protection
			+ this->getEnchantValue(4) * 0.4f       // Projectile Protection
			+ this->getItem()->getToughnessValue()  // Prefer Netherite Over Diamond
			))));
	}

	//Item* item;
	//bool isBlock;
	//uint8_t count;
	BUILD_ACCESS(Item**, item, 0x8);
	BUILD_ACCESS(INT8, count, 0x22);
};