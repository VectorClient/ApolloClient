// dllmain.cpp : Defines the entry point for the DLL application.
bool isRunning = true; // isRunning bool for ejection/detachment
//Includes
#pragma region Includes & Macros

// C++ Includes
#include <Windows.h>
#include <Psapi.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <memory> // Memory Region stuff
#include <map>
#include <vector>
#include <utility>
#include <chrono>
#include <unordered_map>
#include <filesystem>
#include <playsoundapi.h>
#include <format>
#include <winrt/Windows.UI.Notifications.h>
#include <winrt/Windows.Data.Xml.Dom.h>
#include "winrt/windows.applicationmodel.core.h"
#include "winrt/Windows.UI.ViewManagement.h"
#include "winrt/Windows.Foundation.h"
#include <CoreWindow.h>
#include "winrt/Windows.UI.Core.h"
#include "winrt/windows.system.h"
// 'winrt::impl:consume_Window5<d>::DispatcherQueue': a function that returns auto cannot be used before it is defined
//#include <winrt/Windows.Foundation.h>
//#include <cassert>
//#include <queue>
//#include <functional>
//#include <thread>
#include "Libs/json.hpp"
#include <urlmon.h>
#include <winhttp.h>
#pragma comment(lib, "winhttp.lib")
#pragma comment(lib, "urlmon.lib")

HWND window;

using namespace winrt::Windows::UI::Notifications;
using namespace winrt::Windows::UI::ViewManagement;
using namespace winrt::Windows::ApplicationModel::Core;

// Module List
class Module;
class ConfigurationManager;
class CommandManager;
std::vector<Module*> modules = std::vector<Module*>();
std::vector<std::string> GUICategories;
std::vector<std::string> categories;

// DirectX
#include <d2d1.h>
#include <d2d1_2.h>
#include <dwrite.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <dxgi1_4.h>
#include <d2d1_3.h>
#include <dxgi.h>
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

// STB Image
#define STB_IMAGE_IMPLEMENTATION
#include "Libs/imgui/stb_image.h"

// MinHook
#include "Libs/minhook/MinHook.h"
#include "Libs/glm/glm/glm.hpp"
#include "Libs/glm/glm/ext/matrix_transform.hpp"

// libhat
#include "Libs/libhat/libhat.hpp"
#include "Libs/libhat/libhat/Access.hpp"
#include "Libs/libhat/libhat/Callable.hpp"
#include "Libs/libhat/libhat/Concepts.hpp"
#include "Libs/libhat/libhat/CompileTime.hpp"
#include "Libs/libhat/libhat/Defines.hpp"
#include "Libs/libhat/libhat/FixedString.hpp"
#include "Libs/libhat/libhat/MemoryProtector.hpp"
#include "Libs/libhat/libhat/Process.hpp"
#include "Libs/libhat/libhat/Result.hpp"
#include "Libs/libhat/libhat/Scanner.hpp"
#include "Libs/libhat/libhat/Signature.hpp"
#include "Libs/libhat/libhat/StringLiteral.hpp"
#include "Libs/libhat/libhat/Traits.hpp"

// ImGui & Kiero
#include "Libs/kiero/kiero.h"
#include "Libs/imgui/imgui.h"
#include "Libs/imgui/imgui_internal.h"
#include "Libs/imgui/impl/imgui_impl_win32.h"
#include "Libs/imgui/impl/imgui_impl_dx11.h"
#include "Libs/imgui/impl/imgui_impl_dx12.h"
#include "Libs/imgui/imfx.h"

// Maths
#include "Client/Utils/Math/Keys.h"
#include "Client/Utils/Math/Math.h"
#include "Client/Utils/Math/TextFormat.h"
#include "Client/Utils/Math/TextHolder.h"
#include "Client/Utils/Math/UIColor.h"
#include "Client/Utils/Math/Vector2.h"
#include "Client/Utils/Math/Vector3.h"
#include "Client/Utils/Math/Vector4.h"

// Memory
#include "Libs/xorstr.h"
#include "Client/Utils/FileUtils.h"
#include "Client/Utils/Utils.h"
#include "Base/SDK/Memory.h"

#include "Client/Utils/Math/String.h"

// Utils
#include "Client/Utils/Math/svpng.h"
#include "Client/Utils/DiscordUtil.h"
#include "Client/Utils/EasingUtil.h"
#include "Client/Utils/TimeUtils.h"
#include "Client/Utils/KickDecryptorUtils.h"

// SDK
//#include "Base/SDK/Classes/Core/ClientInstance.h"
#include "Base/SDK/Classes/Render/HashedString.h"
#include "Base/SDK/Classes/Packet/LoopbackPacketSender.h"
#include "Base/SDK/Classes/Packet/Packet.h"
#include "Base/SDK/Classes/Block/Block.h"
#include "Base/SDK/Classes/Block/BlockSource.h"
#include "Base/SDK/Classes/Block/BlockActor.h"
#include "Base/SDK/Classes/Container/Item.h"
#include "Base/SDK/Classes/Container/ItemStack.h"
#include "Base/SDK/Classes/Container/Inventory.h"
#include "Base/SDK/Classes/Container/SimpleContainer.h"
#include "Base/SDK/Classes/Container/PlayerInventory.h"
#include "Base/SDK/Classes/Container/ContainerScreenController.h"
//#include "Base/SDK/Classes/Actor/ActorCollision.h"
#include "Base/SDK/Classes/Actor/Actor.h"
#include "Base/SDK/Classes/Actor/Level.h"
#include "Base/SDK/Classes/GameMode/GameMode.h"
#include "Base/SDK/Classes/Actor/Mob.h"
#include "Base/SDK/Classes/Actor/Player.h"
#include "Base/SDK/Classes/Core/ClientInstance.h"
#include "Base/SDK/Classes/Render/ScreenView.h"

