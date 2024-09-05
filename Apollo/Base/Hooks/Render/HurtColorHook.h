#pragma once

// Declare a void pointer(Empty object) called onHurtColor which is the CallBack for HurtColorDetourHook.
void* onHurtColor; // Defined an orignal for HurtColor.

UIColor* HurtColorDetour(void* _this, UIColor* color, void* a3) // Actor*, Vector3*
{
	if (getModuleByName("hurtcolor")->isEnabled()) {
		UIColor RGB = ColorUtils::Rainbow(1, 1, 1, 1);
		color->r = RGB.r;
		color->g = RGB.g;
		color->b = RGB.b;
		color->a = 0.3;
		return color;
	}

	// Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
	Memory::CallFunc<void*, void*, UIColor*, void*>( // CallFunc to call the original.
		onHurtColor, _this, color, a3
	);
}

class HurtColorHook : public FuncHook { // a HurtColor class that inherits the FuncHook template we made
public:
	//
	bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
	{
		// Using the utils class findSig function to locate the address of the signature

		// Actor::HurtColor(Vec3)
		void* HurtColorAddr = Memory::findSig(xorstr_("E8 ? ? ? ? E9 ? ? ? ? 8B 43 ? 48 8D 54 24"));
		auto HurtColorFunc = (uintptr_t)HurtColorAddr + 1 + 4 + *reinterpret_cast<int*>((uintptr_t)HurtColorAddr + 1);

		// Give it the address/ptr to the HurtColor function.
		// Give it the detour ptr and then the original callback ptr.

		// Usage: (Sig/Address/Offset, Detour, Original Callback).
		// Don't forget to return its result.

		// Attempt to hook the function at the address stored in HurtColorAddr by replacing it with the address of HurtColorDetour and store the original function's address in onHurtColor
		return Memory::HookFunction((void*)HurtColorFunc, (void*)&HurtColorDetour, &onHurtColor, xorstr_("HurtColor"));

		return true;
	}

	static HurtColorHook& Instance() { // a class setup function called Instance.
		static HurtColorHook instance;
		return instance;
	}
};