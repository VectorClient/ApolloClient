#pragma once

enum class SettingType {
    BOOL, SLIDER, ENUM,
};

class Setting {
public:
    std::string name;
    std::string description;
    SettingType type;
    void* value = nullptr;
    float Min = 0.0f;
    float Max = 0.0f;
    float Round = 0.1;
    std::vector<std::string> enumValues;
    int* iterator = nullptr;
    bool render = true;

    // not sure if this is necessary
    ~Setting()
    {
        if (type == SettingType::ENUM)
        {
            delete iterator;
        }

        else if (value)
        {
            delete value;
            value = nullptr;
        }
    }

    Setting(const std::string& name, const std::string& description, float* value, float min, float max, float round, bool render)
        : name(name), description(description), type(SettingType::SLIDER), value(value), Min(min), Max(max), Round(round), render(render) {}

    Setting(const std::string& name, const std::string& description, const std::vector<std::string>& enumValues, int* iterator)
        : name(name), description(description), type(SettingType::ENUM), enumValues(enumValues), iterator(iterator) {}

    Setting(const std::string& name, const std::string& description, void* state, bool render)
        : name(name), description(description), type(SettingType::BOOL), value(state), render(render) {}

    std::string getName() const { return name; }
    std::string getDescription() const { return description; }
    SettingType getType() const { return type; }
    float getMin() const { return Min; }
    float getMax() const { return Max; }
    void* getValue() { return value; }
    float getRound() const { return Round; }
    std::vector<std::string> getEnumValues() const { return enumValues; }
    int* getIterator() const { return iterator; }
    bool shouldRender() const { return render; }

    //had to do dis sorry :/
    float sliderEase = 0;
    float boolScale = 0;
    bool isDragging = false;

    bool enumExtended = false;
    float enumSlide = 0;
    float renderEnumSlide = 0;
};
