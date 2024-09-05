#pragma once

#pragma region Command imports

#include "Commands/BindCommand.h"
#include "Commands/ConfigCommand.h"
#include "Commands/TestCommand.h"

#pragma endregion

class CommandManager
{
public:
    void InitializeCommands()
    {
        // CommandsList

        /*

        .compass (no cuz gui)
        .nick (streamner mode yk?)
        .home (set/delete/goto/list) (home)
        .discord
        .spawn (teleport to world spawn)

        */

        //commands.push_back(new LevelCommand());
        commands.push_back(new BindCommand());
        commands.push_back(new ConfigCommand());
        commands.push_back(new TestCommand());

    }

    ~CommandManager()
    {
        for (auto cmd : commands)
            delete cmd;
    }

    bool sendCommand(const std::string& command)
    {
        Command* cur = nullptr;

        if (command.length() < 2)
            return true;

        std::string argString = command.substr(1);
        std::string commandName = "";
        std::vector<std::string> arguments = turnToVec(argString, commandName);

        for (auto cmd : commands)
        {
            if (cmd->name == commandName)
            {
                cur = cmd;
                break;
            }

            for (const auto& alias : cmd->aliases)
            {
                if (alias == commandName)
                {
                    cur = cmd;
                    break;
                }
            }
        }

        if (!cur)
        {
            ChatUtils::sendMessage(xorstr_("Invalid Command!"));
            return true;
        }

        cur->execute(arguments);

        return true;
    }

    std::vector<Command*> commands;

private:
    std::vector<std::string> turnToVec(std::string str, std::string& commandName)
    {
        std::vector<std::string> ret;
        size_t pos = 0;
        bool inQuotes = false;

        while (pos < str.length())
        {
            if (str[pos] == '"')
            {
                inQuotes = !inQuotes;
                pos++;
                continue;
            }

            if (!inQuotes && str[pos] == ' ')
            {
                pos++;
                continue;
            }

            size_t start = pos;
            if (inQuotes)
            {
                pos = str.find('"', pos);
                if (pos == std::string::npos)
                    break;
            }
            else
            {
                pos = str.find(' ', pos);
                if (pos == std::string::npos)
                    pos = str.length();
            }

            std::string arg = str.substr(start, pos - start);
            ret.push_back(arg);
            pos++;
        }

        if (ret.size())
        {
            commandName = ret[0];
            std::transform(commandName.begin(), commandName.end(), commandName.begin(), ::tolower);
            ret.erase(ret.begin());
        }

        return ret;
    }
};