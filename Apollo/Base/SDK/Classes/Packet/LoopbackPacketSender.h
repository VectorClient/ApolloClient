#pragma once

class Packet;
// LoopbackPacketSender VTable
class LoopbackPacketSender { // // 1.21.2
public:
	BUILD_ACCESS(class NetworkSystem*, NetworkSystem, 0x20);
public:
	virtual void Destructor(); // ~LoopbackPacketSender() // LoopbackPacketSender Destructor
	virtual void send(Packet* packet);
	virtual void sendToServer(Packet* packet);
	virtual void sendToClient(class UserEntityIdentifierComponent const*, void* Packet);
	virtual void sendToClient(class NetworkIdentifier const&, void* Packet, unsigned char);
	virtual void sendToClients(std::vector<class NetworkIdentifierWithSubId, std::allocator<class NetworkIdentifierWithSubId>> const&, void* Packet);
	virtual void sendBroadcast(void* Packet);
	virtual void sendBroadcast(class NetworkIdentifier const&, unsigned char, void* Packet);
	virtual void flush(class NetworkIdentifier const&);
};

struct SocialGameConnectionInfo
{
    BUILD_ACCESS(std::string, realIp, 0x8);
    BUILD_ACCESS(std::string, serverIp, 0x28);
};

class NetworkSystem {
public:
	BUILD_ACCESS(class RemoteConnectorComposite*, RemoteConnectorComposite, 0x80);

    virtual ~NetworkSystem();
    virtual SocialGameConnectionInfo* getConnectionInfo(void);
    virtual void useIPv4Only(void);
    virtual void useIPv6Only(void);
    virtual unsigned __int16 getDefaultGamePort(void);
    virtual unsigned __int16 getDefaultGamePortv6(void);
    virtual void _onDisable(void);
    virtual void _onEnable(void);
    virtual void onNewIncomingConnection(class NetworkIdentifier const&, std::shared_ptr<class NetworkPeer>);
    virtual void onNewOutgoingConnection(class NetworkIdentifier const&, std::shared_ptr<class NetworkPeer>);
    virtual void onConnectionClosed(NetworkIdentifier const&, const std::string&, bool);
    virtual void onAllConnectionsClosed(const std::string&, bool);
    virtual void onAllRemoteConnectionsClosed(const std::string&, bool);
    virtual void onOutgoingConnectionFailed(void);
    virtual void onWebsocketRequest(const std::string&, const std::string&);
    virtual void onNewIncomingLocalConnection(NetworkIdentifier const&, std::shared_ptr<class NetworkPeer>);
    virtual void onNewOutgoingLocalConnection(NetworkIdentifier const&, std::shared_ptr<class NetworkPeer>);
};

class RemoteConnectorComposite {
public:
	BUILD_ACCESS(class RaknetConnector*, RakNetConnector, 0x60);
};