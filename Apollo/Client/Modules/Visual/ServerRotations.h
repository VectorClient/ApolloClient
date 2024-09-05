#pragma once

class ServerRotations : public Module
{
public:
    ServerRotations(int keybind = Keys::NONE, bool enabled = true) :
        Module("ServerRotations", "Visual", "Renders rotations using the packets sent", keybind, enabled)
    {
        addBool("LockBodyYaw", "Changes the body yaw to the head yaw", &Global::lockBodyYaw);
        addBool("Reset", "Reset the serverrotations to its original value", &reset);
        addSlider("Yaw Speed", "The speed of the head yaw", &Global::HeadYawSpeed, 1, 10);
        addSlider("Body Speed", "The speed of the body yaw", &Global::BodyYawSpeed, 1, 10);
        addSlider("Pitch Speed", "The speed of the pitch", &Global::PitchSpeed, 1, 10);
    }

    bool reset = false;

    void onEvent(IntersectsTickEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (!player) return;

        if (reset) {
            Global::HeadYawSpeed = 2.07;
            Global::BodyYawSpeed = 1.04;
            Global::PitchSpeed = 2.07;
            reset = false;
        }
    }

    void onEvent(RenderContextEvent* event) override {
        Global::easedHeadYaw = Math::wrap(Global::easedHeadYaw, Global::headYaw - 180.f, Global::headYaw + 180.f);
        Global::easedHeadYaw = Math::lerp(Global::easedHeadYaw, Global::headYaw, ImRenderUtil::getDeltaTime() * Global::HeadYawSpeed);

        if (Global::lockBodyYaw) {
            Global::easedBodyYaw = Math::wrap(Global::easedBodyYaw, Global::easedHeadYaw - 180.f, Global::easedHeadYaw + 180.f);
            Global::easedBodyYaw = Math::lerp(Global::easedBodyYaw, Global::easedHeadYaw, ImRenderUtil::getDeltaTime() * Global::BodyYawSpeed);
        }
        else {
            Global::easedBodyYaw = Math::wrap(Global::easedBodyYaw, Global::bodyYaw - 180.f, Global::bodyYaw + 180.f);
            Global::easedBodyYaw = Math::lerp(Global::easedBodyYaw, Global::bodyYaw, ImRenderUtil::getDeltaTime() * Global::BodyYawSpeed);
        }

        Global::easedPitch = Math::lerp(Global::easedPitch, Global::pitch, ImRenderUtil::getDeltaTime() * Global::PitchSpeed);
    }
};