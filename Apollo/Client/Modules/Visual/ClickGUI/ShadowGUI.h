#pragma once
#include "ClickGUIManager.h"

class ShadowGUI {
public:
    struct CategoryPosition {
        float x = 0.f, y = 0.f;
        bool isDragging = false, isExtended = true, wasExtended = false;
        float yOffset = 0;
        float scrollEase = 0;
        Vector2<float> dragVelocity = Vector2<float>();
        Vector2<float> dragAcceleration = Vector2<float>();
    };

    const float catWidth = 185.f;
    const float catHeight = 28.5f;

    const float catGap = 40;
    int lastDragged = -1;
    std::vector<CategoryPosition> catPositions;
    Module* lastMod = nullptr;

    void render(float animation, float inScale, int& scrollDirection, char* searchQuery, char* h, bool blur) {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        if (blur) {
            ImRenderUtil::fillRectangle(Vector4<float>(0, 0, ImRenderUtil::getScreenSize().x, ImRenderUtil::getScreenSize().y), UIColor(0, 0, 0), animation * 0.38f);
            ImRenderUtil::Blur(Vector4<float>(0, 0, ImRenderUtil::getScreenSize().x, ImRenderUtil::getScreenSize().y), animation * 9, 0);
        }
        

        bool isEnabled = getModuleByName("clickgui")->isEnabled();
        std::string tooltip = "";

        float textSize = inScale;
        float textHeight = ImRenderUtil::getTextHeight(textSize);

        float xPos = Address::getClientInstance()->getGuiData()->mcResolution.x;
        float yPos = Address::getClientInstance()->getGuiData()->mcResolution.y;

        std::string searchQueryLower = searchQuery;

        std::transform(searchQueryLower.begin(), searchQueryLower.end(), searchQueryLower.begin(), [](unsigned char c) {
            return std::tolower(c);
            });

        std::string SearchQueryStr = strlen(searchQuery) == 0 ? "Press any key to search modules..." : searchQueryLower;
        std::string searchIcon = "B";

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
        float textLen = ImRenderUtil::getTextWidth(&SearchQueryStr, 1.14f * inScale);
        ImGui::PopFont();

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[6]);
        float iconLen = ImRenderUtil::getTextWidth(&searchIcon, 1.f * inScale);
        ImGui::PopFont();

        float bothLen = (textLen + iconLen) / 2;

        static float IconX = (xPos / 2) - bothLen;
        //float IconX = (xPos / 2) - bothLen;
        static float TextX = (((xPos / 2) - bothLen) + iconLen) + 15;

