#pragma once

// Define a mapping from Minecraft color codes to ImGui colors
std::unordered_map<char, ImVec4> mcColorMap = {
	{'0', ImVec4(0.0f, 0.0f, 0.0f, 1.0f)}, // Black
	{'1', ImVec4(0.0f, 0.0f, 0.5f, 1.0f)}, // Dark Blue
	{'2', ImVec4(0.0f, 0.5f, 0.0f, 1.0f)}, // Dark Green
	{'3', ImVec4(0.0f, 0.5f, 0.5f, 1.0f)}, // Dark Aqua
	{'4', ImVec4(0.5f, 0.0f, 0.0f, 1.0f)}, // Dark Red
	{'5', ImVec4(0.5f, 0.0f, 0.5f, 1.0f)}, // Dark Purple
	{'6', ImVec4(0.5f, 0.5f, 0.0f, 1.0f)}, // Gold
	{'7', ImVec4(0.75f, 0.75f, 0.75f, 1.0f)}, // Gray
	{'8', ImVec4(0.25f, 0.25f, 0.25f, 1.0f)}, // Dark Gray
	{'9', ImVec4(0.25f, 0.25f, 0.75f, 1.0f)}, // Blue
	{'a', ImVec4(0.25f, 0.75f, 0.25f, 1.0f)}, // Green
	{'b', ImVec4(0.25f, 0.75f, 0.75f, 1.0f)}, // Aqua
	{'c', ImVec4(0.75f, 0.25f, 0.25f, 1.0f)}, // Red
	{'d', ImVec4(0.75f, 0.25f, 0.75f, 1.0f)}, // Light Purple
	{'e', ImVec4(0.75f, 0.75f, 0.25f, 1.0f)}, // Yellow
	{'f', ImVec4(1.0f, 1.0f, 1.0f, 1.0f)}, // White
};

class ImRenderUtil
{
public:
	inline static void drawText(Vector2<float> pos, std::string* textStr, const UIColor& color, float textSize, float alpha = 1.f, bool shadow = false, int index = 0, ImDrawList* d = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;
		ImFont* font = ImGui::GetFont();



		ImVec2 textPos = ImVec2(pos.x, pos.y);
		ImVec2 shadowOffset = ImVec2(textPos.x + 1.f, textPos.y + 1.f);
		ImVec2 shadowOffsetMinecraft = ImVec2(textPos.x + 2.15f, textPos.y + 2.15f);

		if (shadow)
		{
			if (font == ImGui::GetIO().Fonts->Fonts[4]) {
				d->AddText(font, (textSize * 18), shadowOffsetMinecraft, ImColor(color.r * 0.2f, color.g * 0.2f, color.b * 0.2f, alpha * 0.7f), textStr->c_str());
			}
			else {
				d->AddText(font, (textSize * 18), shadowOffset, ImColor(color.r * 0.2f, color.g * 0.2f, color.b * 0.2f, alpha * 0.9f), textStr->c_str());
			}
		}

		d->AddText(font, (textSize * 18), textPos, ImColor(color.r, color.g, color.b, alpha), textStr->c_str());
	};

