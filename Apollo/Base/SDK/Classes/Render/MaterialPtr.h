#pragma once

class MaterialPtr
{
private:
	char pad_0x0[0x138];
public:
	static MaterialPtr* createMaterial(HashedString const& materialName) {
		static void* materialCreator = nullptr;

		if (materialCreator == nullptr) {
			// Sig returns 6 addresses, all of them point to the same offset
			uintptr_t sigOffset = (uintptr_t)Memory::findSig(xorstr_("48 8B 05 ? ? ? ? 48 8D 55 ? 48 8D 0D ? ? ? ? 48 8B 40 ? FF 15 ? ? ? ? 48 8B F8"));
			int offset = *reinterpret_cast<int*>(sigOffset + 3);
			materialCreator = reinterpret_cast<void*>(sigOffset + offset + 7);
		}

		return Memory::CallVFunc<1, MaterialPtr*>(materialCreator, materialName);
	}
};