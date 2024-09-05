#pragma once

class Math
{
public:
	inline static float animate(float endPoint, float current, float speed) { // Animate the position
		if (speed < 0.0) speed = 0.0; // If 0 is less than speed then set speed to 0.
		else if (speed > 1.0) speed = 1.0; // If Speed is faster than 1 then set speed to 1.
		
		float dif = std::fmax(endPoint, current) - std::fmin(endPoint, current); // Animate between max and min.
		float factor = dif * speed;
		return current + (endPoint > current ? factor : -factor); // Animates the distance
	}

	inline static float lerp(float a, float b, float c) { // Lerp the position
		return a + c * (b - a); // Make the lerp smooth
	}

	inline static int lerpInt(int a1, int a2, double a3) {
		return (int)lerp((float)a1, (float)a2, (float)a3);
	}

	__forceinline static float wrap(float value, float min, float max) {
		return fmod(fmod(value - min, max - min) + (max - min), max - min) + min;
	}

	static inline float clamp(float value, float min_val, float max_val) { // Clamp the position
		return value < min_val ? min_val : (value > max_val ? max_val : value);
	}

	static inline float round(float num, float increment) {
		if (increment == 0.0f) {
			return num;
		} else if (increment == 1.0f) {
			return std::round(num);
		} else {
			float halfOfInc = increment / 2.0f;
			float floored = std::floorf(num / increment) * increment;
			if (num >= floored + halfOfInc) {
				return std::ceilf(num / increment) * increment;
			} else {
				return floored;
			}
		}
	}

	static int getDecimalPlaces(float value) {
		if (value == 0.0f) {
			return 0;
		}

		std::ostringstream oss;
		oss << std::fixed << std::setprecision(5) << value;
		std::string strValue = oss.str();

		size_t pointPos = strValue.find('.');

		if (pointPos == std::string::npos) {
			return 0;
		}

		std::string decimalPart = strValue.substr(pointPos + 1);

		decimalPart.erase(decimalPart.find_last_not_of('0') + 1);

		return decimalPart.length();
	}
};