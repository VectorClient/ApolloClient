#include "imfx.h"

#define IMGUI_DEFINE_MATH_OPERATORS

#include "imgui.h"
#include "imgui_internal.h"

#include <dxgi.h>
#include <d3d11.h>
#include <d3d12.h>
#include <d2d1_3.h>

#include <vector>

static ID2D1Factory3* d2dFactory = nullptr;
static ID2D1Device2* d2dDevice = nullptr;
static ID2D1DeviceContext2* d2dDeviceContext = nullptr;
static ID2D1Effect* blurEffect = nullptr;
static ID2D1Effect* displacementMapEffect = nullptr;
static ID2D1Effect* turbulenceEffect = nullptr;
static ID2D1Effect* shadowEffect = nullptr;
static ID2D1Effect* compositeEffect = nullptr;
static IDXGISurface* backBuffer = nullptr;
static ID2D1Bitmap1* sourceBitmap = nullptr;
static ID2D1Bitmap1* shadowBitmap = nullptr;
static bool d2dInit = false;
static float dpi = 0.0f;
static ImFX::FXMode mode = ImFX::FXMode::QUALITY;


void ImFX::SetFXMode(ImFX::FXMode fxmode) {
    mode = fxmode;
}

ImFX::FXMode ImFX::GetFXMode() {
    return mode;
}

bool ImFX::NewFrame(class ::ID3D11Device* d3d11Device, IDXGISurface* dxgiBackBuffer, float fxdpi) {
    if (d2dInit == false && SUCCEEDED(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &d2dFactory))) {
        //Yay?
        //Create d2d & dxgi device(s)
        IDXGIDevice* dxgiDevice;
        d3d11Device->QueryInterface<IDXGIDevice>(&dxgiDevice);
        if (dxgiDevice == nullptr) {
            return false;
        }
        d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
        dxgiDevice->Release();
        if (d2dDevice == nullptr) {
            return false;
        }
        d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dDeviceContext);
        if (d2dDeviceContext == nullptr) {
            return false;
        }

        //Create blur
        d2dDeviceContext->CreateEffect(CLSID_D2D1GaussianBlur, &blurEffect);
        if (blurEffect == nullptr) {
            return false;
        }
        //Create DisplacementMap effect
        d2dDeviceContext->CreateEffect(CLSID_D2D1DisplacementMap, &displacementMapEffect);
        if (displacementMapEffect == nullptr) {
            return false;
        }

        D2D1_BITMAP_PROPERTIES1 bitmapProperties
            = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
                D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED), dpi,
                dpi);
        d2dDeviceContext->CreateBitmapFromDxgiSurface(dxgiBackBuffer, &bitmapProperties, &sourceBitmap);


        d2dInit = true;
    }

    backBuffer = dxgiBackBuffer;
    dpi = fxdpi;


    return true;
}

bool ImFX::CleanupFX() {
    if (!d2dInit)
        return false;
    if (shadowBitmap != nullptr) {
        shadowBitmap->Release();
        shadowBitmap = nullptr;
    }

    if (sourceBitmap != nullptr) {
        sourceBitmap->Release();
        sourceBitmap = nullptr;
    }
    if (backBuffer) {
        backBuffer->Release();
        backBuffer = nullptr;
    }
    d2dFactory->Release();
    d2dDevice->Release();
    d2dDeviceContext->Release();
    blurEffect->Release();
    d2dInit = false;
    return true;
}

bool ImFX::EndFrame() {
    if (!d2dInit) {
        return false;
    }
    backBuffer->Release();
    backBuffer = nullptr;
    return true;
}

struct EffectsContext {
    ID2D1Bitmap* targetBitmap = nullptr;
};
static std::vector<EffectsContext> currentContext;
struct BeginData {
    bool dontCopy = false;
};

