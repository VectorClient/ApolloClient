#pragma once

class Watermark : public Module
{
public:
    Watermark(int keybind = Keys::NONE, bool enabled = true) :
        Module("Watermark", "Visual", "Displays the client's watermark", keybind, enabled)
    {
        addEnum("Style", "The style of the watermark", { "Simple", "Circle", "Aeolus"}, &Style);
        addSlider("Opacity", "The opacity of the rectangle", &opacity, 0, 1);
        addBool("Shadow", "Render shadows for the rectangle", &shadow);
        addBool("Fill Shadow", "Fill the rectangle with shadow", &shadowfilled);
        addSlider("Shadow strenght", "The strenght of the shadow", &shadowstrenght, 0, 200);
        addSlider("Radius", "The radius of the circle letter in circle mode", &radius, 0, 200.f);
    }

    float opacity = 0.2;
    bool shadow = true;
    bool shadowfilled = true;
    float shadowstrenght = 150;
    float rot = 0.f;
    int Style = 0;
    float radius = 60.f;

    void onEvent(ImGuiRenderEvent* event) override {
        if (Style == 0) {
            Vector2<float> pos2 = Vector2<float>(0.f, 0.f);

            int ind = 0;

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            std::string name = xorstr_("apollo");
            std::string version = xorstr_("Dev");

            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
            //ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 46), &version, UIColor(44, 44, 44), 1.1, 1, true);
            ImGui::PopFont();

            for (char c : (std::string)name)
            {
                std::string string = Utils::combine(c, "");
                int colorIndex = ind * 80;

                float charWidth = ImRenderUtil::getTextWidth(&string, 2.3);
                float charHeight = ImRenderUtil::getTextHeight(2.3);
                UIColor RGBColor = ColorUtils::getClientColor(1.9, 1, 1, colorIndex);
                ImRenderUtil::drawShadowSquare(Vector2<float>(pos2.x + charWidth / 2, pos2.y + charHeight / 1.2), 15.f, RGBColor, 0.85f, 70.f, 0);

                ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, RGBColor, 2.3, 1, false);

                pos2.x += charWidth;
                ++ind;
            }
            ImGui::PopFont();
        }
        else if (Style == 1) {
            std::string mainName = "A";
            std::string sideName = "APOLLO CLIENT DEVELOPER ";
            Vector2<float> pos = Vector2<float>(80, 80);
            float letterSize = 1.f;
            float speed = 0.2f;

            int ind = 0;
            float mLetterSize = letterSize * 4;
            float letterHeight = ImRenderUtil::getTextHeight(1.f);
            Vector2<float> actPos = Vector2<float>(pos.x + letterHeight / 4, pos.y + letterHeight / 4);
            ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
            float textSizeX = ImRenderUtil::getTextWidth(&mainName, mLetterSize);
            float textSizeY = ImRenderUtil::getTextHeight(mLetterSize);
            Vector2<float> mPos = Vector2<float>(actPos.x - textSizeX / 2, actPos.y - textSizeY / 2);
            float degree = 360 / sideName.length();
            rot += speed;
            if (rot >= 360) rot = 0;
            for (char c : (std::string)sideName)
            {
                std::string string = Utils::combine(c, "");
                int colorIndex = ind * 80;

                UIColor RGBColor = ColorUtils::getClientColor(1.9, 1, 1, colorIndex);
                float rad = (PI / 180) * (-degree * ind - 90 - rot);
                float radText = (PI / 180) * (degree * ind + rot);
                ImRotateUtil::startRotation();
                Vector2<float> newPos = Vector2<float>(pos.x + std::cosf(radText) * radius, pos.y + std::sinf(radText) * radius);
                Vector2<float> shadowPos = Vector2<float>(newPos.x + letterHeight / 4, newPos.y + letterHeight / 4);
                ImRenderUtil::drawText(newPos, &string, RGBColor, letterSize, 1, false);
                ImRotateUtil::endRotation(rad);
                ImRenderUtil::fillShadowCircle(shadowPos, 12.f, RGBColor, 0.4f, shadowstrenght);
                ++ind;
            }
            ImRenderUtil::drawText(mPos, &mainName, UIColor(255, 255, 255), mLetterSize, 1, false);
            ImGui::PopFont();
        }
        else if (Style == 2) {

            std::string name = xorstr_("apollo - tozic");

            Vector2<float> textPos = Vector2<float>(15.f, 15.f);

            float verlen = ImRenderUtil::getTextWidth(&name, 1.16) + 22;
            float height = ImRenderUtil::getTextHeight(1.16) + 18;

            Vector4<float> boxRect = Vector4<float>(textPos.x - 7.5, textPos.y - 3.5, verlen, height);

            ImRenderUtil::fillRectangle(boxRect, UIColor(0, 0, 0), opacity);
            if (shadow)
            ImRenderUtil::fillShadowRectangle(boxRect, UIColor(0, 0, 0), 1, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground);

            ImRenderUtil::drawText(textPos, &name, UIColor(255, 255, 255), 1.16, 1, true);

        }
    }
};