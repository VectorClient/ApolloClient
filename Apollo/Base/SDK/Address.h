#pragma once

namespace Address {
	class Core {
	public:
		static inline MinecraftUIRenderContext* RenderContext = nullptr;
		static inline ClientInstance* ClientInstance = nullptr;
		static inline FontRepos* Font = nullptr;
	};

	MinecraftUIRenderContext* getRenderContext() {
		return Core::RenderContext;
	}

	ClientInstance* getClientInstance() {
		return Core::ClientInstance;
	}

	FontRepos* getFont() {
		return Core::Font;
	}

	LoopbackPacketSender* getLoopback() {
		return getClientInstance()->getLoopbackPacketSender();
	}

	Player* getLocalPlayer() {
		return getClientInstance()->getLocalPlayer();
	}

	BlockSource* getBlockSource() {
		return getClientInstance()->getBlockSource();
	}

	TimerClass* getTimerClass() {
		return getClientInstance()->getTimerClass();
	}

	MinecraftGame* getMinecraftGame() {
		return getClientInstance()->getMinecraftGame();
	}

	bool canUseKeys() {
		return getMinecraftGame()->getCanUseKeys();
	}
}