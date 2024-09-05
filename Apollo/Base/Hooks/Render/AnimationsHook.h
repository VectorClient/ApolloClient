#pragma once

// Declare a void pointer(Empty object) called onInterpolatedHeadYaw which is the CallBack for InterpolatedHeadYawDetour.
void* onInterpolatedHeadYaw; // Defined an orignal for InterpolatedHeadYaw.
// Declare a void pointer(Empty object) called onInterpolatedBodyYaw which is the CallBack for InterpolatedBodyYawDetour.
void* onInterpolatedBodyYaw; // Defined an orignal for InterpolatedBodyYaw.
// Declare a void pointer(Empty object) called onInterpolatedRotation which is the CallBack for InterpolatedRotationDetour.
void* onInterpolatedRotation; // Defined an orignal for InterpolatedRotation.

// Declare a void pointer(Empty object) called onViewBobbing which is the CallBack for ViewBobbingDetour.
void* onViewBobbing; // Defined an orignal for ViewBobbing.
// Declare a void pointer(Empty object) called onItemUseDuration which is the CallBack for ItemUseDurationDetour.
void* onItemUseDuration; // Defined an orignal for ItemUseDuration.
// Declare a void pointer(Empty object) called onCurrentSwingDuration which is the CallBack for getCurrentSwingDurationDetour.
void* onCurrentSwingDuration; // Defined an orignal for CurrentSwingDuration.

void* onTimeOfDay;

float getInterpolatedHeadYaw(Player* _this, float a1) {
	if (_this == Address::getLocalPlayer() && getModuleByName("serverrotations")->isEnabled()) {
		Global::easedHeadYaw = Math::wrap(Global::easedHeadYaw, Global::headYaw - 180.f, Global::headYaw + 180.f);
		Global::easedHeadYaw = Math::lerp(Global::easedHeadYaw, Global::headYaw, ImRenderUtil::getDeltaTime() * Global::HeadYawSpeed);

		return Global::easedHeadYaw;
	}

	return Memory::CallFunc<float, Player*, float>(onInterpolatedHeadYaw, _this, a1);
}

float getInterpolatedBodyYaw(Player* _this, float a1) {
	if (_this == Address::getLocalPlayer() && getModuleByName("serverrotations")->isEnabled()) {
		if (Global::lockBodyYaw) {
			Global::easedBodyYaw = Math::wrap(Global::easedBodyYaw, Global::easedHeadYaw - 180.f, Global::easedHeadYaw + 180.f);
			Global::easedBodyYaw = Math::lerp(Global::easedBodyYaw, Global::easedHeadYaw, ImRenderUtil::getDeltaTime() * Global::BodyYawSpeed);
		}
		else {
			Global::easedBodyYaw = Math::wrap(Global::easedBodyYaw, Global::bodyYaw - 180.f, Global::bodyYaw + 180.f);
			Global::easedBodyYaw = Math::lerp(Global::easedBodyYaw, Global::bodyYaw, ImRenderUtil::getDeltaTime() * Global::BodyYawSpeed);
		}

		return Global::easedBodyYaw;
	}

	return Memory::CallFunc<float, Player*, float>(onInterpolatedBodyYaw, _this, a1);
}

float* yRot(Player* _this, float* a2, float a3) {
	auto oFunc = Memory::CallFunc<float*, Player*, float*, float>(onInterpolatedRotation, _this, a2, a3);
	if (_this == Address::getLocalPlayer() && getModuleByName("serverrotations")->isEnabled()) {
		Global::easedPitch = Math::lerp(Global::easedPitch, Global::pitch, ImRenderUtil::getDeltaTime() * Global::PitchSpeed);
		a2[0] = Global::easedPitch;
		return oFunc;
	}
	return oFunc;
}

void ViewBobbingDetour(uint64_t _this, glm::mat4* matrix)
{
	ViewBobbingTickEvent event{ matrix };
	event.cancelled = nullptr;
	CallBackEvent(&event);

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<void*, uint64_t, glm::mat4*>( // CallFunc to call the original.
		onViewBobbing, _this, matrix
	);

	return;
}

