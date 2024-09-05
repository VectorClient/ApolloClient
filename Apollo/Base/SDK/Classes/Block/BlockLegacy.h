#pragma once

class BlockLegacy
{
public:
	char pad_0008[32]; //0x0008
	std::string tileName;

	std::string* getName() {
		return reinterpret_cast<std::string*>((uintptr_t)this + 0x28); // 1.21.2
	}

	int getBlockID() {
		return *reinterpret_cast<int*>((uintptr_t)this + 0x1A6); // 1.21.2
	}

	bool isSolid() {
		return *reinterpret_cast<bool*>((uintptr_t)this + 0x16); // 1.21.2
	}

	Material* GetMaterial() {
		return *reinterpret_cast<Material**>((uintptr_t)this + 0x128); // 1.21.2
	}

	MaterialType GetMaterialType() {
		return GetMaterial()->type;
	}
};