#pragma once

class Surround : public Module
{
public:
	Surround(int keybind = Keys::NUM_0, bool enabled = false) :
		Module("Surround", "Misc", "Trap player automatically", keybind, enabled)
	{
		addEnum("PlaceMode", "The type of place", { "Simple", "Insane" }, &placeMode);
		addSlider("Range", "Range for AutoTrap", &range, 1, 10);
		addSlider("Prediction", "Strength of prediction", &prediction, 0, 3);
		addSlider("Place Limitter", "Limit of placing in one tick", &placeLimitter, 1, 20);
		addSlider("Place Distance", "Only smartplace mode", &placeDistance, 1, 5);
		addBool("Smart Place", "No Description", &smart);
	}

private:
	float range = 1.5;
	float prediction = 1.f;
	float placeLimitter = 1.f;
	float placeDistance = 1.f;
	int slot = 0;
	int placeMode = 0;
	bool smart = false;
	Vector3<float> lastTargetPosition = NULL;

	Vector3<float> calcPos = Vector3 <float>{ 0, 0, 0 };
	int HotbarSlotPrev = -1;

public:
	void onEnabled() override {
		auto player = Address::getLocalPlayer();
		if (player == nullptr) return;

		lastTargetPosition = NULL;
		auto* hivefly = getModuleByName("Killaura");
		hivefly->setEnabled(false);
		HotbarSlotPrev = Address::getLocalPlayer()->getSupplies()->hotbarSlot; // This for us to set our slot to the current one
	}

	Vector3<int> GetBlockPosition(Actor* target)
	{
		Vector3<float> pos = target->getAABBShape()->PosLower;

		return { (int)floor(pos.x), (int)floor(pos.y), (int)floor(pos.z) };
	}

	bool isStandingCenter(Actor* target) {
		Vector3<float> pos = target->getAABBShape()->PosLower;
		Vector3<float> centerPos = pos.ToInt().ToFloat().add(Vector3<float>(0.5f, 0.f, 0.5f));
		float width = target->getAABBShape()->Hitbox.x;
		if (abs(centerPos.x - pos.x) <= width / 2 && abs(centerPos.z - pos.z) <= width / 2) return true;
		return false;
	}

	std::vector<Actor*> targetList;
	void updateTargetList() {
		targetList.clear();
		auto instance = Address::getClientInstance();
		auto localPlayer = instance->getLocalPlayer();
		auto list = localPlayer->getLevel()->getRuntimeActorList();
		auto lpPos = localPlayer->getStateVector()->Position;
		for (Actor* actor : list) {
			if (actor != localPlayer && actor->isAlive() && !actor->isBot() && (getModuleByName("Teams")->isEnabled() && !actor->isTeammate(localPlayer))) { //  && !actor->isBot()
				float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
				if (dist <= range) {
					targetList.push_back(actor);
				}
			}
		}
	}

	//Place
	bool canPlace(Vector3<float> pos) {
		return Address::getBlockSource()->getBlock(pos.floor().ToInt())->GetMaterialType() == MaterialType::Air;
	}

	bool findBlock() {
		PlayerInventory* playerInventory = Address::getLocalPlayer()->getSupplies();
		Inventory* inventory = playerInventory->inventory;
		auto previousSlot = playerInventory->hotbarSlot;

		for (int i = 0; i < 36; i++) {
			ItemStack* stack = inventory->getItem(i);
			if (stack->item != nullptr) {
				if (stack->isBlockType() && !stack->getItem()->isBoomBox()) {
					if (previousSlot != i)
						playerInventory->hotbarSlot = i;
					return true;
				}
			}
		}
		return false;
	}

