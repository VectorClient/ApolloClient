#pragma once

class PlaySoundPacket : public Packet {
public:
    std::string          mName;   // this+0x30
    NetworkBlockPosition mPos;    // this+0x50
    float                mVolume; // this+0x5C
    float                mPitch;  // this+0x60
};