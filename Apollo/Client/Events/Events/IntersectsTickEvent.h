#pragma once

class IntersectsTickEvent : public Event
{
public:
    IntersectsTickEvent() {}

    EventType getType() const override { return EventType::IntersectsTick; }
};