#pragma once

class ESP : public Module
{
public:
    ESP(int keybind = 7, bool enabled = true) :
        Module("ESP", "Visual", "Draw basic boxes around entities", keybind, enabled)
    {
        addEnum("Style", "The ESP style", { "2D", "Corners", "3D" }, &style);
        addSlider("Range", "The range entities has to be on to render", &range, 1, 200);
        addSlider("Thickness", "The thickness of the esp width", &thickness, 0, 20);
        addSlider("Alpha", "The opacity of the esp", &alpha, 0, 1);
        addSlider("Rounding", "The rounding of the esp", &rounding, 0, 10);
        addBool("Self", "Renders the esp on localplayer", &selfrender);
        addBool("Ease", "easing", &ease);
    }

    int style = 0;
    float thickness = 2;
    float alpha = 1;
    float rounding = 0;
    float range = 40;
    bool selfrender = true;
    bool ease = false;

    void draw2DESP(Actor* entity)
    {
        Player* player = Address::getLocalPlayer();

        if (!entity || !Address::canUseKeys())
            return;

        AABBShapeComponent* shape = entity->getAABBShape();
        if (!shape || IsBadReadPtr(shape, sizeof(AABBShapeComponent))) return;

        Vector3<float> renderPos = entity->getRenderPosition();
        float distance = renderPos.distance(Address::getClientInstance()->getLevelRender()->getOrigin());

        if (distance > 150)
            return;

        Vector3<float> entDims = { shape->Hitbox.x, shape->Hitbox.y, shape->Hitbox.x };

        if (entDims.x != 0.6f || entDims.y != 1.8f)
            return;

        Vector3<float> position = entity->getRenderPosition();
        Vector3<float> entLower = position.add(Vector3<float>(-0.35, -1.7, -0.35));
        Vector3<float> entUpper = position.add(Vector3<float>(0.35, 0.4, 0.35));
        Vector3<float> entPos = entity->getRenderPosition();

        Vector3<float> origin = Address::getClientInstance()->getLevelRender()->getOrigin();
        float dist = origin.distance(entPos);

        Vector2<float> output1, output2;
        if (!Address::getClientInstance()->WorldToScreen(entLower, output1) || !Address::getClientInstance()->WorldToScreen(entUpper, output2)) return;

        std::vector<Vector3<float>> aabbArr;
        for (float x : { entLower.x, entUpper.x })
        {
            for (float y : { entLower.y, entUpper.y })
            {
                for (float z : { entLower.z, entUpper.z })
                {
                    aabbArr.push_back({ x, y, z });
                }
            }
        }

        std::vector<Vector2<float>> scrPoints;
        for (int i = 0; i < aabbArr.size(); i++)
        {
            Vector2<float> scrPoint;
            if (Address::getClientInstance()->WorldToScreen(aabbArr[i], scrPoint))
                scrPoints.push_back(scrPoint);
        }

        if (scrPoints.empty())
            return;

        Vector4<float> boundingRect = { scrPoints[0].x, scrPoints[0].y, scrPoints[0].x, scrPoints[0].y };
        for (const auto& point : scrPoints)
        {
            boundingRect.x = std::fmin(boundingRect.x, point.x);
            boundingRect.y = std::fmin(boundingRect.y, point.y);
            boundingRect.z = std::fmax(boundingRect.z, point.x);
            boundingRect.w = std::fmax(boundingRect.w, point.y);
        }

        float thickness2 = fmax(1.f, 1.5f / std::fmax(1.f, origin.distance(entPos)));
        thickness2 *= thickness / 10; // Here is the thickness level

        ImRenderUtil::drawRoundRect(boundingRect, 0, rounding, UIColor(0, 0, 0), 0.75f * alpha, thickness2 + 2.f);
        ImRenderUtil::drawRoundRect(boundingRect, 0, rounding, UIColor(255, 255, 255), 1.f * alpha, thickness2);
    }

    void draw3dESP(Actor* entity, Vector3<float> renderPos) {
        Player* player = Address::getLocalPlayer();

        if (!entity || !Address::canUseKeys())
            return;

        AABBShapeComponent* shape = entity->getAABBShape();
        if (!shape || IsBadReadPtr(shape, sizeof(AABBShapeComponent))) return;

        float distance = renderPos.distance(Address::getClientInstance()->getLevelRender()->getOrigin());

        if (distance > 150)
            return;

        Vector3<float> lower = shape->PosLower;
        Vector3<float> upper = shape->PosUpper;

        ImRenderUtil::drawBox(renderPos.add(Vector3<float>(-0.37, -1.6, -0.37)), renderPos.add(Vector3<float>(0.37, 0.2, 0.37)), UIColor(255, 255, 255), ColorUtils::getClientColor(2, 1, 1, 1), (float)fmax((thickness / 10) + 2.f, 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(renderPos))), false, true);
    }

    void onEvent(ImGuiRenderEvent* event) override
    {
        Player* player = Address::getLocalPlayer();
        if (!player || !Address::canUseKeys())
            return;

        if (selfrender && !Global::stopESP) {
            Vector3<float> position = player->getRenderPosition();

            static float outputY = position.y;

            outputY = Math::animate(position.y, outputY, ImRenderUtil::getDeltaTime() * 25.f);

            if (ease) {
                position.y = outputY;
            }

            if (style == 0) {
                draw2DESP(player);
            }

            if (style == 1) {
                ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(0, 0, 0), (float)fmax((thickness / 10) + 2.f, 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(player->getRenderPosition()))));
                if (player->getHurtTime() >= 1) {
                    ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(255, 0, 0).lerp(UIColor(255, 255, 255), ImRenderUtil::getDeltaTime() * 10.f), (float)fmax((thickness / 10), 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(player->getRenderPosition()))));
                }
                else {
                    ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(255, 255, 255), (float)fmax((thickness / 10), 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(player->getRenderPosition()))));
                }
            }

            if (style == 2) {
                draw3dESP(player, position);
            }
        }

        auto list = Address::getLocalPlayer()->getLevel()->getRuntimeActorList();

        for (auto* actor : list) {
            if (actor != Address::getLocalPlayer() && actor->isAlive() && !actor->isBot()) {
                Vector3<float> position = actor->getRenderPosition();

                static float outputY = position.y;

                outputY = Math::animate(position.y, outputY, ImRenderUtil::getDeltaTime() * 25.f);

                if (ease) {
                    position.y = outputY;
                }

                if (style == 0) {
                    draw2DESP(actor);
                }
                
                if (style == 1) {
                    ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(0, 0, 0), (float)fmax((thickness / 10) + 2.f, 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(actor->getRenderPosition()))));
                    if (actor->getHurtTime() >= 1) {
                        ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(255, 0, 0).lerp(UIColor(255, 255, 255), ImRenderUtil::getDeltaTime() * 10.f), (float)fmax((thickness / 10), 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(actor->getRenderPosition()))));
                    }
                    else {
                        ImRenderUtil::drawCorners(position.add(Vector3<float>(-0.43, -1.7, -0.43)), position.add(Vector3<float>(0.43, 0.4, 0.43)), UIColor(255, 255, 255), (float)fmax((thickness / 10), 0.7 / (float)fmax(1, Address::getLocalPlayer()->getRenderPosition().distance(actor->getRenderPosition()))));
                    }
                }

                if (style == 2) {
                    draw3dESP(actor, position);
                }
            }
        }
    }
};
