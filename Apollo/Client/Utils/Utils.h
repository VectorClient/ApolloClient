#pragma once

#pragma region DirectClassAccess

template <typename Ret, typename Type>
// The type and offset
Ret& DirectAccess(Type* type, size_t offset) {
    union {
        size_t raw;
        Type* source;
        Ret* target;
    } u;
    u.source = type;
    u.raw += offset;
    return *u.target;
}

#define AS_FIELD(type, name, fn) __declspec(property(get = fn, put = set##name)) type name
#define DEF_FIELD_RW(type, name) __declspec(property(get = get##name, put = set##name)) type name

#define FAKE_FIELD(type, name)                                                                                       \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name()

#define BUILD_ACCESS(type, name, offset)                                                                             \
AS_FIELD(type, name, get##name);                                                                                     \
type get##name() const { return DirectAccess<type>(this, offset); }												 \
void set##name(type v) const { DirectAccess<type>(this, offset) = v; }

#pragma endregion

namespace Utils {

#pragma region Memory & Debug
#define logDebug(str) OutputDebugString(str)

    // CallFunc is what we'll be using to call the original functions.
    // CallVFunc is what we'll be using to call the vftables by there index.

    // Combine is what we''ll be using to combine strings, ints, arrays etc together.

    // HookFunction is just a clean MinHook util it'll initialize minhook for us if it hasn't already been initialized and created/enabled 
    // the hook aswell as return log information to the debug stream so we can see if hooks fail to create/enable and where they failed

    // template function that converts the specified value to a string using a stringstream
    template <typename T>
    std::string combine(T t)
    {
        std::stringstream ss;
        ss << t;
        return ss.str();
    }

    // template function that converts the specified values to a string using a stringstream
    template <typename T, typename... Args>
    std::string combine(T t, Args... args)
    {
        std::stringstream ss;
        ss << t << combine(args...);
        return ss.str();
    }

    static const uintptr_t RangeStart = reinterpret_cast<uintptr_t>(GetModuleHandleA("Minecraft.Windows.exe"));

    static inline void nopBytes(void* dst, unsigned int size) {
        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memset(dst, 0x90, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    };

    static inline void patchBytes(void* dst, void* src, unsigned int size) {
        DWORD oldprotect;
        VirtualProtect(dst, size, PAGE_EXECUTE_READWRITE, &oldprotect);
        memcpy(dst, src, size);
        VirtualProtect(dst, size, oldprotect, &oldprotect);
    };
#pragma endregion

#pragma region Utils

    static inline bool leftClick, leftDown, rightClick, rightDown, middleClick, middleDown;

    static void onMouseClick(int key, bool isDown)
    {
        switch (key)
        {
        case 1:
            leftClick = isDown;
            leftDown = isDown ? true : leftDown;
            break;
        case 2:
            rightClick = isDown;
            rightDown = isDown ? true : rightDown;
            break;
        case 3:
            middleClick = isDown;
            middleDown = isDown ? true : middleDown;
            break;
        }
    }

    static bool invalidChar(char c) {
        return !(c >= 0 && *reinterpret_cast<unsigned char*>(&c) < 128);
    }

    static void systemPlaySound(std::string name) {
        std::string path = FileUtils::getResourcePath() + "\\" + name.c_str();
        PlaySoundA((LPCSTR)path.c_str(), NULL, SND_FILENAME | SND_ASYNC);
    }

    static std::string sanitize(std::string text) {
        std::string out;
        bool wasValid = true;
        for (char c : text) {
            bool isValid = !invalidChar(c);
            if (wasValid) {
                if (!isValid) {
                    wasValid = false;
                }
                else {
                    out += c;
                }
            }
            else {
                wasValid = isValid;
            }
        }
        return out;
    }

    static inline void toastNotification(std::string title, std::string message) {
        ToastNotification toast = ToastNotification(ToastNotificationManager::GetTemplateContent(ToastTemplateType::ToastImageAndText02));
        winrt::Windows::Data::Xml::Dom::IXmlNodeList elem = toast.Content().GetElementsByTagName(L"text");
        elem.Item(0).InnerText(winrt::to_hstring(title));
        elem.Item(1).InnerText(winrt::to_hstring(message));
        ToastNotificationManager::CreateToastNotifier().Show(toast);
    }

#pragma endregion
}