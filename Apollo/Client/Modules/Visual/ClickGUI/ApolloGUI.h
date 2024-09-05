#pragma once

class ApolloGUI {
public:
	struct CategoryPosition {
		float x = 0.f, y = 0.f;
		bool isDragging = false, isExtended = true, wasExtended = false;
		float yOffset = 0;
		float scrollEase = 0;
		Vector4<float> shadowRect = Vector4<float>();
		Vector2<float> dragVelocity = Vector2<float>();
		Vector2<float> dragAcceleration = Vector2<float>();
	};

	const float catWidth = 200.f;
	const float catHeight = 35.f;

	const float catGap = 40;

	float scrollSize = 0.f;
	std::vector<CategoryPosition> catPositions;

	UIColor textColor = UIColor(255, 255, 255);
	UIColor darkBlack = UIColor(15, 15, 15);
	UIColor mainBlack = UIColor(22, 22, 22);
	UIColor grayBlack = UIColor(40, 40, 40);
	UIColor enumBack = UIColor(30, 30, 30);

	void render(float animation, float inScale, int& scrollDirection) noexcept {

		const Vector2<float> guiPos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2);
		ImRenderUtil::fillRectangle(Vector4<float>(0, 0, ImRenderUtil::getScreenSize().x, ImRenderUtil::getScreenSize().y), UIColor(0, 0, 0), animation * 0.2f);
		ImRenderUtil::Blur(Vector4<float>(0, 0, ImRenderUtil::getScreenSize().x, ImRenderUtil::getScreenSize().y), animation * 9, 0);
		bool isEnabled = getModuleByName("clickgui")->isEnabled();

		if (catPositions.empty() && isEnabled)
		{
			float centerX = ImRenderUtil::getScreenSize().x / 2;
			float xPos = centerX - (categories.size() * (catWidth + catGap) / 2);
			for (std::string& category : categories)
			{
				CategoryPosition pos;
				pos.x = xPos;
				pos.y = catGap;
				xPos += catWidth + catGap;
				catPositions.push_back(pos);
			}
		}

