#pragma once

class ChestStealer : public Module
{
public:
    ChestStealer(int keybind = 7, bool enabled = true) :
        Module("ChestStealer", "Player", "Steal items out of chests", keybind, enabled)
    {
        addSlider("SPS", "How many items are moved a second", &SPS, 1, 20);
    }

    float SPS = 20;

    int item = 0;
    int maxItem = 56;

    void onEvent(ContainerTickEvent* event) override
    {
        if (TimeUtils::hasTimeElapsed("Cs", 500, false))
            item = 0;

        ContainerScreenController* controller = event->Controller;

        if (!controller)
            return;

        if (TimeUtils::hasTimeElapsed("Cs", 1000 / SPS, true)) {
            for (int i = 0; i < 56; ++i)
            {
                if (controller->_getItemStack(Containers::Container, i)->item != nullptr) {
                    controller->shiftItems(Containers::Container, i);
                    return;
                }
                else continue;
            }
        }
        controller->closeContainer();
    }

    std::string getModeName() override {
        return " Delayed";
    }
};
