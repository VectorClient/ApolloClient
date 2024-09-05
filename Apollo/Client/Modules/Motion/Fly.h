#pragma once

class Fly : public Module
{
public:
    Fly(int keybind = 70, bool enabled = false) :
        Module("Fly", "Motion", "Fly like a superhero.", keybind, enabled)
    {
        addEnum("Mode", "The type of fly", { "Vanilla", "Flareon", "silly", "funny", "lol", "AirWalk" }, &mode);
        addSlider("Speed", "the delay between placing with ticks.", &speed, 0.3, 2);
        addSlider("Glide", "No Description", &glide, -1, 0, 0.01);
        addSlider("Friction", "How fast you will slowdown", &friction, 0, 20);
        addBool("Blink", "Stops from changing positions (PlayerAuthInputPacket)", &Blink);

        addSlider("Timer Value", "How fast you will slowdown while boosting", &timer, 1, 30);
        addSlider("Boost Value", "How fast you will slowdown while boosting", &value, -0.2, 0);
    }
private:
    int mode = 0;
    float speed = 0.6;
    float friction = 2.3f;
    float glide = 0.f;
    bool Blink = false;

    float timer = 5;
    float value = 0.55;

    int jumpY = 0;
    float StartPosition = 0;

    Vector3<float> oldPosition;

    Vector3<float> desyncPosition;
    Vector3<float> desyncRotation;

    bool shouldResetFly = false;
public:

    void onEnabled() override {
        if (!Address::getLocalPlayer()) return;

        StartPosition = Address::getLocalPlayer()->getRenderPosition().y;

        oldPosition = Address::getLocalPlayer()->getRenderPosition();

        desyncPosition = Address::getLocalPlayer()->getRenderPosition();
        desyncRotation.x = Address::getLocalPlayer()->getComponent<ActorRotationComponent>()->rotation.x; // Pitch
        desyncRotation.y = Address::getLocalPlayer()->getComponent<MobBodyRotationComponent>()->bodyRot; // Body
        desyncRotation.z = Address::getLocalPlayer()->getComponent<ActorHeadRotationComponent>()->rotation.x; // Head


        switch (mode) {
        case 0: // Vanilla
            break;
        case 1:
            shouldResetFly = false;
            break;
        }

        if (mode == 1) {
            for (int x = -2; x <= 2; x++) {
                for (int z = -2; z <= 2; z++) {
                    for (int y = -2; y <= 2; y++) {
                        jumpY = Address::getLocalPlayer()->getStateVector()->Position.y;
                    }
                }
            }
        }
    }

    void onEvent(ActorBaseTickEvent* event) override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        if (!Address::getLocalPlayer() || !Address::getLocalPlayer()->getStateVector())
            return;

        Player* player = Address::getLocalPlayer();
        StateVectorComponent* state = player->getStateVector();

        if (!shouldResetFly) {
            desyncPosition = player->getRenderPosition();
            desyncRotation.x = player->getComponent<ActorRotationComponent>()->rotation.x; // Pitch
            desyncRotation.y = player->getComponent<MobBodyRotationComponent>()->bodyRot; // Body
            desyncRotation.z = player->getComponent<ActorHeadRotationComponent>()->rotation.x; // Head
        }

        bool keyPressed = player->getMoveInputHandler()->isPressed();

        float yaw = (float)player->getMovementInterpolator()->Rotations.y;

        switch (mode) {
        case 0: // Vanilla
            MovementUtils::setSpeed(speed);
            state->Velocity.y = glide / 10;
            if (player->getMoveInputHandler()->jumping) {
                player->getStateVector()->Velocity.y += 0.3;
            }
            else if (player->getMoveInputHandler()->sneaking) {
                player->getStateVector()->Velocity.y -= 0.3;
            }

            break;
        case 1: // Flareon
            if (keyPressed)
                player->setSprinting(true);

            static float currentSpeed;
            static float lastSpeed;

            Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(timer);

            if (keyPressed) {
                if (state->Position.y < jumpY) {
                    if (TimeUtils::hasTimeElapsed("FlyJumpDelay", 100, true)) {
                        currentSpeed = 0.97f;
                        lastSpeed = 0.97f;
                        player->jumpFromGround();
                    }
                }
                else if (!player->isOnGround()) {
                    Vector3<float> velocity = state->Velocity;
                    if (abs(velocity.magnitudexz()) > lastSpeed) {
                        currentSpeed = velocity.magnitudexz();
                    }
                    lastSpeed = velocity.magnitudexz();
                    currentSpeed = currentSpeed * 0.9f;
                    MovementUtils::setSpeed(currentSpeed);
                }
            }
            break;
        case 5:
            player->setIsOnGround(true);
            MovementUtils::setSpeed(speed);
            state->Velocity.y = glide / 10;
        }

