#pragma once
#include <algorithm>

class ArrayList : public Module
{
public:
    ArrayList(int keybind = 7, bool enabled = false) :
        Module("Arraylist", "Visual", "Display list of modules that are enabled.", keybind, enabled)
    {
        addEnum("Style", "Change the look of the arraylist", { "Simple", "Flux", "None" }, &style);
        addEnum("Animation", "The animation of enabling and disabling", { "Lerp", "Expo", "Bounce", "Elastic" }, &animationMode);
        addBool("Blur", "Blurs the ArrayList's background", &blur);
        addSlider("Blur Strength", "The strength of the blur", &blurStrength, 1, 15);
        addSlider("Size", "The size of the arraylist", &textSize, 5, 25);
        addSlider("Padding", "The padding of the arraylist", &padding, 5, 15);
        addBool("Text Shadow", "Render shadows for the texts", &TextShadow);
        addBool("Shadow", "Render shadows for the arraylist's rectangle", &shadow);
        addBool("Fill Shadow", "Fill the rectangle with shadow", &shadowfilled);
        addSlider("Shadow strenght", "The strenght of the shadow", &shadowstrenght, 0, 200);
        addSlider("Shadow opacity", "The opacity of the shadow", &shadowopacity, 0, 100);
        addSlider("Bar opacity", "The opacity of the bar shadow", &baropacity, 0, 100);
        addSlider("Opacity", "The opacity of the arraylist", &opacity, 0, 100);
        addSlider("Rounding", "The opacity of the arraylist", &rounding, 0, 100);
        addEnum("Font", "The font for the arraylist", { "Comfortaa", "Comfortaa Bold", "Product Sans", "PS Bold", "Mojangles" }, &FontMode);
    }

    int style = 0;
    int animationMode = 0;
    bool blur = true;
    static inline float blurStrength = 9;
    static inline float textSize = 10.5;
    static inline float padding = 10.5;
    bool TextShadow = true;
    bool shadow = true;
    bool shadowfilled = true;
    float shadowstrenght = 50;
    float shadowopacity = 70;
    float baropacity = 100;
    float opacity = 50;
    float rounding = 0;
    int FontMode = 2;