	inline static void drawMCColorText(Vector2<float> pos, std::string* textStr, float textSize, float alpha, bool shadow = false, int index = 0, ImDrawList* d = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;
		ImFont* font = ImGui::GetFont();

		ImVec2 textPos = ImVec2(pos.x, pos.y);
		ImVec2 shadowOffset = ImVec2(textPos.x + 1.f, textPos.y + 1.f);
		ImVec2 shadowOffsetMinecraft = ImVec2(textPos.x + 2.15f, textPos.y + 2.15f);

		ImVec4 currentColor = mcColorMap['f']; // Default to white
		std::string currentText;

		for (size_t i = 0; i < textStr->size(); ++i) {
			if ((*textStr)[i] == '§' && i + 1 < textStr->size()) {
				// Render current text with current color
				if (!currentText.empty()) {
					if (shadow) {
						if (font == ImGui::GetIO().Fonts->Fonts[4]) {
							d->AddText(font, (textSize * 18), shadowOffsetMinecraft, ImColor(currentColor.x * 0.2f, currentColor.y * 0.2f, currentColor.z * 0.2f, alpha * 0.7f), currentText.c_str());
						}
						else {
							d->AddText(font, (textSize * 18), shadowOffset, ImColor(currentColor.x * 0.03f, currentColor.y * 0.03f, currentColor.z * 0.03f, alpha * 0.9f), currentText.c_str());
						}
					}
					d->AddText(font, (textSize * 18), textPos, ImColor(currentColor.x, currentColor.y, currentColor.z, alpha), currentText.c_str());

					// Update the text position after rendering the current segment
					float segmentWidth = ImGui::CalcTextSize(currentText.c_str()).x;
					textPos.x += segmentWidth;
					shadowOffset.x += segmentWidth;
					shadowOffsetMinecraft.x += segmentWidth;

					currentText.clear();
				}
				// Set the new color
				char colorCode = (*textStr)[++i];
				if (mcColorMap.find(colorCode) != mcColorMap.end()) {
					currentColor = mcColorMap[colorCode];
				}
			}
			else {
				currentText += (*textStr)[i];
			}
		}

		// Render any remaining text
		if (!currentText.empty()) {
			if (shadow) {
				if (font == ImGui::GetIO().Fonts->Fonts[4]) {
					d->AddText(font, (textSize * 18), shadowOffsetMinecraft, ImColor(currentColor.x * 0.2f, currentColor.y * 0.2f, currentColor.z * 0.2f, alpha * 0.7f), currentText.c_str());
				}
				else {
					d->AddText(font, (textSize * 18), shadowOffset, ImColor(currentColor.x * 0.03f, currentColor.y * 0.03f, currentColor.z * 0.03f, alpha * 0.9f), currentText.c_str());
				}
			}
			d->AddText(font, (textSize * 18), textPos, ImColor(currentColor.x, currentColor.y, currentColor.z, alpha), currentText.c_str());
		}
	}

	static void drawGradientText(Vector2<float> pos, std::string* text, float size, float alpha, bool shadow, int speed, int index)
	{
		TextHolder str(*text);
		int ind = 0;

		for (char c : std::string(str.getText()))
		{
			std::string string = Utils::combine(c, "");

			// Current color index
			int colorIndex = ind * index;

			// Get each char's width and draw
			float charWidth = getTextWidth(&string, size);

			drawText(Vector2<float>(pos.x, pos.y), &string, ColorUtils::Rainbow(speed, 1.F, 1.F, colorIndex), size, alpha, shadow);

			// Increment the color and pos index
			pos.x += charWidth;
			++ind;
		}
	}