void BeginFX(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    ImGuiIO& io = ImGui::GetIO();

    d2dDeviceContext->SetTarget(sourceBitmap);
    d2dDeviceContext->BeginDraw();

    BeginData* data = (BeginData*)cmd->UserCallbackData;

    //Copy the bitmap
    ID2D1Bitmap* targetBitmap = nullptr;
    D2D1_BITMAP_PROPERTIES props = D2D1::BitmapProperties(sourceBitmap->GetPixelFormat());
    d2dDeviceContext->CreateBitmap(sourceBitmap->GetPixelSize(), props, &targetBitmap);
    auto destPoint = D2D1::Point2U(0, 0);
    auto size = sourceBitmap->GetPixelSize();
    auto rect = D2D1::RectU(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    targetBitmap->CopyFromBitmap(&destPoint, sourceBitmap, &rect);

    currentContext.emplace_back();
    currentContext.back().targetBitmap = targetBitmap;
}

struct EndData {
    bool composite = false;
};

void EndFX(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    EndData* data = (EndData*)cmd->UserCallbackData;
    ImGuiIO& io = ImGui::GetIO();

    if (data->composite) {
        compositeEffect->SetInput(0, sourceBitmap);
        compositeEffect->SetInput(1, currentContext.back().targetBitmap);

        ID2D1Image* outImage = nullptr;
        compositeEffect->GetOutput(&outImage);
        d2dDeviceContext->SetTarget(sourceBitmap);
        d2dDeviceContext->DrawImage(outImage);
        outImage->Release();
    }

    d2dDeviceContext->Flush();
    d2dDeviceContext->EndDraw();

    currentContext.back().targetBitmap->Release();
    currentContext.pop_back();
}

static std::vector<ImDrawList*> currentDrawlist;

bool ImFX::Begin(ImDrawList* drawList, bool dontCopy) {
    if (!d2dInit) {
        return false;
    }

    float rounding = ImGui::GetStyle().FrameRounding;
    drawList->AddCallback(BeginFX, new BeginData{ dontCopy });
    currentDrawlist.push_back(drawList);
    return true;
}

bool ImFX::End(bool composite) {
    if (!d2dInit) {
        return false;
    }

    //Finish Direct2d draw
    d2dDeviceContext->Flush();
    d2dDeviceContext->EndDraw();

    float rounding = ImGui::GetStyle().FrameRounding;
    currentDrawlist.back()->AddCallback(EndFX, new EndData{ composite });
    currentDrawlist.pop_back();
    return true;
}

struct PushLayerData {
    float rounding = 0.0f;
    std::optional<ImVec4> clipRect;

    PushLayerData(std::optional<ImVec4> clipRect) {
        this->clipRect = clipRect;
    }
};

static std::vector<ID2D1Layer*> layers;

void PushLayerCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    ID2D1Layer* layer;
    d2dDeviceContext->CreateLayer(&layer);

    ImGuiIO& io = ImGui::GetIO();
    //Create rects and rounded rects
    PushLayerData* data = (PushLayerData*)cmd->UserCallbackData;
    ImVec4 clipRect = data->clipRect.has_value() ? *data->clipRect : cmd->ClipRect;

    //Create rects and rounded rects
    D2D1_RECT_F clipRectD2D = D2D1::RectF(
        clipRect.x,
        clipRect.y,
        clipRect.z,
        clipRect.w
    );

    D2D1_LAYER_PARAMETERS parameters;
    parameters.contentBounds = clipRectD2D;
    parameters.geometricMask = nullptr;
    parameters.maskAntialiasMode = D2D1_ANTIALIAS_MODE_ALIASED;
    parameters.maskTransform = D2D1::Matrix3x2F::Identity();
    parameters.opacity = 1.0f;
    parameters.opacityBrush = nullptr;
    parameters.layerOptions = D2D1_LAYER_OPTIONS_NONE;
    d2dDeviceContext->PushLayer(parameters, layer);
    layers.push_back(layer);
}

void PopLayerCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    ID2D1Layer* layer = layers.back();
    d2dDeviceContext->PopLayer();
    layers.pop_back();
    layer->Release();
}

bool ImFX::PushLayer(std::optional<ImVec4> clipRect) {
    if (!d2dInit) {
        return false;
    }

    currentDrawlist.back()->AddCallback(PushLayerCallback, new PushLayerData(clipRect));
    return true;
}

