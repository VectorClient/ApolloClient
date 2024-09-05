#pragma once

class Desync : public Module
{
public:
    Desync(int keybind = 7, bool enabled = false) :
        Module("Desync", "Misc", "Desyncs the player movement & actions.", keybind, enabled)
    {
        addSlider("Desync Value", "The desync duh", &desyncMs, 1, 1000);
    }

private:
    static inline float desyncMs = 500;

    Vector3<float> desyncPosition;
    Vector3<float> desyncRotation;

    std::vector<Vector3<float>> linePoints;
    Vector3<float> floorPos;
    Vector3<float> startPosTop;

    struct BreadCrumbsLog {
        int time;
        Vector3<float> pos;
    };

    long long timeOffset = 0;
    std::vector<BreadCrumbsLog> trailLog;
    Vector3<float> oldPos;
public:

    void onEnabled() override {
        if (Address::getLocalPlayer() == nullptr)
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        desyncPosition = player->getRenderPosition();
        desyncRotation.x = player->getComponent<ActorRotationComponent>()->rotation.x; // Pitch
        desyncRotation.y = player->getComponent<MobBodyRotationComponent>()->bodyRot; // Body
        desyncRotation.z = player->getComponent<ActorHeadRotationComponent>()->rotation.x; // Head
        TimeUtils::resetTime("desyncMs");
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (Address::getLocalPlayer() == nullptr)
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        Vector3<float> floorPos = state->Position;
        floorPos.y = state->Position.floor().y - 1.5f;

        if (oldPos.distance(floorPos) > 0.01) {
            BreadCrumbsLog trail = {};
            //trail.time = time;
            trail.pos = floorPos;

            trailLog.push_back(trail);
            oldPos = floorPos;
        }

        if (TimeUtils::hasTimeElapsed("desyncMs", desyncMs, true)) {
            desyncPosition = player->getRenderPosition();
            desyncRotation.x = player->getComponent<ActorRotationComponent>()->rotation.x; // Pitch
            desyncRotation.y = player->getComponent<MobBodyRotationComponent>()->bodyRot; // Body
            desyncRotation.z = player->getComponent<ActorHeadRotationComponent>()->rotation.x; // Head
            trailLog.clear();
        }
    }

    void onEvent(ImGuiRenderEvent* event) override {
        if (Address::getLocalPlayer() == nullptr || !Address::canUseKeys())
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        for (auto trail : trailLog) {
            Vector2<float> screen, screen2, screen3;
            if (!Address::getClientInstance()->WorldToScreen(trail.pos, screen)) continue;
            if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(trail.pos.x - 0.1f, trail.pos.y - 0.1f, trail.pos.z - 0.1f), screen2)) continue;
            if (!Address::getClientInstance()->WorldToScreen(Vector3<float>(trail.pos.x + 0.1f, trail.pos.y + 0.1f, trail.pos.z + 0.1f), screen2)) continue;

            float circleSize = std::fmin(std::fmax(abs(screen.x - screen2.x), abs(screen.y - screen2.y)), 4);
            float opacity = 1;
            circleSize = circleSize;

            ImRenderUtil::fillCircle(screen, circleSize, UIColor(255, 0, 0), opacity, 12.f);
            ImRenderUtil::fillShadowCircle(screen, circleSize, UIColor(255, 0, 0), opacity, 40.f, 0);
        }
    }

    void onEvent(PacketEvent* event) override {
        if (Address::getLocalPlayer() == nullptr)
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        PlayerAuthInputPacket* packet = nullptr;

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }

        if (packet != nullptr) {
            packet->position = desyncPosition;

            packet->rotation.x = desyncRotation.x;
            packet->rotation.y = desyncRotation.y;
            packet->headYaw = desyncRotation.z;

            //*event->cancelled = true;

            Global::desyncPosition = desyncPosition;
        }
    }
};