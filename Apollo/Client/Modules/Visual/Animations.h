#pragma once

class Animations : public Module
{
public:
    Animations(int keybind = 7, bool enabled = true) :
        Module("Animations", "Visual", "Animations like its flux", keybind, enabled)
    {
        addEnum("Swing", "The Swing type", { "Flux", "None" }, &swingType);
        addSlider("Swing Speed", "The swing speed", &Global::swingSpeed, 1, 70);
        addBool("CustomSwingAngle", "Changes the swing angle", &CustomSwingAngle);
        addSlider("Swing Angle", "The custom swing angle value", &SwingAngleValue, -360, 360);
    }

    int swingType = 0;
    bool CustomSwingAngle = false;
    bool isSword = false;
    float SwingAngleValue = 12;

    void* fluxSwingAddr = (void*)(Memory::findSig("E8 ? ? ? ? F3 0F ? ? ? ? ? ? F3 0F ? ? ? ? ? ? 48 8B ? F3 0F ? ? 48 8B"));
    float* tapSwingVal = nullptr;
    void* ogBytes[5] = { 0 };

    //void* punchAddr = (void*)(Memory::findSig(xorstr_("48 89 43 ? 48 8b 5c 24 ? 48 83 c4 ? 5f c3 cc cc cc cc 48 89 5c 24 ? 57 48 83 ec ? 48 8b 42 ? 48 8b da 0f 29 74 24 ? 48 8b f9 0f 29 7c 24 ? 0f 57 ff 48 85 c0 74 ? f3 0f 10 70")));
    //char ogPunch[4] = {};

    void patchAnimFunc(bool patch = true)
    {
        static uintptr_t addr = (uintptr_t)Memory::findSig("F3 0F ? ? EB ? 0F 57 ? F3 41");
        if (addr == 0) return;

        std::vector<uint8_t> bytes = {};

        if (patch) {
            bytes = { 0x90, 0x90, 0x90, 0x90 };
        }
        else {
            bytes = { 0xF3, 0x0F, 0x5C, 0xD0 };
        }

        //Memory::writeBytes(addr, bytes, (int)bytes.size());
    }

    void onEnabled() override {
        if (!Address::getClientInstance())
            return;

        if (tapSwingVal == nullptr) {
            //tapSwingVal = reinterpret_cast<float*>(Memory::getXref((uintptr_t)Memory::findSig("F3 44 ? ? ? ? ? ? ? 41 0F ? ? F3 0F ? ? ? ? C6 40 38 ? 48 8B ? ? 41 0F ? ? E8 ? ? ? ? 48 8B"), 5));
            //Memory::setProtection(reinterpret_cast<uintptr_t>(tapSwingVal), 4, PAGE_READWRITE);
        }

        Memory::readBytes((void*)fluxSwingAddr, ogBytes, 5);

        if (swingType == 0) {
            Memory::writeBytes((uintptr_t)fluxSwingAddr, "\x90\x90\x90\x90\x90", 5);
        }

        patchAnimFunc(false);
    }

    void onEvent(ActorBaseTickEvent* event) override
    {
        PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        ItemStack* stack = inventory->getItem(playerInventory->hotbarSlot);
        if (stack->item == nullptr) return;
        if (stack->getItem()->isSword()) isSword = true;
        else isSword = false;
    }

    void onEvent(ViewBobbingTickEvent* event) override
    {
        glm::mat4& matrix = *event->Matrix;

        if (swingType == 0) {
            Memory::writeBytes((uintptr_t)fluxSwingAddr, "\x90\x90\x90\x90\x90", 5);
        }
        if (swingType == 1) {
            Memory::writeBytes((uintptr_t)fluxSwingAddr, (char*)ogBytes, 5);
        }

        if (CustomSwingAngle) {
            //*tapSwingVal = SwingAngleValue;
        }
        else {
            //*tapSwingVal = -80.f;
        }

        if ((GetAsyncKeyState(VK_RBUTTON) || Global::ShouldBlock) && Address::canUseKeys() && isSword) // RenderUtil::GetCTX()->ClientInstance->mcGame->CanUseKeys
        {
            matrix = glm::translate<float>(matrix, glm::vec3(0.42222223281, 0.0, -0.16666666269302368));
            matrix = glm::translate<float>(matrix, glm::vec3(-0.1f, 0.15f, -0.2f));
            matrix = glm::translate<float>(matrix, glm::vec3(-0.24F, 0.25f, -0.20F));
            matrix = glm::rotate<float>(matrix, -1.98F, glm::vec3(0.0F, 1.0F, 0.0F));
            matrix = glm::rotate<float>(matrix, 1.30F, glm::vec3(4.0F, 0.0F, 0.0F));
            matrix = glm::rotate<float>(matrix, 60.0F, glm::vec3(0.0F, 1.0F, 0.0F));
        }
        else {
            //Utils::patchBytes((BYTE*)((uintptr_t)tapSwing), (BYTE*)"\xF3\x0F\x51\xF0", 4);
        }

        patchAnimFunc(false);
    }

    void onDisabled() override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        if (swingType == 0) {
            Memory::writeBytes((uintptr_t)fluxSwingAddr, (char*)ogBytes, 5);
        }

        patchAnimFunc(false);

        //*tapSwingVal = -80.f;
    }

    std::string getModeName() override {
        return " " + std::string("Flux");
    }
};