bool ItemUseDurationDetour(Mob* _this)
{
	if (Address::getLocalPlayer()->getAttribute(AttributeId::Health) == nullptr) {
		return false;
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<bool, Mob*>( // CallFunc to call the original.
		onItemUseDuration, _this
	);
}

int getCurrentSwingDurationDetour(Mob* _this)
{
	if (Address::getLocalPlayer()) {
		auto vTable = *(uintptr_t**)_this;

		static bool hooked = false;
		static bool hookedDurability = false;

		if (!hookedDurability) {
			//hookedDurability = Memory::HookFunction((void*)vTable[162], (void*)&ItemUseDurationDetour, &onItemUseDuration, xorstr_("ItemUseDuration"));
		}

		if (!hooked) {
			// Hook InterpolatedHeadYaw from Mob's VTable
			//Memory::HookFunction((void*)vTable[16], (void*)&getInterpolatedHeadYaw, &onInterpolatedHeadYaw, xorstr_("InterpolatedHeadYaw"));
			// Hook InterpolatedBodyYaw from Mob's VTable
			//Memory::HookFunction((void*)vTable[17], (void*)&getInterpolatedBodyYaw, &onInterpolatedBodyYaw, xorstr_("InterpolatedBodyYaw"));
			hooked = true;
		}
	}

	if (getModuleByName("noswing")->isEnabled()) {
		return 0;
	}

	if (getModuleByName("scaffold")->isEnabled() && Global::SwingType == 1) {
		return 0;
	}

	return Global::swingSpeed;

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<void*, Mob*>( // CallFunc to call the original.
		onCurrentSwingDuration, _this
	);
}

float getTimeOfDayDetour(Dimension* _this, int time, float a) {
	if (getModuleByName("ambience")->isEnabled()) {
		return Global::AmbienceTime / 30000;
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<float, Dimension*, int, float>( // CallFunc to call the original.
		onTimeOfDay, _this, time, a
	);
}

class AnimationsHook : public FuncHook { // a Animations class that inherits the FuncHook template we made
public:
	//
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Player VTable signature
		void* playerVTableAddr = Memory::findSig(xorstr_("48 8d 05 ? ? ? ? 48 89 01 b8 ? ? ? ? 8d 50 ? 44 8d 48 ? 44 8d 40 ? 66 89 44 24 ? e8 ? ? ? ? 48 8b 8b"));

		int offset = *reinterpret_cast<int*>((uintptr_t)playerVTableAddr + 3);
		uintptr_t** VTable = reinterpret_cast<uintptr_t**>((uintptr_t)playerVTableAddr + offset + 7);

		// ViewBobbing's signature sub_1414FC590
		void* ViewBobbingAddr = Memory::findSig(xorstr_("40 53 56 48 83 EC 78 0F 29 7C 24 ? 48 81 C1 ? ? ? ? 0F 28 FA 48 8B F2 E8"));

		// Mob::getCurrentSwingDuration(void)
		void* getCurrentSwingDurationAddr = Memory::findSig(xorstr_("48 89 5C 24 ?? 57 48 83 EC ?? 48 8B 15 ?? ?? ?? ?? 48 8B F9"));

		// Player::getItemUseDuration(void)
		void* itemUseDurationAddr = Memory::findSig(xorstr_("8b 81 ? ? ? ? c3 cc cc cc cc cc cc cc cc cc 48 89 6c 24"));
		
		// Dimension::getTimeOfDay(void)
		void* getTimeOfDayAddr = Memory::findSig(xorstr_("44 8B C2 B8 ? ? ? ? F7 EA"));

		// Attempt to hook the function at the address stored in ViewBobbingAddr by replacing it with the address of ViewBobbingDetour and store the original function's address in onViewBobbing
		if (!Memory::HookFunction(ViewBobbingAddr, (void*)&ViewBobbingDetour, &onViewBobbing, xorstr_("ViewBobbing"))) { return false; }

		// Attempt to hook the function at the address stored in itemUseDurationAddr by replacing it with the address of ItemUseDurationDetour and store the original function's address in onItemUseDuration
		//if (!Memory::HookFunction(VTable[162], (void*)&ItemUseDurationDetour, &onItemUseDuration, xorstr_("ItemUseDuration"))) { return false; }

		// Attempt to hook the function at the address stored in itemUseDurationAddr by replacing it with the address of ItemUseDurationDetour and store the original function's address in onItemUseDuration
		if (!Memory::HookFunction(getCurrentSwingDurationAddr, (void*)&getCurrentSwingDurationDetour, &onCurrentSwingDuration, xorstr_("SwingDuration"))) { return false; }
		
		// Attempt to hook the function at the address stored in getTimeOfDayAddr by replacing it with the address of getTimeOfDayDetour and store the original function's address in onTimeOfDay
		if (!Memory::HookFunction(getTimeOfDayAddr, (void*)&getTimeOfDayDetour, &onTimeOfDay, xorstr_("TimeOfDay"))) { return false; }

		return true;
	}

	static AnimationsHook& Instance() { // a class setup function called Instance.
		static AnimationsHook instance;
		return instance;
	}
};