#pragma once

// Declare a void pointer(Empty object) called onActorRenderDispatcher which is the CallBack for ActorRenderDispatcherDetour.
void* onActorRenderDispatcher; // Defined an orignal for onActorRenderDispatcher

// Declare a void pointer(Empty object) called onIntersects which is the CallBack for IntersectsDetour.
void* onIntersects; // Defined an orignal for onIntersects

// Declare a void pointer(Empty object) called onIsImmobile which is the CallBack for IsImmobileDetour.
void* onIsImmobile; // Defined an orignal for onIsImmobile

void* onIsOnFire;

void* onCanBeEffected;

void* onDestroySpeed;

void* ApplyRotationChanges(void* _this, void* a2, Actor* actor, Vector3<float> cameraPos, Vector3<float> actorPosition, Vector2<float>* a6, bool renderCurrentPos) {
	bool displayRotations = getModuleByName("serverrotations")->isEnabled();

	if (!displayRotations) {
		return Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
			onActorRenderDispatcher, _this, a2, actor, cameraPos, actorPosition, a6, renderCurrentPos
		);
	}

	float pitch = actor->getPitch();
	float bodyYaw = actor->getBodyRotation();
	float headYaw = actor->getHeadYaw();

	float oldPitch = actor->getComponent<ActorRotationComponent>()->oldRotation.x;
	float oldHeadYaw = actor->getComponent<ActorHeadRotationComponent>()->rotation.y;
	float oldBodyYaw = actor->getComponent<MobBodyRotationComponent>()->prevBodyRot;

	// Pitch
	actor->setPitch(Global::easedPitch);
	actor->getComponent<ActorRotationComponent>()->oldRotation.x = Global::easedPitch;

	// Head Yaw
	actor->setHeadYaw(Global::easedHeadYaw);
	actor->getComponent<ActorHeadRotationComponent>()->rotation.y = Global::easedHeadYaw;

	// Body Yaw
	actor->setBodyRotation(Global::easedBodyYaw);
	actor->getComponent<MobBodyRotationComponent>()->prevBodyRot = Global::easedBodyYaw;

	if (getModuleByName("testmodule")->isEnabled()) {
		//actor->getSupplies()->hotbarSlot = 1;
	}

	auto ret = Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
		onActorRenderDispatcher, _this, a2, actor, cameraPos, actorPosition, a6, renderCurrentPos
	);

	if (getModuleByName("testmodule")->isEnabled()) {
		//actor->getSupplies()->hotbarSlot = 0;
	}

	// Pitch
	actor->setPitch(pitch);
	actor->getComponent<ActorRotationComponent>()->oldRotation.x = oldPitch;

	// Head Yaw
	actor->setHeadYaw(headYaw);
	actor->getComponent<ActorHeadRotationComponent>()->rotation.y = oldHeadYaw;

	// Body Yaw
	actor->setBodyRotation(bodyYaw);
	actor->getComponent<MobBodyRotationComponent>()->prevBodyRot = oldBodyYaw;

	return ret;
}

void* ActorRenderDispatcherDetour(void* _this, void* a2, Actor* actor, Vector3<float> cameraPos, Vector3<float> actorPosition, Vector2<float>* a6, bool renderCurrentPos) {
	/*if (actor == Address::getLocalPlayer()) {
		if (getModuleByName("desync")->isEnabled() || getModuleByName("fly")->isEnabled()) {
			auto newPos = Global::testPosition;
			auto newPosCalc = actorPosition.submissive(cameraPos).submissive(actorPosition).add(newPos);

			if (cameraPos.x == 0 && cameraPos.y == 0 && cameraPos.z == 0) {
				Global::stopESP = true;
			}
			else {
				Global::stopESP = false;
			}

			// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
			Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
				onActorRenderDispatcher, _this, a2, actor, cameraPos, newPosCalc, a6, false
			);
		}
	}*/

	Player* player = Address::getLocalPlayer();

	bool isLocalPlayer = actor == player;

	if (actor != player) {
		return Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
			onActorRenderDispatcher, _this, a2, actor, cameraPos, actorPosition, a6, renderCurrentPos
		);
	}

	if (getModuleByName("desync")->isEnabled()) {
		auto newPos = Global::desyncPosition;
		auto newPosCalc = actorPosition.submissive(cameraPos).submissive(actorPosition).add(newPos);

		return Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
			onActorRenderDispatcher, _this, a2, actor, cameraPos, newPosCalc, a6, renderCurrentPos
		);
	}

	if (getModuleByName("freecam")->isEnabled()) {
		auto newPos = Global::freecamPosition;
		auto newPosCalc = actorPosition.submissive(cameraPos).submissive(actorPosition).add(newPos);

		return Memory::CallFunc<void*, void*, void*, Actor*, Vector3<float>, Vector3<float>, Vector2<float>*, bool>( // CallFunc to call the original.
			onActorRenderDispatcher, _this, a2, actor, cameraPos, newPosCalc, a6, renderCurrentPos
		);
	}

	ApplyRotationChanges(_this, a2, actor, cameraPos, actorPosition, a6, renderCurrentPos);

	return nullptr;
}

