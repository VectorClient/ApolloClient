#pragma once

class Circle {
public:
    Vector3<float> position;
    float scale = 0;
    float alpha = 0;

    Circle(Vector3<float> const& position, float scale, float alpha) : position(position), scale(scale), alpha(alpha) {

    }

    void render(float circleScale, float speed, float alphaSpeed, float test) {
        UIColor color = ColorUtils::getClientColor(2.5, 1, 1, 1);

        ImRenderUtil::drawCircle3D(position, scale, 2, color, 8);
        for (int i = 0; i < 16; i++) {
            color.a = (alpha * (1.f - i / 16.f));
            ImRenderUtil::drawCircle3D(position, scale - i / test, 6, color, 8);
        }

        scale += (circleScale - scale) * speed * ImRenderUtil::getDeltaTime();
        alpha = std::fmaxf(alpha - alphaSpeed, 0.0f);
    }

    bool shouldRemove() {
        return alpha < 0.05f;
    }
};

class JumpCircle : public Module
{
public:
    JumpCircle(int keybind = Keys::NONE, bool enabled = false) :
        Module("Jump Circle", "Visual", "Shows invisable targets.", keybind, enabled)
    {
        addSlider("Scale", "idk", &scale, 0.5f, 1.5f, 0.01);
        addSlider("Test", "idk", &test, 0.001f, 400.f, 0.01);
        addSlider("Speed", "idk", &speed, 0.2f, 2, 0.01);
        addSlider("Fade", "idk", &fade, 0.5f, 6, 0.01);
    }

private:
    //std::shared_ptr<FloatSetting> scale = setting("Scale", 1.5f, 0.5f, 2, 2);
    //std::shared_ptr<FloatSetting> test = setting("Test", 30.f, 0.001f, 400.f, 3);
    //std::shared_ptr<FloatSetting> speed = setting("Speed", 1, 0.2f, 2, 2);
    //std::shared_ptr<FloatSetting> fade = setting("Fade", 2, 0.5f, 6, 2);

    float scale = 1.5f;
    float test = 30;
    float speed = 1;
    float fade = 2;

    

    std::vector<Circle> circles;
    bool previousStatus = false;
    int groundTicks = 0;

public:
    void onEvent(ActorBaseTickEvent* event) override {
        if (Address::getLocalPlayer()->isOnGround() && !previousStatus && groundTicks > 5) {
            circles.push_back(Circle(Address::getLocalPlayer()->getRenderPosition(), 0, 1));
        }

        for (int i = 0; i < circles.size(); i++) {
            if (circles[i].shouldRemove()) {
                circles.erase(circles.begin() + i);
            }
        }

        groundTicks = !Address::getLocalPlayer()->isOnGround() ? groundTicks + 1 : 0;
        previousStatus = Address::getLocalPlayer()->isOnGround();
    }

    void onEvent(ImGuiRenderEvent* event) override {
        if (Address::getLocalPlayer() == nullptr || !Address::canUseKeys()) {
            return;
        }

        for (int i = 0; i < circles.size(); i++) {
            circles[i].render(scale, speed / 10, fade / 100, test);
        }
    }
};