bool ImFX::PopLayer() {
    if (!d2dInit) {
        return false;
    }

    float rounding = ImGui::GetStyle().FrameRounding;
    currentDrawlist.back()->AddCallback(PopLayerCallback, nullptr);
    return true;
}

struct BlurCallbackData {
    float strength = 0.0f;
    float rounding = 0.0f;
    std::optional<ImVec4> clipRect;

    BlurCallbackData(float strength, float rounding, std::optional<ImVec4> clipRect = std::optional<ImVec4>()) {
        this->strength = strength;
        this->rounding = rounding;
        this->clipRect = clipRect;
    }
};

void BlurCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    //Do D2D rendering stuff

    if (!d2dInit) {
        return;
    }

    if (currentContext.size() <= 0) {
        throw std::exception("You must call ImFX::Begin before using effects!");
    }

    ImGuiIO& io = ImGui::GetIO();
    BlurCallbackData* data = (BlurCallbackData*)cmd->UserCallbackData;
    ImVec4 clipRect = data->clipRect.has_value() ? *data->clipRect : cmd->ClipRect;

    //Create rects and rounded rects
    D2D1_RECT_F screenRectF = D2D1::RectF(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    D2D1_RECT_F clipRectD2D = D2D1::RectF(
        clipRect.x,
        clipRect.y,
        clipRect.z,
        clipRect.w
    );
    D2D1_ROUNDED_RECT clipRectRounded = D2D1::RoundedRect(clipRectD2D, data->rounding, data->rounding);

    //Apply blur effect
    blurEffect->SetInput(0, currentContext.back().targetBitmap);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, data->strength);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    D2D1_GAUSSIANBLUR_OPTIMIZATION opt = mode == ImFX::FXMode::QUALITY ? D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY
        : D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED;
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, opt);

    //Get the output image
    ID2D1Image* outImage = nullptr;
    blurEffect->GetOutput(&outImage);

    //Create bitmap brush for the clipping
    ID2D1ImageBrush* outImageBrush = nullptr;
    D2D1_IMAGE_BRUSH_PROPERTIES outImage_props = D2D1::ImageBrushProperties(screenRectF);
    d2dDeviceContext->CreateImageBrush(
        outImage,
        outImage_props,
        &outImageBrush
    );

    //Draw the blur
    ID2D1RoundedRectangleGeometry* clipRectGeo = nullptr;
    d2dFactory->CreateRoundedRectangleGeometry(clipRectRounded, &clipRectGeo);
    d2dDeviceContext->FillGeometry(clipRectGeo, outImageBrush);
    clipRectGeo->Release();

    //Release interfaces
    outImageBrush->Release();
    outImage->Release();

    //Free data
    delete data;
}

bool ImFX::AddBlur(float strength, std::optional<ImVec4> clipRect, float rounding) {
    if (!d2dInit) {
        return false;
    }

    if (mode == FXMode::PERFORMANCE) {
        return true;
    }

    BlurCallbackData* data = new BlurCallbackData(strength, rounding, clipRect);
    currentDrawlist.back()->AddCallback(BlurCallback, data);
    return true;
}

