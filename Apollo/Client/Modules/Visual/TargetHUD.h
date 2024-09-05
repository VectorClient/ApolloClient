#pragma once
#include <vector>
#include <regex>

class TargetHUD : public Module {
public:
	TargetHUD(int keybind = Keys::NONE, bool enabled = false) 
		: Module("TargetHUD", "Visual", "Display Target", keybind, enabled) 
	{
		addEnum("Style", "The style of the targethud", { "Apollo" }, &Style);
		addBool("Track3D", "Changes the targethud position to the targets", &track3d);
		addBool("Track Ease", "Changes the targethud position to the targets", &ease);
		addBool("Blur", "Add blur on targethud", &blur);
		//addBool("Heart Check", "Calcultes Absorption / HP for hive and other servers that have health checks", &showCalculatedHearts);
		addSlider("Rounding", "The rounding of the targethud", &rounding, 1, 30);
	}

private:
	int Style = 0;
	bool track3d = false;
	bool ease = false;
	bool blur = true;
	float rounding = 5;

	bool showCalculatedHearts = true;
	Vector3<float> targetPos;

	//static inline float targetHealth = 20.f;
	int targetIndex = 0;
public:

	std::vector<Actor*> targetList;
	void CreateOurTargetList() {
		auto instance = Address::getClientInstance();
		auto localPlayer = instance->getLocalPlayer();
		auto list = localPlayer->getLevel()->getRuntimeActorList();
		auto lpPos = localPlayer->getStateVector()->Position;
		for (Actor* actor : list) {
			if (actor != localPlayer && actor->isAlive() && !actor->isBot() && actor->isPlayer()) {
				float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
				if (dist <= 7) {
					if (getModuleByName("teams")->isEnabled()) {
						if (!actor->isTeammate(localPlayer)) {
							targetList.push_back(actor);
						}
					}
					else {
						targetList.push_back(actor);
					}
				}
			}
		}
	}

	void onEvent(ImGuiRenderEvent* event) {
		if (getModuleByName("clickgui")->isEnabled()) return;

		auto player = Address::getLocalPlayer();
		if (!player)
		{
			return;
		}

		if (player->getAttribute(AttributeId::Health) == nullptr) {
			return;
		}

		//sort based on distance
		targetList.clear();
		CreateOurTargetList();

		static EasingUtil inEase;

		bool shouldDisplay = !targetList.empty() && Address::canUseKeys();

		(shouldDisplay) ? inEase.incrementPercentage(ImRenderUtil::getDeltaTime() * 15.f / 10)
			: inEase.decrementPercentage(ImRenderUtil::getDeltaTime() * 2 * 15.f / 10);

		float inScale = shouldDisplay ? inEase.easeOutElastic() : inEase.easeOutBack();

		if (inEase.isPercentageMax()) 
			inScale = 1;

		ImScaleUtil::ImScaleStart();
		static Vector2<float> pos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2 + 10, ImRenderUtil::getScreenSize().y / 2 + 10);
		static Vector4<float> rect = Vector4<float>(pos.x, pos.y, pos.x + 250, pos.y + 80);

		static std::string targetName;
		static std::string targetHealthStr;
		static float targetHealth = 0.f;
		static float targetAbsorption = 0;
		static bool calculateHealth = false;
		static float textSize = 1.f;
		int damageTime = 0;

		if (!targetList.empty()) {
			if (TimeUtils::hasTimeElapsed("TargetHudIndex", 300, true)) {
				targetIndex++;
			}
			
			if (targetIndex >= targetList.size())
				targetIndex = 0;

			auto target = targetList[targetIndex];

			damageTime = target->getHurtTime();

			std::string Name = target->getNametag()->c_str();
			Name = Utils::sanitize(Name);
			Name = Name.substr(0, Name.find('\n'));

			if (targetName != Name) {
				targetName = Name;
			}

			if (track3d) {
				if (!Address::canUseKeys())
					return;

				Vector2<float> output1;
				if (!Address::getClientInstance()->WorldToScreen(target->getRenderPosition(), output1)) return;

				static float outputX = output1.x;
				static float outputY = output1.y;

				outputX = Math::animate(output1.x, outputX, ImRenderUtil::getDeltaTime() * 10.f);
				outputY = Math::animate(output1.y, outputY, ImRenderUtil::getDeltaTime() * 10.f);

				Vector2<float> outputEase = Vector2<float>(outputX, outputY);

				float distance = target->getRenderPosition().distance(player->getRenderPosition());

				textSize = fmax(1.f, 0.6f / distance);

				if (ease) {
					pos = outputEase;
					rect = Vector4<float>(outputEase.x, outputEase.y, outputEase.x + 250, outputEase.y + 80);
				}
				else {
					pos = output1;
					rect = Vector4<float>(output1.x, output1.y, output1.x + 250, output1.y + 80);
				}
			}
			else {
				pos = Vector2<float>(ImRenderUtil::getScreenSize().x / 2 + 10, ImRenderUtil::getScreenSize().y / 2 + 10);
				rect = Vector4<float>(pos.x, pos.y, pos.x + (250 * textSize), pos.y + (80 * textSize));
				textSize = 1.f;
			}

			if (target->getHealth() == 20.f) {
				calculateHealth = true;
				targetHealth = target->getHealth(); // Later
			}
			else {
				calculateHealth = false;
				targetHealth = target->getHealth();
			}

			targetAbsorption = target->getAbsorption();

			std::ostringstream oss;
			std::ostringstream ossAbsor;
			oss << std::fixed << std::setprecision(0) << (targetHealth);
			ossAbsor << std::fixed << std::setprecision(1) << (targetAbsorption);
			targetHealthStr = oss.str() + " HP";

			if (targetAbsorption > 0.5) {
				targetHealthStr = oss.str() + " HP" + " " + ossAbsor.str() + " HP+";
			}

			if (target == nullptr) 
				return;
		}
		else {
			targetIndex = 0;
		}

