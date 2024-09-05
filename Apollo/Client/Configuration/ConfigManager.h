#pragma once

class ConfigurationManager {
public:
    nlohmann::json GetSettings(std::vector<Setting*> _settings)
    {
        nlohmann::json arr = nlohmann::json::array();

        for (const auto& obj : _settings) {
            nlohmann::json obj_json = nlohmann::json::object();

            obj_json["name"] = obj->name;
            obj_json["type"] = obj->type;

            switch (obj->type)
            {
            case SettingType::BOOL:
                if (!obj->value)
                    break;
                {
                    bool myBool = *reinterpret_cast<bool*>(obj->value);
                    int intBool = myBool ? 1 : 0;

                    obj_json["value"] = intBool;
                }
                
                break;
            case SettingType::ENUM:
                if (!obj->getIterator())
                    break;

                obj_json["value"] = *obj->getIterator();
                break;
            case SettingType::SLIDER:
                if (!obj->value)
                    break;

                obj_json["value"] = *reinterpret_cast<float*>(obj->value);
                break;
            }

            arr.push_back(obj_json);
        }

        return arr;
    }

    void SetSettings(std::vector<Setting*> _settings, nlohmann::json settings)
    {
        for (nlohmann::json::iterator it = settings.begin(); it != settings.end(); ++it)
        {
            nlohmann::json item = it.value();

            std::string itemname = item["name"].get<std::string>();

            for (Setting* setting : _settings)
            {
                if (strcmp(setting->getName().c_str(), itemname.c_str()) == 0)
                {
                    //_logf(combine(item["value"], "\r\n"));
                    //continue;

                    //if (itemname.is_null())
                        //continue;

                    switch (setting->getType())
                    {
                    case SettingType::BOOL:
                        //if (item["value"].get<std::string>() == "false") {
                            //*(bool*)setting->getValue() = 0;
                        //}
                        //else if (item["value"].get<std::string>() == "true") {
                            //*(bool*)setting->getValue() = 1;
                        //}
                        *(bool*)setting->getValue() = item["value"].get<int>();
                        //*(bool*)setting->getValue() = !*(bool*)setting->getValue();
                        break;
                    case SettingType::SLIDER:
                        *(float*)setting->getValue() = item["value"].get<float>();
                        //*reinterpret_cast<float*>(setting->value) = item["value"].get<float>();
                        break;
                    case SettingType::ENUM:
                        int value = item["value"].get<int>();

                        if (value >= 0 && value < setting->getEnumValues().size())
                            *setting->getIterator() = value;
                        break;
                    }
                }
            }
        }
    }

    void SaveConfig(std::string config)
    {
        nlohmann::json top_config;

        nlohmann::json global_config;
        global_config["version"] = "V2";
        global_config["name"] = "Apollo-Client";
        top_config.push_back(global_config);

        for (Module* mod : modules)
        {
            nlohmann::json mod_config;
            mod_config["name"] = mod->getName();
            mod_config["keybind"] = mod->getKeybind();
            mod_config["enabled"] = mod->isEnabled();
            mod_config["visible"] = mod->isVisible();
            mod_config["ingameOnly"] = mod->ingameOnly;
            mod_config["settings"] = GetSettings(mod->getSettings());

            top_config.push_back(mod_config);
        }

        std::ofstream file(FileUtils::getClientPath() + "\\configurations\\" + config + ".json");
        if (file.is_open())
        {
            file << std::setw(4) << top_config;
            file.close();
        }
    }

    void LoadConfig(std::string config)
    {
        std::ifstream file(FileUtils::getClientPath() + "\\configurations\\" + config + ".json");

        if (file.good()) // Check if the file exists and can be opened
        {
            nlohmann::json _config = nlohmann::json::parse(file);

            for (nlohmann::json::iterator it = _config.begin(); it != _config.end(); ++it)
            {
                nlohmann::json item = it.value();

                if (item.contains("version"))
                {
                    //Game::Iris::ClientColor = item["version"].get<int>();
                    //Game::Iris::ClientName = item["name"].get<std::string>();

                    //_logf("Config has global config (loading...)");

                    continue; // skip cuz global config (probably)
                }

                //continue;

                std::string itemname = item["name"].get<std::string>();

                for (Module* mod : modules)
                {
                    if (strcmp(mod->getName().c_str(), itemname.c_str()) == 0)
                    {
                        // correct module so lets unload the json data

                        mod->setKeybind(item["keybind"].get<int>());
                        mod->setEnabled(item["enabled"].get<bool>());
                        //mod->exists = item["exists"].get<bool>();
                        mod->setVisible(item["visible"].get<bool>());
                        mod->ingameOnly = item["ingameOnly"].get<bool>();
                        //mod->requiresIngame = item["requiresIngame"].get<bool>();
                        //mod->showNotifications = item["showNotifications"].get<bool>();

                        SetSettings(mod->getSettings(), item["settings"]);
                    }
                }
            }
            ChatUtils::sendMessage(Utils::combine("Successfully loaded config ", GRAY, config.c_str(), RESET, "!"));
        }
        else
        {
            ChatUtils::sendMessage(Utils::combine("Config not found ", GRAY, config.c_str(), RESET, "!"));
        }
    }
};