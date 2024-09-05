#pragma once

class Speed : public Module
{
public:
    Speed(int keybind = Keys::X, bool enabled = false) :
        Module("Speed", "Motion", "Bhop like the flash.", keybind, enabled)
    {
        addEnum("Mode", "The mode of the speed", { "Bunny hop", "Custom", "Flareon", "Sprint hop", "Flareon Fast", "Flareon Custom" }, &mode);
        addSlider("Speed", "How fast you will go", &speed, 1, 10);
        addSlider("SwiftSpeed", "How fast you will go when boosting", &swiftspeed, 1, 10);
        addSlider("Height", "(BunnyHop only)", &height, 0, 7);
        addSlider("Friction", "How fast you will slowdown off ground", &friction, 0, 10);
        addBool("AutoSwiftness", "Automaticly uses swiftness spell books", &autoswiftness);
        addSlider("Timer", "Increase the game timer", &timerSpeed, 0, 60);
        addBool("Fast Fall", "Fast falls into the ground", &fastfall);
        addSlider("Fall Speed", "The amount speed to fall (only when fast fall enabled)", &fastfallspeed, 2, 50);
        addBool("DamageBoost", "Boost speed when you knockback", &damageBoost);
        addSlider("DamageBoostSpeed", "How fast you will go when you get damage", &damageBoostSpeed, 0, 5);
        addSlider("Down Value", "Only custom mode", &downValue, 0, 5);
        addSlider("Up Timer", "Only custom mode", &upTimer, 18, 24);
        addSlider("Down Timer", "Only custom mode", &downTimer, 18, 32);
        addSlider("Down Timing (Min)", "Only custom mode", &timMin_D, 1, 8, 1);
        addSlider("Down Timing (Max)", "Only custom mode", &timMax_D, 1, 8, 1);
        addSlider("Boost Timing (Min)", "Only custom mode", &timMin_S, 1, 8, 1);
        addSlider("Boost Timing (Max)", "Only custom mode", &timMax_S, 1, 8, 1);
        addBool("Boost", "Only custom mode", &boost);
    }

    float speed = 3.5f;
    float swiftspeed = 7.5f;
    float height = 4.2f;
    float friction = 1.f;

    bool autoswiftness = false;

    float timerSpeed = 20;

    bool fastfall = false;
    float fastfallspeed = 20;
    bool gotBoost = false;
    float antiban = 0.f;

    //HiveLow
    bool fallen = false;
    bool heightApplied = false;

    // DamageBoost
    bool damageBoost = false;
    float damageBoostSpeed = 1.5f;

    //Custom
    float downValue = 1.f;
    float upTimer = 20.f;
    float downTimer = 26.f;
    float timMin_D = 1.f;
    float timMax_D = 1.f;
    float timMin_S = 1.f;
    float timMax_S = 1.f;
    bool boost = false;

    static inline bool shouldDoSwiftness = true;
    int jmpCount = 0;

    int mode = 0;

    void onEnabled() override {
        Global::lastLerpVelocity = NULL;
    }

