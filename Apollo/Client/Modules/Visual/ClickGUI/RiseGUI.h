#pragma once

class RiseGUI {
public:
	void render(float animation, float inScale, int& scrollDirection) noexcept {
		const float guiWidth = 360.f;
		const float guiHeight = 280.f;

		const float catOffset = 200.f;

		const Vector2<float> guiPos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2, ImRenderUtil::getScreenSize().y / 2);

		Vector4<float> baseRect = Vector4<float>(guiPos.x - guiWidth, guiPos.y - guiHeight, guiPos.x + guiWidth, guiPos.y + guiHeight);
		Vector4<float> catRect = Vector4<float>(baseRect.x, baseRect.y, baseRect.x + catOffset, baseRect.w);
		Vector4<float> mainRect = Vector4<float>(baseRect.x + catOffset, baseRect.y, baseRect.z, baseRect.w);

		static float scrollEased = 0;
		static float scroll = 0;

		std::string ClientName = "Rise";

		bool isEnabled = getModuleByName("clickgui")->isEnabled();
		scrollEased = Math::animate(scroll, scrollEased, ImRenderUtil::getDeltaTime() * 10.f);

		if (ImRenderUtil::isMouseOver(mainRect)) {
			scroll += scrollDirection * 20;

			if (scrollDirection < 0 && scroll < 0)
				scroll = 0;

			scrollDirection = 0;
		}

		static std::string currentCategory = "Combat";
		static std::vector<Module*> modList = ClickGUIManager::getModulesInCategory(currentCategory, modules);

		ImScaleUtil::ImScaleStart();

		ImRenderUtil::fillRectangleCustom(catRect, UIColor(18, 20, 25), animation * 0.98, Vector4<float>(20, 20, 0, 0));
		ImRenderUtil::fillRectangleCustom(mainRect, UIColor(23, 26, 33), animation, Vector4<float>(0, 0, 20, 20));

		ImRenderUtil::fillShadowCircle(Vector2<float>(catRect.x + 25.f + ImRenderUtil::getTextWidth(&ClientName, 2.5f) / 2, catRect.y + 15.f + ImRenderUtil::getTextHeight(2.5) / 2), 1.f, UIColor(255, 255, 255), animation * 0.7F, 100, 0);
		ImRenderUtil::drawText(Vector2<float>(catRect.x + 25.f, catRect.y + 15.f), &ClientName, UIColor(255, 255, 255), 2.5f, animation, true);
		//std::string vers = "0.5";
		//ImRenderUtil::drawText(Vector2<float>(catRect.x + 25.f + ImRenderUtil::getTextWidth(&ClientName, 2.5f), catRect.y + 14.f), &vers, ColorUtil::getClientColor(5, 1, 1, 0), 1.f, animation, true);

		int ind = 0;
		std::string symbols[] = { "search", "combat", "misc", "motion", "player", "visual", "scripts", "themes" };
		//std::string symbols[] = { "combat", "misc", "motion", "player", "misc", "search", "scripts", "themes" };

