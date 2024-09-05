#pragma once

class NotificationsModule : public Module
{
public:
    NotificationsModule(int keybind = Keys::NONE, bool enabled = true) :
        Module("Notifications", "Visual", "Renders the client game notifications", keybind, enabled)
    {

    }
};