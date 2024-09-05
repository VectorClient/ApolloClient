#pragma once

enum class PositionMode : int {
    Normal = 0,
    Respawn = 1,
    Teleport = 2,
    OnlyHeadRot = 3,
};

enum class TeleportationCause : int {
    Unknown = 0x0,
    Projectile = 0x1,
    ChorusFruit = 0x2,
    Command = 0x3,
    Behavior = 0x4,
    Count = 0x5,
};

class MovePlayerPacket : public Packet {
public:
    int                    mPlayerID;         // this+0x30
    Vector3<float>         mPos;              // this+0x38
    Vector2<float>         mRot;              // this+0x44
    float                  mYHeadRot;         // this+0x4C
    PositionMode           mResetPosition;    // this+0x50
    bool                   mOnGround;         // this+0x51
    int                    mRidingID;         // this+0x58
    TeleportationCause     mCause;            // this+0x60
    int                    mSourceEntityType; // this+0x64
    int                    mTick;             // this+0x68
};