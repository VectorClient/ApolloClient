#pragma once

class LevelEventPacket : public Packet {
public:
    static const int EVENT_START_RAIN = 3001;
    static const int EVENT_START_THUNDER = 3002;
    static const int EVENT_STOP_RAIN = 3003;
    static const int EVENT_STOP_THUNDER = 3004;
    int            eventId;  // this+0x30
    Vector3<float> pos;      // this+0x34
    int            data;     // this+0x40
};

