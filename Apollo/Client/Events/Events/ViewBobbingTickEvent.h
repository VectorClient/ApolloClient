#pragma once

class ViewBobbingTickEvent : public Event
{
public:
    ViewBobbingTickEvent(glm::mat4* matrix)
    {
        Matrix = matrix;
    }

    EventType getType() const override { return EventType::ViewBobbingTick; }
    glm::mat4* Matrix;
};