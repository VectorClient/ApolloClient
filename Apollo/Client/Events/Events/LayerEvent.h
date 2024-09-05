#pragma once

class LayerEvent : public Event
{
public:
    LayerEvent(ScreenView* guilayer)
    {
        GuiLayer = guilayer;
    }

    EventType getType() const override { return EventType::Layer; }
    ScreenView* GuiLayer;
};