		if (inScale == 1) { 
			if(blur) ImRenderUtil::Blur(rect, 5, 14.f);
		}
		
		ImRenderUtil::fillRectangle(rect, UIColor(0, 0, 0), 0.6f, 14.f);
		ImRenderUtil::fillShadowRectangle(rect, UIColor(0, 0, 0), 0.8f, 50.f, 0, 14.f);

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
		ImRenderUtil::drawText(Vector2<float>(pos.x + 75, pos.y + 23), &targetName, UIColor(255, 255, 255), 1.2f * textSize, 1.f, true);
		ImGui::PopFont();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
		ImRenderUtil::drawText(Vector2<float>(pos.x + 75, pos.y + 43), &targetHealthStr, UIColor(170, 170, 170), 1.f * textSize, 1.f, true);
		ImGui::PopFont();

		static float DamageAnimation = damageTime * 1.5; //* 2
		DamageAnimation = Math::animate(damageTime * 1.5, DamageAnimation, ImRenderUtil::getDeltaTime() * 30.f);

		Vector4<float> HeadPos = Vector4<float>((rect.x + 8) + DamageAnimation, (rect.y + 12) + DamageAnimation, rect.x + (65 * textSize) - DamageAnimation, (rect.y) + (69 * textSize) - DamageAnimation);
		ImColor HeadColor = IM_COL32_WHITE;

		static ParticleEngine particleMgr;

		if (damageTime >= 1) {
			UIColor UIHeadColor = UIColor(255, 114, 118); //255, 114, 118
			HeadColor = ImColor(UIHeadColor.r, UIHeadColor.g, UIHeadColor.b, UIHeadColor.a);
			ImRenderUtil::fillShadowRectangle(HeadPos, UIHeadColor, 1 * (DamageAnimation / 10), 40.f * (DamageAnimation / 10), 0, 14.f);
		}

		ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), HeadColor, 14.f);
		ImScaleUtil::ImScaleEnd(inScale, inScale, ImVec2(pos.x + 125, pos.y + 40));
		/*for (Actor* TargetPlayerList : AvailableTargetList) {
			if (TargetPlayerList != player && TargetPlayerList->isAlive() && !TargetPlayerList->isBot()) { //  && !actor->isBot()
				Vector3<float> targetPos = AvailableTargetList[0]->getStateVector()->Position;

				float size = 47.f;
				

				float playerDistance = player->getStateVector()->Position.distance(targetPos);

				if (playerDistance <= 7) {
					// Get the TargetName
					std::string targetName = AvailableTargetList[0]->getNametag()->c_str();

					targetName = Utils::sanitize(targetName);
					targetName = targetName.substr(0, targetName.find('\n'));

					ImRenderUtil::fillRectangle(rect, UIColor(0, 0, 0), 0.6f, rounding);
					ImRenderUtil::fillShadowRectangle(rect, UIColor(0, 0, 0), 0.8f, 30.f, 0, rounding + 10.f);

					Vector4<float> HeadPos = Vector4<float>((rect.x + 8) + (AvailableTargetList[0]->getHurtTime() * 2), (rect.y + 8) + (AvailableTargetList[0]->getHurtTime() * 2), (rect.x + 55) - (AvailableTargetList[0]->getHurtTime() * 2), (rect.y + 55) - (AvailableTargetList[0]->getHurtTime() * 2));
					if (AvailableTargetList[0]->getHurtTime() >= 1) {
						ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), ImColor(255, 114, 118, 255), rounding);
					}
					else {
						ImGui::GetBackgroundDrawList()->AddImageRounded((void*)Global::RenderInfo::HeadTexture, ImVec2(HeadPos.x, HeadPos.y), ImVec2(HeadPos.z, HeadPos.w), ImVec2(0, 0), ImVec2(1, 1), IM_COL32_WHITE, rounding);
					}

					ImRenderUtil::drawText(Vector2<float>(rect.x + 70, rect.y + 21), &targetName, UIColor(255, 255, 255), 1.15f, 1.f, true);
				}
			}
		}*/
	}

	void onEnabled() override {}

private:
};