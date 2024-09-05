#pragma once

class PacketEvent : public Event
{
public:
    PacketEvent(LoopbackPacketSender* sender, Packet* packet)
    {
        Sender = sender;
        Packet = packet;
    }

    EventType getType() const override { return EventType::PacketSend; }
    LoopbackPacketSender* Sender;
    Packet* Packet;
};