        IconX = Math::animate((xPos / 2) - bothLen, IconX, ImRenderUtil::getDeltaTime() * 10.f);
        TextX = Math::animate((((xPos / 2) - bothLen) + iconLen) + 15, TextX, ImRenderUtil::getDeltaTime() * 10.f);

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[6]);
        ImRenderUtil::drawText(Vector2<float>(IconX, yPos - 40), &searchIcon, UIColor(255, 255, 255), 1.14f * inScale, 1, false);
        ImGui::PopFont();
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
        ImRenderUtil::drawText(Vector2<float>(TextX, yPos - 40), &SearchQueryStr, UIColor(255, 255, 255), 1.14f * inScale, 1, false);
        ImGui::PopFont();


        if (catPositions.empty() && isEnabled)
        {
            float centerX = ImRenderUtil::getScreenSize().x / 2.f;
            float xPos = centerX - (categories.size() * (catWidth + catGap) / 2);
            for (std::string& category : categories)
            {
                CategoryPosition pos;
                pos.x = xPos;
                pos.y = catGap * 2;
                xPos += catWidth + catGap;
                catPositions.push_back(pos);
            }
        }

        if (!catPositions.empty()) {
            for (size_t i = 0; i < categories.size(); i++) {
                // Mod math stuff
                const float modWidth = catWidth;
                const float modHeight = catHeight;
                float moduleY = -catPositions[i].yOffset;

                // Get all the modules and populate our vector
                const auto& modsInCategory = ClickGUIManager::getModulesInCategory(categories[i], modules);

                // Calculate the catRect pos
                Vector4<float> catRect = Vector4<float>(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + catHeight)
                    .scaleToPoint(Vector4<float>(ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2, ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2), inScale);

                /* Calculate the height of the catWindow including the settings */
                float settingsHeight = 0;

                for (const auto& mod : modsInCategory) {
                    std::string modLower = mod->getName();

                    std::transform(modLower.begin(), modLower.end(), modLower.begin(), [](unsigned char c) {
                        return std::tolower(c);
                        });

                    if (modLower.find(searchQueryLower) != std::string::npos && strlen(searchQuery) != 0) {

                    }
                    else if (strlen(searchQuery) == 0) {

                    }
                    else continue;

                    for (const auto& setting : mod->getSettings()) {
                        if (!setting->render) {
                            continue;
                        }

                        switch (setting->getType())
                        {
                        case SettingType::BOOL: {
                            settingsHeight = Math::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            break;
                        }
                        case SettingType::ENUM: {
                            std::vector<std::string> enumValues = setting->getEnumValues();
                            int numValues = static_cast<int>(enumValues.size());

                            settingsHeight = Math::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            if (setting->enumSlide > 0.01)
                            {
                                for (int j = 0; j < numValues; j++)
                                    settingsHeight = Math::lerp(settingsHeight, settingsHeight + modHeight, setting->enumSlide);
                            }
                            break;
                        }
                        case SettingType::SLIDER: {
                            settingsHeight = Math::lerp(settingsHeight, settingsHeight + modHeight, mod->cAnim);
                            break;
                        }
                        }
                    }
                }

                float catWindowHeight = catHeight + modHeight * modsInCategory.size() + settingsHeight;
                Vector4<float> catWindow = Vector4<float>(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + moduleY + catWindowHeight)
                    .scaleToPoint(Vector4<float>(ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2, ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2), inScale);
                UIColor rgb = ColorUtils::getAstolfoRainbow(2.5, 1 * 0.5, 1, i * 25);

                // Can we scroll?
                if (ImRenderUtil::isMouseOver(catWindow) && catPositions[i].isExtended) {
                    if (scrollDirection > 0) {
                        catPositions[i].scrollEase += scrollDirection * catHeight;
                        if (catPositions[i].scrollEase > catWindowHeight - modHeight * 2)
                            catPositions[i].scrollEase = catWindowHeight - modHeight * 2;
                    }
                    else if (scrollDirection < 0) {
                        catPositions[i].scrollEase += scrollDirection * catHeight;
                        if (catPositions[i].scrollEase < 0)
                            catPositions[i].scrollEase = 0;
                    }
                    scrollDirection = 0;
                }

                // Lerp the category extending
                if (!catPositions[i].isExtended) {
                    catPositions[i].scrollEase = catWindowHeight - catHeight;
                    catPositions[i].wasExtended = false;
                }
                else if (!catPositions[i].wasExtended) {
                    catPositions[i].scrollEase = 0;
                    catPositions[i].wasExtended = true;
                }

                // Lerp the scrolling cuz smooth
                catPositions[i].yOffset = Math::animate(catPositions[i].scrollEase, catPositions[i].yOffset, ImRenderUtil::getDeltaTime() * 10.5);

                for (const auto& mod : modsInCategory) {
                    std::string modLower = mod->getName();

                    std::transform(modLower.begin(), modLower.end(), modLower.begin(), [](unsigned char c) {
                        return std::tolower(c);
                        });

                    if (modLower.find(searchQueryLower) != std::string::npos && strlen(searchQuery) != 0) {

                    }
                    else if (strlen(searchQuery) == 0) {

                    }
                    else continue;

                    UIColor rgb = ColorUtils::getAstolfoRainbow(2.5, 1 * 0.5, 1, moduleY * 2.5);

                    // If the mod belongs to the category
                    if (mod->getCategory() == categories[i]) {
                        // Calculate the modRect pos
                        Vector4<float> modRect = Vector4<float>(catPositions[i].x, catPositions[i].y + catHeight + moduleY, catPositions[i].x + modWidth, catPositions[i].y + catHeight + moduleY + modHeight)
                            .scaleToPoint(Vector4<float>(ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2, ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2), inScale);

                        // Animate the setting animation percentage
                        float targetAnim = mod->showSettings ? 1.f : 0.f;
                        mod->cAnim = Math::animate(targetAnim, mod->cAnim, ImRenderUtil::getDeltaTime() * 15.5);
                        mod->cAnim = Math::clamp(mod->cAnim, 0.f, 1.f);

                        // Settings
                        if (mod->cAnim > 0.001) {
                            for (const auto& setting : mod->getSettings()) {
                                if (!setting->render) {
                                    continue;
                                }

                                UIColor rgb = ColorUtils::getAstolfoRainbow(2.5, 1 * 0.5, 1, moduleY * 2.5);

                                switch (setting->getType()) {
                                case SettingType::BOOL: {
                                    moduleY = Math::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    Vector4<float> rect = Vector4<float>(modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z, catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(Vector4<float>(modRect.x, ImRenderUtil::getScreenSize().y / 2, modRect.z, ImRenderUtil::getScreenSize().y / 2), inScale);

                                    if (rect.y > catRect.y + 0.5f) {
                                        std::string setName = setting->getName();
                                        ImRenderUtil::fillRectangle(rect, UIColor(30, 30, 30), animation);

                                        if (ImRenderUtil::isMouseOver(rect) && isEnabled) {
                                            tooltip = setting->getDescription();
                                            if (Utils::leftDown) {
                                                *(bool*)setting->getValue() = !*(bool*)setting->getValue();
                                                Utils::leftDown = false;
                                            }
                                        }

                                        setting->boolScale = Math::animate(*(bool*)setting->getValue() ? 1 : 1, setting->boolScale, ImRenderUtil::getDeltaTime() * 10);

                                        float scaledWidth = rect.getWidth();
                                        float scaledHeight = rect.getHeight();
                                        Vector2<float> center = Vector2<float>(rect.x + rect.getWidth() / 2.f, rect.y + rect.getHeight() / 2.f);
                                        Vector4<float> scaledRect = Vector4<float>(center.x - scaledWidth / 2.f, center.y - scaledHeight / 2.f, center.x + scaledWidth / 2.f, center.y + scaledHeight / 2.f);

                                        Vector4<float> smoothScaledRect = Vector4<float>(scaledRect.z - 19, scaledRect.y + 5, scaledRect.z - 5, scaledRect.w - 5);//
                                        Vector2<float> circleRect = Vector2<float>(smoothScaledRect.getCenter().x, smoothScaledRect.getCenter().y);

                                        //Vector4<float> BallRect = *(bool*)setting->getValue() ? Vector4<float>(smoothScaledRect.z - 17, smoothScaledRect.y + 1, smoothScaledRect.z - 1, smoothScaledRect.w - 4) : Vector4<float>(smoothScaledRect.x + 17, smoothScaledRect.y + 1, smoothScaledRect.x + , smoothScaledRect.w - 4);
                                        //Vector2<float> BallRectCenter = Vector2<float>(BallRect.x + BallRect.getWidth() / 2.f, BallRect.y + BallRect.getHeight() / 2.f);

                                        //ImRenderUtil::fillShadowRectangle(smoothScaledRect, UIColor(0, 0, 0), 1, 100, 0);
                                        //ImRenderUtil::fillRectangle(smoothScaledRect, *(bool*)setting->getValue() ? rgb : UIColor(15, 15, 15), 1, 0);
                                        ImRenderUtil::fillCircle(circleRect, 5, *(bool*)setting->getValue() ? rgb : UIColor(15, 15, 15), 1, 12.f);
                                        ImRenderUtil::fillShadowCircle(circleRect, 5, *(bool*)setting->getValue() ? rgb : UIColor(15, 15, 15), 1, 40, 0);
                                        //ImRenderUtil::fillRectangle(BallRect, *(bool*)setting->getValue() ? UIColor(255, 255, 255) : UIColor(170, 170, 170), 1, 20);
                                        //ImRenderUtil::fillCircle(BallRectCenter, 20, UIColor(255, 255, 255), 1, 3);
                                        //ImRenderUtil::fillRectangleCustom(smoothScaledRect, rgb, animation * (setting->boolScale), Vector4<float>(20, 20, 20, 20));
                                        //ImRenderUtil::fillRectangle(scaledRect, rgb, animation * (setting->boolScale));

                                        float cSetRectCentreX = rect.x + ((rect.z - rect.x) - ImRenderUtil::getTextWidth(&setName, textSize)) / 2;
                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;
                                        ImRenderUtil::drawText(Vector2(rect.x + 5.f, cSetRectCentreY), &setName, UIColor(255, 255, 255), textSize, animation, true);
                                    }
                                    break;
                                }
                                case SettingType::ENUM: {
                                    std::string setName = setting->getName();
                                    std::vector<std::string> enumValues = setting->getEnumValues();
                                    int* iterator = setting->getIterator();
                                    int numValues = static_cast<int>(enumValues.size());

                                    // Increment the yOffset
                                    moduleY = Math::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    Vector4<float> rect = Vector4<float>(modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z, catPositions[i].y + catHeight + moduleY + modHeight)
                                        .scaleToPoint(Vector4<float>(modRect.x, ImRenderUtil::getScreenSize().y / 2, modRect.z, ImRenderUtil::getScreenSize().y / 2), inScale);

                                    // Animate the setting animation percentage
                                    float targetAnim = setting->enumExtended && mod->showSettings ? 1.f : 0.f;
                                    setting->enumSlide = Math::animate(targetAnim, setting->enumSlide, ImRenderUtil::getDeltaTime() * 10);
                                    setting->enumSlide = Math::clamp(setting->enumSlide, 0.f, 1.f);

                                    if (setting->enumSlide > 0.001)
                                    {
                                        for (int j = 0; j < numValues; j++)
                                        {
                                            std::string enumValue = enumValues[j];

                                            // Calculate current position using easing function
                                            moduleY = Math::lerp(moduleY, moduleY + modHeight, setting->enumSlide);

                                            Vector4<float> rect2 = Vector4<float>(modRect.x, catPositions[i].y + catHeight + moduleY, modRect.z, catPositions[i].y + catHeight + moduleY + modHeight)
                                                .scaleToPoint(Vector4<float>(modRect.x, ImRenderUtil::getScreenSize().y / 2, modRect.z, ImRenderUtil::getScreenSize().y / 2), inScale);

                                            if (rect2.y > catRect.y + 0.5f)
                                            {
                                                float cSetRectCentreY = rect2.y + ((rect2.w - rect2.y) - textHeight) / 2;

                                                ImRenderUtil::fillRectangle(rect2, UIColor(20, 20, 20), animation);

                                                if (*iterator == j)
                                                    ImRenderUtil::fillRectangle(Vector4<float>(rect2.x, rect2.y, rect2.x + 1.5f, rect2.w), rgb, animation);

                                                if (ImRenderUtil::isMouseOver(rect2) && Utils::leftDown && isEnabled) {
                                                    *iterator = j;
                                                    Utils::leftDown = false;
                                                }

                                                ImRenderUtil::drawText(Vector2<float>(rect2.x + 5.f, cSetRectCentreY), &enumValue, UIColor(255, 255, 255), textSize, animation, true);
                                            }
                                        }
                                    }

                                    if (rect.y > catRect.y + 0.5f) {
                                        ImRenderUtil::fillRectangle(rect, UIColor(30, 30, 30), animation);

                                        if (ImRenderUtil::isMouseOver(rect) && isEnabled) {
                                            tooltip = setting->getDescription();
                                            if (Utils::leftDown) {
                                                *iterator = (*iterator + 1) % enumValues.size();
                                            }
                                            else if (Utils::rightDown && mod->showSettings) {
                                                setting->enumExtended = !setting->enumExtended;
                                            }
                                        }

                                        float cSetRectCentreY = rect.y + ((rect.w - rect.y) - textHeight) / 2;

                                        std::string enumValue = enumValues[*iterator];
                                        std::string settingName = setName;
                                        std::string settingString = enumValue;
                                        auto ValueLen = ImRenderUtil::getTextWidth(&settingString, textSize);

                                        ImRenderUtil::drawText(Vector2<float>(rect.x + 5.f, cSetRectCentreY), &settingName, UIColor(255, 255, 255), textSize, animation, true);
                                        ImRenderUtil::drawText(Vector2<float>((rect.z - 5.f) - ValueLen, cSetRectCentreY), &settingString, UIColor(170, 170, 170), textSize, animation, true);
                                    }
                                    if (rect.y > catRect.y - modHeight) {
                                        ImRenderUtil::fillGradientOpaqueRectangle(Vector4<float>(rect.x, rect.w, rect.z, rect.w + 10.f * setting->enumSlide * animation), UIColor(0, 0, 0), UIColor(0, 0, 0), 0.F * animation, 0.55F * animation);
                                    }
                                    break;
                                }
                                case SettingType::SLIDER: {
                                    const float value = *(float*)setting->getValue();
                                    const float min = setting->getMin();
                                    const float max = setting->getMax();
                                    const float round = setting->getRound();


                                    std::ostringstream oss;
                                    oss << std::fixed << std::setprecision(Math::getDecimalPlaces(round)) << (value);
                                    std::string str = oss.str();

                                    std::string rVal = str;

                                    std::string setName = setting->getName();
                                    std::string valueName = rVal;

                                    moduleY = Math::lerp(moduleY, moduleY + modHeight, mod->cAnim);

                                    Vector4<float> rect = Vector4<float>(modRect.x, (catPositions[i].y + catHeight + moduleY), modRect.z, catPositions[i].y + catHeight + moduleY + modHeight);
                                        //.scaleToPoint(Vector4<float>(modRect.x, ImRenderUtil::getScreenSize().y / 2, modRect.z, ImRenderUtil::getScreenSize().y / 2), inScale);

                                    if (rect.y > catRect.y + 0.5f)
                                    {
                                        ImRenderUtil::fillRectangle(rect, UIColor(30, 30, 30), animation);

                                        const float sliderPos = (value - min) / (max - min) * (rect.z - rect.x);

                                        setting->sliderEase = Math::animate(sliderPos, setting->sliderEase, ImRenderUtil::getDeltaTime() * 10);
                                        setting->sliderEase = std::clamp(setting->sliderEase, 0.f, rect.getWidth());

                                        if (ImRenderUtil::isMouseOver(rect) && isEnabled) {
                                            tooltip = setting->getDescription();
                                            if (Utils::leftDown) {
                                                setting->isDragging = true;
                                            }
                                        }

                                        if (Utils::leftClick && setting->isDragging && isEnabled) {
                                            const float newValue = std::fmax(std::fmin((ImRenderUtil::getMousePos().x - rect.x) / (rect.z - rect.x) * (max - min) + min, max), min);
                                            *(float*)setting->getValue() = newValue;
                                        }
                                        else {
                                            setting->isDragging = false;
                                        }

                                        Vector4<float> newbarlol = Vector4<float>(rect.x + 2, rect.y, rect.x + setting->sliderEase - 5.f, rect.w - 3);

                                        ImRenderUtil::fillRectangle(Vector4<float>(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation);
                                        ImRenderUtil::fillShadowRectangle(Vector4<float>(rect.x, (catPositions[i].y + catHeight + moduleY + modHeight) - 3, rect.x + setting->sliderEase, rect.w), rgb, animation, 50.f, 0);
                                        //ImRenderUtil::fillRectangleCustom(Vector4<float>(newbarlol.z - 6.f, newbarlol.y - 2, newbarlol.z + 6, newbarlol.w + 2), UIColor(255, 255, 255), animation, Vector4<float>(20, 20, 20, 20));
                                        auto ValueLen = ImRenderUtil::getTextWidth(&valueName, textSize);
                                        ImRenderUtil::drawText(Vector2<float>((rect.z - 5.f) - ValueLen, rect.y + 2.5), &valueName, UIColor(170, 170, 170), textSize, animation, true);
                                        ImRenderUtil::drawText(Vector2<float>(rect.x + 5.f, rect.y + 2.5), &setName, UIColor(255, 255, 255), textSize, animation, true);
                                    }
                                    break;
                                }
                                }
                            }
                        }

                        if (modRect.y > catRect.y + 0.5f)
                        {
                            // Draw the rect
                            if (mod->cScale <= 1) {
                                ImRenderUtil::fillRectangle(modRect, UIColor(40, 40, 40), animation);
                            }

                            std::string modName = mod->getName();

                            // Calculate the centre of the rect
                            Vector2<float> center = Vector2<float>(modRect.x + modRect.getWidth() / 2.f, modRect.y + modRect.getHeight() / 2.f);

                            mod->cScale = Math::animate(mod->isEnabled() ? 1 : 0, mod->cScale, ImRenderUtil::getDeltaTime() * 10);

                            // Calculate scaled size based on cScale
                            float scaledWidth = modRect.getWidth() * mod->cScale;
                            float scaledHeight = modRect.getHeight() * mod->cScale;

                            // Calculate new rectangle based on scaled size and center point
                            Vector4<float> scaledRect = Vector4<float>(center.x - scaledWidth / 2.f, center.y - scaledHeight / 2.f, center.x + scaledWidth / 2.f, center.y + scaledHeight / 2.f);

                            // Interpolate between original rectangle and scaled rectangle
                            if (mod->cScale > 0) {
                                ImRenderUtil::fillRectangle(scaledRect, rgb, animation* mod->cScale);
                                //ImRenderUtil::fillGradientOpaqueRectangle(scaledRect, UIColor(0, 0, 0), UIColor(0, 0, 0), 0.F * animation, 0.25F * animation);
                                //ImRenderUtil::fillGradientOpaqueRectangle(scaledRect, ColorUtils::RainbowDark(2.5, (moduleY * 2)), ColorUtils::RainbowDark(2.5, (moduleY * 2) + 400), 1, 1);
                                //ImRenderUtil::fillRectangle(Vector4(scaledRect.x, scaledRect.y, scaledRect.z, scaledRect.y + 1), UIColor(0, 0, 0), animation);
                            }

                            float cRectCentreX = modRect.x + ((modRect.z - modRect.x) - ImRenderUtil::getTextWidth(&modName, textSize)) / 2;
                            float cRectCentreY = modRect.y + ((modRect.w - modRect.y) - textHeight) / 2;

                            // cRectCentreX. vRectCentreY
                            //.lerp(Vector2<float>(modRect.x + 5, cRectCentreY), mod->cAnim) // if we want lerp to left on extend
                            Vector2<float> modPosLerped = Vector2<float>(cRectCentreX, cRectCentreY);

                            ImRenderUtil::drawText(modPosLerped, &modName, UIColor(mod->isEnabled() ? UIColor(255, 255, 255) : UIColor(180, 180, 180)).lerp(UIColor(100, 100, 100), mod->cAnim), textSize, animation, true);

                            /*std::string bindName = (mod == lastMod && ClickGUIManager::isBinding) ? "Binding..." : std::to_string((char)mod->getKeybind()).c_str();
                            float bindNameLen = ImRenderUtil::getTextWidth(&bindName, textSize);
                            Vector4<float> bindRect = Vector4<float>((modRect.z - 10) - bindNameLen, modRect.y + 2.5, modRect.z - 2.5, modRect.w - 2.5);
                            Vector2<float> bindTextPos = Vector2<float>(bindRect.x + 3.5, cRectCentreY);
                            
                            if (mod->getKeybind() != 7) {
                                ImRenderUtil::fillRectangle(bindRect, UIColor(29, 29, 29), 0.9, 4);
                                ImRenderUtil::drawText(bindTextPos, &bindName, UIColor(255, 255, 255), textSize, animation, true);
                            }
                            else if (mod->getKeybind() == 7 && mod == lastMod && ClickGUIManager::isBinding) {
                                ImRenderUtil::fillRectangle(bindRect, UIColor(29, 29, 29), 0.9, 4);
                                ImRenderUtil::drawText(bindTextPos, &bindName, UIColor(255, 255, 255), textSize, animation, true);
                            }*/

                            GuiData* guidata = Address::getClientInstance()->getGuiData();
                            float renderx = guidata->mcResolution.x / 2;
                            float rendery = (guidata->mcResolution.y / 2) + 110;

                            Vector2<float> textPos = Vector2<float>(renderx, rendery + 70.f);

                            string text = "Hover a module and click the Middle button to bind";
                            float textLen = ImRenderUtil::getTextWidth(&text, 1.3) / 2;
                            textPos.x -= textLen;

                            ImRenderUtil::drawText(textPos, &text, UIColor(170, 170, 170), 1.3f * inScale, inScale, true);

                            if (ImRenderUtil::isMouseOver(modRect) && catPositions[i].isExtended && isEnabled)
                            {
                                if (ImRenderUtil::isMouseOver(catWindow) && catPositions[i].isExtended) {
                                    tooltip = mod->getDescription();
                                }

                                if (Utils::leftDown)
                                {
                                    mod->toggle();
                                    Utils::leftDown = false;
                                    //particleMgr.addParticles(5, GuiInfo::MousePos.x, GuiInfo::MousePos.y, 80, 2.f);
                                }
                                else if (Utils::rightClick)
                                {
                                    mod->showSettings = !mod->showSettings;
                                    Utils::rightClick = false;
                                }
                                else if (Utils::middleClick) {
                                    lastMod = mod;
                                    ClickGUIManager::isBinding = true;
                                    Utils::middleClick = false;
                                }
                            }
                        }
                        if (modRect.y > catRect.y - modHeight) {
                            // Render a slight glow effect
                            ImRenderUtil::fillGradientOpaqueRectangle(Vector4<float>(modRect.x, modRect.w, modRect.z, modRect.w + 10.f * mod->cAnim * animation), UIColor(0, 0, 0), UIColor(0, 0, 0), 0.F * animation, 0.55F * animation);
                        }
                        moduleY += modHeight;
                    }
                }

                if (ClickGUIManager::isBinding) {
                    tooltip = Utils::combine("Currently binding ", lastMod->getName(), "...", " Press ESC to unbind.");
                    for (const auto& key : Global::Keymap) {
                        if (key.second && lastMod) {
                            lastMod->setKeybind(key.first == Keys::ESC ? 7 : key.first);
                            ClickGUIManager::isBinding = false;
                        }
                    }
                }

                std::string catName = categories[i];

                if (ImRenderUtil::isMouseOver(catRect) && Utils::rightDown)
                    catPositions[i].isExtended = !catPositions[i].isExtended;

                ImRenderUtil::fillRectangle(catRect, UIColor(20, 20, 20), animation);

                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);

                // Calculate the centre of the rect
                float cRectCentreX = catRect.x + ((catRect.z - catRect.x) - ImRenderUtil::getTextWidth(&catName, textSize * 1.15)) / 2;
                float cRectCentreY = catRect.y + ((catRect.w - catRect.y) - textHeight) / 2;

                ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[6]);

                std::string Symbol = "L";

                if (catName == "Combat") {
                    Symbol = "c";
                }

                if (catName == "Motion") {
                    Symbol = "f";
                }

                if (catName == "Misc") {
                    Symbol = "a";
                }

                if (catName == "Player") {
                    Symbol = "e";
                }

                if (catName == "Visual") {
                    Symbol = "d";
                }

                ImRenderUtil::drawText(Vector2(catRect.x + 10, cRectCentreY), &Symbol, UIColor(255, 255, 255), textSize * 1.f, animation, false);
                ImGui::PopFont();

                // Draw the string
                ImRenderUtil::drawText(Vector2(catRect.x + 34, cRectCentreY), &catName, UIColor(255, 255, 255), textSize * 1.14, animation, false);

#pragma region DraggingLogic
                static bool dragging = false;
                static Vector2<float> dragOffset;
                if (catPositions[i].isDragging)
                {
                    if (Utils::leftClick)
                    {
                        if (!dragging)
                        {
                            dragOffset = Vector2<float>(ImRenderUtil::getMousePos().x - catRect.x, ImRenderUtil::getMousePos().y - catRect.y);
                            dragging = true;
                        }
                        Vector2<float> newPosition = Vector2<float>(ImRenderUtil::getMousePos().x - dragOffset.x, ImRenderUtil::getMousePos().y - dragOffset.y);
                        newPosition.x = std::clamp(newPosition.toFloat().x, 0.f, ImRenderUtil::getScreenSize().x - catWidth);
                        newPosition.y = std::clamp(newPosition.toFloat().y, 0.f, ImRenderUtil::getScreenSize().y - catHeight);
                        catPositions[i].x = newPosition.x;
                        catPositions[i].y = newPosition.y;
                    }
                    else
                    {
                        catPositions[i].isDragging = false;
                        dragging = false;
                    }
                }
                else if (ImRenderUtil::isMouseOver(catRect) && Utils::leftDown && isEnabled)
                {
                    Utils::leftDown = false;
                    catPositions[i].isDragging = true;
                    dragOffset = Vector2<float>(ImRenderUtil::getMousePos().x - catRect.x, ImRenderUtil::getMousePos().y - catRect.y);
                }
#pragma endregion
                ImGui::PopFont();
            }

            if (!tooltip.empty()) {
                float textWidth = ImRenderUtil::getTextWidth(&tooltip, textSize * 0.8f);
                float textHeight = ImRenderUtil::getTextHeight(textSize * 0.8f);
                float padding = 2.5f;
                float offset = 8.f;

                Vector4<float> tooltipRect = Vector4<float>(
                    ImRenderUtil::getMousePos().x + offset - padding,
                    ImRenderUtil::getMousePos().y + textHeight / 2 - textHeight - padding,
                    ImRenderUtil::getMousePos().x + offset + textWidth + padding * 2,
                    ImRenderUtil::getMousePos().y + textHeight / 2 + padding
                ).scaleToPoint(Vector4<float>(
                    ImRenderUtil::getScreenSize().x / 2,
                    ImRenderUtil::getScreenSize().y / 2,
                    ImRenderUtil::getScreenSize().x / 2,
                    ImRenderUtil::getScreenSize().y / 2
                ), inScale);

                ImRenderUtil::fillRectangle(tooltipRect, UIColor(20, 20, 20), animation, 0.f);
                ImRenderUtil::drawText(Vector2(tooltipRect.x + padding, tooltipRect.y + padding), &tooltip, UIColor(255, 255, 255), textSize * 0.8f, animation, true);
            }

            if (isEnabled) {
                Utils::leftDown = false;
                Utils::rightDown = false;
                Utils::rightClick = false;
                Address::getClientInstance()->grabMouse();
                scrollDirection = 0;
            }
        }
        ImGui::PopFont();

        Vector4<float> ProfileRectPos(10, ImRenderUtil::getScreenSize().y - 35, 130, ImRenderUtil::getScreenSize().y - 5);
        ImRenderUtil::fillRectangle(ProfileRectPos, UIColor(15, 13, 15), animation, 5);
        ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::PlayerTexture, ImVec2(ProfileRectPos.x + 9, ProfileRectPos.y + 3), ImVec2(ProfileRectPos.x + 20, ProfileRectPos.w - 3), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 30);

        if (ImRenderUtil::isMouseOver(ProfileRectPos))
        {
            if (Utils::leftClick) {
                getModuleByName("clickgui")->setEnabled(false);
                Global::renderUI = true;
                Utils::leftClick = false;
            }
        }
    }
};