		for (std::string category : GUICategories) {
			Vector2<float> textPosition(catRect.x + 25.f, catRect.y + 90.f + ind);

			Vector4<float> catTab(textPosition.x - 5.f, textPosition.y - 5, textPosition.x - 5.f + ImRenderUtil::getTextWidth(&category, 1.f) + 12.5f + 25.f, textPosition.y + ImRenderUtil::getTextHeight(1.f) + 5);

			catTab.y = Math::animate(textPosition.y - 5, catTab.y, ImRenderUtil::getDeltaTime() * 15.f);
			catTab.w = Math::animate(textPosition.y + ImRenderUtil::getTextHeight(1.f) + 5, catTab.w, ImRenderUtil::getDeltaTime() * 15.f);

			if (ImRenderUtil::isMouseOver(catTab) && Utils::leftClick) {
				Utils::leftClick = false;
				currentCategory = category;
				modList.clear();
				scroll = 0;
				modList = ClickGUIManager::getModulesInCategory(currentCategory, modules);

				if (currentCategory == "Search") {
					modList = modules;
				}
			}

			if (category == currentCategory) {
				ImRenderUtil::fillShadowRectangle(catTab.scale(2), ColorUtils::RainbowDark(5, 0), animation * 0.5f, 40.f, 0, 10);
				ImRenderUtil::fillRectangle(catTab, ColorUtils::RainbowDark(5, 0), animation, 8.f);
			}
			int symbolIndex = std::distance(GUICategories.begin(), std::find(GUICategories.begin(), GUICategories.end(), category));

			if (symbolIndex >= 0 && symbolIndex < sizeof(symbols) / sizeof(symbols[0])) {
				std::string symbol = symbols[symbolIndex];

				//ImRenderUtil::drawText(Vector2<float>(textPosition.x, textPosition.y), &symbol, UIColor(255, 255, 255), 1.f, animation, true, 1);

				if (symbol == "search") { // Search
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "combat") { // Combat
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::CombatTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "misc") { // Misc
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::MiscTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "motion") { // Motion
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::MovementTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "player") { // Player
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::PlayerTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "visual") {// Visual
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::VisualTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}

				if (symbol == "themes") {
					ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::VisualTexture, ImVec2(textPosition.x, textPosition.y - 2.5), ImVec2(textPosition.x + 20, textPosition.y + 17.5), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, 1);
				}
			}
			ImRenderUtil::drawText(Vector2<float>(textPosition.x + 25.f, textPosition.y), &category, UIColor(255, 255, 255), 1.f, animation, true);
			ind += (ImRenderUtil::getTextHeight(1.2) + 20);
		}

		float modInd = -scrollEased;

		Vector4<float> clipRect = Vector4<float>(baseRect.x, baseRect.y, baseRect.z, baseRect.w).scaleToCenter(inScale);
		ImClippingUtil::beginClipping(clipRect);

		if (currentCategory == "Themes") {
			std::string Arraylist = "Arraylist:";
			std::string ColourTheme = "You can choose a color by clicking on it";

			std::string rgb = "Rainbow";
			std::string astolfo = "Astolfo";

			Vector2<float> ColourThemeText(mainRect.x + 15.f, mainRect.y + 15.f);
			Vector2<float> ArrayListText(mainRect.x + 15.f, mainRect.y + 45.f);
			Vector2<float> sp(mainRect.x + 15.f, mainRect.y + 90.f);

			// Draw the ArrayList text
			ImRenderUtil::drawText(ColourThemeText, &ColourTheme, UIColor(160, 160, 160), 1.f, animation, true);
			ImRenderUtil::drawText(ArrayListText, &Arraylist, UIColor(255, 255, 255), 1.35f, animation, true);

			Vector4<float> RainbowPos(sp.x + 29, sp.y, sp.x + 116.25, sp.y + 50);
			Vector4<float> RainbowPosCornerR(sp.x, sp.y, sp.x + 29.25, sp.y + 50);
			Vector4<float> RainbowPosCornerL(sp.x + 116.25, sp.y, sp.x + 145, sp.y + 50);
			Vector4<float> RainbowPosBottom(sp.x, RainbowPos.w, RainbowPos.z + 29, RainbowPos.w + 30);

			// 165 new 194
			Vector4<float> AstolfoPos(sp.x + 194, sp.y, sp.x + 281.25, sp.y + 50);
			Vector4<float> AstolfoPosCornerR(sp.x + 165, sp.y, sp.x + 194.25, sp.y + 50);
			Vector4<float> AstolfoPosCornerL(sp.x + 281.25, sp.y, sp.x + 310.25, sp.y + 50);
			//Vector4<float> AstolfoPos(sp.x + 194, sp.y, sp.x + 281.25, sp.y + 50);
			Vector4<float> AstolfoPosBottom(sp.x + 165, AstolfoPos.w, AstolfoPos.z + 29, AstolfoPos.w + 30);

			// Rainbow
			ImRenderUtil::fillGradientOpaqueRectangle(RainbowPos, ColorUtils::Rainbow(2.5, 1, 1, 1), ColorUtils::Rainbow(2.5, 1, 1, 400), 1, 1);
			ImRenderUtil::fillRectangleCustom(RainbowPosCornerR, ColorUtils::Rainbow(2.5, 1, 1, 1), 1, Vector4<float>(20, 0, 0, 0));
			ImRenderUtil::fillRectangleCustom(RainbowPosCornerL, ColorUtils::Rainbow(2.5, 1, 1, 400), 1, Vector4<float>(0, 0, 20, 0));

			ImRenderUtil::fillShadowRectangle(Vector4<float>(RainbowPosCornerR.x, RainbowPosCornerR.y, RainbowPosCornerR.z + 29.25, RainbowPosCornerR.w), ColorUtils::Rainbow(2.5, 1, 1, 1), 1, 70, 0);
			ImRenderUtil::fillShadowRectangle(Vector4<float>(RainbowPosCornerL.x - 29.25, RainbowPosCornerL.y, RainbowPosCornerL.z, RainbowPosCornerL.w), ColorUtils::Rainbow(2.5, 1, 1, 400), 1, 70, 0);

			ImRenderUtil::fillRectangleCustom(RainbowPosBottom, UIColor(8, 19, 30), 1, Vector4<float>(0, 20, 0, 20));
			
			// Astolfo
			ImRenderUtil::fillGradientOpaqueRectangle(AstolfoPos, ColorUtils::Rainbow(2.5, 0.5, 1, 1), ColorUtils::Rainbow(2.5, 0.5, 1, 400), 1, 1);
			ImRenderUtil::fillRectangleCustom(AstolfoPosCornerR, ColorUtils::Rainbow(2.5, 0.5, 1, 1), 1, Vector4<float>(20, 0, 0, 0));
			ImRenderUtil::fillRectangleCustom(AstolfoPosCornerL, ColorUtils::Rainbow(2.5, 0.5, 1, 400), 1, Vector4<float>(0, 0, 20, 0));

			ImRenderUtil::fillShadowRectangle(Vector4<float>(AstolfoPosCornerR.x, AstolfoPosCornerR.y, AstolfoPosCornerR.z + 29.25, AstolfoPosCornerR.w), ColorUtils::Rainbow(2.5, 0.5, 1, 1), 1, 70, 0);
			ImRenderUtil::fillShadowRectangle(Vector4<float>(AstolfoPosCornerL.x - 29.25, AstolfoPosCornerL.y, AstolfoPosCornerL.z, AstolfoPosCornerL.w), ColorUtils::Rainbow(2.5, 0.5, 1, 400), 1, 70, 0);

			ImRenderUtil::fillRectangleCustom(AstolfoPosBottom, UIColor(8, 19, 30), 1, Vector4<float>(0, 20, 0, 20));

			float rgbTextLen = ImRenderUtil::getTextWidth(&rgb, 1) / 2;

			Vector2<float> rgbText(RainbowPos.x + (rgbTextLen / 2) - 5, RainbowPosBottom.y + 7);

			Vector2<float> astolfoText(AstolfoPos.x + rgbTextLen - 7, AstolfoPosBottom.y + 7);

			ImRenderUtil::drawText(rgbText, &rgb, UIColor(255, 255, 255), 1.f, animation, true);
			ImRenderUtil::drawText(astolfoText, &astolfo, UIColor(255, 255, 255), 1.f, animation, true);
			//ImRenderUtil::drawText(descPos, &desc, UIColor(160, 160, 160), 1.f, animation, true);

			if (ImRenderUtil::isMouseOver(RainbowPos) || ImRenderUtil::isMouseOver(RainbowPosBottom) || ImRenderUtil::isMouseOver(RainbowPosCornerR) || ImRenderUtil::isMouseOver(RainbowPosCornerL)) {
				if (Utils::leftDown) {
					Global::ArrayListColors = 0; // RGB
				}
			}

			if (ImRenderUtil::isMouseOver(AstolfoPos) || ImRenderUtil::isMouseOver(AstolfoPosBottom) || ImRenderUtil::isMouseOver(AstolfoPosCornerR) || ImRenderUtil::isMouseOver(AstolfoPosCornerL)) {
				if (Utils::leftDown) {
					Global::ArrayListColors = 2; // RGB
				}
			}
		}

		for (auto mod : modList) {
			UIColor rgb = ColorUtils::RainbowDark(5, modInd * 2);
			Vector2<float> modulePosition(mainRect.x + 15.f, mainRect.y + 10.f + modInd);
			Vector4<float> moduleRect(modulePosition.x, modulePosition.y, mainRect.z - 15.f, modulePosition.y + 65.f);

			ImRenderUtil::fillRectangle(moduleRect, UIColor(18, 20, 25), animation * 0.5f, 10);

			std::string modName = mod->getName();
			std::string desc = mod->getDescription();

			// Calculate the text positions
			Vector2<float> modNamePos = Vector2<float>(moduleRect.x + 10.f, moduleRect.y + 8.f);
			Vector2<float> descPos = Vector2<float>(moduleRect.x + 10.f, moduleRect.y + 38.f);

			if (ImRenderUtil::isMouseOver(moduleRect) && isEnabled) {
				if (Utils::leftClick) {
					mod->toggle();
					Utils::leftClick = false;
				}
				else if (Utils::rightClick) {
					mod->showSettings = !mod->showSettings;
					Utils::rightClick = false;
				}
			}

			// Draw module name
			ImRenderUtil::drawText(modNamePos, &modName, mod->isEnabled() ? rgb : UIColor(255, 255, 255), 1.4f, animation, true);

			// Draw module description
			ImRenderUtil::drawText(descPos, &desc, UIColor(160, 160, 160), 1.f, animation, true);

			modInd += 75.f;
		}
		ImClippingUtil::restoreClipping();

		ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(baseRect.getCenter().x, baseRect.getCenter().y));
		if (isEnabled) {
			Address::getClientInstance()->grabMouse();
		}
	}
};