//https://stackoverflow.com/a/73769914
ID2D1PathGeometry*
CreateRoundRect(int x, int y, int width, int height, int leftTop, int rightTop, int rightBottom, int leftBottom) {
    ID2D1GeometrySink* sink = nullptr;
    ID2D1PathGeometry* path = nullptr;

    d2dFactory->CreatePathGeometry(&path);
    path->Open(&sink);

    D2D1_POINT_2F p[2];

    p[0].x = x + leftTop;
    p[0].y = y;
    sink->BeginFigure(p[0], D2D1_FIGURE_BEGIN::D2D1_FIGURE_BEGIN_FILLED);
    p[1].x = x + width - rightTop;
    p[1].y = y;
    sink->AddLines(p, 2);

    p[0].x = x + width;
    p[0].y = y + rightTop;

    if (rightTop) {
        D2D1_POINT_2F p2 = D2D1::Matrix3x2F::Rotation(0, p[1]).TransformPoint(p[0]);
        sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(rightTop, rightTop), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL));
    }

    p[1].x = x + width;
    p[1].y = y + height - rightBottom;
    sink->AddLines(p, 2);

    p[0].x = x + width - rightBottom;
    p[0].y = y + height;

    if (rightBottom) {
        D2D1_POINT_2F p2 = D2D1::Matrix3x2F::Rotation(0, p[1]).TransformPoint(p[0]);
        sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(rightBottom, rightBottom), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL));
    }

    p[1].x = x + leftBottom;
    p[1].y = y + height;
    sink->AddLines(p, 2);

    p[0].x = x;
    p[0].y = y + height - leftBottom;
    if (leftBottom) {
        D2D1_POINT_2F p2 = D2D1::Matrix3x2F::Rotation(0, p[1]).TransformPoint(p[0]);
        sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(leftBottom, leftBottom), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL));
    }


    p[1].x = x;
    p[1].y = y + leftTop;
    sink->AddLines(p, 2);
    p[0].x = x + leftTop;
    p[0].y = y;
    if (leftTop) {
        D2D1_POINT_2F p2 = D2D1::Matrix3x2F::Rotation(0, p[1]).TransformPoint(p[0]);
        sink->AddArc(D2D1::ArcSegment(p2, D2D1::SizeF(leftTop, leftTop), 0, D2D1_SWEEP_DIRECTION_CLOCKWISE,
            D2D1_ARC_SIZE_SMALL));
    }

    sink->EndFigure(D2D1_FIGURE_END::D2D1_FIGURE_END_CLOSED);
    sink->Close();
    sink->Release();

    return path;
}

struct ColorCallbackData {
    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    std::optional<ImVec4> rounding;
    std::optional<ImVec4> clipRect;

    ColorCallbackData(ImVec4 color, std::optional<ImVec4> rounding,
        std::optional<ImVec4> clipRect = std::optional<ImVec4>()) {
        this->color = color;
        this->rounding = rounding;
        this->clipRect = clipRect;
    }
};

void ColorCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    if (!d2dInit) {
        return;
    }

    if (backBuffer == nullptr) {
        return;
    }

    ImGuiIO& io = ImGui::GetIO();
    ColorCallbackData* data = (ColorCallbackData*)cmd->UserCallbackData;
    ImVec4 clipRect = data->clipRect.has_value() ? *data->clipRect : cmd->ClipRect;
    ImVec4 rounding = data->rounding.has_value() ? *data->rounding : ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

    //Create rects and rounded rects
    D2D1_RECT_F screenRectF = D2D1::RectF(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    D2D1_RECT_F clipRectD2D = D2D1::RectF(
        clipRect.x,
        clipRect.y,
        clipRect.z,
        clipRect.w
    );
    //This uses width and height so we have to subtract the positions too
    ID2D1PathGeometry* roundedRectGeo = CreateRoundRect(clipRect.x, clipRect.y, clipRect.z - clipRect.x,
        clipRect.w - clipRect.y, rounding.x, rounding.y, rounding.z,
        rounding.w);

    ID2D1SolidColorBrush* colorBrush = nullptr;
    d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(data->color.x, data->color.y, data->color.z, data->color.w),
        &colorBrush);
    d2dDeviceContext->FillGeometry(roundedRectGeo, colorBrush);

    //Release interfaces
    colorBrush->Release();
    roundedRectGeo->Release();

    //Free data
    delete data;
}

bool ImFX::AddColor(ImVec4 color, std::optional<ImVec4> rounding, std::optional<ImVec4> clipRect) {
    if (!d2dInit) {
        return false;
    }

    ColorCallbackData* data = new ColorCallbackData(color, rounding, clipRect);
    currentDrawlist.back()->AddCallback(ColorCallback, data);
    return true;
}

bool ImFX::AddColor(ImVec4 color, float rounding, std::optional<ImVec4> clipRect) {
    return AddColor(color, ImVec4(rounding, rounding, rounding, rounding), clipRect);
}

bool ImFX::AddColor(ImVec4 color, std::optional<ImVec4> clipRect) {
    if (!d2dInit) {
        return false;
    }

    float rounding = ImGui::GetStyle().FrameRounding;
    return AddColor(color, ImVec4(rounding, rounding, rounding, rounding), clipRect);
}

