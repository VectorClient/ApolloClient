#pragma once
#include "PacketID.h"


enum class PacketPriority {
	High,
	Immediate,
	Low,
	Medium,
	Count
};

enum class Reliability {
	Reliable,
	ReliableOrdered,
	Unreliable,
	UnreliableSequenced
};

enum class Compressibility {
	Compressible,
	Incompressible
};

class PacketHandlerDispatcherInstance {
public:
	uintptr_t** vTable;
};

class Packet {
public:
	/* Fields */
	PacketPriority packetPriority = PacketPriority::Low; //0x0008
	Reliability peerReliability = Reliability::Reliable; //0x000C
	uint64_t clientSubID; //0x0010
	char pad_0018[8]; //0x0018
	PacketHandlerDispatcherInstance* packetHandlerDispatcher; //0x0020
	Compressibility compressType = Compressibility::Compressible; //0x0028
	char pad_002C[4]; //0x002C

	// You can use those If we didn't have vftables for Packet.
	/*int getId() { return Memory::CallVFunc<1, int>(this); } // get ID from Packet's VTables.
	std::string getName(void) { return Memory::CallVFunc<2, TextHolder*>(this)->getText(); } // get Name from Packet's VTables.*/

	virtual void packetConstructor(void) {};
	virtual PacketID getId(void) { return (PacketID)0x0; };
	virtual class TextHolder getTypeName(void) { return (class TextHolder)nullptr; };
	virtual void write(class BinaryStream&) {};
	virtual void read(class ReadOnlyBinaryStream&) {};
	virtual void readExtended(class ReadOnlyBinaryStream&) {};
	virtual void disallowBatching(void) {};
};

class MinecraftPackets {
public:
	static std::shared_ptr<Packet> createPacket(int packetId) {
		using createPacket = std::shared_ptr<Packet>(__cdecl*)(int);
		static createPacket createPacketFunc = 0x0;
		if (!createPacketFunc) createPacketFunc = (createPacket)Memory::getXref(
			(uintptr_t)Memory::findSig(xorstr_("E8 ? ? ? ? 90 48 83 BD ? ? 00 00 ? 0F 84 ? ? ? ? ff")));
		return createPacketFunc(packetId);
	}

	/*static std::shared_ptr<Packet> createPacket(int id) {

		static void* Address;

		if (Address == NULL) {
			Address = Memory::findSig("40 53 48 83 EC 30 45 33 C0 48 8B D9 FF CA 81 FA ? ? ? ? 0F 87 ? ? ? ? 48 63 C2 48 8D 0D ? ? ? ? 0F B6 84 01 ? ? ? ? 8B 94 81 ? ? ? ? 48 03 D1");
		}

		auto pFunction = reinterpret_cast<std::shared_ptr<Packet>(__fastcall*)(int)>(Address);
		return pFunction(id);
	}*/
};

#include "NetworkBlockPosition.h"

#include "CommandRequestPacket.h"
#include "DisconnectPacket.h"
#include "InventoryTransactionPacket.h"
#include "LevelEventPacket.h"
#include "MobEquipmentPacket.h"
#include "MovePlayerPacket.h"
#include "NetworkStackLatencyPacket.h"
#include "PlayerActionPacket.h"
#include "PlayerAuthInputPacket.h"
#include "PlaySoundPacket.h"
#include "TextPacket.h"