	template <typename T>
	static void fillRectangle(Vector4<T> pos, const UIColor& color, float alpha, float rounding = 0.f, const ImDrawFlags& flags = ImDrawFlags_None, ImDrawList* list = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		list->AddRectFilled(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), rounding, flags);
	}

	template <typename T>
	static void Blur(Vector4<T> pos, float strenght, float rounding = 0.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImFX::Begin(ImGui::GetBackgroundDrawList());
		ImFX::AddBlur(strenght, ImVec4(pos.x, pos.y, pos.z, pos.w), rounding);
		ImFX::End();
	}

	template <typename T>
	static void fillRectangleCustom(Vector4<T> pos, const UIColor& color, float alpha, Vector4<float>rounding) {
		if (!ImGui::GetCurrentContext()) return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		list->AddRectFilledCustomRadius(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), rounding.x, rounding.z, rounding.y, rounding.w);
	}

	static void fillShadowRectangle(Vector4<float> pos, const UIColor& color, float alpha, float thickness, ImDrawFlags flags, float rounding = 0.f, ImDrawList* list = ImGui::GetBackgroundDrawList())
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags, rounding);
	}

	static void fillShadowCircle(Vector2<float> pos, float rounding, const UIColor& color, float alpha, float thickness, ImDrawFlags flags = ImDrawFlags_RoundCornersAll, float segments = 12.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		ImVec2 offset = ImVec2(0, 0);
		list->AddShadowCircle(ImVec2(pos.x, pos.y), rounding, ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags, rounding);
	}

	static void drawShadowSquare(Vector2<float> center, float size, const UIColor& color, float alpha, float thickness, ImDrawFlags flags)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		ImVec2 offset = ImVec2(0, 0);

		// Define the four corners of the square
		ImVec2 points[4];
		points[0] = ImVec2(center.x - size / 2.f, center.y - size / 2.f);
		points[1] = ImVec2(center.x + size / 2.f, center.y - size / 2.f);
		points[2] = ImVec2(center.x + size / 2.f, center.y + size / 2.f);
		points[3] = ImVec2(center.x - size / 2.f, center.y + size / 2.f);

		list->AddShadowConvexPoly(points, 4, ImColor(color.r, color.g, color.b, alpha), thickness, offset, flags);
	}

	static void drawRoundRect(Vector4<float> pos, const ImDrawFlags& flags, float rounding, const UIColor& color, float alpha, float lineWidth)
	{
		if (!ImGui::GetCurrentContext())
			return;
		const auto d = ImGui::GetBackgroundDrawList();
		d->AddRect(ImVec2(pos.x, pos.y), ImVec2(pos.z, pos.w), ImColor(color.r, color.g, color.b, alpha), rounding, flags, lineWidth);
	}

	static void fillGradientOpaqueRectangle(Vector4<float> pos, const UIColor& firstColor, const UIColor& secondColor, float firstAlpha, float secondAlpha)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		list->AddRectFilledMultiColor(topLeft, bottomRight,
			ImColor(firstColor.r, firstColor.g, firstColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha),
			ImColor(secondColor.r, secondColor.g, secondColor.b, firstAlpha),
			ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha));
	}

	static void fillRoundedGradientRectangle(Vector4<float> pos, const UIColor& firstColor, const UIColor& secondColor, float rounding = 0.f, float firstAlpha = 1.f, float secondAlpha = 1.f, ImDrawFlags flags = ImDrawFlags_None)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		int startBufferSize = list->VtxBuffer.Size;
		list->AddRectFilled(topLeft, bottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), rounding, flags);
		int endBufferSize = list->VtxBuffer.Size;
		list->AddRectFilled(topLeft, bottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), rounding, flags);
		int endBufferSize2 = list->VtxBuffer.Size;

		ImGui::ShadeVertsLinearColorGradientKeepAlpha(list, startBufferSize, endBufferSize, topLeft, bottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha));
		ImGui::ShadeVertsLinearColorGradientKeepAlpha(list, endBufferSize, endBufferSize2, topLeft, bottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha));
	}

	static void drawRoundedGradientRectangleOutline(Vector4<float> pos, const UIColor& firstColor, const UIColor& secondColor, float rounding = 0.f, float firstAlpha = 1.f, float secondAlpha = 1.f, float outlineThickness = 1.0f, const UIColor& outlineColor = UIColor{ 0, 0, 0, 1 })
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();

		ImVec2 topLeft = ImVec2(pos.x, pos.y);
		ImVec2 bottomRight = ImVec2(pos.z, pos.w);

		ImVec2 shrinkedTopLeft = ImVec2(pos.x + outlineThickness, pos.y + outlineThickness);
		ImVec2 shrinkedBottomRight = ImVec2(pos.z - outlineThickness, pos.w - outlineThickness);

		int startBufferSize = list->VtxBuffer.Size;
		list->AddRect(shrinkedTopLeft, shrinkedBottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), rounding, 0, outlineThickness);
		int endBufferSize = list->VtxBuffer.Size;

		ImGui::ShadeVertsLinearColorGradientKeepAlpha(list, startBufferSize, endBufferSize, topLeft, bottomRight, ImColor(firstColor.r, firstColor.g, firstColor.b, firstAlpha), ImColor(secondColor.r, secondColor.g, secondColor.b, secondAlpha));
	}

	static void drawCircleOutline(Vector2<float> center, float radius, const UIColor& color, float alpha, int segments, float thickness = 1.f)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		list->AddCircle(ImVec2(center.x, center.y), radius, ImColor(color.r, color.g, color.b, alpha), segments, thickness);
	}

	static void fillCircle(Vector2<float> center, float radius, const UIColor& color, float alpha, int segments)
	{
		if (!ImGui::GetCurrentContext())
			return;

		ImDrawList* list = ImGui::GetBackgroundDrawList();
		list->AddCircleFilled(ImVec2(center.x , center.y), radius, ImColor(color.r, color.g, color.b, alpha), segments);
	}

	static inline Vector2<float> getScreenSize() {
		RECT desktop;

		GetWindowRect(window, &desktop);
		int w = desktop.right - desktop.left;
		int h = desktop.bottom - desktop.top;
		// not in fullscreen
		if (desktop.top != 0 || desktop.left != 0) {
			//w -= 30;
			//h -= 45;
		}
		return Vector2<float>(w, h);
	}

	static bool isFullScreen() {
		RECT desktop;

		GetWindowRect(window, &desktop);
		if (desktop.top != 0 || desktop.left != 0)
			return false;
		return true;
	}

	static inline float getTextHeightStr(std::string* textStr, float textSize) {
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).y;
	}

	static inline float getTextWidth(std::string* textStr, float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, textStr->c_str()).x;
	}

	static inline float getTextHeight(float textSize)
	{
		return ImGui::GetFont()->CalcTextSizeA(textSize * 18, FLT_MAX, -1, "").y;
	}

	static inline float getDeltaTime() {
		return 0.016f;
	}

	static inline Vector2<float> getMousePos() {
		if (!Address::getClientInstance()->getGuiData()) {
			return { 0, 0 };
		}
		return Address::getClientInstance()->getGuiData()->getMousePos().toFloat();
	}

	inline static bool isMouseOver(Vector4<float>(pos))
	{
		Vector2<short> mousePos = Address::getClientInstance()->getGuiData()->getMousePos();
		return mousePos.x >= pos.x && mousePos.y >= pos.y && mousePos.x < pos.z && mousePos.y < pos.w;
	}

	static __forceinline void drawLine(Vector2<float> start, Vector2<float> end, UIColor color, float lineWidth) {
		if (!ImGui::GetCurrentContext()) return;
		const auto d = ImGui::GetBackgroundDrawList();
		d->AddLine(ImVec2(start.x, start.y), ImVec2(end.x, end.y), ImColor(color.r, color.g, color.b, color.a), lineWidth);
	}

	static Vector2<float> TestWorldToScreen(const Vector3<float>& world) {  //
		if (ImGui::GetCurrentContext()) {
			Vector2<float> ret;
			Vector2<float> windowSize = Vector2<float>(getScreenSize().x + (isFullScreen() ? 0 : 10), getScreenSize().y - (isFullScreen() ? -10 : 5));
			Address::getClientInstance()->WorldToScreen(world, ret);
			return ret;
		}
	}

	static void drawLine3D(const Vector3<float>& start, const Vector3<float>& end, UIColor color) {
		if (!ImGui::GetCurrentContext()) return;

		Vector2<float> startScreen = TestWorldToScreen({ start.x, start.y, start.z });
		Vector2<float> endScreen = TestWorldToScreen({ end.x, end.y, end.z });

		drawLine(startScreen, endScreen, color, 1.4);
	}

	static void drawRing3D(const Vector3<float>& center, float radius, int segments, float speed) {
		if (!ImGui::GetCurrentContext()) return;
		std::vector<Vector3<float>> points;
		for (int i = 0; i < segments; ++i) {
			float angle = static_cast<float>(i) / static_cast<float>(segments) * 2.0f * PI;
			float x = center.x + radius * std::cos(angle);
			float y = center.y;
			float z = center.z + radius * std::sin(angle);
			points.push_back(Vector3<float>(x, y, z));
		}

		int ind = 0;

		for (size_t i = 0; i < points.size() - 1; ++i) {
			//Vector2<float> Pos1;
			int colorIndex = ind * 7;
			Vector2 Pos1 = TestWorldToScreen(center);
			if (!Address::getClientInstance()->WorldToScreen(center, Pos1)) continue;
			drawLine3D(points[i], points[i + 1], ColorUtils::Rainbow(speed, 1.F, 1.F, colorIndex));
			++ind;
		}
		drawLine3D(points.back(), points.front(), ColorUtils::Rainbow(speed, 1.F, 1.F, ind * 7));
	}

	static __forceinline void drawBox(Vector3<float> lower, Vector3<float> upper, UIColor color, UIColor lineColor, float lineWidth, bool fill, bool outline) {
		Vector3<float> diff;
		diff.x = upper.x - lower.x;
		diff.y = upper.y - lower.y;
		diff.z = upper.z - lower.z;

		//Vector3<float> diff = upper.submissive(lower);
		Vector3<float> vertices[8];
		vertices[0] = Vector3<float>(lower.x, lower.y, lower.z);
		vertices[1] = Vector3<float>(lower.x + diff.x, lower.y, lower.z);
		vertices[2] = Vector3<float>(lower.x, lower.y + diff.y, lower.z);
		vertices[3] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z);
		vertices[4] = Vector3<float>(lower.x, lower.y, lower.z + diff.z);
		vertices[5] = Vector3<float>(lower.x + diff.x, lower.y, lower.z + diff.z);
		vertices[6] = Vector3<float>(lower.x, lower.y + diff.y, lower.z + diff.z);
		vertices[7] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z + diff.z);

		auto instance = Address::getClientInstance();

		const auto d = ImGui::GetBackgroundDrawList();

		if (fill) {
			// Convert the vertices to screen coordinates
			std::vector<Vector2<float>> screenCords;
			for (int i = 0; i < 8; i++) {
				Vector2<float> screen;
				if (instance->WorldToScreen(vertices[i], screen)) {
					screenCords.push_back(screen);
				}
			}

			// Return if there are less than four points to draw quads with
			if (screenCords.size() < 8) return;

			// Define the indices of the vertices to use for each quad face
			std::vector<std::tuple<int, int, int, int>> faces = {
				{0, 1, 3, 2},  // Bottom face
				{4, 5, 7, 6},  // Top face
				{0, 1, 5, 4},  // Front face
				{2, 3, 7, 6},  // Back face
				{1, 3, 7, 5},  // Right face
				{0, 2, 6, 4}   // Left face
			};

			// Draw the quads to fill the box
			for (auto face : faces) {
				ImVec2 posScreenCords = ImVec2(screenCords[std::get<0>(face)].x, screenCords[std::get<0>(face)].y);
				ImVec2 posScreenCords1 = ImVec2(screenCords[std::get<1>(face)].x, screenCords[std::get<1>(face)].y);
				ImVec2 posScreenCords2 = ImVec2(screenCords[std::get<2>(face)].x, screenCords[std::get<2>(face)].y);
				ImVec2 posScreenCords3 = ImVec2(screenCords[std::get<3>(face)].x, screenCords[std::get<3>(face)].y);
				//d->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(posScreenCords, posScreenCords1), ImVec2(posScreenCords2, posScreenCords3), ImVec2(0, 0), ImVec2(1, 1), HeadColor, 14.f);
				d->AddQuadFilled(posScreenCords, posScreenCords1, posScreenCords2, posScreenCords3, ImColor(color.r, color.g, color.b, color.a));
				//drawlist->AddQuadFilled(screenCords[std::get<3>(face)].toImVec2(), screenCords[std::get<2>(face)].toImVec2(), screenCords[std::get<1>(face)].toImVec2(), screenCords[std::get<0>(face)].toImVec2(), color.toImColor());
			}
		}

		{
			// Convert the vertices to screen coordinates
			std::vector<std::tuple<int, Vector2<float>>> screenCords;
			for (int i = 0; i < 8; i++) {
				Vector2<float> screen;
				if (instance->WorldToScreen(vertices[i], screen)) {
					screenCords.emplace_back(outline ? (int)screenCords.size() : i, screen);
				}
			}

			// Return if there are less than two points to draw lines between
			if (screenCords.size() < 2) return;

			switch (outline) {
			case false: {
				// Draw lines between all pairs of vertices
				for (auto it = screenCords.begin(); it != screenCords.end(); it++) {
					auto from = *it;
					auto fromOrig = vertices[std::get<0>(from)];

					for (auto to : screenCords) {
						auto toOrig = vertices[std::get<0>(to)];

						// Determine if the line should be drawn based on the relative positions of the vertices
						bool shouldDraw = false;
						// X direction
						shouldDraw |= fromOrig.y == toOrig.y && fromOrig.z == toOrig.z && fromOrig.x < toOrig.x;
						// Y direction
						shouldDraw |= fromOrig.x == toOrig.x && fromOrig.z == toOrig.z && fromOrig.y < toOrig.y;
						// Z direction
						shouldDraw |= fromOrig.x == toOrig.x && fromOrig.y == toOrig.y && fromOrig.z < toOrig.z;

						ImVec2 posForm = ImVec2(std::get<1>(from).x, std::get<1>(from).y);
						ImVec2 posTo = ImVec2(std::get<1>(to).x, std::get<1>(to).y);
						if (shouldDraw) d->AddLine(posForm, posTo, ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a), lineWidth);
					}
				}
				return;
				break;
			}
			case true: {
				// Find start vertex
				auto it = screenCords.begin();
				std::tuple<int, Vector2<float>> start = *it;
				it++;
				for (; it != screenCords.end(); it++) {
					auto cur = *it;
					if (std::get<1>(cur).x < std::get<1>(start).x) {
						start = cur;
					}
				}

				// Follow outer line
				std::vector<int> indices;

				auto current = start;
				indices.push_back(std::get<0>(current));
				Vector2<float> lastDir(0, -1);
				do {
					float smallestAngle = PI * 2;
					Vector2<float> smallestDir;
					std::tuple<int, Vector2<float>> smallestE;
					auto lastDirAtan2 = atan2(lastDir.y, lastDir.x);
					for (auto cur : screenCords) {
						if (std::get<0>(current) == std::get<0>(cur))
							continue;

						// angle between vecs
						Vector2<float> dir = Vector2<float>(std::get<1>(cur)).submissive(std::get<1>(current));
						float angle = atan2(dir.y, dir.x) - lastDirAtan2;
						if (angle > PI) {
							angle -= 2 * PI;
						}
						else if (angle <= -PI) {
							angle += 2 * PI;
						}
						if (angle >= 0 && angle < smallestAngle) {
							smallestAngle = angle;
							smallestDir = dir;
							smallestE = cur;
						}
					}
					indices.push_back(std::get<0>(smallestE));
					lastDir = smallestDir;
					current = smallestE;
				} while (std::get<0>(current) != std::get<0>(start) && indices.size() < 8);

				// draw

				Vector2<float> lastVertex;
				bool hasLastVertex = false;
				for (auto& indice : indices) {
					Vector2<float> curVertex = std::get<1>(screenCords[indice]);
					if (!hasLastVertex) {
						hasLastVertex = true;
						lastVertex = curVertex;
						continue;
					}
					ImVec2 lastVertexPos = ImVec2(lastVertex.x, lastVertex.y);
					ImVec2 curVertexPos = ImVec2(curVertex.x, curVertex.y);
					d->AddLine(lastVertexPos, curVertexPos, ImColor(lineColor.r, lineColor.g, lineColor.b, lineColor.a), lineWidth);
					lastVertex = curVertex;
				}
				return;
				break;
			}
			}
		}
	}

	static __forceinline void drawCorners(Vector3<float> lower, Vector3<float> upper, UIColor color, float lineWidth) {
		if (Address::getLocalPlayer() == nullptr) return;
		Vector3<float> worldPoints[8];
		worldPoints[0] = Vector3<float>(lower.x, lower.y, lower.z);
		worldPoints[1] = Vector3<float>(lower.x, lower.y, upper.z);
		worldPoints[2] = Vector3<float>(upper.x, lower.y, lower.z);
		worldPoints[3] = Vector3<float>(upper.x, lower.y, upper.z);
		worldPoints[4] = Vector3<float>(lower.x, upper.y, lower.z);
		worldPoints[5] = Vector3<float>(lower.x, upper.y, upper.z);
		worldPoints[6] = Vector3<float>(upper.x, upper.y, lower.z);
		worldPoints[7] = Vector3<float>(upper.x, upper.y, upper.z);

		std::vector<Vector2<float>> points;
		for (int i = 0; i < 8; i++) {
			Vector2<float> result;
			if (Address::getClientInstance()->WorldToScreen(worldPoints[i], result))
				points.emplace_back(result);
		}
		if (points.size() < 1) return;

		Vector4<float> resultRect = { points[0].x, points[0].y, points[0].x, points[0].y };
		for (const auto& point : points) {
			if (point.x < resultRect.x) resultRect.x = point.x;
			if (point.y < resultRect.y) resultRect.y = point.y;
			if (point.x > resultRect.z) resultRect.z = point.x;
			if (point.y > resultRect.w) resultRect.w = point.y;
		}

		float length = (resultRect.x - resultRect.z) / 4.f;

		// Top left
		drawLine(Vector2(resultRect.x, resultRect.y), Vector2(resultRect.x - length, resultRect.y), color, lineWidth);
		drawLine(Vector2(resultRect.x, resultRect.y), Vector2(resultRect.x, resultRect.y - length), color, lineWidth);

		// Top right
		drawLine(Vector2(resultRect.z, resultRect.y), Vector2(resultRect.z + length, resultRect.y), color, lineWidth);
		drawLine(Vector2(resultRect.z, resultRect.y), Vector2(resultRect.z, resultRect.y - length), color, lineWidth);

		// Bottom left
		drawLine(Vector2(resultRect.x, resultRect.w), Vector2(resultRect.x - length, resultRect.w), color, lineWidth);
		drawLine(Vector2(resultRect.x, resultRect.w), Vector2(resultRect.x, resultRect.w + length), color, lineWidth);

		// Bottom right
		drawLine(Vector2(resultRect.z, resultRect.w), Vector2(resultRect.z + length, resultRect.w), color, lineWidth);
		drawLine(Vector2(resultRect.z, resultRect.w), Vector2(resultRect.z, resultRect.w + length), color, lineWidth);
	}

	static void drawCircle3D(Vector3<float> const& center, float size, float thickness, UIColor const& color, float resolution) {
		Vector2<float> previousScreenPos;
		for (float angle = 0.0f; angle <= 360; angle += resolution) {
			float radians = angle * PI / 180;
			float x = center.x + std::sinf(radians) * size;
			float z = center.z + std::cosf(radians) * size;
			Vector3<float> worldPos = Vector3<float>(x, center.y, z);

			Vector2<float> screenPos;
			Address::getClientInstance()->WorldToScreen(worldPos, screenPos);

			drawLine(Vector2(previousScreenPos.x, previousScreenPos.y), screenPos, color, thickness);
			previousScreenPos = screenPos;
		}
	}
};

