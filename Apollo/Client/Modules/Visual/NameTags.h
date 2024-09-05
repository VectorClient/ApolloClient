#pragma once

class NameTags : public Module
{
public:
    NameTags(int keybind = 7, bool enabled = true) :
        Module("NameTags", "Visual", "Displays peoples names through walls", keybind, enabled)
    {
        addSlider("Range", "The range entities has to be on to render", &range, 1, 200);
        addBool("Self", "Renders the esp on localplayer", &selfrender);
        addSlider("Opacity", "The opacity of the arraylist", &opacity, 0, 100);
        addSlider("Rounding", "The opacity of the arraylist", &rounding, 0, 100);
        addBool("Blur", "Add blur on targethud", &blur);
        addBool("Ease", "Lerps the nametags position", &ease);
        addBool("Shadow", "Render shadows for the rectangle", &shadow);
        addBool("Text Shadow", "Render shadows for the text", &textShadow);
        addBool("Fill Shadow", "Fill the rectangle with shadow", &shadowfilled);
        addSlider("Shadow strenght", "The strenght of the shadow", &shadowstrenght, 0, 200);
    }

    float range = 70;
    bool selfrender = true;
    bool ease = false;
    bool blur = true;
    bool shadow = true;
    bool textShadow = true;
    bool shadowfilled = true;
    float shadowstrenght = 150;
    float opacity = 50;
    float rounding = 0;


