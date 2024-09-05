#pragma once

enum class CommandOriginType : int8_t {
    Player = 0x0,
    CommandBlock = 0x1,
    MinecartCommandBlock = 0x2,
    DevConsole = 0x3,
    Test = 0x4,
    AutomationPlayer = 0x5,
    ClientAutomation = 0x6,
    DedicatedServer = 0x7,
    Entity = 0x8,
    Virtual = 0x9,
    GameArgument = 0xA,
    EntityServer = 0xB,
    Precompiled = 0xC,
    GameDirectorEntityServer = 0xD,
    Scripting = 0xE,
    ExecuteContext = 0xF,
};


class mcUUID {
public:
    uint64_t mostSig, leastSig;
};
class CommandOriginData {
public:
    CommandOriginType mType;
    mcUUID            mUuid;
    std::string       mRequestId;
    int64_t           mPlayerId;
};

class CommandRequestPacket : public Packet {

public:
    std::string       Command;        // this+0x30
    CommandOriginData Origin;         // this+0x50
    bool              InternalSource; // this+0x90
};