bool ImFX::AddColor(ImGuiCol color, std::optional<ImVec4> clipRect) {
    return AddColor(ImGui::GetStyleColorVec4(color), clipRect);
}

struct ShadowCallbackData {
    float strength = 0.0f;
    float rounding = 0.0f;
    ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
    std::optional<ImVec4> clipRect;

    ShadowCallbackData(float strength, float rounding, ImVec4 color,
        std::optional<ImVec4> clipRect = std::optional<ImVec4>()) {
        this->strength = strength;
        this->rounding = rounding;
        this->color = color;
        this->clipRect = clipRect;
    }
};

void ShadowCallback(const ImDrawList* parent_list, const ImDrawCmd* cmd) {
    //Do D2D rendering stuff

    if (!d2dInit) {
        return;
    }

    if (currentContext.size() <= 0) {
        throw std::exception("You must call ImFX::Begin before using effects!");
    }

    ImGuiIO& io = ImGui::GetIO();
    ShadowCallbackData* data = (ShadowCallbackData*)cmd->UserCallbackData;
    ImVec4 clipRect = data->clipRect.has_value() ? *data->clipRect : cmd->ClipRect;

    //Create rects and rounded rects
    D2D1_RECT_F screenRectF = D2D1::RectF(0, 0, io.DisplaySize.x, io.DisplaySize.y);
    D2D1_RECT_F clipRectD2D = D2D1::RectF(
        clipRect.x,
        clipRect.y,
        clipRect.z,
        clipRect.w
    );
    D2D1_ROUNDED_RECT clipRectRounded = D2D1::RoundedRect(clipRectD2D, data->rounding, data->rounding);

    if (shadowBitmap == nullptr) {
        //Create a new blank bitmap
        D2D1_BITMAP_PROPERTIES1 newLayerProps = D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET,
            sourceBitmap->GetPixelFormat());
        d2dDeviceContext->CreateBitmap(sourceBitmap->GetPixelSize(), nullptr, 0, newLayerProps, &shadowBitmap);
    }

    //Draw the color
    d2dDeviceContext->SetTarget(shadowBitmap);
    d2dDeviceContext->Clear(D2D1::ColorF(0, 0, 0, 0));
    ID2D1SolidColorBrush* colorBrush = nullptr;
    d2dDeviceContext->CreateSolidColorBrush(D2D1::ColorF(data->color.x, data->color.y, data->color.z, data->color.w),
        &colorBrush);
    d2dDeviceContext->FillRoundedRectangle(clipRectRounded, colorBrush);

    //Apply blur effect
    blurEffect->SetInput(0, shadowBitmap);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, data->strength);
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_BORDER_MODE, D2D1_BORDER_MODE_HARD);
    D2D1_GAUSSIANBLUR_OPTIMIZATION opt = mode == ImFX::FXMode::QUALITY ? D2D1_GAUSSIANBLUR_OPTIMIZATION_QUALITY
        : D2D1_GAUSSIANBLUR_OPTIMIZATION_SPEED;
    blurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_OPTIMIZATION, opt);

    d2dDeviceContext->SetTarget(sourceBitmap);

    //Release interfaces
    colorBrush->Release();

    //Get the output image
    ID2D1Image* outImage = nullptr;
    blurEffect->GetOutput(&outImage);

    //Create bitmap brush for the clipping
    ID2D1ImageBrush* outImageBrush = nullptr;
    D2D1_IMAGE_BRUSH_PROPERTIES outImage_props = D2D1::ImageBrushProperties(screenRectF);
    d2dDeviceContext->CreateImageBrush(
        outImage,
        outImage_props,
        &outImageBrush
    );

    //Draw the shadow
    ID2D1RectangleGeometry* screenRectGeo = nullptr;
    d2dFactory->CreateRectangleGeometry(screenRectF, &screenRectGeo);
    d2dDeviceContext->FillGeometry(screenRectGeo, outImageBrush);
    screenRectGeo->Release();

    //Release interfaces
    outImageBrush->Release();
    outImage->Release();

    //Free data
    delete data;
}