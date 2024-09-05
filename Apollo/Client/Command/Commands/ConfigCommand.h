#pragma once

class ConfigCommand : public Command
{
public:

    ConfigCommand() : Command("config", "Manage your configurations", { "c", "configuration" })
    {}

    bool execute(std::vector<std::string> cmd) override
    {
        if (cmd.size() == 0)
            return false;

        static std::string SavedConfig = "default";

        //.config save
        if (strcmp(cmd[0].c_str(), "save") == 0 || strcmp(cmd[0].c_str(), "s") == 0)
        {
            if (cmd.size() == 2)
            {
                SavedConfig = cmd[1];
                SaveCfg(cmd[1]);
            }
            else
            {
                SaveCfg(SavedConfig);
            }
        }

        //.config load
        if (strcmp(cmd[0].c_str(), "load") == 0 || strcmp(cmd[0].c_str(), "l") == 0)
        {
            if (cmd.size() == 2)
            {
                SavedConfig = cmd[1];
                ConfigMgr.LoadConfig(cmd[1]);
                Global::Notification.addNotification("Client:", Utils::combine("Loaded configuration ", cmd[1], "!"), 2.f);
            }
            else
            {
                ConfigMgr.LoadConfig(SavedConfig);
            }
        }

        return false;
    }

    void SaveCfg(std::string cfg)
    {
        ConfigMgr.SaveConfig(cfg);
        ChatUtils::sendMessage(Utils::combine("Successfully saved configuration ", GRAY, cfg.c_str(), RESET, "!"));
        Global::Notification.addNotification("Client:", Utils::combine("Saved configuration ", cfg.c_str(), "!"), 2.f);
    }
};