// Render Utils
#include "Client/Utils/RenderUtils.h" // This must be under SDK to avoid errors

#include "Client/Notifications/NotificationsSettings.h"

// Globals
#include "Base/Globals/Global.h"
#include "Base/SDK/Address.h"
#include "Base/SDK/Misc.h"

#include "Client/Utils/ColorUtils.h"

// Movement
#include "Client/Utils/MovementUtils.h"

// ImRender Utils
#include "Client/Utils/ImRenderUtils.h"

#include "Client/Notifications/Notification.h"
//#include "Client/Notifications/NotificationsSettings.h"

// Event
#include "Client/Events/Event.h"

#include "Client/Utils/ChatUtils.h"

// Module
#include "Client/Modules/Module.h"

template <typename TRet>
TRet* getModule() {
    for (auto pMod : modules) {
        if (auto pRet = dynamic_cast<typename std::remove_pointer<TRet>::type*>(pMod->getModule())) {

            return pRet;
        }
    }
    return nullptr;
};

Module* getModuleByName(const std::string& name) {
    std::string nameLower(name);
    std::transform(nameLower.begin(), nameLower.end(), nameLower.begin(), ::tolower);

    auto it = std::find_if(modules.begin(), modules.end(), [&](Module* m) {
        std::string moduleNameLower = m->getName();
        std::transform(moduleNameLower.begin(), moduleNameLower.end(), moduleNameLower.begin(), ::tolower);
        moduleNameLower.erase(std::remove(moduleNameLower.begin(), moduleNameLower.end(), ' '), moduleNameLower.end());
        return nameLower == moduleNameLower;
        }
    );
    return it != modules.end() ? *it : nullptr;
}

#include "Client/Configuration/ConfigManager.h"

ConfigurationManager ConfigMgr;

#include "Client/Command/Command.h"
#include "Client/Command/CommandManager.h"

CommandManager CommandMgr;

#include "Client/Authentication/Authentication.h"

// Hooks
#include "Base/Hooks/FuncHook.h"
//#include "Base/DirectX/DirectXHook.h"

// ModuleAssistant
#include "Client/Modules/ModuleAssistant.h"

#pragma endregion


void setWindowTitle(const std::wstring& title) {
    winrt::Windows::ApplicationModel::Core::CoreApplication::MainView().CoreWindow().DispatcherQueue().TryEnqueue([title]() {
        winrt::Windows::UI::ViewManagement::ApplicationView::GetForCurrentView().Title(title);
        });
}

void InitializeClient() {
    setWindowTitle(L"");
    FileUtils::InitializeClientFolders(); // This is used to initialize clients folders.
    FileUtils::debugOutput("[apollo] injected");
    FileUtils::debugOutput("[apollo] initializing");
    InitializeMods(); // This is used to initialize modules & sort categories.
    FileUtils::debugOutput("[apollo] initialized modules (1/3)");
    //Authentication::Authorize();
    CommandMgr.InitializeCommands(); // This is used to initialize commands.
    FileUtils::debugOutput("[apollo] Initialized commands (2/3)");
    InitializeHooks(); // This is used to initialize hooks to the proccess.
    FileUtils::debugOutput("[apollo] Initialized hooks (3/3)");
    Global::Notification.addNotification("Apollo", "Initialized!", 3);
    setWindowTitle(L"Apollo Client");
}

DWORD APIENTRY ejectThread(HMODULE lpParam)
{
    while (isRunning) { // When the client is running.
        // When Control and L pressed.
        if ((Global::Keymap[VK_CONTROL] && Global::Keymap['L'])) {
            // set bool isRunning to false. for ejection.
            isRunning = false;  // Uninject
        }
        Sleep(0); // I use this to avoid little decreasing in fps.
    }

    Sleep(50); // Sleep for a little bit.
    kiero::shutdown();
    MH_DisableHook(MH_ALL_HOOKS); // Disable all Hooks.
    MH_RemoveHook(MH_ALL_HOOKS); // Remove all Hooks.
    UninitializeMods(); // Disable all modules.
    FreeLibraryAndExitThread(lpParam, 1); // Uninject the client completly.
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    if (ul_reason_for_call == DLL_PROCESS_ATTACH) {
        // Prevent the DLL from being unloaded
        DisableThreadLibraryCalls(hModule);

        // Create a new thread to initialize the client on.
        CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)InitializeClient, hModule, 0, nullptr);

        // Create a new thread for ejection.
        CreateThread(0, 0, (LPTHREAD_START_ROUTINE)ejectThread, hModule, 0, 0);
    }
    if (ul_reason_for_call == DLL_PROCESS_DETACH) {
        // Here is where to write code for when a dll detach.
        // We have that writen on ejectThread so no worries.
    }
    return TRUE;
}