		if (!catPositions.empty()) {
			scrollSize = -scrollDirection * catHeight;
			for (size_t i = 0; i < categories.size(); i++) {
				catPositions[i].y += scrollSize;
				Vector4<float> catTabRect = Vector4<float>(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + catHeight);
				Vector2<float> catNameRect = Vector2<float>(catPositions[i].x + 7.f, catPositions[i].y + 7.f);

				ImRenderUtil::fillShadowRectangle(catPositions[i].shadowRect, mainBlack, 1.f, 70.f, ImDrawFlags_RoundCornersAll, 10.f);
				ImRenderUtil::fillShadowRectangle(catPositions[i].shadowRect, mainBlack, 1.f, 70.f, ImDrawFlags_RoundCornersAll, 10.f);

				const float modWidth = catWidth;
				const float modHeight = catHeight;
				float moduleY = 0;
				const auto& modsInCategory = ClickGUIManager::getModulesInCategory(categories[i], modules);
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
				int mcount = 0;
				for (const auto& mod : modsInCategory) {
					mcount++;
					float myHeight = modHeight;
					Vector4<float> modRect = Vector4<float>(catPositions[i].x, catPositions[i].y + moduleY + catHeight, catPositions[i].x + modWidth, catPositions[i].y + moduleY + catHeight + modHeight);
					Vector2<float> modNameRect = Vector2<float>(modRect.x + 7.5f, modRect.y + 7.5f);
					UIColor modColor = mainBlack;
					UIColor modSecColor = mainBlack;
					if (mod->isEnabled()) {
						modColor = ColorUtils::getClientColor(1.9, 1, 1, 0);
						modSecColor = ColorUtils::getClientColor(1.9, 1, 1, 180);
					}
					float round = 0.f;
					if (mcount == modsInCategory.size() && !mod->showSettings) round = 12.f;
					ImRenderUtil::fillRoundedGradientRectangle(modRect, modColor, modSecColor, round, 1.f, 1.f, ImDrawFlags_RoundCornersBottom);
					std::string modName = mod->getName();
					ImRenderUtil::drawText(modNameRect, &modName, textColor, 1.2f);

					if (ImRenderUtil::isMouseOver(modRect) && isEnabled) {
						if (Utils::leftClick) {
							mod->toggle();
							Utils::leftClick = false;
						} else if (Utils::rightClick) {
							mod->showSettings = !mod->showSettings;
							Utils::rightClick = false;
						}
					}

					Vector4<float> backgroundRect = Vector4<float>(modRect.x, modRect.w, modRect.z, modRect.w + mod->settingsScale);
					round = 0.f;
					if (mcount == modsInCategory.size() && mod->showSettings) round = 12.f;
					ImRenderUtil::fillRectangle(backgroundRect, mainBlack, 1.f, round, ImDrawFlags_RoundCornersBottom);
					float settingsHeight = 0.f;
					if (mod->showSettings) {
						float padding = 5.f;
						ImClippingUtil::beginClipping(Vector4<float>(modRect.x, modRect.w, modRect.z, modRect.w + mod->settingsScale));
						for (const auto& setting : mod->getSettings()) {
							if (!setting->render) continue;
							float settingHeight = 0.f;
							setting->renderEnumSlide = Math::animate(setting->enumSlide, setting->renderEnumSlide, ImRenderUtil::getDeltaTime() * 10);
							if (setting->getType() == SettingType::BOOL) settingHeight = modHeight + padding;
							else if (setting->getType() == SettingType::ENUM) settingHeight = modHeight * 1.5 + padding;
							else if (setting->getType() == SettingType::SLIDER) settingHeight = modHeight + padding;
							Vector4<float> settingRect = Vector4<float>(modRect.x, modRect.w + settingsHeight, modRect.z, modRect.w + settingsHeight + settingHeight);
							Vector2<float> settingCenter = Vector2<float>((settingRect.x + settingRect.z) / 2, (settingRect.y + settingRect.w) / 2);
							modColor = ColorUtils::getClientColor(1.9, 1, 1, 0);
							modSecColor = ColorUtils::getClientColor(1.9, 1, 1, 180);
							settingHeight += setting->renderEnumSlide;

							switch (setting->getType()) {
							case SettingType::BOOL: {
								Vector4<float> buttonRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 15, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 15);
								if (*(bool*)setting->getValue()) {
									ImRenderUtil::drawRoundedGradientRectangleOutline(buttonRect, modColor, modSecColor, 6.f, 1.f, 1.f, 2.f);
								} else {
									ImRenderUtil::drawRoundRect(buttonRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
								}

								std::string settingName = setting->getName();
								float textSizeX = ImRenderUtil::getTextWidth(&settingName, 1.f);
								float textSizeY = ImRenderUtil::getTextHeightStr(&settingName, 1.f);
								Vector2<float> settingTextRect = Vector2<float>(settingCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2);
								ImRenderUtil::drawText(settingTextRect, &settingName, textColor, 1.f);

								if (ImRenderUtil::isMouseOver(buttonRect) && isEnabled) {
									if (Utils::leftClick) {
										*(bool*)setting->value = !*(bool*)setting->value;
										Utils::leftClick = false;
									}
								}
								break;
							}
							case SettingType::ENUM: {
								Vector4<float> buttonRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 5.f, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 25.f);
								Vector2<float> buttonCenter = Vector2<float>((buttonRect.x + buttonRect.z) / 2, (buttonRect.y + buttonRect.w) / 2);
								Vector4<float> extendRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - 5.f, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + 25.f + setting->renderEnumSlide);
								if (setting->enumExtended) {
									ImRenderUtil::fillRectangle(extendRect, mainBlack, 1.f, 6.f);
									ImRenderUtil::drawRoundRect(extendRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
									int count = 0;
									for (std::string& enumValue : setting->enumValues) {
										count++;
										float textSizeX = ImRenderUtil::getTextWidth(&enumValue, 1.f);
										float textSizeY = ImRenderUtil::getTextHeightStr(&enumValue, 1.f);
										Vector4<float> enumRect = Vector4<float>(buttonRect.x, buttonRect.y + modHeight * count, buttonRect.z, buttonRect.w + modHeight * count);
										Vector2<float> settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, buttonCenter.y - textSizeY / 2 + modHeight * count);
										ImRenderUtil::ImRenderUtil::drawText(settingTextRect, &enumValue, textColor, 1.f);

										if (ImRenderUtil::isMouseOver(enumRect) && isEnabled) {
											if (Utils::leftClick) {
												*setting->iterator = count - 1;
												Utils::leftClick = false;
											}
										}
									}
								}
								ImRenderUtil::fillRectangle(buttonRect, enumBack, 1.f, 6.f);
								ImRenderUtil::drawRoundRect(buttonRect, ImDrawFlags_None, 6.f, grayBlack, 1.f, 2.f);
								std::string settingName = setting->getName();
								float textSizeX = ImRenderUtil::getTextWidth(&settingName, 0.9f);
								float textSizeY = ImRenderUtil::getTextHeightStr(&settingName, 0.9f);
								Vector2<float> settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2 - 15.f);
								ImRenderUtil::drawText(settingTextRect, &settingName, textColor, 0.9f);

								int iterator = *(int*)setting->getIterator();
								std::string& enumValue = setting->enumValues[iterator];
								textSizeX = ImRenderUtil::getTextWidth(&enumValue, 1.f);
								textSizeY = ImRenderUtil::getTextHeightStr(&enumValue, 1.f);
								settingTextRect = Vector2<float>(buttonCenter.x - textSizeX / 2, buttonCenter.y - textSizeY / 2);
								ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
								ImRenderUtil::ImRenderUtil::drawText(settingTextRect, &enumValue, textColor, 1.f);
								ImGui::PopFont();

								if (ImRenderUtil::isMouseOver(buttonRect) && isEnabled) {
									if (Utils::leftClick) {
										*setting->iterator = (iterator + 1) % setting->getEnumValues().size();
										Utils::leftClick = false;
									} else if (Utils::rightClick) {
										if (setting->enumExtended) {
											setting->enumExtended = false;
											setting->enumSlide = 0;
										} else {
											setting->enumExtended = true;
											setting->enumSlide = modHeight * setting->enumValues.size();
										}
										Utils::rightClick = false;
									}
								}
								break;
							}
							case SettingType::SLIDER: {
								float sliderHeight = 8.f;
								float sliderPad = 8.f;
								Vector4<float> sliderRect = Vector4<float>(settingCenter.x - modWidth / 2 + 10.f, settingCenter.y - sliderHeight / 2 + sliderPad, settingCenter.x + modWidth / 2 - 10.f, settingCenter.y + sliderHeight / 2 + sliderPad);
								Vector2<float> sliderCenter = Vector2<float>((sliderRect.x + sliderRect.z) / 2, (sliderRect.y + sliderRect.w) / 2);

								ImRenderUtil::fillRectangle(sliderRect, enumBack, 1.f, 20.f);
								ImRenderUtil::drawRoundRect(sliderRect, ImDrawFlags_None, 20.f, grayBlack, 1.f, 2.f);
								float value = *(float*)setting->getValue();
								float round = setting->getRound();
								float currentValue = Math::round(value, round);

								std::ostringstream oss;
								oss << std::fixed << std::setprecision(Math::getDecimalPlaces(round)) << (currentValue);
								std::string str = oss.str();

								std::string settingName = setting->getName() + ": " + str;
								float textSizeX = ImRenderUtil::getTextWidth(&settingName, 0.9f);
								float textSizeY = ImRenderUtil::getTextHeightStr(&settingName, 0.9f);
								Vector2<float> settingTextRect = Vector2<float>(settingCenter.x - textSizeX / 2, settingCenter.y - textSizeY / 2 - sliderPad);
								std::string tmpModeText = setting->getName() + ": ";
								float tmpTextSizeX = ImRenderUtil::getTextWidth(&tmpModeText, 0.9f);
								Vector2<float> tmpValueTextRect = Vector2<float>(settingCenter.x - textSizeX / 2 + tmpTextSizeX, settingCenter.y - textSizeY / 2 - sliderPad);
								std::string tmpValueText = str;
								ImRenderUtil::drawText(settingTextRect, &tmpModeText, textColor, 0.9f);
								ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
								ImRenderUtil::drawText(tmpValueTextRect, &tmpValueText, textColor, 0.9f);
								ImGui::PopFont();

								float maxX = settingCenter.x + modWidth / 2 - 10.f;
								float minX = settingCenter.x - modWidth / 2 + 10.f;
								float deltaBar = maxX - minX;

								float maxSetting = setting->getMax();
								float minSetting = setting->getMin();
								float deltaSetting = maxSetting - minSetting;

								if (currentValue > maxSetting) currentValue = maxSetting;
								if (currentValue < minSetting) currentValue = minSetting;

								float deltaNowBar = currentValue - minSetting;

								float barPerSetting = deltaNowBar / deltaSetting;
								float barWidth = deltaBar * barPerSetting;

								Vector4<float> fillBar = Vector4<float>(minX, settingCenter.y - sliderHeight / 2 + sliderPad, minX + barWidth, settingCenter.y + sliderHeight / 2 + sliderPad);
								ImRenderUtil::fillRoundedGradientRectangle(fillBar, modColor, modSecColor, 20.f);

								Vector2<float> circlePos = Vector2<float>(minX + barWidth, settingCenter.y + sliderPad);
								ImRenderUtil::fillCircle(circlePos, 7.f, mainBlack, 1.f, 12.f);
								ImRenderUtil::drawCircleOutline(circlePos, 7.f, modSecColor, 1.f, 12.f, 2.f);

								if (ImRenderUtil::isMouseOver(sliderRect) && isEnabled && Utils::leftClick) setting->isDragging = true;

								if (Utils::leftClick && setting->isDragging && isEnabled) {
									float mouseX = ImRenderUtil::getMousePos().x;
									float addVal = mouseX - minX;
									if (addVal + minX > maxX) addVal = deltaBar;
									if (addVal < 0) addVal = 0;
									float idk = addVal / deltaBar;
									*(float*)setting->value = Math::round(idk * deltaSetting + minSetting, round);
								} 
								else setting->isDragging = false;
								break;
							}
							}
							settingsHeight += settingHeight;
						}
						ImClippingUtil::restoreClipping();
					}
					mod->settingsScale = Math::animate(settingsHeight, mod->settingsScale, ImRenderUtil::getDeltaTime() * 10);
					myHeight += mod->settingsScale;
					moduleY += myHeight;
				}

				//Render Category tabs
				ImRenderUtil::fillRectangle(catTabRect, darkBlack, 1.f, 12, ImDrawFlags_RoundCornersTop);
				std::string catName = categories[i];
				ImGui::PopFont();
				ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
				ImRenderUtil::drawText(catNameRect, &catName, textColor, 1.3f);
				ImGui::PopFont();

				catPositions[i].shadowRect = Vector4<float>(catPositions[i].x, catPositions[i].y, catPositions[i].x + catWidth, catPositions[i].y + catHeight + moduleY);
			}
		}
		if (isEnabled) {
			Utils::leftDown = false;
			Utils::rightDown = false;
			Utils::rightClick = false;
			Address::getClientInstance()->grabMouse();
			scrollDirection = 0;
		}
	}
};
