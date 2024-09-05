#pragma once

// Declare a void pointer(Empty object) called onGameMode which is the CallBack for GameModeDetour.
void* onAttack;

void AttackDetour(GameMode* _this, Player* a1) {
	Memory::CallFunc<void*, GameMode*, Player*>(
		onAttack,
		_this,
		a1
	);
}

class GameModeHook : public FuncHook {
public:
	bool Initialize() override
	{
		// GameMode::GameMode()
		void* GameModeAddr = Memory::findSig(xorstr_("48 8D 05 ? ? ? ? 48 89 01 48 89 51 08 48 C7 41 ? ? ? ? ? C7 41 ? ? ? ? ?"));
		auto Offset = *(int*)((uintptr_t)GameModeAddr + 3);
		auto GameModeVTable = reinterpret_cast<uintptr_t**>((uintptr_t)GameModeAddr + Offset + 7);
		//auto GameModeVTable = *(uintptr_t**)Address::getLocalPlayer()->getGamemode();

		// Attempt to hook the function at the address stored in GameModeVTable by replacing it with the address of AttackDetour and store the original function's address in onAttack
		return Memory::HookFunction((void*)GameModeVTable[14], (void*)&AttackDetour, &onAttack, xorstr_("GameMode::attack"));

		return true;
	}

	static GameModeHook& Instance() { // a class setup function called Instance.
		static GameModeHook instance;
		return instance;
	}
};