#pragma once

class ViewModel : public Module
{
public:
    ViewModel(int keybind = Keys::NONE, bool enabled = false) :
        Module("Viewmodel", "Visual", "Move your item's postion", keybind, enabled)
    {
        addSlider("PosX", "Changes bobhurt position", &viewhurtItemX, -300, 300);
        addSlider("PosY", "Changes bobhurt position", &viewhurtItemY, -300, 300);
        addSlider("PosZ", "Changes bobhurt position", &viewhurtItemZ, -300, 300);

        addSlider("Rot Angle", "Changes bobhurt rotation angle", &viewhurtItemRotAngle, 0, 360);

        addSlider("RotX", "Changes bobhurt rotation", &viewhurtItemRotX, 0, 360);
        addSlider("RotY", "Changes bobhurt rotation", &viewhurtItemRotY, 0, 360);
        addSlider("RotZ", "Changes bobhurt rotation", &viewhurtItemRotZ, 0, 360);
    }

    float viewhurtItemX = 0.3;
    float viewhurtItemY = -0.1;
    float viewhurtItemZ = -1.f;

    float viewhurtItemRotAngle = 0;
    float viewhurtItemRotX = 0;
    float viewhurtItemRotY = 0;
    float viewhurtItemRotZ = 0;

    void onEvent(ViewBobbingTickEvent* event) override
    {
        glm::mat4& matrix = *event->Matrix;

        matrix = glm::translate<float>(matrix, glm::vec3(viewhurtItemX / 100, viewhurtItemY / 100, viewhurtItemZ / 100));

        if (viewhurtItemRotAngle != 0 || viewhurtItemRotX != 0 || viewhurtItemRotY != 0 || viewhurtItemRotZ != 0)
        {
            matrix = glm::rotate<float>(matrix, viewhurtItemRotAngle, glm::vec3(viewhurtItemRotX, viewhurtItemRotY, viewhurtItemRotZ));
        }
    }
};