#pragma once

// Include Fonts
#include "Comfortaa.h" // Comfortaa's font.
#include "ComfortaaBold.h" // Comfortaa's font.
#include "Comic.h" // MojangLes's font.
#include "Icons.h" // Icon's font.
#include "MojangLes.h" // MojangLes's font.
#include "ProductSansBold.h" // ProductSans's font.
#include "ProductSans.h" // ProductSans's font.

void* oPresent;
void* oResize;

typedef HRESULT(__thiscall* drawIndexed)(struct ID3D11DeviceContext*, unsigned int, unsigned int, int);
drawIndexed oDrawIndexed;

DWORD frameHostPID;
bool imguiInit = false;

bool hookedDrawIndexed = false;

//SimpleHook<HRESULT, ID3D11DeviceContext*, UINT, UINT, INT> drawIndexedHook;

ID3D11Device* d3d11Device = nullptr;
ID3D12Device* d3d12Device = nullptr;

int TargetHeadWidth = 0;
int TargetHeadHeight = 0;

// Include LoadTextureFromFile
#include "LoadTexture.h"

struct FrameTransform {
	//glmatrixf mMatrix{};
	glm::vec3 mOrigin{};
	glm::vec3 mPlayerPos{};
};

// in your D3DHook class...
static inline std::queue<FrameTransform> FrameTransforms = {};
static inline int transformDelay = 3;

static void CHK(HRESULT hr) {
	if (FAILED(hr)) {
		FileUtils::debugOutput("HRESULT FAILED");
		throw std::runtime_error("HRESULT is failed value.");
	}
}

void callRender() { // onImGui CallBack for modules
	ImGui::GetIO().FontGlobalScale = 0.5f;

	// ImGui Render stuff here
	ImGuiRenderEvent event{}; // ImGuiRenderEvent
	event.cancelled = nullptr;
	CallBackEvent(&event); // Call ImGui event for modules to be writen on this hook.

	if (Global::RenderNotifications) {
		Notifications::renderNotifications();
	}
}

void loadFonts() { // load ImGui font.
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ComfortaaTTF, sizeof(ComfortaaTTF), 48); // 0
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ComfortaaBoldTTF, sizeof(ComfortaaBoldTTF), 48); // 1
	ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(ProductSansCompressedData, ProductSansCompressedSize, 48.f); // 
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF((void*)ProductSansBoldTTF, sizeof(ProductSansBoldTTF), 48); // 3
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(Mojangles, sizeof(Mojangles), 48.f); // 4
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(ComicTTF, sizeof(ComicTTF), 48.f); // 5
	ImGui::GetIO().Fonts->AddFontFromMemoryTTF(IconTTF, sizeof(IconTTF), 48.f); // 6

	ImGui::GetStyle().WindowRounding = 13.f; // Set the default Windows Rounding to 8

	std::string HeadImageFilePath = FileUtils::getResourcePath() + "\\TargetHead.png";
	LoadTextureFromFile(HeadImageFilePath.c_str(), &Global::RenderInfo::HeadTexture, &TargetHeadWidth, &TargetHeadHeight);
}

HRESULT DrawIndexed(struct ID3D11DeviceContext* Context, unsigned int IndexCount, unsigned int StartIndexLocation, int BaseVertexLocation) {
	ID3D11Buffer* vertBuffer;
	UINT vertBufferOffset;
	UINT stride;

	ImGui::ShowDemoWindow();

	Context->IAGetVertexBuffers(0, 1, &vertBuffer, &stride, &vertBufferOffset);
	if (stride == 24) {
		D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
		ID3D11DepthStencilState* depthStencil;
		UINT stencilRef;

		// get info about current depth stencil
		Context->OMGetDepthStencilState(&depthStencil, &stencilRef);
		depthStencil->GetDesc(&depthStencilDesc);
		depthStencilDesc.DepthEnable = false; // disable depth to ignore all other geometry
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO; // disable writing to depth buffer
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS; // always pass depth test
		depthStencilDesc.StencilEnable = false; // disable stencil
		depthStencilDesc.StencilReadMask = 0; // disable stencil


		// create a new depth stencil based on current drawn one but with the depth disabled as mentioned before
		d3d11Device->CreateDepthStencilState(&depthStencilDesc, &depthStencil);
		Context->OMSetDepthStencilState(depthStencil, stencilRef);

		// call original function
		oDrawIndexed(Context, IndexCount, StartIndexLocation, BaseVertexLocation);

		// release memory
		depthStencil->Release();

		


		// draw a glow effect

		return 0;
	}

	/*if (isSecondPass && daSwapChain)
	{
		isSecondPass = false;

		ImGuiHelper::Init(daSwapChain, device11.get(), context.get());
		D2D::Init(daSwapChain, device11.get());

		// Start a new imgui fram
		ImGuiHelper::NewFrame();

		// draw a rect
		ImGui::GetBackgroundDrawList()->AddRect(ImVec2(100, 100), ImVec2(1000, 600), IM_COL32(255, 0, 255, 255));

		ImGuiHelper::EndFrame();

		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	}*/

	return oDrawIndexed(Context, IndexCount, StartIndexLocation, BaseVertexLocation);
}

