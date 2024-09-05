#pragma once

#include "VertextFormat.h"
#include "MaterialPtr.h"

class ScreenContext;

class Tessellator
{
private:
	char pad_0000[0x184]; // 1.21.2
	Vector3<float> transformOffset;

public:
	void addPostTransformOffset(int x, int y, int z) // 1.21.2
	{
		addPostTransformOffset((float)x, (float)y, (float)z);
	}

	void addPostTransformOffset(float x, float y, float z) // 1.21.2
	{
		this->transformOffset = this->transformOffset.add(Vector3<float>(x, y, z));
	};

	// Tessellator::begin
	inline void begin(VertextFormat format, int reservedVerticies = 0) // 1.21.2
	{
		using tess_begin_t = void(__fastcall*)(Tessellator*, VertextFormat, bool);
		static tess_begin_t tess_begin = reinterpret_cast<tess_begin_t>(Memory::findSig(xorstr_("48 89 5C 24 ? 56 48 83 EC ? 80 B9 ? ? ? ? ? 45 0F B6")));
		tess_begin(this, format, reservedVerticies);
	}

	// Tessellator::vertex
	inline void vertex(float x, float y, float z) // 1.21.2
	{
		using tess_vertex_t = void(__fastcall*)(Tessellator*, float, float, float);
		static tess_vertex_t tess_vertex = reinterpret_cast<tess_vertex_t>(Memory::findSig(xorstr_("40 57 48 81 EC ? ? ? ? 0F 29 7C ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 44 24 ? 8B 81")));
		tess_vertex(this, x, y, z);
	}

	// Tessellator::color
	inline void color(float r, float g, float b, float a) // 1.21.2
	{
		using tess_color_t = void(__fastcall*)(Tessellator*, float, float, float, float);
		static tess_color_t tess_color = reinterpret_cast<tess_color_t>(Memory::findSig(xorstr_("80 B9 ? ? ? ? ? 4C 8B D2 4C 8B C1")));
		// The same sig in 1.20.0.1
		tess_color(this, r, g, b, a);
	}

	// Tessellator::setRotation
	inline void setRotation(float angle, Vector3<float> pivot) // 1.21.2
	{
		using tess_setRotation_t = void(__fastcall*)(Tessellator*, float, Vector3<float>);
		static tess_setRotation_t tess_setRotation = reinterpret_cast<tess_setRotation_t>(Memory::findSig(xorstr_("48 89 5C 24 ? 57 48 81 EC ? ? ? ? F3 41 0F 10")));
		tess_setRotation(this, angle, pivot);
	}

	// Tessellator::resetTransform
	inline void resetTransform(bool a1) // 1.21.2
	{
		using tess_resetTransform_t = void(__fastcall*)(Tessellator*, bool);
		static tess_resetTransform_t tess_resetTransform = reinterpret_cast<tess_resetTransform_t>(Memory::findSig(xorstr_("40 53 48 81 EC ? ? ? ? C6 81 ? ? ? ? ? 48 8B D9 84 D2")));
		tess_resetTransform(this, a1);
	}

	// MeshHelpers::renderMeshImmediately
	inline void renderMeshImmediately(ScreenContext* ctx, MaterialPtr* material) // 1.21.2
	{
		using meshHelper_renderImm_t = void(__fastcall*)(ScreenContext*, Tessellator*, MaterialPtr*);
		static meshHelper_renderImm_t meshHelper_renderImm = reinterpret_cast<meshHelper_renderImm_t>(Memory::findSig(xorstr_("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 49 8B F8 48 8B DA 48 8B F1 80 BA")));
		meshHelper_renderImm(ctx, this, material);
	}
};
