#pragma once

class ChatUtils
{
public:

	static void sendMessage(std::string message)
	{
		Address::getLocalPlayer()->displayClientMessage(Utils::combine(DARK_GRAY, "[", GRAY, "apollo", DARK_GRAY, "] ", RESET, message));
	}

	static void sendDisablerMessage(std::string message)
	{
		Address::getLocalPlayer()->displayClientMessage(Utils::combine(GRAY, "[", RESET, "Disabler", GRAY, "] ", RESET, message));
	}

	static void sendCustomMessage(std::string message, std::string title)
	{
		Address::getLocalPlayer()->displayClientMessage(Utils::combine(GRAY, "[", RESET, title.c_str(), GRAY, "] ", RESET, message));
	}

	static void sendNormalMessage(std::string message)
	{
		Address::getLocalPlayer()->displayClientMessage(message);
	}
};
