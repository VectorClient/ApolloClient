#pragma once

class PlayerInventory
{
private:
	char pad_0x0000[16];  //0x0000
public:
	int hotbarSlot;  //0x0010
private:
	char pad_0x0014[168];  // 0x0014
public:
	Inventory* inventory;  //0x00B0
};