bool IntersectsDetour(Actor* _this, Vector3<float> pos1, Vector3<float> pos2) {
	IntersectsTickEvent event{}; // IntersectsTickEvent
	event.cancelled = nullptr;
	CallBackEvent(&event); // Call IntersectsTick event for modules to be writen on this hook.

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	return Memory::CallFunc<bool, Actor*, Vector3<float>, Vector3<float>>( // CallFunc to call the original.
		onIntersects, _this, pos1, pos2
	);
}

bool isImmobileDetour(Mob* _this)
{
	if (getModuleByName("antiimmobile")->isEnabled()) {
		return false;
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<bool, Mob*>( // CallFunc to call the original.
		onIsImmobile, _this
	);
}

bool isOnFireDetour(Actor* _this)
{
	Global::isOnFire = true;

	if (getModuleByName("nofire")->isEnabled()) {
		if (_this == Address::getLocalPlayer()) {
			return false;
		}
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<bool, Actor*>( // CallFunc to call the original.
		onIsOnFire, _this
	);
}

bool canBeAffected(Actor* _this, int duh) {
	if (_this == Address::getLocalPlayer()) {
		if (duh == 1) {
			Global::effectSwiftness = true;
			TimeUtils::resetTime("SwiftnessBoost");
		}
		ChatUtils::sendMessage(std::to_string(duh));
	}

	return Memory::CallFunc<bool, Actor*, int>(
		onCanBeEffected, _this, duh
	);
}

float DestroySpeedDetour(Player* _this, Block& a1)
{
	if (getModuleByName("fastbreak")->isEnabled()) {
		return Global::BreakSpeed;
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<float, Player*, Block&>( // CallFunc to call the original.
		onDestroySpeed, _this, a1
	);
}

class ActorHooks : public FuncHook { // a Animations class that inherits the FuncHook template we made
public:
	//
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Player VTable signature
		void* playerVTableAddr = Memory::findSig(xorstr_("48 8d 05 ? ? ? ? 48 89 01 b8 ? ? ? ? 8d 50 ? 44 8d 48 ? 44 8d 40 ? 66 89 44 24 ? e8 ? ? ? ? 48 8b 8b"));

		int offset = *reinterpret_cast<int*>((uintptr_t)playerVTableAddr + 3);
		uintptr_t** VTable = reinterpret_cast<uintptr_t**>((uintptr_t)playerVTableAddr + offset + 7);

		// ActorRenderDispatcher
		void* ActorRenderDispatcherAddr = Memory::findSig(xorstr_("48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 8D ? ? ? 48 81 EC ? ? ? ? 48 8B ? ? ? ? ? 48 33 ? 48 89 ? ? 4D 8B ? 49 8B ? 48 8B ? 4C 8B ? 41 8B"));
		
		// Actor::intersects
		void* intersectsAddr = Memory::findSig(xorstr_("48 83 EC ? 48 8B 81 ? ? ? ? 48 85 C0 74 50"));

		// Actor::isImmobile
		void* isImmobileAddr = Memory::findSig(xorstr_("40 53 48 83 EC 20 48 8D 59 08"));
		
		// Player::getDestroySpeed
		void* destroySpeedAddr = Memory::findSig(xorstr_("48 89 5C ? ? 57 48 83 EC ? 48 8B FA ? ? 74 24 ? 48 8B 91"));

		// Attempt to hook the function at the address stored in ActorRenderDispatcher by replacing it with the address of ActorRenderDispatcherDetour and store the original function's address in onActorRenderDispatcher
		if (!Memory::HookFunction(ActorRenderDispatcherAddr, (void*)&ActorRenderDispatcherDetour, &onActorRenderDispatcher, xorstr_("ActorRenderDispatcher"))) { return false; }

		// Attempt to hook the function at the address stored in isImmobileAddr by replacing it with the address of isImmobileDetour and store the original function's address in onIsImmobile
		if (!Memory::HookFunction(intersectsAddr, (void*)&IntersectsDetour, &onIntersects, xorstr_("Intersects"))) { return false; }

		// Attempt to hook the function at the address stored in isImmobileAddr by replacing it with the address of isImmobileDetour and store the original function's address in onIsImmobile
		if (!Memory::HookFunction((void*)VTable[43], (void*)&isImmobileDetour, &onIsImmobile, xorstr_("IsImmobile"))) { return false; }

		// Attempt to hook the function at the address stored in isOnFireAddr by replacing it with the address of isOnFireDetour and store the original function's address in onIsOnFire
		if (!Memory::HookFunction((void*)VTable[51], (void*)&isOnFireDetour, &onIsOnFire, xorstr_("isOnFire"))) { return false; }

		if (!Memory::HookFunction((void*)VTable[111], (void*)&canBeAffected, &onCanBeEffected, xorstr_("canBeEffected"))) { return false; }
		
		// Attempt to hook the function at the address stored in destroySpeedAddr by replacing it with the address of destroySpeedDetour and store the original function's address in onDestroySpeed
		if (!Memory::HookFunction(destroySpeedAddr, (void*)&DestroySpeedDetour, &onDestroySpeed, xorstr_("DestroySpeed"))) { return false; }

		return true;
	}

	static ActorHooks& Instance() { // a class setup function called Instance.
		static ActorHooks instance;
		return instance;
	}
};