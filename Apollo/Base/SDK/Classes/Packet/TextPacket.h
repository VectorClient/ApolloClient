#pragma once

enum class TextPacketType {
	RAW = 0,
	CHAT = 1,
	TRANSLATION = 2,
	POPUP = 3,
	JUKEBOX_POPUP = 4,
	TIP = 5,
	SYSTEM = 6,
	WHISPER = 7,
	ANNOUNCEMENT = 8,
	JSON_WHISPER = 9,
	JSON = 10,
	JSON_ANNOUNCEMENT = 11
};

class TextPacket : public Packet
{
public:
	TextPacketType           type;       // this+0x30
	std::string              author;     // this+0x38
	std::string              message;    // this+0x58
	std::vector<std::string> Params;     // this+0x78
	bool                     Localize;   // this+0x90
	std::string              Xuid;       // this+0x98
	std::string              PlatformId; // this+0xB8
};