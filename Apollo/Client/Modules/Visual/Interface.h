#pragma once

#include <regex>

class Interface : public Module
{
public:
    Interface(int keybind = Keys::NONE, bool enabled = true) :
        Module("Interface", "Visual", "Interface", keybind, enabled)
    {
        addEnum("Mode", "The font for the display", { "Comfortaa", "Comfortaa Bold", "Product Sans", "PS Bold", "Mojangles" }, &font);
        addEnum("Color", "The theme color", { "GreenBlue", "Astolfo", "BubbleGum", "Rainbow", "Tenacity", "Wave", "Cherry", "Xextreame", "White" }, &Global::ClientColor);
        addBool("KeyStrokes", "Displays the keystrokes.", &keystrokes);
        addSlider("Keys Scale", "The scale of the keystrokes", &keystrokessize, 1, 5);

        visible = false;
    }

    int font = 0;

    bool keystrokes = false;
    float keystrokessize = 2.5;


    int fontNum = 0;
    float rounding = 8.f;
    int textSize = 28;
    float blur = 13.f;
    UIColor bgColor = UIColor(0.f, 0.f, 0.f, 0.78f);

    void onEvent(ImGuiRenderEvent* event) override {
        ImGuiIO& io = ImGui::GetIO();

        if (ImGui::GetIO == nullptr)
            return;

        io.FontDefault = io.Fonts->Fonts[font];

        Player* player = Address::getLocalPlayer();
        ClientInstance* instance = Address::getClientInstance();

        // Check if player or valid key is null
        if (!player)
            return;

        if (keystrokes) {
            DrawKeyStroke(1, 1, "W", player->getMoveInputHandler()->forward);
            DrawKeyStroke(0, 0, "A", player->getMoveInputHandler()->left);
            DrawKeyStroke(1, 0, "S", player->getMoveInputHandler()->backward);
            DrawKeyStroke(2, 0, "D", player->getMoveInputHandler()->right);
        }

        if (Global::info.show) {
            auto pos = Vector2<float>(Global::info.pos.x + 10, Global::info.pos.y + 10);

            std::string cleanText = std::regex_replace(Global::info.text, std::regex("§"), "");
            //std::string cleanText = Global::info.text;
            auto measureX = ImRenderUtil::getTextWidth(&cleanText, textSize);
            auto measureY = ImRenderUtil::getTextHeightStr(&cleanText, textSize);
            auto rect = Vector4<float>(
                pos.x,
                pos.y,
                pos.x + measureX + 12,
                pos.y + measureY + 12
                //pos.x + 100 + 12,
                //pos.y + 100 + 12
                //pos.x + measure.x + 12,
                //pos.y + measure.y + 12
            );

            /*ImFX::Begin(ImGui::GetBackgroundDrawList());
            ImFX::AddBlur(blur, ImVec4(rect.x, rect.y, rect.z, rect.w), rounding);
            ImFX::End();

            ImRenderUtil::fillRectangle(rect, bgColor, 0.78f, rounding);

            ImRenderUtil::textMCColor(Vector2<float>(pos.x + 6, pos.y + 4), Global::info.text, textSize, io.FontDefault);*/
        }
    }

    void DrawKeyStroke(int xIndex, int yIndex, std::string key, bool held) {
        auto scale = keystrokessize;
        auto space = scale * 3;
        std::string name = key;
        float verlen = ImRenderUtil::getTextWidth(&name, scale);
        float height = ImRenderUtil::getTextHeight(scale);

        float boxHeight = ImRenderUtil::getScreenSize().y * 0.98 - height - ((height + space) * yIndex);
        Vector4<float> boxRect = Vector4<float>((height + space) * xIndex + space, boxHeight - 0.35, (height + space) * (xIndex + 1), boxHeight + height);
        float boxWidth = boxRect.z - boxRect.x;
        Vector2<float> textPos = Vector2<float>(boxRect.x + (boxWidth / 2) - (verlen / 2), boxHeight);

        if (held) {
            ImRenderUtil::fillRectangle(boxRect, UIColor(0, 0, 0), 0.5 + 0.25, 10);
        }
        else
        {
            ImRenderUtil::fillRectangle(boxRect, UIColor(0, 0, 0), 0.5, 10);
        }

        ImRenderUtil::fillShadowRectangle(boxRect, UIColor(0, 0, 0), 1, 50, ImDrawFlags_ShadowCutOutShapeBackground, 10);

        ImRenderUtil::drawText(textPos, &name, UIColor(255, 255, 255), scale, 1, true);

    }

    void onDisabled() override {
        this->toggle();
    }
};