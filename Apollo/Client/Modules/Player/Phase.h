#pragma once

class Phase : public Module
{
public:
    Phase(int keybind = 7, bool enabled = false) :
        Module("Phase", "Player", "Allow phasing through blocks", keybind, enabled)
    {
        addEnum("Mode", "The mode for the delay", { "Horizontal", "Vertical" }, &Mode);
        addSlider("Speed", "The speed of vertical movement", &VerticalSpeed, 0.2, 1);
    }

private:
    int Mode = 0;
    float VerticalSpeed = 0.5;
public:
    void onEnabled() override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getLocalPlayer() || !Address::getLocalPlayer()->getStateVector())
            return;

        ClientInstance* ins = Address::getClientInstance();
        Player* player = Address::getLocalPlayer();
        Vector3<int> playerPos = player->getAABBShape()->PosLower.ToInt();
        StateVectorComponent* state = player->getStateVector();
        BlockSource* source = Address::getBlockSource();

        if (Mode == 0) {
            player->getAABBShape()->PosUpper.y = player->getAABBShape()->PosLower.y;
        }
        else {
            bool isSneaking = Global::Keymap[VK_SHIFT];
            bool isJumping = Global::Keymap[VK_SPACE];

            if (0 < player->getStateVector()->Velocity.y) player->getStateVector()->Velocity.y = 0.f;
            if (source->getBlock(playerPos)->GetMaterialType() == MaterialType::Air && source->getBlock(playerPos.add(Vector3<int>(0, -1, 0)))->GetMaterialType() == MaterialType::Air && source->getBlock(playerPos.add(Vector3<int>(0, 2, 0)))->GetMaterialType() == MaterialType::Air) return;

            player->getStateVector()->Velocity = Vector3<float>(0.f, 0.f, 0.f);

            player->getAABBShape()->PosUpper.y = 0.f;
            if (isJumping) {
                player->getStateVector()->Velocity.y += VerticalSpeed;
            }
            else if (isSneaking) {
                player->getStateVector()->Velocity.y -= VerticalSpeed;
            }
            player->stopSwimming();
        }
    }

    void onDisabled() override
    {
        Player* player = Address::getLocalPlayer();
        AABBShapeComponent* aabb = player->getAABBShape();

        if (player == nullptr) { return; }

        aabb->PosUpper.y = aabb->PosLower.y + 1.8f;
    }
};