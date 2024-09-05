#pragma once

class BindCommand : public Command
{
public:

    BindCommand() : Command("bind", "Bind a module to a key", { "b" })
    {}

    bool execute(std::vector<std::string> cmd) override // bro has no error handling with chat messages imagineeee
    {
        if (cmd.size() < 2)
            return false;

        if (cmd[0] == "all" && cmd[1] == "none")
        {
            for (auto module : modules)
            {
                module->setKeybind(0);
            }
            ChatUtils::sendMessage("Successfully unbound all!");
            return true;
        }

        auto mod = getModuleByName(cmd[0]);
        if (mod == nullptr) {
            ChatUtils::sendMessage(Utils::combine("Invalid Module: ", GRAY, cmd[0].c_str()));
            return false;
        }

        if (cmd[0] == "all" && cmd[1] == "none")
        {
            for (auto module : modules)
            {
                module->setKeybind(0);
            }
            ChatUtils::sendMessage("Successfully unbound all!");
            return true;
        }

        if (strcmp(cmd[1].c_str(), "none") == 0)
        {
            mod->setKeybind(0);
            ChatUtils::sendMessage(Utils::combine("Successfully unbound ", GRAY, mod->getName(), RESET, "!"));
            return true;
        }

        if (cmd[1].length() == 1)
        {
            if (cmd[1][0] > 96 && cmd[1][0] < 122)
                std::transform(cmd[1].begin(), cmd[1].end(), cmd[1].begin(), [](unsigned char c) { return::toupper(c); });
        }

        std::string cmdLower = cmd[1];
        std::transform(cmdLower.begin(), cmdLower.end(), cmdLower.begin(), [](unsigned char c) { return std::tolower(c); });

        for (int i = 0; i < 193; i++)
        {
            std::string keyLower = KeyName[i];
            std::transform(keyLower.begin(), keyLower.end(), keyLower.begin(), [](unsigned char c) { return std::tolower(c); });

            if (cmdLower == keyLower)
            {
                mod->setKeybind(i);
                ChatUtils::sendMessage(Utils::combine("Successfully bound ", GRAY, mod->getName(), RESET, " to ", GRAY, KeyName[i], RESET, "!"));
                return true;
            }
        }

        ChatUtils::sendMessage(Utils::combine("Key not found!"));
        return false;
    }
};
