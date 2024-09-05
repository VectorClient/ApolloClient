#pragma once

void* onRenderItemInHandDescriptionCtor;

class RenderItemInHandDescription
{
public:
    BUILD_ACCESS(UIColor, GlintColor, 0x8c);
    BUILD_ACCESS(float, GlintAlpha, 0x9c);
    BUILD_ACCESS(UIColor, OverlayColor, 0xac);
    BUILD_ACCESS(float, OverlayAlpha, 0xbc);
    BUILD_ACCESS(UIColor, ChangeColor, 0xcc);
    BUILD_ACCESS(float, ChangeAlpha, 0xdc);
    BUILD_ACCESS(UIColor, MultiplicativeTintColor, 0xec);
    BUILD_ACCESS(float, MultiplicativeTintColorAlpha, 0xfc);
};

void RenderItemInHandDescriptionCtorDetour(RenderItemInHandDescription* _this, void* renderObject, void* itemFlags, void* material, void* glintTexture, void* worldMatrix, bool isDrawingUI, void* globalConstantBuffers, unsigned short viewId, void* renderMetadata) {
    if (getModuleByName("glint")->isEnabled()) {
        //_this->GlintColor = ColorUtils::Rainbow(1.5, 1, 1, 1);
    }

	Memory::CallFunc<void*, RenderItemInHandDescription*, void*, void*, void*, void*, void*, bool, void*, __int16, void*>(
		onRenderItemInHandDescriptionCtor, _this, renderObject, itemFlags, material, glintTexture, worldMatrix, isDrawingUI, globalConstantBuffers, viewId, renderMetadata
	);
}

class RenderItemInHandDescriptionHook : public FuncHook {
public:
    bool Initialize() override {

        // RenderItemInHandDescription::ctor

        void* renderItemInHandDescriptionCtorAddr = Memory::findSig(xorstr_("48 89 ? ? ? 48 89 ? ? ? 55 56 57 41 ? 41 ? 41 ? 41 ? 48 83 EC ? 4D 8B ? 4D 8B ? 4C 8B ? 48 8B ? 45 33"));

        //if (!Memory::HookFunction(renderItemInHandDescriptionCtorAddr, (void*)&RenderItemInHandDescriptionCtorDetour, &onRenderItemInHandDescriptionCtor, xorstr_("RenderItemInHandDescription::ctor"))) { return false; }

        return true;
    }

	static RenderItemInHandDescriptionHook& Instance() {
		static RenderItemInHandDescriptionHook instance;
		return instance;
	}
};