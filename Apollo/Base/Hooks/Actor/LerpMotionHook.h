#pragma once

// Declare a void pointer(Empty object) called onLerpMotion which is the CallBack for LerpMotionDetourHook.
void* onLerpMotion; // Defined an orignal for LerpMotion.

void LerpMotionDetourHook(Actor* _this, Vector3<float> motion) // Actor*, Vector3*
{
	if (_this == Address::getLocalPlayer()) {
		Global::lastLerpVelocity = motion;
	}

	if (getModuleByName("testmodule")->isEnabled()) {
		motion = Address::getLocalPlayer()->getStateVector()->Velocity.lerp(motion, Global::testModuleFly, 0, Global::testModuleFly);
	}

	if (getModuleByName("velocity")->isEnabled()) {
		motion = Address::getLocalPlayer()->getStateVector()->Velocity.lerp(motion, 0, 0, 0);
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<void*, Actor*, Vector3<float>>( // CallFunc to call the original.
		onLerpMotion, _this, motion
	);
}

class LerpMotionHook : public FuncHook { // a LerpMotion class that inherits the FuncHook template we made
public:
	//
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Using the utils class findSig function to locate the address of the signature
		
		// Actor::lerpMotion(Vec3)
		void* lerpMotionAddr = Memory::findSig(xorstr_("48 83 EC 28 4C 8B 81 ? ? ? ? 4D 85 C0 74 19"));
		//  8B 02 41 89 40 18

		// Give it the address/ptr to the LerpMotion function.
		// Give it the detour ptr and then the original callback ptr.

		// Usage: (Sig/Address/Offset, Detour, Original Callback).
		// Don't forget to return its result.

		// Attempt to hook the function at the address stored in lerpMotionAddr by replacing it with the address of LerpMotionDetour and store the original function's address in onLerpMotion
		return Memory::HookFunction(lerpMotionAddr, (void*)&LerpMotionDetourHook, &onLerpMotion, xorstr_("LerpMotion"));

		return true;
	}

	static LerpMotionHook& Instance() { // a class setup function called Instance.
		static LerpMotionHook instance;
		return instance;
	}
};