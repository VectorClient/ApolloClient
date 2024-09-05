#pragma once

class NetworkStackLatencyPacket : public Packet {
public:
    int    mCreateTime; // this+0x30
    bool   mFromServer; // this+0x38
};