#pragma once

class NoFire : public Module
{
public:
    NoFire(int keybind = Keys::NONE, bool enabled = false) :
        Module("NoFire", "Visual", "Stops fire from rendering", keybind, enabled)
    {

    }

public:
    void onEvent(ImGuiRenderEvent* event) override {
        Global::isOnFire = false;

        Player* player = Address::getLocalPlayer();

        if (player == nullptr || !Global::isOnFire)
            return;

        GuiData* guidata = Address::getClientInstance()->getGuiData();
        float renderx = guidata->mcResolution.x / 2;
        float rendery = (guidata->mcResolution.y / 2) + 40;
        Vector2<float> textPos = Vector2<float>(renderx, rendery);

        string text = "You are on fire.";

        float textLen = ImRenderUtil::getTextWidth(&text, 1.05);
        textPos.x -= textLen / 2;

        ImRenderUtil::drawText(textPos, &text, UIColor(255, 0, 0), 1.05f, 1.f, true);
    }
};