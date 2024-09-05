#pragma once

class Trails : public Module
{
public:
	Trails(int keybind = 7, bool enabled = true) :
		Module("Trail", "Visual", "Draw Trail", keybind, enabled)
	{
		addSlider("Time", "The duration for rendering trails", &trailDuration, 1, 15);
		addSlider("Delay", "The delay between rendering trails", &trailDelay, 1, 30);
	}

	int delayCount = 0;
	float trailDuration = 1;
	float trailDelay = 3;

	struct CircleStatus {
		Vector3<float> pos;
		int duration;
	};
	long long timeOffset = 0;
	std::vector<CircleStatus> Circles;

	Vector3<float> oldPos;

	void onEvent(RenderContextEvent* event) {
		delayCount--;

		if (timeOffset == 0) 
			timeOffset = TimeUtils::getCurrentMs();

		auto localPlayer = Address::getLocalPlayer();
		if (localPlayer == nullptr) return;
		int time = TimeUtils::getCurrentMs() - timeOffset;
		auto pos = localPlayer->getStateVector()->Position;
		pos.y = localPlayer->getStateVector()->Position.floor().y - 1.5f;
		if (delayCount <= 0) {
			CircleStatus status = {};

			status.duration = time;

			status.pos = Address::getLocalPlayer()->getRenderPosition();
			status.pos.y -= Address::getLocalPlayer()->getAABBShape()->Hitbox.y;

			if (MovementUtils::isMoving()) {
				Circles.push_back(status);
			}
			
			delayCount = trailDelay;
		}

		for (auto it = Circles.begin(); it != Circles.end(); ) {
			if ((time - it->duration) > (trailDuration * 1000)) {
				it = Circles.erase(it);
			}
			else {
				++it;
			}
		}
	}

	void onEvent(ImGuiRenderEvent* event) override
	{
		if (!Address::canUseKeys()) return;
		int time = TimeUtils::getCurrentMs() - timeOffset;
		for (auto trail : Circles) {
			int time2 = time - trail.duration;
			int time3 = (trailDuration * 1000) - time2;
			Vector2<float> screen, screen2, screen3;
			if (!Address::getClientInstance()->WorldToScreen(trail.pos, screen)) continue;
			if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(trail.pos.x - 0.1f, trail.pos.y - 0.1f, trail.pos.z - 0.1f), screen2)) continue;
			if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(trail.pos.x + 0.1f, trail.pos.y + 0.1f, trail.pos.z + 0.1f), screen2)) continue;

			float circleSize = std::fmin(std::fmax(abs(screen.x - screen2.x), abs(screen.y - screen2.y)), 8);
			float opacity = time3 < 300 ? time3 / 300.f : 1;
			circleSize = circleSize * (time2 < 100 ? time2 / 100.f : 1) * opacity;

			ImRenderUtil::fillCircle(screen, circleSize, UIColor(255, 255, 255), opacity, 12.f);
			if (circleSize <= 2) continue;
			ImRenderUtil::fillShadowCircle(screen, circleSize, UIColor(255, 255, 255), opacity, 40.f, 0);
		}
	}

	void onEnabled() override {
		timeOffset = TimeUtils::getCurrentMs();
	}

	void onDisabled() override {
		Circles.clear();
	}
};