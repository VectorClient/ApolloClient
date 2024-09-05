#pragma once

class SuperHeroFX : public Module
{
public:
    SuperHeroFX(int keybind = Keys::NONE, bool enabled = true) :
        Module("SuperHeroFX", "Visual", "Interface", keybind, enabled)
    {
        //addBool("ProductSans", "ProductSans font", &ProductSansFont);
    }

    struct ParticleData {
        Vector3<float> pos;
        std::string text;
        int duration;
        int colorR;
        int colorG;
        int colorB;
    };

    std::vector<ParticleData> particles;
    bool tracers = false;
    bool mobs = false;
    bool is2D = false;

    std::vector<Actor*> TargetList;
    void createTargetList() {
        auto instance = Address::getClientInstance();
        auto localPlayer = instance->getLocalPlayer();
        auto list = localPlayer->getLevel()->getRuntimeActorList();
        auto lpPos = localPlayer->getStateVector()->Position;
        for (auto* actor : list) {
            if (actor != localPlayer && actor->isAlive() && !actor->isBot()) {
                float dist = localPlayer->getStateVector()->Position.distance(actor->getStateVector()->Position);
                if (dist < 6.f) {
                    TargetList.push_back(actor);
                }
            }
        }
    }

    string FX[5] = {
    "BANG!",
    "ZAP!",
    "SMASH!",
    "PROOF!",
    "BAM!" };

    inline float RandomFloat(float a, float b) {
        float random = ((float)rand()) / (float)RAND_MAX;
        float diff = b - a;
        float r = random * diff;
        return a + r;
    }


    void onEvent(ActorBaseTickEvent* event) override {
        auto player = Address::getLocalPlayer();

        if (player == nullptr || !Address::canUseKeys()) return;

        bool particleAlive = false;

        TargetList.clear();
        createTargetList();

        if (!TargetList.empty()) {
            UIColor color = ColorUtils::Rainbow(2, 1.f, 1, 0);

            auto target = TargetList[0]; // Set the target to TargetList[0]

            if (target->getHurtTime() == 10) {
                ParticleData data;
                data.pos = target->getStateVector()->Position.add(Vector3<float>(RandomFloat(-2.f, 2.f), RandomFloat(-1.f, 1.2f), RandomFloat(-2.f, 2.f)));
                int random = rand() % 5;
                data.text = FX[random];
                data.duration = 80;
                data.colorR = color.r * 255;
                data.colorG = color.g * 255;
                data.colorB = color.b * 255;
                particles.push_back(data);
            }
            for (auto& i : particles) {
                if (0 < i.duration) {
                    particleAlive = true;
                }
            }
        }

        if (!particleAlive) particles.clear();
    }

    void onEvent(ImGuiRenderEvent* event) override {
        auto player = Address::getLocalPlayer();
        if (player == nullptr) return;

        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[5]);

        for (auto& i : particles) {
            //Vec2 textpos = Address::getClientInstance()->WorldToScreen(i.pos);

            Vector2<float> textpos;
            if (!Address::getClientInstance()->WorldToScreen(i.pos, textpos)) continue;    

            if (0 < i.duration) {
                float dist = (i.pos).distance(Address::getClientInstance()->getLevelRender()->getOrigin()) * 3;
                float multiplier = i.duration - 25;
                float alpha;

                if (i.duration <= 15)
                    alpha = i.duration / 15;
                else
                    alpha = 1.f;

                if (0 < textpos.x && 0 < textpos.y) {
                    ImRenderUtil::drawText(textpos, &i.text, UIColor(i.colorR, i.colorG, i.colorB), (25.f / dist), alpha, true);
                    for (char c : (std::string)i.text) {
                        std::string string = Utils::combine(c, "");

                        float charWidth = ImRenderUtil::getTextWidth(&string, 2.3);
                        float charHeight = ImRenderUtil::getTextHeight(2.3);
                        ImRenderUtil::drawShadowSquare(Vector2<float>(textpos.x + charWidth / 2, textpos.y + charHeight / 1.2), 15.f, UIColor(i.colorR, i.colorG, i.colorB), 0.85f, 70.f, 0);

                        //ImRenderUtil::drawText(Vector2<float>(pos2.x + 10, pos2.y + 10), &string, RGBColor, 2.3, 1, true);
                        ImRenderUtil::drawText(Vector2<float>(textpos.x + 10, textpos.y + 10), &i.text, UIColor(i.colorR, i.colorG, i.colorB), (25.f / dist), alpha, true);
                    }
                }
                i.duration--;
                i.pos.y += 0.01;
            }
        }

        ImGui::PopFont();
    }
    
    void onDisabled() override {
        particles.clear();
    }
};