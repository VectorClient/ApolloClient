#pragma once

class PingHolder : public Module
{
public:
	PingHolder(int keybind = Keys::NUM_0, bool enabled = false) :
		Module("Ping Holder", "Misc", "Ping holder", keybind, enabled)
	{
		addSlider("Ping", "The ping", &Global::PingHolderValue, 0, 130);
		addBool("Randomize", "randomize the ping between Low and High", &Global::RandomizePing);
		addSlider("Low Ping", "lowest ping", &Global::LowPingValue, 0.f, 130);
		addSlider("High Ping", "highest ping", &Global::HighPingValue, 0.f, 130);
	}

	void onEvent(PacketEvent* event) override {
        auto s = event->Sender;
        auto ns = s->NetworkSystem;
        auto rcc = ns->RemoteConnectorComposite;
        auto rkc = rcc->RakNetConnector;
        auto gp = rkc != nullptr ? rkc->getPeer() : nullptr;

        if (Global::PingHolderValue) {
            if (rkc != nullptr && Address::getLocalPlayer() != nullptr) {
                std::string jIp = rkc->JoinedIp;
                if (!jIp.empty()) {
                    int p1ng = gp != nullptr ? gp->getPing() : -1;

                    bool c1 = p1ng > Global::PingHolderValue;
                    bool c2 = !gp->RunUpdateCycle();
                    bool c3 = TimeUtils::hasTimeElapsed("DelayForSend", Global::PingHolderValue, true);
                    bool c4 = TimeUtils::hasTimeElapsed("DelayForRelease", 7000, true);

                    if (c1) {
                        if (s && ns && rcc && rkc && gp && Address::getLocalPlayer()) {
                            rkc->connect(1, 1);

                            if (c2 && c3) {
                                *event->cancelled = true;
                            }

                            if (c4) {
                                gp->Ping(Global::LowPingValue);
                            }
                        }
                    }
                }
            }
        }

		/*if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector != nullptr && Address::getLocalPlayer() != nullptr) {
			if (event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->JoinedIp != "") { // Checks if User is in a server
				int ping = event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->getPing();

				if (ping > Global::PingHolderValue) {
					event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->connect(1, 1);

					if (!event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->RunUpdateCycle()) {
						*event->cancelled = true;
					}

					if (TimeUtils::hasTimeElapsed("DelayForRelease", 7000, true)) {
						//event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->ReleaseSockets(event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->GetSocket());
						event->Sender->NetworkSystem->RemoteConnectorComposite->RakNetConnector->getPeer()->Ping(Global::LowPingValue);
					}
				}
			}
		}*/
	}
};