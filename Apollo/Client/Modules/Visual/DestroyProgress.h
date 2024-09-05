#pragma once

class DestroyProgress : public Module
{
public:
    DestroyProgress(int keybind = 7, bool enabled = true) :
        Module("Destroy Progress", "Visual", "Render destroy progress", keybind, enabled)
    {
        addSlider("LineWidth", "The line width of box", &lineWidth, 0, 5, 0.01);
        addSlider("Opacity", "The opacity of box", &opacity, 0, 100, 1);
    }

    float lineWidth = 1.f;
    float opacity = 75;

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        GameMode* gamemode = player->getGamemode();
        if (!gamemode) return;

        float progress = gamemode->getBreakProgress();
        static float animatedProgress = 0;
        animatedProgress = Math::animate(progress, animatedProgress, ImRenderUtil::getDeltaTime() * 25);

        if (0 < progress) {
            Vector3<int> destroyingBlockPos = player->getLevel()->getHitResult()->IBlockPos;
            float scale = animatedProgress / 2.f;
            Vector3<float> lower = destroyingBlockPos.ToFloat().add(Vector3<float>(0.5f - scale, 0.5f - scale, 0.5f - scale));
            Vector3<float> upper = destroyingBlockPos.ToFloat().add(Vector3<float>(0.5f + scale, 0.5f + scale, 0.5f + scale));
            UIColor color = UIColor((int)(animatedProgress * 255), (int)((1 - animatedProgress) * 255), 0, opacity);
            UIColor lineColor = UIColor(color.r - 40, color.g - 40, color.b, color.a);
            ImRenderUtil::drawBox(lower, upper, color, lineColor, lineWidth, true, true);
        }
        else animatedProgress = 0;
    }
};