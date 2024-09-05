#pragma once

void* onConnectionRequestCreate;

void ConnectionRequestCreateDetour(ConnectionRequest* result, PrivateKeyManager* privKeyManager, Certificate* cert,
	std::string* SelfSignedID, std::string* ServerAddress, uint64_t ClientRandomID, std::string* SkinID, __int64 SkinData, __int64 CapeData,
	SerializedSkin* Skin, std::string* DeviceID, int InputMode, int UIProfile, int GuiScale, std::string* LanguageCode, bool isEditorMode,
	bool IsEduMode, std::string* TenantId, int8_t ADRole, std::string* PlatformUserId, std::string* ThirdPartyName, bool ThirdPartyNameOnly,
	std::string* PlatformOnlineID, std::string* PlatformOfflineID, std::string* CapeID) {

	const auto& skin = ((Player*)Address::getLocalPlayer())->getSkin();

	if (skin != nullptr) {

	}

	/*std::string path = FileUtils::getResourcePath() + "\\TargetHead.png";

	FILE* fp;
	errno_t err = fopen_s(&fp, path.c_str(), "wb");
	if (fp) {
		svpng(fp, Skin->mSkinImage.mWidth, Skin->mSkinImage.mHeight, (const uint8_t*)SkinData, 1);
		fclose(fp);
	}*/

	std::string serverAddr = *ServerAddress;
	// if the server address contains a colon, then split it and use the first part
	if (serverAddr.find(":") != std::string::npos) {
		serverAddr = serverAddr.substr(0, serverAddr.find(':'));
	}

	Global::Notification.addNotification("Apollo", "Connecting to " + serverAddr + "...", 3.f);

	Memory::CallFunc<void*, ConnectionRequest*, PrivateKeyManager*, Certificate*, std::string*, std::string*,
		uint64_t, std::string*, __int64, __int64, SerializedSkin*, std::string*, int, int, int, std::string*,
		bool, bool, std::string*, int8_t, std::string*, std::string*, bool, std::string*, std::string*, std::string*>(
			onConnectionRequestCreate, result, privKeyManager, cert, SelfSignedID, ServerAddress, ClientRandomID, SkinID, SkinData, CapeData,
			Skin, DeviceID, InputMode, UIProfile, GuiScale, LanguageCode, isEditorMode, IsEduMode, TenantId, ADRole,
			PlatformUserId, ThirdPartyName, ThirdPartyNameOnly, PlatformOnlineID, PlatformOfflineID, CapeID
	);
}

class ConnectionRequestHook : public FuncHook {
public:
	bool Initialize() override {
		// ConnectionRequest::create
		void* connectionRequestCreateAddr = Memory::findSig(xorstr_("40 55 53 56 57 41 54 41 55 41 56 41 57 48 8D AC 24 ? ? ? ? 48 81 EC ? ? ? ? 0F 29 B4 24 ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 85 ? ? ? ? 49 8B D9 48 89 55"));

		if (!Memory::HookFunction(connectionRequestCreateAddr, (void*)&ConnectionRequestCreateDetour, &onConnectionRequestCreate, xorstr_("ConnectionRequestCreate"))) { return false; }

		return true;
	}

	static ConnectionRequestHook& Instance() {
		static ConnectionRequestHook instance;
		return instance;
	}
};