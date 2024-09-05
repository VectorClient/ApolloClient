#pragma once

class HitResult
{
//private:
	//char pad_0x0000[0x1C]; //0x0000
public:
	Vector3<float> Origin;
	Vector3<float> RelativeRayEnd;
	int HitType;
	int BlockFace;
	Vector3<int> IBlockPos;
	Vector3<float> AbsoluteHitPos;

	void* entity; //0x0038 // WeakEntityRef
	bool isLiquid; //0x004C
	char pad_004D[3]; //0x004D
	int32_t liquidFace; //0x0050
	Vector3<int> liquidBlockPos; //0x0054
	Vector3<float> liquidPos; //0x0060
	bool indirectHit; //0x006C
	char pad_006D[3]; //0x006D
}; //Size: 0x0070


class Level { // Level VTable
public:
	// Level::getRuntimeActorList
	// Use CallVFunc to call the VTables by there index.
	std::vector<Actor*> getRuntimeActorList() { // Using a CallVFunc for getRuntimeActorList because There is no need for the entire VTable class
		std::vector<Actor*> listOut;
		Memory::CallVFunc<278, decltype(&listOut)>(this, &listOut); // 1.21.2.0
		return listOut;
	}

	// Level::getHitResult
	// Use CallVFunc to call the VTables by there index.
	HitResult* getHitResult() {
		return (HitResult*)Memory::CallVFunc<288, HitResult*>(this);
	}

	//BUILD_ACCESS(class HitResult, HitResult, 0xB30);

private:
	char pad_0x0000[0xB30]; //0x0000
public:
	Vector3<float> Origin; //0x0BD8 
	Vector3<float> RelativeRayEnd;
	int HitType;
	int BlockFace;
	Vector3<int> IBlockPos; //0x0BF8 
	Vector3<float> AbsoluteHitPos;
};