class ImScaleUtil {
public:
	ImScaleUtil() {
		scale_start_index = 0.f;
	}

	static void ImScaleStart()
	{
		scale_start_index = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
	}

	static inline int scale_start_index;

	static ImVec2 ImScaleCenter()
	{
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

		const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
		for (int i = scale_start_index; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	static void ImScaleEnd(float scaleX, float scaleY, ImVec2 center = ImScaleCenter())
	{
		auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;

		for (int i = scale_start_index; i < buf.Size; i++)
		{
			ImVec2 pos = ImVec2(buf[i].pos.x - center.x, buf[i].pos.y - center.y);
			pos.x *= scaleX;
			pos.y *= scaleY;
			buf[i].pos = ImVec2(pos.x + center.x, pos.y + center.y);
		}
	}
};

class ImRotateUtil {
public:
	ImRotateUtil() {
		rotationStartIndex = 0.f;
	}

	static void startRotation() {
		rotationStartIndex = ImGui::GetBackgroundDrawList()->VtxBuffer.Size;
	}

	static ImVec2 getRotationCenter() {
		ImVec2 l(FLT_MAX, FLT_MAX), u(-FLT_MAX, -FLT_MAX);

		const auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;
		for (int i = rotationStartIndex; i < buf.Size; i++)
			l = ImMin(l, buf[i].pos), u = ImMax(u, buf[i].pos);

		return ImVec2((l.x + u.x) / 2, (l.y + u.y) / 2);
	}

	static void endRotation(float rad, ImVec2 center = getRotationCenter()) {
		rad += PI * 0.5f;
		float s = sin(rad), c = cos(rad);
		center = ImVec2(ImRotate(center, s, c).x - center.x, ImRotate(center, s, c).y - center.y);

		auto& buf = ImGui::GetBackgroundDrawList()->VtxBuffer;

		for (int i = rotationStartIndex; i < buf.Size; i++)
			buf[i].pos = ImVec2(ImRotate(buf[i].pos, s, c).x - center.x, ImRotate(buf[i].pos, s, c).y - center.y);
	}

private:
	inline static int rotationStartIndex = 0;
};

class ImClippingUtil {
public:
	static void beginClipping(Vector4<float> rect) {
		ImGui::GetBackgroundDrawList()->PushClipRect(ImVec2(rect.x, rect.y), ImVec2(rect.z, rect.w), true);
	}

	static void restoreClipping() {
		ImGui::GetBackgroundDrawList()->PopClipRect();
	}
};

class ParticleEngine
{
public:
	struct Particle {
		float x, y, vx, vy, lifetime, opacity, size, distance, delta;
		UIColor color;

		Particle() : x(0), y(0), vx(0), vy(0), lifetime(0), opacity(0), color(UIColor(0, 0, 0)), size(0), distance(0), delta(0) {}
		Particle(float x, float y, float vx, float vy, float lifetime, float opacity, UIColor color, float size, float distance, float delta)
			: x(x), y(y), vx(vx), vy(vy), lifetime(lifetime), opacity(opacity), color(color), size(size), distance(distance), delta(delta) {}
	};

	std::vector<Particle> particles;

	void addParticles(int numParticles, float startX, float startY, float distance, float size, float seconds = 4.f)
	{
		for (int i = 0; i < numParticles; i++)
		{
			Particle p;
			p.x = startX + rand() % 10 - 5;
			p.y = startY + rand() % 10 - 5;
			p.vx = rand() % 200 - 100 + rand() % 10 - 5;
			p.vy = rand() % 200 - 100 + rand() % 10 - 5;
			p.lifetime = seconds;
			p.opacity = 1.0f;
			p.color = ColorUtils::Rainbow(1, 1, 1, p.x * 45);
			p.size = size + rand() % ((int)size / 2 + 1);
			p.distance = distance;
			particles.push_back(p);
		}
	}

	void clearParticles() {
		particles.clear();
	}

	void updateParticles(float delta) {
		for (auto& p : particles) {
			EasingUtil easing;
			easing.percentage = p.lifetime / p.distance;
			float easedPercentage = easing.easeOutElastic();

			float targetX = Math::lerp(p.x, p.x + p.vx * delta, easedPercentage);
			float targetY = Math::lerp(p.y, p.y + p.vy * delta, easedPercentage);

			p.x = Math::lerp(p.x, targetX, easedPercentage);
			p.y = Math::lerp(p.y, targetY, easedPercentage);

			p.lifetime -= delta;
			p.opacity -= delta * 0.3f;
		}

		particles.erase(std::remove_if(particles.begin(), particles.end(),
			[](const Particle& p) { return p.lifetime <= 0; }), particles.end());

		if (particles.size() > 250) {
			particles.resize(250);
		}
	}

	void drawParticles() {
		for (const auto& p : particles) {
			Vector2<float> center(p.x + p.size * 0.5f, p.y + p.size * 0.5f);
			float radius = p.size * 0.5f;
			int segments = 100;

			ImRenderUtil::fillCircle(center, radius, p.color, p.opacity, segments);
		}
	}
};