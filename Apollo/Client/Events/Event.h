#pragma once

enum EventType
{
    ActorBaseTick,
    ContainerTick,
    ImGuiRender,
    IntersectsTick,
    Keyboard,
    Layer,
    Mouse,
    MouseScroll,
    PacketSend,
    RenderContext,
    ViewBobbingTick,
};

class Event
{
public:
    virtual EventType getType() const = 0; // get the Event type.
    bool* cancelled; // Canceled bool to stop from sending.
};

// Include Events
#pragma region Events

#include "Events/ActorBaseTickEvent.h"
#include "Events/ContainerTickEvent.h"
#include "Events/ImGuiRenderEvent.h"
#include "Events/IntersectsTickEvent.h"
#include "Events/KeyboardEvent.h"
#include "Events/LayerEvent.h"
#include "Events/MouseEvent.h"
#include "Events/MouseScrollEvent.h"
#include "Events/PacketEvent.h"
#include "Events/RenderContextEvent.h"
#include "Events/ViewBobbingTickEvent.h"

#pragma endregion

// CallBack Event
template<typename EventT>
void CallBackEvent(EventT* event) // CallBack the event for modules and others etc.
{
    for (auto& module : modules)
    {
        if (isRunning)
        {
            if (module->isEnabled() or module->getCallWhenDisabled())
            {
                module->onEvent(event);
            }
        }
    }
}