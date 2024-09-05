#pragma once

// Includes
//#include "../../SDK/Classes/Render/ScreenView.h"
#include "../../SDK/Classes/Render/MinecraftUIRenderContext.h"
#include "../../SDK/Classes/Render/UIControl.h"
#include "../../SDK/Classes/Render/HoverTextRenderer.h"
#include "../../SDK/Classes/Render/UILayer.h"

// Declare a void pointer(Empty object) called onRender which is the CallBack for renderDetourHook.
void* onRender; // Defined an orignal for RenderDetour.

void* onHoverRenderHook;
__int64* onDrawText;
__int64* onDrawImage;
__int64* onDrawNineSlice;

// Declare a variable called layerCounter and initialize it to 0
int layerCounter = 0; // Defined an orignal for RenderDetour.

#pragma region HoverRender
void HoverRenderDetour(HoverTextRenderer* a1, MinecraftUIRenderContext* renderContext, ClientInstance* client, UIControl* owner, int pass, Vector4<float>* renderAABB) {
    /*if (getModuleByName("interface")->isEnabled()) {
        Global::info.show = true;
        Global::info.text = a1->filteredContenxt;
        Global::info.pos = a1->cursorPosition;
        Global::info.pos.x *= client->getGuiData()->getScale();
        Global::info.pos.y *= client->getGuiData()->getScale();
    }
    else {

    }

    // Inside our function we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
    Memory::CallFunc<void, HoverTextRenderer*, MinecraftUIRenderContext*, ClientInstance*, UIControl*, int, Vector4<float>*>(
        onHoverRenderHook, a1, renderContext, client, owner, pass, renderAABB);*/
}
#pragma endregion
#pragma region onDrawText

// MinecraftUIRenderContext, FontRepos, TextPos, TextHolder, UIColor, Alpha, AlinM, TextMeasureData, CaretMeasureData
void onDrawTextDetour(MinecraftUIRenderContext* _this, FontRepos* font, Vector4<float> pos, std::string* str, UIColor const& colour, float alpha, float alinM, TextMeasureData const& textdata, CaretMeasureData const& caretdata) {
    Address::Core::Font = font;

    // Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
    Memory::CallFunc<void, MinecraftUIRenderContext*, FontRepos*, Vector4<float>, std::string*, UIColor const&, float, float, TextMeasureData const&, CaretMeasureData const&>(
        onDrawText, _this, font, pos, str, colour, alpha, alinM, textdata, caretdata);
}

#pragma endregion
#pragma region onDrawImage

// MinecraftUIRenderContext, TexturePtr, ImagePos, ImageDimesnion, UvPos, UvSize
void onDrawImageDetour(MinecraftUIRenderContext* ctx, TexturePtr* path, Vector2<float>& ImagePos, Vector2<float>& a3, Vector2<float>& a4, Vector2<float>& a5) {
    
    if (strcmp(path->getFilePath().getText(), "textures/ui/title") == 0) {
        //RenderUtils::renderImage(FileUtils::getResourcePath() + "\\Title.png", Vector2<float>(ImagePos.x, ImagePos.y - 30), Vector2<float>(a3.x, a3.y + 100));
        //RenderUtils::flushImage(ColorUtils::Rainbow(2, 1, 1, 1), 1);
        return;
    }

    if (strcmp(path->getFilePath().getText(), "textures/ui/selected_hotbar_slot") == 0) {
        static float lerpedPos = ImagePos.x;
        lerpedPos = Math::animate(ImagePos.x, lerpedPos, ImRenderUtil::getDeltaTime() * 10.f);
        ImagePos.x = lerpedPos;

        //RenderUtils::fillRectangle(Vector4<float>(ImagePos.x + 1, ImagePos.y + 1, ImagePos.x + a3.x - 1, ImagePos.y + a3.y - 1), UIColor(0, 0, 0), 0.5f);
        //RenderUtils::fillRectangle(Vector4<float>(ImagePos.x + 1, ImagePos.y + 1, ImagePos.x + a3.x - 1, ImagePos.y + 1.5f), ColorUtils::Rainbow(5, 1, 1, 1), 1.f);
        //return;
    }

    const char* prefix = "textures/ui/hotbar_";
    int prefixLen = strlen(prefix);
    if (strncmp(path->getFilePath().getText(), prefix, prefixLen) == 0) {
        //RenderUtils::fillRectangle(Vector4<float>(ImagePos.x, ImagePos.y, ImagePos.x + a3.x, ImagePos.y + a3.y), UIColor(0, 0, 0), 0.5f);
        //return;
    }
    
    // Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
    Memory::CallFunc<void*, MinecraftUIRenderContext*, TexturePtr*, Vector2<float>&, Vector2<float>&, Vector2<float>&, Vector2<float>&>(
        onDrawImage, ctx, path, ImagePos, a3, a4, a5);
}

#pragma endregion
#pragma region onDrawNineSlice