        if (mode == 0) {
            state->Velocity.y = 0;

            if (player->getMoveInputHandler()->forward) {
                state->Velocity.z = sin((yaw + 90) * 0.01745329251f) * speed; // 0.01745329251f
                state->Velocity.x = cos((yaw + 90) * 0.01745329251f) * speed;
            }
            if (player->getMoveInputHandler()->left) {
                state->Velocity.z = sin((yaw + 0) * 0.01745329251f) * speed;
                state->Velocity.x = cos((yaw + 0) * 0.01745329251f) * speed;
            }
            if (player->getMoveInputHandler()->backward) {
                state->Velocity.z = sin((yaw - 90) * 0.01745329251f) * speed;
                state->Velocity.x = cos((yaw - 90) * 0.01745329251f) * speed;
            }
            if (player->getMoveInputHandler()->right) {
                state->Velocity.z = sin((yaw + 180) * 0.01745329251f) * speed;
                state->Velocity.x = cos((yaw + 180) * 0.01745329251f) * speed;
            }

            if (player->getMoveInputHandler()->jumping) {
                player->getStateVector()->Velocity.y += 0.3;
            }
            else if (player->getMoveInputHandler()->sneaking) {
                player->getStateVector()->Velocity.y -= 0.3;
            }
        }
        else if (mode == 2) {
            Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(timer);
            state->Velocity.y = value;

            MovementUtils::setSpeed(speed);

            if (Global::Keymap[VK_SPACE]) {
                player->getStateVector()->Velocity.y += 0.3;
            }
            else if (Global::Keymap[VK_SHIFT]) {
                player->getStateVector()->Velocity.y -= 0.3;
            }
        }
        else if (mode == 3) {
            if (!player->isOnGround()) {
                MovementUtils::setSpeed(0.307);
            }

            state->Velocity.y = 0;

            if (Global::Keymap[VK_SPACE]) {
                state->Velocity.y += 0.3;
                player->getComponent<FallDistanceComponent>()->fallDistance = 0;
            }

            else if (Global::Keymap[VK_SHIFT]) {
                state->Velocity.y -= 0.3;
                player->getComponent<FallDistanceComponent>()->fallDistance = 0;
            }
        }
        else if (mode == 4) {
            MovementUtils::setSpeed(speed);

            state->Velocity.y = 0;

            if (Global::Keymap[VK_SPACE]) {
                state->Velocity.y += 0.3;
                player->getComponent<FallDistanceComponent>()->fallDistance = 0;
            }

            else if (Global::Keymap[VK_SHIFT]) {
                state->Velocity.y -= 0.3;
                player->getComponent<FallDistanceComponent>()->fallDistance = 0;
            }
        }
        // ???
        /*else if (mode == 5) {
            if (state->Position.y < jumpY) {
                Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
                player->jumpFromGround();
                state->Position.y = jumpY;
                //player->setPos(Vector3<float>(state->Position.x, jumpY, state->Position.z));
            }
            Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(8);
            player->setSprinting(true);
            player->setSneaking(false);
        }*/
    }

    void onEvent(PacketEvent* event) override
    {
        if (strcmp(event->Packet->getTypeName().getText(), "PlayerAuthInputPacket") == 0 && Blink) {
            *event->cancelled = true;
        }

        PlayerAuthInputPacket* packet = nullptr;

        if (event->Packet->getId() == PacketID::PlayerAuthInput) {
            packet = (PlayerAuthInputPacket*)event->Packet;
        }

        if (mode == 1) {
            if (packet != nullptr) {
                if (shouldResetFly) {
                    packet->position = desyncPosition;

                    packet->rotation.x = desyncRotation.x;
                    packet->rotation.y = desyncRotation.y;
                    packet->headYaw = desyncRotation.z;

                    Global::testPosition = desyncPosition;
                }
            }
        }

        if (mode == 3) {
            if (packet != nullptr) {
                packet->position.y = StartPosition;
            }
        }

        if (mode == 4) {
            if (packet != nullptr) {
                packet->position = oldPosition;

                Global::testPosition = oldPosition;
            }
        }

        if (mode == 5) {
            if (packet != nullptr) {
                packet->position.y = StartPosition;
            }
        }
    }

    void onDisabled() override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        Address::getClientInstance()->getTimerClass()->setMainTimerSpeed(20);
        StartPosition = 0;
    }
};