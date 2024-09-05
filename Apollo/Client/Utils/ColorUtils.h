#pragma once

class ColorUtils
{
public:
	inline static UIColor lerpColor(UIColor color1, UIColor color2, float amount)
	{
		amount = std::min(1.f, std::max(0.f, amount));
		return UIColor(Math::lerpInt(color1.r * 255, color2.r * 255, amount), Math::lerpInt(color1.g * 255, color2.g * 255, amount), Math::lerpInt(color1.b * 255, color2.b * 255, amount), Math::lerpInt(color1.a * 255, color2.a * 255, amount));
	}

	inline static UIColor Rainbow(float seconds, float saturation, float brightness, int index) {
		float hue = ((TimeUtils::getCurrentMs() + index) % (int)(seconds * 1000)) / (float)(seconds * 1000);
		float r, g, b = 0;
		hsvToRgb(hue, saturation, brightness, r, g, b);
		return UIColor(r * 255, g * 255, b * 255);
	}

	inline static UIColor RainbowDark(float speed, int index) {
		float hue = ((TimeUtils::getCurrentMs() + index) % (int)(speed * 1000)) / (float)(speed * 1000);
		float r, g, b = 0;
		hsvToRgb(hue, 1.F, 1.F, r, g, b);
		return UIColor(r * 180, g * 180, b * 190);
	}

	inline static UIColor GradientColor(float speed, int red, int green, int blue, int red2, int green2, int blue2, long index) {
		double offset = ((TimeUtils::getCurrentMs() - index) / 8) / (double)120;

		double primaryHue = ((TimeUtils::getCurrentMs() - index) % 1000 / 1000.000);

		int secoundaryHue = ((TimeUtils::getCurrentMs() - index) % 2000 / 2000.000) * 2;

		primaryHue = secoundaryHue % 2 == 0 ? primaryHue : 1 - primaryHue;
		double inverse_percent = 1 - primaryHue;
		int redPart = (int)(red * inverse_percent + red2 * primaryHue);
		int greenPart = (int)(green * inverse_percent + green2 * primaryHue);
		int bluePart = (int)(blue * inverse_percent + blue2 * primaryHue);

		return UIColor(redPart, greenPart, bluePart);
	}

	inline static UIColor getCustomColors(float seconds, int index, const std::vector<std::pair<float, UIColor>>& colorMap)
	{
		float hue = ((TimeUtils::getCurrentMs() + index) % (int)(seconds * 1000)) / (float)(seconds * 1000);

		UIColor currentColor = UIColor(0, 0, 0);
		for (int i = 0; i < colorMap.size() - 1; i++)
		{
			if (hue >= colorMap[i].first && hue <= colorMap[i + 1].first)
			{
				float amount = (hue - colorMap[i].first) / (colorMap[i + 1].first - colorMap[i].first);
				currentColor = lerpColor(colorMap[i].second, colorMap[i + 1].second, amount);
				break;
			}
		}
		return UIColor(currentColor.r * 255, currentColor.g * 255, currentColor.b * 255);
	}

	inline static UIColor getAstolfoRainbow(float seconds, float saturation, float brightness, int index)
	{
		float hue = 0;
		float speed = seconds * 1450.f;
		float tempHue = (float)((TimeUtils::getCurrentMs() + index) % (int)speed);
		while (tempHue > speed)
			tempHue -= speed;
		tempHue /= speed;
		if (tempHue > 0.5)
			tempHue = 0.5F - (tempHue - 0.5f);
		tempHue += 0.5F;
		hue = tempHue;


		float r, g, b = 0;
		hsvToRgb(hue, saturation, brightness, r, g, b);
		return UIColor(r * 255, g * 255, b * 255);
	}

	inline static UIColor getClientColor(float seconds, float saturation, float brightness, int index) {
		std::vector<std::pair<float, UIColor>> colorMap;

		switch (Global::ClientColor)
		{
		case 0: // Blue and Green
			colorMap =
			{
				{0.0f, UIColor(65, 201, 163)},
				{0.5f, UIColor(0, 13, 175)},
				{1.0f, UIColor(65, 201, 163)}
			};
			return getCustomColors(seconds, index * 2, colorMap);
			break;
		case 1: // Astolofo
			return getAstolfoRainbow(seconds, saturation * 0.5f, brightness, index * 2);
			break;
		case 2: // Bubble Gum
			colorMap =
			{
				{0.0f, UIColor(255, 192, 203)},
				{0.2f, UIColor(255, 153, 204)},
				{0.4f, UIColor(255, 102, 178)},
				{0.6f, UIColor(255, 51, 180)},
				{0.8f, UIColor(255, 0, 128)},
				{1.0f, UIColor(255, 192, 203)}
			};
			return getCustomColors(seconds, index * 2, colorMap);
			break;
		case 3: // Rainbow
			return Rainbow(seconds, saturation, brightness, index * 2);
			break;
		case 4: // Tenacity
		{
			colorMap =
			{
				{0.0f, UIColor(236.f, 133.f, 209.f)},
				{0.5f, UIColor(28.f, 167.f, 222.f)},
				{1.0f, UIColor(236.f, 133.f, 209.f)}
			};
			return getCustomColors(seconds, index * 2, colorMap);
			break;
		case 5: // Wave
			colorMap =
			{
				{0.0f, UIColor(33, 33, 33)},
				{0.5f, UIColor(255, 255, 255)},
				{1.0f, UIColor(33, 33, 33)}
			};
			return getCustomColors(seconds, index * 2, colorMap);
			break;
		case 6: // Cherry
			colorMap =
			{
				{0.0f, UIColor(139, 0, 0)},
				{0.25f, UIColor(205, 92, 92)},
				{0.5f, UIColor(220, 20, 60)},
				{0.75f, UIColor(255, 36, 0)},
				{1.0f, UIColor(139, 0, 0)},
			};
			return getCustomColors(seconds, index * 2, colorMap);
			break;
		case 7: // Xextreame
			colorMap =
			{
				{0.0f, UIColor(255, 255, 255)},
				{0.5f, UIColor(226, 13, 15)},
				{1.0f, UIColor(255, 255, 255)}
			};
			return getCustomColors(seconds, index, colorMap);
			break;
		case 8: // White
			return UIColor(255, 255, 255);
			break;
		}
		};
	}
private:
	inline static void hsvToRgb(float h, float s, float v, float& rOut, float& gOut, float& bOut)
	{

		if (s == 0.0f)
		{
			// gray
			rOut = gOut = bOut = v;
			return;
		}

		h = fmodf(h, 1.0f) / (60.0f / 360.0f);
		int i = (int)h;
		float f = h - (float)i;
		float p = v * (1.0f - s);
		float q = v * (1.0f - s * f);
		float t = v * (1.0f - s * (1.0f - f));

		switch (i)
		{
		case 0:
			rOut = v;
			gOut = t;
			bOut = p;
			break;
		case 1:
			rOut = q;
			gOut = v;
			bOut = p;
			break;
		case 2:
			rOut = p;
			gOut = v;
			bOut = t;
			break;
		case 3:
			rOut = p;
			gOut = q;
			bOut = v;
			break;
		case 4:
			rOut = t;
			gOut = p;
			bOut = v;
			break;
		case 5:
		default:
			rOut = v;
			gOut = p;
			bOut = q;
			break;
		}
	}
};