// MinecraftUIRenderContext, TexturePtr, nineSliceInfo
void onDrawNineSliceDetour(MinecraftUIRenderContext* _this, TexturePtr* texturePtr, NinesliceInfo* nineSliceInfo) {
    float* x = reinterpret_cast<float*>((__int64)nineSliceInfo);
    float* y = reinterpret_cast<float*>((__int64)nineSliceInfo + 0x4);
    float* z = reinterpret_cast<float*>((__int64)nineSliceInfo + 0x60); // funny cuh offset
    float* w = reinterpret_cast<float*>((__int64)nineSliceInfo + 0x64);
    Vector4<float> position(*x, *y, *z, *w);

    if (strcmp(texturePtr->getFilePath().getText(), "textures/ui/hotbar_start_cap") == 0 || (strcmp(texturePtr->getFilePath().getText(), "textures/ui/hotbar_end_cap") == 0)) {
        return;
    }

    // Inside our functiion we're calling the original code that was there/the original function we hooked so the games behavior doesn't change.
    Memory::CallFunc<void*, MinecraftUIRenderContext*, TexturePtr*, NinesliceInfo*>(
        onDrawNineSlice, _this, texturePtr, nineSliceInfo);
}

#pragma endregion

void renderDetourHook(ScreenView* __this, MinecraftUIRenderContext* ctx) { // ScreenContext, MinecraftUIRenderContext
    Address::Core::RenderContext = ctx;
    Address::Core::ClientInstance = ctx->ClientInstance;

    RenderUtils::Initialize(ctx);

    Global::LayerName = __this->tree->root->GetName();

    LayerEvent layerEvent{ __this };
    layerEvent.cancelled = nullptr;
    CallBackEvent(&layerEvent);

    RenderContextEvent event{};
    event.cancelled = nullptr;
    CallBackEvent(&event);

    auto vtable = *(uintptr_t**)ctx;

    static bool tryHookDrawText = false;
    static bool tryHookDrawImage = false;
    static bool tryHookDrawNineSlice = false;

    if (!tryHookDrawText) {
        tryHookDrawText = Memory::HookFunction((void*)vtable[5], (void*)&onDrawTextDetour, &onDrawText, "DrawText");
    }

    if (!tryHookDrawImage) {
        tryHookDrawImage = Memory::HookFunction((void*)vtable[7], (void*)&onDrawImageDetour, &onDrawImage, xorstr_("DrawImage"));
    }

    if (!tryHookDrawNineSlice) {
        tryHookDrawNineSlice = Memory::HookFunction((void*)vtable[8], (void*)&onDrawNineSliceDetour, &onDrawNineSlice, xorstr_("DrawNineSlice"));
    }

    // Increment layerCounter
    layerCounter++;

    // If layerCounter is equal to 3
    if (layerCounter == 3) {
        // Set layerCounter to 0
        layerCounter = 0;

        // Declare a UIColor object called color and initialize it with the values 255, 255, 255, 255
        UIColor color = UIColor{ 255, 255, 255, 50 };

        // Declare a Vector4 object called rectPos and initialize it with the values { 25, 25 }, { 25, 25 }
        Vector4<float> rectPos({ 25, 25 }, { 50, 50 });

        // Call the fillRectangle function on ctx, passing in rectPos and color as arguments
        //ctx->fillRectangle(rectPos, color, color.a);
    }

    // Call the function at the address stored in onRender, passing in __this and ctx as arguments
    Memory::CallFunc<void*, void*, MinecraftUIRenderContext*>(
        onRender, __this, ctx
    );
}

class RenderContextHook : public FuncHook {
public:
    bool Initialize() override
    {
        // 2873 is the size of the machine code (Excluding variables) at the time im making this

        // Find the address of the function specified by the signature {SIG} and store it in a void pointer called renderAddr
        // Using the utils class findSig function to locate the address of the signature
        void* renderAddr = Memory::findSig(xorstr_("48 8B C4 48 89 58 18 55 56 57 41 54 41 55 41 56 41 57 48 8D A8 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 70 B8 0F 29 78 A8 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 4C 8B FA"));
        // 48 8b c4 48 89 58 ? 55 56 57 41 54 41 55 41 56 41 57 48 8d a8 ? ? ? ? 48 81 ec ? ? ? ? 0f 29 70 ? 0f 29 78 ? 48 8b 05 ? ? ? ? 48 33 c4 48 89 85 ? ? ? ? 4c 8b fa 48 89 54 24 ? 4c 8b e9 48 89 4c 24

        void* hoverRenderAddr = Memory::findSig(xorstr_("48 8B C4 48 89 58 ? 48 89 70 ? 48 89 78 ? 4C 89 70 ? 55 48 8D 68 ? 48 81 EC ? ? ? ? 0F 29 70 ? 0F 29 78 ? 44 0F 29 40 ? 49 8B D9"));

        // Give it the address/ptr to the renderDetour function.
        // Give it the detour ptr and then the original callback ptr.

        // Usage: (Sig/Address/Offset, Detour, Original Callback).
        // Don't forget to return its result.
        
        // Attempt to hook the function at the address stored in renderAddr by replacing it with the address of renderDetour and store the original function's address in onRender
        if (!Memory::HookFunction(renderAddr, (void*)&renderDetourHook, &onRender, xorstr_("RenderContext"))) { return false; };
        
        // Attempt to hook the function at the address stored in renderAddr by replacing it with the address of renderDetour and store the original function's address in onRender
        //if (!Memory::HookFunction(hoverRenderAddr, (void*)&HoverRenderDetour, &onHoverRenderHook, xorstr_("HoverRender"))) { return false; };

        return true;
    }

    static RenderContextHook& Instance() { // a class setup function called Instance.
        static RenderContextHook instance;
        return instance;
    }
};