    void onEvent(ImGuiRenderEvent* event) override {
        if (!ImGui::GetBackgroundDrawList() && Address::getClientInstance() == nullptr) return;

        // Create a copy of the modules list
        static size_t previousSize;
        static std::vector<Module*> sortedModules;
        static std::vector<Module*> currentModules;

        for (auto mod : modules) {
            // Hide modules that aren't enabled or visible
            if (!mod->isEnabled() && mod->animPercentage <= 0.01 || !mod->isVisible()) continue;
            sortedModules.push_back(mod);
        }

        // Sort the modules if the vector size has changed (this is so scuffed lmao)
        if (sortedModules.size() != previousSize) {
            sortModules(sortedModules);
            currentModules.clear();
            currentModules = sortedModules;
            previousSize = sortedModules.size();
        }

        // Define the textSize and module index
        const float textHeight = ImRenderUtil::getTextHeight((textSize / 10)) * (padding / 10);
        const float padding = 0.5f * (textSize / 10);
        float textPosY = -1 + 10.20;
        int index = 0; // The RGB Index

        float easeAnim = 0;

        std::vector<std::tuple<Vector4<float>, std::string, std::string, Vector2<float>, float, float>> shadowDraw;
        for (Module* mod : currentModules) {
            float targetAnim = mod->isEnabled() ? 1.f : 0.f;
            mod->animPercentage = Math::animate(targetAnim, mod->animPercentage, ImRenderUtil::getDeltaTime() * (animationMode == 0 ? 15 : 4));
            mod->animPercentage = Math::clamp(mod->animPercentage, 0.f, 1.f);

            if (animationMode == 0) {
                easeAnim = mod->animPercentage;
            }
            else if (animationMode == 1) {
                easeAnim = easeInOutExpo(mod->animPercentage);
            }
            else if (animationMode == 2) {
                easeAnim = easeInOutBounce(mod->animPercentage);
            }
            else if (animationMode == 3) {
                easeAnim = easeInOutElastic(mod->animPercentage);
            }

            float fluxlen = style == 1 ? 3.2 : 0;

            std::string ModuleName = mod->getName();
            std::string ModeName = mod->getModeName();
            float modWidth = ImRenderUtil::getTextWidth(&ModuleName, (textSize / 10));
            float modeWidth = !ModeName.empty() ? ImRenderUtil::getTextWidth(&ModeName, (textSize / 10)) : 0.f;
            float modulePosX = (ImGui::GetIO().DisplaySize.x - modWidth - modeWidth - padding - fluxlen) - 10.20;
            mod->pos.x = Math::lerp(ImGui::GetIO().DisplaySize.x + 5.f, modulePosX, easeAnim);
            mod->pos.y = textPosY;

            UIColor arrayColor = ColorUtils::getClientColor(1.8, 1, 1, index * 40);

            // Normal
            Vector4<float> boxRect(mod->pos.x - padding - 2.f, textPosY, ImRenderUtil::getScreenSize().x - 6.5f, textPosY + textHeight);

            // Flux
            Vector4<float> rightSide(boxRect.z - fluxlen, boxRect.y, boxRect.z, boxRect.w);

            if (style != 2) {
                if (blur) {
                    ImRenderUtil::Blur(boxRect, blurStrength, rounding / 10);
                }
                ImRenderUtil::fillRectangle(boxRect, UIColor(0, 0, 0), easeAnim * (opacity / 100), rounding / 10);
            }
            //ImRenderUtil::Blur(boxRect, 6, rounding / 10);

            if (style == 1) {
                ImRenderUtil::fillRectangle(rightSide, arrayColor, easeAnim, rounding / 10);
            }

            shadowDraw.push_back(std::make_tuple(boxRect, ModuleName, ModeName, Vector2<float>(mod->pos.x, mod->pos.y), modWidth, easeAnim));

            textPosY = Math::lerp(textPosY, textPosY + textHeight, easeAnim); // Set a space between arraylist modules.

            ++index;
        }

        int colorIndex = 0;
        for (const auto& pos : shadowDraw) {
            Vector4<float> boxPos = std::get<0>(pos);
            std::string ModuleName = std::get<1>(pos);
            std::string ModeName = std::get<2>(pos);
            Vector2<float> ModulePos = std::get<3>(pos);
            float ModuleWidth = std::get<4>(pos);
            float anim = std::get<5>(pos);

            UIColor arrayColor = ColorUtils::getClientColor(1.8, 1, 1, colorIndex * 40);
            Vector4<float> rightSide(boxPos.z - 2.5, boxPos.y, boxPos.z, boxPos.w);

            if (shadow) {
                if (style == 0) {
                    ImRenderUtil::fillShadowRectangle(boxPos, UIColor(0, 0, 0), (shadowopacity / 100) * anim, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground, rounding / 10);
                }
                else if (style == 1) {
                    ImRenderUtil::fillShadowRectangle(boxPos, UIColor(0, 0, 0), (shadowopacity / 100) * anim, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground, rounding / 10);
                    ImRenderUtil::fillShadowRectangle(rightSide, arrayColor, (baropacity / 100) * anim, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground, rounding / 10);
                }
                else if (style == 2) {
                    ImRenderUtil::fillShadowRectangle(boxPos, arrayColor, (shadowopacity / 100) * anim, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground, rounding / 10);
                }
            }

            ImRenderUtil::drawText(ModulePos, &ModuleName, arrayColor, (textSize / 10), anim, TextShadow);
            ImRenderUtil::drawText(Vector2<float>(ModulePos.x + ModuleWidth, ModulePos.y), &ModeName, UIColor(170, 170, 170), (textSize / 10), anim, TextShadow);

            colorIndex++;
        }

        sortedModules.clear(); // Clear sortedModules
    }
private:
    inline float easeInOutExpo(float x) {
        return x == 0
            ? 0
            : x == 1
            ? 1
            : x < 0.5 ? pow(2, 20 * x - 10) / 2
            : (2 - pow(2, -20 * x + 10)) / 2;
    };

    inline float easeInOutBounce(float x) {
        if (x < 0.5) {
            return 8 * pow(2, 8 * (x - 1)) * abs(sin(x * PI * 7));
        }
        else {
            return 1 - 8 * pow(2, -8 * x) * abs(sin(x * PI * 7));
        }
    };

    __forceinline double easeInOutElastic(float x) {
        double t2;
        if (x < 0.45) {
            t2 = x * x;
            return 8 * t2 * t2 * sin(x * PI * 9);
        }
        else if (x < 0.55) {
            return 0.5 + 0.75 * sin(x * PI * 4);
        }
        else {
            t2 = (x - 1) * (x - 1);
            return 1 - 8 * t2 * t2 * sin(x * PI * 9);
        }
    }

    void sortModules(std::vector<Module*>& sortedModules) { // Sort the modules by length.
        std::sort(sortedModules.begin(), sortedModules.end(), [](Module* a, Module* b) {
            std::string nameA = a->getName() + a->getModeName(); // First module.
            std::string nameB = b->getName() + b->getModeName(); // Last module.
            float textSizeA = ImRenderUtil::getTextWidth(&nameA, (textSize / 10)); // First module's Length.
            float textSizeB = ImRenderUtil::getTextWidth(&nameB, (textSize / 10)); // Last module's Lenght.
            return textSizeA > textSizeB; // Sort by the most lenght to lower.
        });
    }

};