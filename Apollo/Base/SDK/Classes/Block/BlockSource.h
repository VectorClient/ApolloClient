#pragma once

#include "Block.h"
#include "../Actor/Weather.h"
#include "../Actor/Dimension.h"

class BlockSource {
public:
	BUILD_ACCESS(class Dimension*, Dimension, 0x30); // 1.21.2

	Block* getBlock(Vector3<int> blockPos)
	{
		return Memory::CallVFunc<2, Block*, Vector3<int>&>(this, blockPos);
	}
};