	bool buildBlock(Vector3<float> blockBelow) {
		auto blockPos = blockBelow.floor().ToInt();
		if (Address::getBlockSource()->getBlock(blockPos)->GetBlockLegacy()->getBlockID() != 0) return false;

		static const std::vector<Vector3<int>> checklist = {
			{0, -1, 0}, {0, 1, 0}, {0, 0, -1}, {0, 0, 1}, {-1, 0, 0}, {1, 0, 0}
		};

		for (int i = 0; i < checklist.size(); ++i) {
			auto calc = Vector3<int>{ blockPos.x - checklist[i].x, blockPos.y - checklist[i].y, blockPos.z - checklist[i].z };
			if (Address::getBlockSource()->getBlock(calc)->GetBlockLegacy()->getBlockID() != 0) {
				calcPos = calc.add(Vector3<int>(0.78f, 0.78f, 0.78f)).ToFloat();
				auto player = Address::getLocalPlayer();
				auto hitResult = player->getLevel()->getHitResult();
				hitResult->BlockFace = i;
				hitResult->IBlockPos = blockPos;
				hitResult->HitType = 0;
				hitResult->AbsoluteHitPos = blockPos.ToFloat();
				player->getGamemode()->buildBlock(calc, i, true);
				return true;
			}
		}
		return false;
	}

	void onEvent(ActorBaseTickEvent* event) override {
		static bool placedLastTick = false;
		auto player = Address::getLocalPlayer();
		if (!player || !Address::canUseKeys()) return;

		auto supplies = player->getSupplies();
		if (placedLastTick) {
			supplies->hotbarSlot = HotbarSlotPrev;
			placedLastTick = false;
		} else {
			HotbarSlotPrev = supplies->hotbarSlot;
		}

		updateTargetList();
		if (!targetList.empty()) {
			auto state = targetList[0]->getStateVector();
			Vector3<float> predictVector = { 0.f, 0.f, 0.f };
			if (lastTargetPosition != Vector3<float>{0, 0, 0}) {
				predictVector = Vector3<float>((state->Position.x - lastTargetPosition.x), 0, (state->Position.z - lastTargetPosition.z));
				predictVector.x *= prediction;
				predictVector.z *= prediction;
			}
			auto targetPosition = GetBlockPosition(targetList[0]).ToFloat().add(predictVector);
			lastTargetPosition = state->Position;

			static const std::vector<Vector3<int>> posListSimple = {
				{1, 0, 0}, {1, 1, 0}, {-1, 0, 0}, {-1, 1, 0},
				{0, 0, 1}, {0, 1, 1}, {0, 0, -1}, {0, 1, -1},
				{0, 2, -1}, {0, 2, 0}
			};

			static const std::vector<Vector3<int>> posListInsane = {
		{1, 0, 0}, {1, 1, 0}, {-1, 0, 0}, {-1, 1, 0},
		{0, 0, 1}, {0, 1, 1}, {0, 0, -1}, {0, 1, -1},
		{0, 2, -1}, {0, 2, 0},
		{2, 0, 0}, {2, 1, 0}, {2, -1, 0}, {2, -2, 0},
		{-2, 0, 0}, {-2, 1, 0}, {-2, -1, 0}, {-2, -2, 0},
		{0, 2, 1}, {0, 2, -2},
		{1, 2, 0}, {-1, 2, 0}, {1, -2, 0}, {-1, -2, 0},
		{0, -2, 1}, {0, -2, -1}
			};

			std::vector<Vector3<int>> posList;
			switch (placeMode) {
			case 0:
				posList = posListSimple;
				break;
			case 1:
				posList = posListInsane;
				break;
			}


			Vector3<float> vel = state->Velocity;
			std::vector<Vector3<int>> smartPosList = {
				{(int)(vel.x * placeDistance), 0, (int)(vel.z * placeDistance)}, {(int)(vel.x * placeDistance), 1, (int)(vel.z * placeDistance)}
			};
			if (smart) posList.insert(posList.begin(), std::begin(smartPosList), std::end(smartPosList));

			int count = 0;
			for (const auto& pos : posList) {
				auto placePosition = targetPosition.add(Vector3<float>(pos.x, pos.y, pos.z));
				if (canPlace(placePosition) && findBlock() && buildBlock(placePosition)) {
					placedLastTick = true;
					count++;
					if (placeLimitter < count) return;
				}
			}
		} else {
			lastTargetPosition = { 0, 0, 0 };
		}
	}

	void onDisabled() override {
		if (!Address::getLocalPlayer()) {
			return;
		}

		Address::getLocalPlayer()->getSupplies()->hotbarSlot = HotbarSlotPrev;
	}
};