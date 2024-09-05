#pragma once

class Teams : public Module
{
public:
    Teams(int keybind = Keys::NONE, bool enabled = false) :
        Module("Teams", "Combat", "Automatically friends everyone on your team", keybind, enabled)
    {

    }

    std::string getModeName() override {
        return " Color";
    }
};