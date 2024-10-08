#pragma once

void* onActorBaseTick;

void ActorBaseTickDetourHook(Actor* actor) // Actor*
{
	Player* localPlayer = Address::getLocalPlayer();

	if (localPlayer != nullptr && localPlayer == actor) {
		Level* level = actor->getLevel();
		GameMode* gm = localPlayer->getGamemode();

		ActorBaseTickEvent event{ actor, level, gm }; // ActorBaseTickEvent
		event.cancelled = nullptr;
		CallBackEvent(&event); // Call ActorBaseTick event for modules to be writen on this hook.
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<void*, Actor*>( // CallFunc to call the original.
		onActorBaseTick, actor
	);
}

class ActorBaseTickHook : public FuncHook { // a ActorBaseTick class that inherits the FuncHook template we made
public:
	//
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Using the utils class findSig function to locate the address of the signature

		// Player VTable signature
		void* playerVTableAddr = Memory::findSig(xorstr_("48 8d 05 ? ? ? ? 48 89 01 b8 ? ? ? ? 8d 50 ? 44 8d 48 ? 44 8d 40 ? 66 89 44 24 ? e8 ? ? ? ? 48 8b 8b"));
		
		int offset = *reinterpret_cast<int*>((uintptr_t)playerVTableAddr + 3);
		uintptr_t** VTable = reinterpret_cast<uintptr_t**>((uintptr_t)playerVTableAddr + offset + 7);

		// Give it the address/ptr to the ActorBaseTick function.
		// Give it the detour ptr and then the original callback ptr.

		// Usage: (Sig/Address/Offset, Detour, Original Callback).
		// Don't forget to return its result.

		// Attempt to hook the function at the address stored in playerVTableAddr by replacing it with the address of ActorBaseTickDetour and store the original function's address in onActorBaseTick
		return Memory::HookFunction((void*)VTable[27], (void*)&ActorBaseTickDetourHook, &onActorBaseTick, xorstr_("ActorBaseTick"));

		return true;
	}

	static ActorBaseTickHook& Instance() { // a class setup function called Instance.
		static ActorBaseTickHook instance;
		return instance;
	}
};