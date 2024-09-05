#pragma once


class IRC : public Module
{
public:
    IRC(int keybind = Keys::NONE, bool enabled = false) :
        Module("IRC", "Misc", "Controls if the IRC feature is enabled.", keybind, enabled)
    {

    }// 59.85.23.228
    //std::string ipAddress = "59.85.23.228";
    std::string ipAddress = "192.168.1.10";
    int port = 6667;
    //std::string username = "USA";
    std::string username = "Hiroki";
    //std::string username = u8"hiroki ˜r(˜cOwner˜r)˜r";
    std::string IRCMessage = "";

    SOCKET sock;

    

    void onEnabled() {
        if (Address::getLocalPlayer() == nullptr)
            return;

        ChatUtils::sendMessage(Utils::combine(Utils::combine(BOLD, GRAY, "[IRC] ", RESET, "Attempting to join IRC Chat...")));
        DiscordUtil::sendIRCMessage(Utils::combine(Global::Username, " joined the room").c_str(), "IRC");
        ChatUtils::sendMessage(Utils::combine(Utils::combine(BOLD, GRAY, "[IRC] ", RESET, Global::Username, " joined the room")));
    }

    void onDisabled() {
        if (Address::getLocalPlayer() == nullptr)
            return;

        DiscordUtil::sendIRCMessage(Utils::combine(Global::Username, " left the room").c_str(), "IRC");
        ChatUtils::sendMessage(Utils::combine(Utils::combine(BOLD, GRAY, "[IRC] ", RESET, Global::Username, " left the room")));
    }

    void onEvent(PacketEvent* event) {
        if (Address::getLocalPlayer() == nullptr)
            return;

        if (strcmp(event->Packet->getTypeName().getText(), "TextPacket") == 0) {
            std::string* Message = (std::string*)(__int64)((char*)event->Packet + 0x58);
            if (Message->at(0) == '#') {
                //std::string IRCMsg = Message->c_str();
                std::string IRCMsg = Message->substr(1);
                ChatUtils::sendNormalMessage(Utils::combine(Utils::combine(BOLD, GRAY, "[IRC] ", RESET, DARK_GRAY, "[", GRAY, "sigma", DARK_GRAY, "] [", "Fat redstone eater", "] ", RESET, IRCMsg.c_str())));
                
                TestUtil::sendIRCMessage(IRCMsg, "Fat redstone eater", "Sigma");

                *event->cancelled = true;
            }
        }
    }

    void sendMessage(std::string message) {
        std::string IRCMessageLMAO = Utils::combine(BOLD, GRAY, "[IRC] ", RESET).c_str();
        ChatUtils::sendMessage(Utils::combine(IRCMessageLMAO, message.c_str()));
    }
};