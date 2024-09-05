#pragma once

class HoverTextRenderer
{
private:
    char pad_0000[24]; //0x0000
public:
    std::string content; //0x0018
    std::string filteredContenxt; //0x0038
    Vector2<float> cursorPosition; //0x0058
    Vector2<float> offset; //0x0060
    Vector2<float> boxDimensions; //0x0068
};