HRESULT D3D12_PresentDetour(IDXGISwapChain3* pSwapChain, int syncInterval, int flags) {
	if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d11Device)))) {
		ID3D11DeviceContext* pContext = nullptr;
		d3d11Device->GetImmediateContext(&pContext);

		// VTable
	     uintptr_t* contextVfT = *(uintptr_t**)(pContext);

		ID3D11Texture2D* pBackBuffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));

		IDXGISurface* dxgiBackBuffer;
		pSwapChain->GetBuffer(0, IID_PPV_ARGS(&dxgiBackBuffer));

		ID3D11RenderTargetView* mainRenderTargetView = nullptr;
		if (pBackBuffer != nullptr) {
			d3d11Device->CreateRenderTargetView(pBackBuffer, NULL, &mainRenderTargetView);
		}

		if (!imguiInit) {
			ImGui::CreateContext();
			loadFonts();
			ImGuiIO& io = ImGui::GetIO();
			io.IniFilename = NULL;
			ImGui_ImplWin32_Init(window);
			ImGui_ImplDX11_Init(d3d11Device, pContext);
			imguiInit = true;
		}
		pBackBuffer->Release();
		ImFX::NewFrame(d3d11Device, dxgiBackBuffer, GetDpiForWindow(window));
		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		// in your present hook...
		while (FrameTransforms.size() > transformDelay)
		{
			//RenderUtils::transform = FrameTransforms.front();
			FrameTransforms.pop();
		}

		ImGui::NewFrame();
		callRender(); // Render the modules and stuff in imgui
		ImGui::EndFrame();
		ImGui::Render();

		if (mainRenderTargetView != nullptr)
			pContext->OMSetRenderTargets(1, &mainRenderTargetView, NULL);
		ImDrawData* drawData = ImGui::GetDrawData();
		ImGui_ImplDX11_RenderDrawData(drawData);
		ImFX::EndFrame();
		pContext->Flush();
		pContext->Release();
		if (mainRenderTargetView)
			mainRenderTargetView->Release();
		d3d11Device->Release();
	}
	else if (SUCCEEDED(pSwapChain->GetDevice(IID_PPV_ARGS(&d3d12Device)))) {
		pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		static_cast<ID3D12Device5*>(d3d12Device)->RemoveDevice();
	}

	return Memory::CallFunc<HRESULT, IDXGISwapChain3*, UINT, UINT>(oPresent, pSwapChain, syncInterval, flags);
}

HRESULT resizeBuffersCallback(IDXGISwapChain* pSwapChain, int bufferCount, int width, int height, DXGI_FORMAT newFormat, int swapChainFlags) {
	ImFX::CleanupFX();

	return Memory::CallFunc<HRESULT, IDXGISwapChain*, int, int, int, DXGI_FORMAT, int>(oResize, pSwapChain, bufferCount, width,
		height, newFormat, swapChainFlags);
}

class DirectXHook : public FuncHook {
public:
	bool Initialize() override {
		winrt::Windows::UI::Core::CoreWindow cw = winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow();
		winrt::com_ptr<ICoreWindowInterop> interop;
		winrt::check_hresult(winrt::get_unknown(cw)->QueryInterface(interop.put()));
		winrt::check_hresult(interop->get_WindowHandle(&window));

		if (kiero::init(kiero::RenderType::D3D12) != kiero::Status::Success && kiero::init(kiero::RenderType::D3D11) != kiero::Status::Success) {
			FileUtils::debugOutput("[apollo] [DirectX] failed to initialize hook");
			return false;
		}

		uint16_t index = kiero::getRenderType() == kiero::RenderType::D3D12 ? 145 : 13;

		if (kiero::bind(index, (void**)&oResize, resizeBuffersCallback) != kiero::Status::Success) {
			FileUtils::debugOutput("[apollo] [DirectX] failed to create hook for resize buffer");
			return false;
		}

		uint16_t index2 = kiero::getRenderType() == kiero::RenderType::D3D12 ? 140 : 8;

		if (kiero::bind(index2, (void**)&oPresent, D3D12_PresentDetour) != kiero::Status::Success) {
			FileUtils::debugOutput("[apollo] [DirectX] failed to create hook for present");
			return false;
		}

		return true;
	}

	static DirectXHook& Instance() {
		static DirectXHook instance;
		return instance;
	}
};