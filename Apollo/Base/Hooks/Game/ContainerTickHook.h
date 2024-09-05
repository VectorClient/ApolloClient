#pragma once

void* onContainerTick;

void* ContainerTickDetour(ContainerScreenController* _this) {

    ContainerTickEvent event{ _this }; // ContainerTickEvent // ChestScreenController::tick
    event.cancelled = nullptr;
    CallBackEvent(&event); // Call ContainerTick event for modules to be writen on this hook.

    return Memory::CallFunc<void*, ContainerScreenController*>(onContainerTick, _this);
}

class ContainerTickHook : public FuncHook
{
public:
    bool Initialize() override // Now override the Initialize function so we can actually hook our stuff
    {
        // Using the utils class findSig function to locate the address of the signature

        // ContainerScreenController::tick(ContainerScreenController *this)
        void* containerTickAddr = Memory::findSig(xorstr_("48 8B C4 48 89 58 10 48 89 68 18 48 89 70 20 57 41 56 41 57 48 81 EC ? ? ? ? 0F 29 70 D8 4C 8B F1 48 8B 89 ? ? ? ? E8 ? ? ? ?"));

        // Give it the address/ptr to the LoopbackPacketSender function.
        // Give it the detour ptr and then the original callback ptr.

        // Usage: (Sig/Address/Offset/VTable, Detour, Original Callback).
        // Don't forget to return its result.

        // Attempt to hook the function at the address stored in LoopbackVTable by replacing it with the address of LoopbackPacketSenderDetour and store the original function's address in onLoopbackPacketSender
        return Memory::HookFunction(containerTickAddr, (void*)&ContainerTickDetour, &onContainerTick, xorstr_("ContainerTick"));

        return true;
    }

    static ContainerTickHook& Instance()
    {
        static ContainerTickHook instance;
        return instance;
    }
};