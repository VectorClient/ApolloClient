#pragma once

class HiveFly : public Module
{
public:
    HiveFly(int keybind = Keys::NONE, bool enabled = false) :
        Module("Hive Fly", "Motion", "Surf on air", keybind, enabled)
    {
        addEnum("SpeedMode", "The type of speed", { "Static", "Decrease", "Loop", "Wave" }, &speedMode);
        addEnum("TimerMode", "The type of timer", { "Static", "Decrease", "Loop", "Wave"}, &timerMode);
        addSlider("Speed", "No Description", &speedValue, 1, 80);
        addSlider("SpeedMin", "No Description", &speedMin, 0, 80);
        addSlider("SpeedMax", "No Description", &speedMax, 0, 80);
        addSlider("SpeedMultiplier", "No Description", &increaseSpeed, 1, 10);
        addSlider("Timer", "No Description", &timerValue, 0.0, 3);
        addSlider("TimerMin", "No Description", &timerMin, 0.0, 1);
        addSlider("TimerMax", "No Description", &timerMax, 0.0, 5);
        addSlider("TimerMultiplier", "No Description", &increaseTimer, 1, 10);
        addSlider("Glide", "No Description", &glide, -10, 0);
        addSlider("NetSkipMs", "Delay to wait before resending packets (ms)", &dsyncDelay, 0.f, 4000);
        addBool("NetSkip", "No Description", &netskip);
        addBool("Debug", "Send message", &debug);
    }
private:
    int speedMode = 0;
    int timerMode = 0;

    float speedValue = 40;
    float speedMin = 40;
    float speedMax = 50;
    float timerValue = 0.4f;
    float timerMin = 1;
    float timerMax = 3;
    float glide = 0;
    float increaseSpeed = 1;
    float increaseTimer = 1;
    float dsyncDelay = 0;

    float currentTimer = 0;
    float currentSpeed = 0;

    bool debug = false;
    bool netskip = false;

public:

    void onEnabled() override {
        if (!Address::getLocalPlayer()) return;

        if (!Address::getClientInstance()->getTimerClass())
            return;
        currentSpeed = speedMax;
        currentTimer = timerMax;
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

        // Adjustment
        if (speedMax < speedMin) speedMax = speedMin;
        if (timerMax < timerMin) timerMax = timerMin;

        float deltaSpeed = speedMax - speedMin;
        float deltaTimer = timerMax - timerMin;
        float nextSpeed = 0;
        float nextTimer = 0;
        float addSpeed = -deltaSpeed / increaseSpeed;
        float addTimer = -deltaTimer / increaseTimer;
        float fakeTimer = Global::fakeTimer;

        bool canChange = Global::canChange;

        if (fakeTimer < 0 || !getModuleByName("Disabler")->isEnabled()) {
            Global::Notification.addNotification("HiveFly", "Use with FakeTimer mode in disabler", 1.5f);
            this->enabled = false;
        }

        switch (speedMode) {
        case 0: // Static
            currentSpeed = speedValue / 10;
            break;
        case 1: // Decrease
            if (canChange) {
                nextSpeed = currentSpeed + addSpeed;
                if (nextSpeed > speedMin) currentSpeed = nextSpeed;
                else currentSpeed = speedMin;
            }
            currentSpeed /= 10;
            break;
        case 2: // Loop
            if (canChange) {
                nextSpeed = currentSpeed + addSpeed;
                if (currentSpeed == speedMin) currentSpeed = speedMax;
                else if (nextSpeed < speedMin) currentSpeed = speedMin;
                else currentSpeed = nextSpeed;
            }
            currentSpeed /= 10;
            break;
        case 3: //Wave
            if (canChange) {

            }
            break;
        }

        switch (timerMode) {
        case 0: // Static
            currentTimer = timerValue;
            break;
        case 1: // Decrease
            if (canChange) {
                nextTimer = currentTimer + addTimer;
                if (nextTimer > timerMin) currentTimer = nextSpeed;
                else currentTimer = timerMin;
            }
            break;
        case 2: // Loop
            if (canChange) {
                nextTimer = currentTimer + addTimer;
                if (currentTimer == timerMin) currentTimer = timerMax;
                else if (nextTimer < timerMin) currentTimer = timerMin;
                else currentTimer = nextTimer;
            }
            break;
        }

        if (netskip) {
            static auto lastTime = std::chrono::high_resolution_clock::now();
            auto currentTime = std::chrono::high_resolution_clock::now();
            if (std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastTime).count() < dsyncDelay) {
                Global::shouldLagTicks = true;
            } else {
                Global::shouldLagTicks = false;
                lastTime = std::chrono::high_resolution_clock::now();
            }
        }

        float currentGlide = glide / 1000;

        if (debug && canChange) {
            ChatUtils::sendCustomMessage("Current Speed:" + to_string(currentSpeed * 10), "HiveFly");
            if (netskip && !Global::shouldLagTicks) ChatUtils::sendCustomMessage(Utils::combine(YELLOW, "Sending...", RESET), "HiveFly");
            ChatUtils::sendCustomMessage("Current Timer;" + to_string(currentTimer), "HiveFly");
            ChatUtils::sendCustomMessage("Current Glide;" + to_string(currentGlide), "HiveFly");
        }

        state->Velocity.y = currentGlide / (20 / fakeTimer);
        MovementUtils::setSpeed(currentSpeed / (20 / fakeTimer));
        Global::fakeTimer = currentTimer;
        Global::canChange = false;

        bool keyPressed = player->getMoveInputHandler()->isPressed();

        if (!keyPressed) return;
        player->setSprinting(true);
    }

    void onDisabled() override {
        if (!Address::getClientInstance())
            return;

        if (!Address::getClientInstance()->getTimerClass())
            return;

        if (!Address::getLocalPlayer() || !Address::getLocalPlayer()->getStateVector())
            return;

        MovementUtils::setSpeed(0);

        Global::shouldLagTicks = false;
    }
};
