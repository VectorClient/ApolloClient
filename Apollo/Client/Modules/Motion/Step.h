#pragma once

class Step : public Module
{
public:
    Step(int keybind = Keys::NONE, bool enabled = false) :
        Module("Step", "Motion", "Stepping on blocks lmao", keybind, enabled)
    {
        addEnum("Mode", "The type of step", {"Flareon", "Normal"}, &mode);
        addSlider("Height", "The maximum height which you will automatically step.", &height, 0, 5);
    }

    int mode = 0;

    float height = 1.3;

    bool shouldClimb = false;
    bool shouldStop = false;

    void onEvent(RenderContextEvent* event) override
    {
        if (!Address::getClientInstance() || !Address::getLocalPlayer() || !Address::getLocalPlayer()->getStateVector() || !Address::getClientInstance()->getTimerClass() || !Address::canUseKeys())
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        switch (mode) {
        case 0: //Flareon
            if (player->getMoveInputHandler()->isPressed() && player->isCollidingHorizontal() && (player->isOnGround() || shouldClimb)) {
                state->Velocity.y = height / 10;
                shouldStop = true;
                shouldClimb = true;
            } else if (shouldStop) {
                shouldStop = false;
                shouldClimb = false;
                state->Velocity.y = 0;
            }
            break;
        case 1: //Normal
            Address::getLocalPlayer()->getComponent<MaxAutoStepComponent>()->maxAutoStep = height;
            break;
        }
    }

    void onDisabled() override {
        if (Address::getLocalPlayer() != nullptr)
            Address::getLocalPlayer()->getComponent<MaxAutoStepComponent>()->maxAutoStep = 0.5625f;
    }
};