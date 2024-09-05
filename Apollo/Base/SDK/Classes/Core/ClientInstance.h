#pragma once

#include "Font.h"
#include "GLMatrix.h"
#include "MinecraftGame.h"
#include "TimerClass.h"
#include "GuiData.h"
#include "LevelRender.h"
#include "bgfx.h"
#include "../Network/RakNetConnector.h"
#include "../Network/ConnectionRequest.h"

// Define 
#define LocalPlayerIndex 29 // 1.21.2
#define DisableInputIndex 331 // 1.21.2
#define ReleaseMouseIndex 332 // 1.21.2
#define GrabMouseIndex 333 // 1.21.2

class ClientInstance {
public:
	// Use CallVFunc to call the VTables by there index.

	Player* getLocalPlayer() {
		return (Player*)Memory::CallVFunc<LocalPlayerIndex, Player*>(this); // 1.21.2
	}

	BlockSource* getBlockSource() {
		return Memory::CallVFunc<28, BlockSource*>(this); // 1.21.2
	}

	void setDisableInput(bool disable) { // grabMouse - 1
		return Memory::CallVFunc<DisableInputIndex, void>(this, disable); // 1.21.2
	}

	void releaseMouse() { // grabMouse - 1
		return Memory::CallVFunc<ReleaseMouseIndex, void>(this); // 1.21.2
	}

	void grabMouse() {
		return Memory::CallVFunc<GrabMouseIndex, void>(this); // 1.21.2
	}

	bgfx_context* getBGFX()
	{
		static auto pCtx = Memory::scanSig("48 8B 0D ? ? ? ? 66 44 89 74 24 ? E8 ? ? ? ? 0F B7 08 66 89 0F E9 ? ? ? ? 8B 53 3C 49 2B C8", 3);
		return *reinterpret_cast<bgfx_context**>(pCtx);
	}

	Vector2<float> getFov() { // Check for fov in clientinstance
		Vector2<float> fov;
		fov.x = *(float*)(reinterpret_cast<uintptr_t>(this) + 0x6F0); // 1.21.2
		fov.y = *(float*)(reinterpret_cast<uintptr_t>(this) + 0x704); // 1.21.2
		return fov;
	}

	GLMatrix* getGLMatrix() {
		return (GLMatrix*)((uintptr_t)(this) + 0x330); // 1.21.2
	}

	inline bool WorldToScreen(Vector3<float> pos, Vector2<float>& screen)
	{ // pos = pos 2 w2s, screen = output screen coords
		if (!getGuiData()) {
			return false;
		}
		Vector2<float> displaySize = getGuiData()->getmcResolution();
		class LevelRender* lr = getLevelRender();
		Vector3<float> origin = lr->getOrigin();
		Vector2<float> fov = getFov();

		pos.x -= origin.x;
		pos.y -= origin.y;
		pos.z -= origin.z;

		auto glmatrix = getGLMatrix();
		std::shared_ptr<GLMatrix> matrix = std::shared_ptr<GLMatrix>(glmatrix->correct());

		float x = matrix->transformx(pos);
		float y = matrix->transformy(pos);
		float z = matrix->transformz(pos);

		if (z > 0) return false;

		float mX = (float)displaySize.x / 2.0F;
		float mY = (float)displaySize.y / 2.0F;

		screen.x = mX + (mX * x / -z * fov.x);
		screen.y = mY - (mY * y / -z * fov.y);

		return true;
	}
public:
	BUILD_ACCESS(class LoopbackPacketSender*, LoopbackPacketSender, 0xF0); // 1.21.2
	BUILD_ACCESS(class MinecraftGame*, MinecraftGame, 0xC8); // 1.21.2
	BUILD_ACCESS(class TimerClass*, TimerClass, 0xD0); // 1.21.2
	BUILD_ACCESS(class GuiData*, GuiData, 0x558); // 1.21.2
	BUILD_ACCESS(class LevelRender*, LevelRender, 0xE0); // 1.21.2
};