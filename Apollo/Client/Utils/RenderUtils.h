#pragma once
#include "../../Base/SDK/Classes/Render/MinecraftUIRenderContext.h"
#include "../../Base/SDK/Classes/Render/ScreenContext.h"

enum Type
{
	Internal = 0, // UserPackage
	External = 1, // Raw
};

class RenderUtils
{
public:
	static inline MinecraftUIRenderContext* renderContext = nullptr;
	static inline MaterialPtr* uiMaterial = nullptr;
	static inline Tessellator* tess = nullptr;
	static inline MaterialPtr* blendMaterial = nullptr;
	static inline float* colorHolder;

	static void Initialize(MinecraftUIRenderContext* RenderCtx) {
		static bool inited = false;
		if (inited) return;

		renderContext = RenderCtx;
		tess = renderContext->ScreenContext->getTessellator();
		colorHolder = renderContext->ScreenContext->getColorHolder();
		uiMaterial = MaterialPtr::createMaterial(HashedString("ui_textured_and_glcolor"));
		blendMaterial = MaterialPtr::createMaterial(HashedString("fullscreen_cube_overlay_blend"));

		if (renderContext != nullptr) 
			inited = true;
	};

	inline static void fillRectangle(Vector4<float> pos, UIColor col, float alpha) {
		renderContext->fillRectangle(Vector4<float>(pos.x, pos.z, pos.y, pos.w), col, alpha);
	}

	inline static void drawRectangle(Vector4<float> pos, UIColor col, float alpha, float width) {
		renderContext->drawRectangle(Vector4<float>(pos.x, pos.z, pos.y, pos.w), col, alpha, width);
	}

	static void drawText(FontRepos* font, Vector2<float> pos, std::string* str, UIColor color, float size, float alpha, bool shadow)
	{
		static CaretMeasureData black = CaretMeasureData(20, false);

		float tPos[4] = { pos.x, pos.x + 1000, pos.y, pos.y + 1000 };
		TextMeasureData data = TextMeasureData(size, shadow);

		renderContext->drawText(font, tPos, str, color.arr, alpha, 0, &data, &black);
	}

	static void renderImage(std::string filePath, Vector2<float> ImagePos, Vector2<float> ImageDimension, Type type = External)
	{
		TexturePtr* texturePtr = new TexturePtr();
		renderContext->getTexture(texturePtr, new ResourceLocation(type, filePath));
		renderContext->drawImage(texturePtr, &ImagePos, &ImageDimension, Vector2<float>(0.f, 0.f), Vector2<float>(1.f, 1.f));
	}

	static void flushImage(UIColor color = UIColor(255, 255, 255), float alpha = 1.f)
	{
		static StringHasher flushString = StringHasher(0xA99285D21E94FC80, "ui_flush");
		renderContext->flushImages(color, alpha, flushString);
	}

	static __forceinline void setColor(float r, float g, float b, float a) {
		colorHolder[0] = r;
		colorHolder[1] = g;
		colorHolder[2] = b;
		colorHolder[3] = a;
		*reinterpret_cast<uint8_t*>(colorHolder + 4) = 1;
	}

	static void drawLine(Vector2<float> start, Vector2<float> end, UIColor color, float lineWidth) {
		float modX = 0 - (start.y - end.y);
		float modY = start.x - end.x;

		float len = sqrtf(modX * modX + modY * modY);

		modX /= len;
		modY /= len;
		modX *= lineWidth;
		modY *= lineWidth;

		setColor(color.r, color.g, color.g, 1);

		tess->begin(VertextFormat::TRIANGLE_STRIP, 6);

		tess->vertex(start.x + modX, start.y + modY, 0);
		tess->vertex(start.x - modX, start.y - modY, 0);
		tess->vertex(end.x - modX, end.y - modY, 0);

		tess->vertex(start.x + modX, start.y + modY, 0);
		tess->vertex(end.x + modX, end.y + modY, 0);
		tess->vertex(end.x - modX, end.y - modY, 0);

		tess->renderMeshImmediately(renderContext->ScreenContext, uiMaterial);
	}

	static void drawCorners(Vector3<float> lower, Vector3<float> upper, UIColor color, float lineWidth) {
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
			if (renderContext->ClientInstance->WorldToScreen(worldPoints[i], result))
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

	static __forceinline void drawBox3d(Vector3<float> lower, Vector3<float> upper, float scale, bool onUi) {
		Tessellator* myTess = tess;
		auto instance = renderContext->ClientInstance;
		auto origin = instance->getLevelRender()->getOrigin();

		Vector3<float> diff;
		diff.x = upper.x - lower.x;
		diff.y = upper.y - lower.y;
		diff.z = upper.z - lower.z;

		Vector3<float> vertices[8];
		vertices[0] = Vector3<float>(lower.x, lower.y, lower.z);
		vertices[1] = Vector3<float>(lower.x + diff.x, lower.y, lower.z);
		vertices[2] = Vector3<float>(lower.x, lower.y + diff.y, lower.z);
		vertices[3] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z);
		vertices[4] = Vector3<float>(lower.x, lower.y, lower.z + diff.z);
		vertices[5] = Vector3<float>(lower.x + diff.x, lower.y, lower.z + diff.z);
		vertices[6] = Vector3<float>(lower.x, lower.y + diff.y, lower.z + diff.z);
		vertices[7] = Vector3<float>(lower.x + diff.x, lower.y + diff.y, lower.z + diff.z);

		glm::mat4 rotationMatrix = glm::rotate(glm::mat4(scale), 0.f, glm::vec3(1.0f, 1.0f, 1.0f));
		Vector3<float> newLowerReal = lower.add(Vector3<float>(0.5f, 0.5f, 0.5f));  // .add(0.4375f, 0.4375f, 0.4375f) is chest
		for (int i = 0; i < 8; i++) {
			glm::vec4 rotatedVertex = rotationMatrix * glm::vec4(vertices[i].x - newLowerReal.x, vertices[i].y - newLowerReal.y, vertices[i].z - newLowerReal.z, 0.0f);
			vertices[i] = Vector3<float>{ rotatedVertex.x + newLowerReal.x, rotatedVertex.y + newLowerReal.y, rotatedVertex.z + newLowerReal.z };
		}

#define line(m, n)                      \
		myTess->vertex(m.x, m.y, m.z); \
		myTess->vertex(n.x, n.y, n.z);

		// Top square
		line(vertices[4], vertices[5]);
		line(vertices[5], vertices[7]);
		line(vertices[7], vertices[6]);
		line(vertices[6], vertices[4]);

		// Bottom Square
		line(vertices[0], vertices[1]);
		line(vertices[1], vertices[3]);
		line(vertices[3], vertices[2]);
		line(vertices[2], vertices[0]);

		// Sides
		line(vertices[0], vertices[4]);
		line(vertices[1], vertices[5]);
		line(vertices[2], vertices[6]);
		line(vertices[3], vertices[7]);
#undef line

		myTess->renderMeshImmediately(renderContext->ScreenContext, onUi ? uiMaterial : blendMaterial);
	}
};