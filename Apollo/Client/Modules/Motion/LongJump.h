#pragma once

class LongJump : public Module
{
public:
    LongJump(int keybind = Keys::NONE, bool enabled = false) :
        Module("LongJump", "Motion", "Make a big jump once.", keybind, enabled)
    {
        addEnum("Mode", "The mode of the longjump", { "Custom" }, &mode);
        addEnum("Fall", "The mode of falling", { "Legit", "Multiplier", "Add", "Both"}, &fallMode);
        addEnum("Fricton", "The mode of friction", { "Legit", "Multiplier", "Add", "Both" }, &frictionMode);
        addSlider("Speed", "How fast you will go", &speed, 3, 15);
        addSlider("Height", "How much velocity to add.", &height, 0, 7);
        addSlider("Timer", "Increase the game timer", &timer, 0, 40, 1);
        addSlider("FallSpeed (Multiplier)", "How fast you will fall (1 = Legit speed)", &fallSpeed_M, 0.5, 2.0, 0.01);
        addSlider("FallSpeed (Add)", "How fast you will fall (0 = Legit speed)", &fallSpeed_A, -1.0, 1.0, 0.01);
        addSlider("Friction (Multiplier)", "How fast you will slowdown off ground (0.98 = Legit Speed?)", &friction_M, 0.5, 1.0, 0.01);
        addSlider("Friction (Add)", "How fast you will slowdown off ground (0 = Legit Speed)", &friction_A, -1.0, 1.0, 0.01);
        addSlider("Boost Timing", "The timing of adding boost", &timing, 1, 8, 1);
        addBool("Debug", "Send message", &debug);
    }

    int mode = 0;
    int fallMode = 0;
    int frictionMode = 0;

    float speed = 3.f;
    float height = 0.f;
    float timer = 20.f;
    float fallSpeed_M = 1.f;
    float fallSpeed_A = 0.f;
    float friction_M = 0.98f;
    float friction_A = 0.f;
    float timing = 1.f;

    bool debug = false;

    int jmpCount = 0;
    bool jmped = false;

    void onEnabled() override {
        Global::lastLerpVelocity = NULL;
        jmped = false;
        jmpCount = 100;
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Address::getClientInstance() || !Address::getLocalPlayer() || !Address::getLocalPlayer()->getStateVector() || !Address::getClientInstance()->getTimerClass() || !Address::canUseKeys())
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();


        auto onGround = player->isOnGround();

        bool keyPressed = player->getMoveInputHandler()->isPressed();
        if (keyPressed) player->setSprinting(true);

        static float currentSpeed;
        static float lastSpeed;

        float yaw = (float)player->getMovementInterpolator()->Rotations.y;

        if (player->getMoveInputHandler()->jumping) return;

        switch (mode) {
        case 0: // Custom
            Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(timer);

            jmpCount++;
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 100, true)) {
                        if (jmped) {
                            Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
                            Global::Notification.addNotification("LongJump", "Do not longjump for 10s", 1.5f);
                            this->enabled = false;
                            return;
                        }
                        else jmped = true;
                        Vector3<float> velocity = state->Velocity;
                        currentSpeed = velocity.magnitudexz();
                        player->jumpFromGround();
                        jmpCount = 0;
                    }
                } else {
                    Vector3<float> velocity = state->Velocity;
                    switch (frictionMode) {
                    case 0: //Legit
                        currentSpeed = velocity.magnitudexz();
                        break;
                    case 1: //Multiplier
                        currentSpeed *= friction_M;
                        break;
                    case 2: //Add
                        currentSpeed = velocity.magnitudexz();
                        currentSpeed += friction_A / 10;
                        break;
                    case 3: //Both
                        currentSpeed *= friction_M;
                        currentSpeed += friction_A / 10;
                        break;
                    }
                    MovementUtils::setSpeed(currentSpeed);

                    if (state->Velocity.y < 0) {
                        switch (fallMode) {
                        case 0: //Legit
                            break;
                        case 1: //Multiplier
                            state->Velocity.y /= fallSpeed_M;
                            break;
                        case 2: //Add
                            state->Velocity.y += fallSpeed_A / 10;
                            break;
                        case 3: //Both
                            state->Velocity.y /= fallSpeed_M;
                            state->Velocity.y += fallSpeed_A / 10;
                            break;
                        }
                    }
                }

                if (jmpCount == (int)(timing)) {
                    state->Velocity.y += height / 10;
                    float tmpSpeed = 0.f;
                    if (frictionMode == 2 || frictionMode == 3) tmpSpeed = friction_A;
                    MovementUtils::setSpeed((speed - tmpSpeed) / 10);
                    break;
                }

                if (debug) ChatUtils::sendNormalMessage("Velocity: " + to_string(state->Velocity.y) + ", Speed: " + to_string(currentSpeed / 10));
            }
            break;
        }
    }

    void onDisabled() override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
    }
};