    void drawNametags(Actor* entity)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[3]);
        Player* player = Address::getLocalPlayer();

        if (!entity || !Address::canUseKeys())
            return;

        AABBShapeComponent* shape = entity->getAABBShape();
        if (!shape || IsBadReadPtr(shape, sizeof(AABBShapeComponent))) return;

        Vector3<float> renderPos = entity->getRenderPosition().submissive(Vector3<float>(0, -1.f, 0));

        float distance = renderPos.distance(Address::getClientInstance()->getLevelRender()->getOrigin());

        float textSize = fmax(0.8f, 1.35f / distance);

        Vector2<float> output1;
        if (!Address::getClientInstance()->WorldToScreen(renderPos, output1)) return;

        static float outputX = output1.x;
        static float outputY = output1.y;

        outputX = Math::animate(output1.x, outputX, ImRenderUtil::getDeltaTime() * 10.f);
        outputY = Math::animate(output1.y, outputY, ImRenderUtil::getDeltaTime() * 10.f);

        Vector2<float> outputEase = Vector2<float>(outputX, outputY);

        if (ease) {
            outputEase = Vector2<float>(output1.x, outputY);;
        }
        else {
            outputEase = output1;
        }

        std::string user(entity->getNametag()->c_str());
        user = Utils::sanitize(user);
        user = user.substr(0, user.find('\n'));

        float textWidth = ImRenderUtil::getTextWidth(&user, textSize);
        float textHeight = ImRenderUtil::getTextHeight(textSize);
        Vector2<float> textPos = Vector2<float>(outputEase.submissive(Vector2<float>(textWidth / 2, 0)));

        Vector4<float> rectPos;
        rectPos.x = textPos.x - 6.f * textSize;
        rectPos.y = textPos.y - 2.f * textSize;
        rectPos.z = textPos.x + textWidth + 6.f * textSize;
        rectPos.w = textPos.y + textHeight + 2.f * textSize;

        if (blur) 
            ImRenderUtil::Blur(rectPos, 5, rounding / 10);

        ImRenderUtil::fillRectangle(rectPos, UIColor(0, 0, 0), opacity / 100, rounding / 10);
        if (shadow)
        ImRenderUtil::fillShadowRectangle(rectPos, UIColor(0, 0, 0), (opacity / 100) * 2, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground);

        ImRenderUtil::drawText(outputEase.submissive(Vector2<float>(textWidth / 2, 0)), &user, UIColor(255, 255, 255), textSize, 1, textShadow);
        ImGui::PopFont();
    }

    void drawClientNametag(Actor* entity)
    {
        ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[2]);
        Player* player = Address::getLocalPlayer();

        if (!entity || !Address::canUseKeys())
            return;

        AABBShapeComponent* shape = entity->getAABBShape();
        if (!shape || IsBadReadPtr(shape, sizeof(AABBShapeComponent))) return;

        Vector3<float> renderPos = entity->getRenderPosition().submissive(Vector3<float>(0, -1.f, 0));

        float distance = renderPos.distance(Address::getClientInstance()->getLevelRender()->getOrigin());

        float textSize = fmax(0.97f, 1.03f / distance);

        Vector2<float> output1;
        if (!Address::getClientInstance()->WorldToScreen(renderPos, output1)) return;

        static float outputX = output1.x;
        static float outputY = output1.y;

        outputX = Math::animate(output1.x, outputX, ImRenderUtil::getDeltaTime() * 10.f);
        outputY = Math::animate(output1.y, outputY, ImRenderUtil::getDeltaTime() * 10.f);

        Vector2<float> outputEase = Vector2<float>(outputX, outputY);

        if (ease) {
            outputEase = Vector2<float>(outputX, outputY);;
        }
        else {
            outputEase = output1;
        }

        std::string user(entity->getNametag()->c_str());
        user = Utils::sanitize(user);
        user = user.substr(0, user.find('\n'));

        std::string connectedClient = Utils::combine("[", "Apollo", "]");

        std::string clientUser = "";

        if (Authentication::getUsername() == "Solar" || Authentication::getUsername() == "Tozic" || Authentication::getUsername() == "Lenovo") {
            clientUser = "Tozic";
        }

        if (Authentication::getUsername() == "AntiBanCheat") {
            clientUser = "ABC";
        }

        if (Authentication::getUsername() == "xperi") {
            clientUser = "Dark";
        }

        std::string connectedUser = Utils::combine(" (", clientUser, ")");

        std::string space = " ";

        std::string text = Utils::combine(user, " ", connectedClient, "  ", connectedUser);

        float spaceWidth = ImRenderUtil::getTextWidth(&space, textSize);
        float usernameWidth = ImRenderUtil::getTextWidth(&user, textSize);
        float cleintNameWidth = ImRenderUtil::getTextWidth(&connectedClient, textSize);
        float cleintUserWidth = ImRenderUtil::getTextWidth(&connectedUser, textSize);

        float entireTextWidth = ImRenderUtil::getTextWidth(&text, textSize);
        float entireTextHeight = ImRenderUtil::getTextHeight(textSize);

        Vector2<float> textPos = Vector2<float>(outputEase.submissive(Vector2<float>(entireTextWidth / 2, 0)));

        Vector4<float> rectPos;
        rectPos.x = textPos.x - 6.f * textSize;
        rectPos.y = textPos.y - 2.f * textSize;
        rectPos.z = textPos.x + entireTextWidth + 6.f * textSize;
        rectPos.w = textPos.y + entireTextHeight + 2.f * textSize;

        if (blur)
            ImRenderUtil::Blur(rectPos, 5, rounding / 10);

        Vector2<float> usernamePos(entireTextWidth / 2, 0);
        Vector2<float> cleintNamePos((entireTextWidth / 2) - usernameWidth - spaceWidth, 0);
        Vector2<float> connectedUserPos((entireTextWidth / 2) - usernameWidth - spaceWidth - cleintUserWidth - spaceWidth - spaceWidth, 0);

        ImRenderUtil::fillRectangle(rectPos, UIColor(0, 0, 0), opacity / 100, rounding / 10);
        if (shadow)
            ImRenderUtil::fillShadowRectangle(rectPos, UIColor(0, 0, 0), 0.97f, shadowstrenght, shadowfilled ? ImDrawFlags_None : ImDrawFlags_ShadowCutOutShapeBackground, rounding / 10);

        ImRenderUtil::drawText(outputEase.submissive(usernamePos), &user, UIColor(232, 243, 242), textSize, 1, textShadow);
        ImRenderUtil::drawText(outputEase.submissive(cleintNamePos), &connectedClient, UIColor(83, 89, 233), textSize, 1, textShadow);
        ImRenderUtil::drawText(outputEase.submissive(connectedUserPos), &connectedUser, UIColor(170, 170, 170), textSize, 1, textShadow);
        ImGui::PopFont();
    }

    void onEvent(ImGuiRenderEvent* event) override
    {
        Player* player = Address::getLocalPlayer();
        if (!player || !Address::canUseKeys())
            return;

        //if (selfrender) {
            //drawSelfESP();
        //}

        if (selfrender) {
            drawClientNametag(player);
        }
       

        auto list = Address::getLocalPlayer()->getLevel()->getRuntimeActorList();

        for (auto* actor : list) {
            if (actor != player && actor->isAlive() && !actor->isBot()) {
                drawNametags(actor);
            }
        }
    }
};