    void doSwiftness(Player* player) {
        PlayerInventory* playerInventory = player->getSupplies();
        Inventory* inventory = playerInventory->inventory;
        auto previousSlot = playerInventory->hotbarSlot;

        for (int n = 0; n < 36; n++) {
            ItemStack* stack = inventory->getItem(n);
            if (stack->item != nullptr) {
                std::string ItemName = stack->getItem()->name;
                if (stack->customNameContains("Spell of Swiftness") && !Global::effectSwiftness) {
                    if (previousSlot != n && !shouldDoSwiftness) {
                        playerInventory->hotbarSlot = n;
                        player->getGamemode()->baseUseItem(*stack);
                        playerInventory->hotbarSlot = previousSlot;
                        TimeUtils::resetTime("Swiftness");
                        shouldDoSwiftness = true;
                    }
                }
            }
        }
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

        if (TimeUtils::hasTimeElapsed("Swiftness", 5000, false)) {
            shouldDoSwiftness = false;
        }

        switch (mode) {
        case 0: //BunnyHop
            // Are we holding movement keys?
            if (keyPressed)
            {
                // Should we jump?
                if ((height / 10) > 0 && onGround)
                {
                    //player->jumpFromGround();
                    state->Velocity.y += 1;
                    state->Velocity.y = height / 10;
                }
                MovementUtils::setSpeed(speed / 10);
            }
            break;
        case 1: // Custom mode
        {
            static Vector3<float> lastVelocity;
            if (lastVelocity == state->Velocity) return;
            float vanillaFriction = state->Velocity.magnitudexz() / lastVelocity.magnitudexz();
            lastVelocity = state->Velocity;
            static long long lastJumpedMS = 0;
            // Damage Boost
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                MovementUtils::setSpeed(currentSpeed);
                Global::lastLerpVelocity = NULL;
            }
            if (keyPressed)
            {
                if (onGround && TimeUtils::getCurrentMs() - lastJumpedMS >= 100)
                {
                    Vector3<float> velocity = state->Velocity;
                    currentSpeed = speed / 10;
                    player->jumpFromGround();
                    lastJumpedMS = TimeUtils::getCurrentMs();
                    heightApplied = false;
                }
                else if(!onGround)
                {
                    if (!heightApplied) {
                        state->Velocity.y = height / 10;
                        heightApplied = true;
                    }
                    else
                    {
                        Vector3<float> velocity = state->Velocity;
                        if (velocity.magnitudexz() < currentSpeed * vanillaFriction) {
                            currentSpeed = velocity.magnitudexz();
                        }
                        currentSpeed *= friction / 10;
                    }
                    MovementUtils::setSpeed(currentSpeed);
                }
            }
        }
        break;
        case 2: // Flareon
            // Damage Boost
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                MovementUtils::setSpeed(currentSpeed);
                Global::lastLerpVelocity = NULL;
            }
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 100, true)) {
                        Vector3<float> velocity = state->Velocity;
                        currentSpeed = velocity.magnitudexz();
                        player->jumpFromGround();
                    }
                }
                else
                {
                    Vector3<float> velocity = state->Velocity;
                    currentSpeed = velocity.magnitudexz();
                    MovementUtils::setSpeed(currentSpeed);
                }
            }
            //FastFall
            if (state->Velocity.y >= -0.2) {
                fallen = false;
            }
            else if (!fallen) {
                state->Velocity.y *= 1.75f;
                fallen = true;
            }
            break;
        case 3: // SprintHop
            // Damage Boost
            player->setSprinting(true);

            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                lastSpeed = boostedSpeed;
                gotBoost = true;
                TimeUtils::resetTime("DamageBoost");
                Global::lastLerpVelocity = NULL;
            }

            if (TimeUtils::hasTimeElapsed("DamageBoost", 590, false)) {
                gotBoost = false;
            }

            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 93, true)) {
                        player->jumpFromGround();
                    }
                }
                else
                {
                    if (autoswiftness) {
                        doSwiftness(player);
                    }

                    Vector3<float> velocity = state->Velocity;
                    if (abs(velocity.magnitudexz()) > lastSpeed) {
                        currentSpeed = velocity.magnitudexz();
                    }
                    lastSpeed = velocity.magnitudexz();
                    currentSpeed = currentSpeed * (friction / 10);
                    if (gotBoost) {
                        MovementUtils::setSpeed(currentSpeed);
                    }
                }
            }
            break;
        case 4: // Flareon Fast
            // Damage Boost
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                MovementUtils::setSpeed(currentSpeed);
                Global::lastLerpVelocity = NULL;
            }
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 100, true)) {
                        Vector3<float> velocity = state->Velocity;
                        currentSpeed = velocity.magnitudexz();
                        player->jumpFromGround();
                    }
                } else {
                    Vector3<float> velocity = state->Velocity;
                    currentSpeed = velocity.magnitudexz();
                    MovementUtils::setSpeed(currentSpeed);
                }
                float motion = Math::round(state->Velocity.y, 0.001f);
                std::vector<float> motions = { 0.248f, 0.126, 0.005 };
                for (const auto& item : motions) {
                    if (fabs(motion - item) < 0.0001f) {
                        state->Velocity.y -= 0.04f;
                        break;
                    }
                }
            }
            if (state->Velocity.y < 0) Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(26);
            else Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
            break;
        case 5: // Flareon Custom
            // Damage Boost
            jmpCount++;
            if (damageBoost && Global::lastLerpVelocity != NULL) {
                float boostedSpeed = Global::lastLerpVelocity.magnitudexz() * damageBoostSpeed;
                currentSpeed = boostedSpeed;
                MovementUtils::setSpeed(currentSpeed);
                Global::lastLerpVelocity = NULL;
            }
            if (keyPressed)
            {
                if (onGround)
                {
                    if (TimeUtils::hasTimeElapsed("jumpDelay", 100, true)) {
                        Vector3<float> velocity = state->Velocity;
                        currentSpeed = velocity.magnitudexz();
                        player->jumpFromGround();
                        jmpCount = 0;
                        antiban = 0;
                    }
                } else {
                    Vector3<float> velocity = state->Velocity;
                    currentSpeed = velocity.magnitudexz();
                    if (!boost) MovementUtils::setSpeed(currentSpeed);
                }
                if (jmpCount >= (int)(timMin_D) && jmpCount <= (int)(timMax_D)) {
                    state->Velocity.y -= downValue / 10;
                }
                if (jmpCount >= (int)(timMin_S) && jmpCount <= (int)(timMax_S)) {
                    MovementUtils::setSpeed((speed - antiban) / 10);
                    antiban += 0.1f;
                }
            }
            if (state->Velocity.y < 0) Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(downTimer);
            else Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(upTimer);
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
