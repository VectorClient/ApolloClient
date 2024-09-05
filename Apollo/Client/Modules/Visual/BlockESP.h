#pragma once

class BlockESP : public Module
{
public:
	BlockESP(int keybind = 7, bool enabled = true) :
		Module("Block ESP", "Visual", "Esp for ore", keybind, enabled)
	{
		addEnum("Style", "The style of your blockESP", { "Circle", "Box" }, &style);
		addSlider("Range", "Range for blockesp", &range, 1, 10);
		addBool("Iron", "Render iron ore", &iron);
		addBool("Gold", "Render gold ore", &gold);
		addBool("lapis", "Render lapis ore", &lapis);
		addBool("Redstone", "Render redstone ore", &redstone);
		addBool("Diamond", "Render diamond ore", &diamond);
		addBool("Emerald", "Render emerald ore", &emerald);
		addSlider("Opacity", "The opacity of blockesp", &opacity, 0, 100, 1);
		addSlider("Line", "The width of blockesp", &line, 0, 3, 1);
	}

	int style = 0;
	float range = 5;
	float opacity = 50;
	float line = 1;

	bool iron = false;
	bool gold = false;
	bool lapis = false;
	bool redstone = false;
	bool diamond = false;
	bool emerald = false;

	struct OreStatus {
		Vector3<float> pos;
		int oreIndex;
	};
	std::vector<OreStatus> Ores;

	void onEvent(ActorBaseTickEvent* event) override
	{
		auto player = Address::getLocalPlayer();
		if (player == nullptr || !Address::canUseKeys()) return;

		BlockSource* source = Address::getBlockSource();
		if (!source) return;

		// Initialize position list
		static vector<Vector3<int>> blocks;
		if (blocks.empty()) {
			for (int x = -range; x <= range; x++) {
				for (int z = -range; z <= range; z++) {
					for (int y = -range; y <= range; y++) {
						blocks.push_back(Vector3<int>(x, y, z));
					}
				}
			}
			sort(blocks.begin(), blocks.end(), [](Vector3<int> start, Vector3<int> end) {
				return sqrtf((start.x * start.x) + (start.y * start.y) + (start.z * start.z)) < sqrtf((end.x * end.x) + (end.y * end.y) + (end.z * end.z));
				});
		}

		// Clear list
		Ores.clear();

		// Define player position
		Vector3<int> playerBlockPos = player->getAABBShape()->PosLower.ToInt();

		// Push back ores to list
		for (const Vector3<int>& offset : blocks) {
			Vector3<int> blockPos = Vector3<int>(playerBlockPos.x + offset.x, playerBlockPos.y + offset.y, playerBlockPos.z + offset.z);
			Vector3<float> renderPos = blockPos.ToFloat().add(Vector3<float>(0.f, 0.f, 0.f));
			int blockId = source->getBlock(blockPos)->GetBlockLegacy()->getBlockID();
			switch (blockId) {
			case 15: // Iron
				if (iron) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 0;
					Ores.emplace_back(status);
				}
				break;
			case 14: // Gold
				if (gold) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 1;
					Ores.emplace_back(status);
				}
				break;
			case 21: // Lapis
				if (lapis) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 2;
					Ores.emplace_back(status);
				}
				break;
			case 73: // Redstone
			case 74: // Lit Redstone
				if (redstone) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 3;
					Ores.emplace_back(status);
				}
				break;
			case 56: // Diamond
				if (diamond) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 4;
					Ores.emplace_back(status);
				}
				break;
			case 129: // Emerald
				if (emerald) {
					OreStatus status;
					status.pos = renderPos;
					status.oreIndex = 5;
					Ores.emplace_back(status);
				}
				break;
			}
		}
	}

	void onEvent(ImGuiRenderEvent* event) override
	{
		auto player = Address::getLocalPlayer();
		if (player == nullptr || !Address::canUseKeys()) return;
		static std::vector<UIColor> colorLists = {
			UIColor(219, 194, 150), // Iron
			UIColor(255, 230, 31), // Gold
			UIColor(0, 0, 255), // Lapis
			UIColor(255, 0, 0), // Redstone
			UIColor(0, 204, 255), // Diamond
			UIColor(0, 255, 13), // Emerald
		};
		for (auto ore : Ores) {
			UIColor color = colorLists[ore.oreIndex];

			if (style == 0) {
				Vector2<float> screen, screen2, screen3;
				Vector3<float> pos = ore.pos.add(Vector3<float>(0.5f, 0.5f, 0.5f));
				if (!Address::getClientInstance()->WorldToScreen(pos, screen)) continue;
				if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(pos.x - 0.1f, pos.y - 0.1f, pos.z - 0.1f), screen2)) continue;
				if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(pos.x + 0.1f, pos.y + 0.1f, pos.z + 0.1f), screen2)) continue;

				float circleSize = std::fmin(std::fmax(abs(screen.x - screen2.x), abs(screen.y - screen2.y)), 8);

				ImRenderUtil::fillCircle(screen, circleSize, color, opacity / 100, 12.f);
				if (circleSize <= 2) continue;
				ImRenderUtil::fillShadowCircle(screen, circleSize, color, opacity / 100, 40.f, 0);
			}
			
			if (style == 1) {
				Vector3<float> angle = ore.pos;
				angle = angle.floor();
				angle.x += 1;
				angle.y += 1;
				angle.z += 1;

				ImRenderUtil::drawBox(ore.pos, angle, color, color, line, false, true);
			}
		}
	}

	void onEnabled() override {
		//
	}

	void onDisabled() override {
		//
	}
};