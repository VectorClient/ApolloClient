#pragma once

class ComponentRenderBatch;

#include "Tessellator.h"

struct TextMeasureData
{ // Stores information about how the text should be displayed
public:
    TextMeasureData(float size, bool showShadow)
    {
        this->textSize = size;
        this->showShadow = showShadow;
        this->id = 0;
        this->inObject = false;
    };

public:
    float textSize;
    int id;
    bool showShadow;
    bool inObject;
};

struct CaretMeasureData
{
    int Position;
    bool isSingleline;// false|insert

    CaretMeasureData()
    {
        CaretMeasureData(0xFFFFFFFF, true);
    };

    CaretMeasureData(int position, bool singlelines)
    {
        this->Position = position;
        this->isSingleline = singlelines;
    };
};

class TexturePtr // TextureData's class
{
private:
    char pad_0x0[0x18];
    struct PathStruct // FilePath's sturct
    {
        char pad_0x0[0x18]; // The offset for filePath string
        TextHolder filePath; // filePath string
    }
    *ptrToPath;
    char pad_0x0038[0x20];
public:
    TextHolder getFilePath() const // getFilePath from PathStruct
    {
        return ptrToPath->filePath;
    }
};

class ResourceLocation {
private:
    uint64_t type;        //0x0000
    TextHolder filePath;  //0x0008
public:
    ResourceLocation(bool external, std::string filePath) {
        memset(this, 0, sizeof(ResourceLocation));
        this->filePath = filePath;
        if (external) { this->type = 2; }
    };
};

class StringHasher {
public:
    uint64_t hash;
    TextHolder text;
    const StringHasher* unk;

    StringHasher(uint64_t inputhash, std::string inputtext) {
        memset(this, 0x0, sizeof(StringHasher));
        this->hash = inputhash;
        this->text.setText(inputtext);
    }

    StringHasher(const std::string& text) {
        memset(this, 0x0, sizeof(StringHasher));
        this->text.setText(text);
        this->computeHash();
    }

    void computeHash() {
        hash = 0xCBF29CE484222325i64;
        if (this->text.getTextLength() <= 0)
            return;
        char* textP = this->text.getText();
        auto c = *textP;

        do {
            hash = c ^ 0x100000001B3i64 * hash;
            c = *++textP;
        } while (*textP);
    }

    bool operator==(StringHasher& rhs) {
        if (this->text.getText() == rhs.text.getText()) {
            return ((this->unk == &rhs) && (rhs.unk == this));
        }
        return false;
    }
    bool operator!=(StringHasher& rhs) {
        return !(*this == rhs);
    }
    bool operator<(StringHasher& rhs) {
        if (this->hash < rhs.hash) {
            return true;
        }
        if (this->hash <= rhs.hash) {
            return (strcmp(this->text.getText(), rhs.text.getText()) < 0);
        }
        return false;
    }
};

class NinesliceInfo { // NinesliceInfo
private:
    char pad_0x0000[0xFF]; //0x0000
};

class MinecraftUIRenderContext {
public:
    ClientInstance* ClientInstance;
    ScreenContext* ScreenContext;

private:
    virtual void Destructor();
public:
    virtual float getLineLength(FontRepos* font, std::string* str, float measureCalculation, bool calculateWidth);
private:
    virtual int getTextAlpha();
    virtual void setTextAlpha();
    virtual void drawDebugText();
public:
    virtual void drawText(FontRepos* font, const float* pos, std::string* str, const float* colour, float, float alinM, const TextMeasureData* textdata, const CaretMeasureData* caretdata);
    virtual void flushText(float flushDelta);
public:
    virtual void drawImage(TexturePtr*, Vector2<float>* ImagePos, Vector2<float>* ImageDimension, Vector2<float> const& uvPos, Vector2<float> const& uvSize);
    virtual void drawNineslice(TexturePtr* const& texturePtr, NinesliceInfo NinesliceInfo);
    virtual void flushImages(UIColor& color, float opacity, class StringHasher& hashedString);
    virtual void beginSharedMeshBatch(uintptr_t ComponentRenderBatch);
    virtual void endSharedMeshBatch(float timeSinceLastFlush);
public:
    virtual void drawRectangle(Vector4<float> const& rect, UIColor const& colour, float alpha, int width);
    virtual void fillRectangle(Vector4<float> const& rect, UIColor const& colour, float alpha);
public:
    virtual auto increaseStencilRef() -> void {};
    virtual auto decreaseStencilRef() -> void {};
    virtual auto resetStencilRef() -> void {};
    virtual auto fillRectangleStencil(Vector4<float> const& position) -> void {};
    virtual auto enableScissorTest(Vector4<float> const& position) -> void {};
    virtual auto disableScissorTest() -> void {};
    virtual auto setClippingRectangle(Vector4<float> const& position) -> void {};
    virtual auto setFullClippingRectangle() -> void {};
    virtual auto saveCurrentClippingRectangle() -> void {};
    virtual auto restoreSavedClippingRectangle() -> void {};
    virtual auto getFullClippingRectangle() -> int {};
    virtual auto updateCustom(uintptr_t a1) -> void {};
    virtual auto renderCustom(uintptr_t a1, int a2, Vector4<float> const& position) -> void {};
    virtual auto cleanup() -> void {};
    virtual auto removePersistentMeshes() -> void {};
    virtual auto getTexture(class TexturePtr* texture, ResourceLocation* resourceLocation) -> int {};
    virtual auto getZippedTexture(class TexturePtr* Path, class TexturePtr* ResourceLocation, bool a3) -> int {};
    virtual auto unloadTexture(class TexturePtr* ResourceLocation) -> void {};
    virtual auto getUITextureInfo(class TexturePtr* ResourceLocation, bool a2) -> int {};
    virtual auto touchTexture(class TexturePtr* ResourceLocation) -> void {};
    virtual auto getMeasureStrategy(Vector2<float> const&) -> int {};
    virtual auto snapImageSizeToGrid(Vector2<float> const&) -> void {};
    virtual auto snapImagePositionToGrid(Vector2<float> const&) -> void {};
    virtual auto notifyImageEstimate(ULONG) -> void {};
};