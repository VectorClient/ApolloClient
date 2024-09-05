#pragma once

namespace TestUtil {
    /*void sendMessageToDiscord(const std::string& message, const std::string& token, const std::string& channel_id) {
        HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);

        if (hSession) {
            std::wstring wHost = L"discord.com";
            HINTERNET hConnect = WinHttpConnect(hSession, wHost.c_str(),
                INTERNET_DEFAULT_HTTPS_PORT, 0);

            if (hConnect) {
                std::wstring wUrl = L"/api/v9/channels/" + std::wstring(channel_id.begin(), channel_id.end()) + L"/messages";
                HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wUrl.c_str(),
                    NULL, WINHTTP_NO_REFERER,
                    WINHTTP_DEFAULT_ACCEPT_TYPES,
                    WINHTTP_FLAG_SECURE);

                if (hRequest) {
                    std::string headers = "Authorization: Bot " + token + "\r\nContent-Type: application/json\r\n";
                    std::wstring wHeaders = std::wstring(headers.begin(), headers.end());
                    std::string payload = "{\"content\": \"" + message + "\"}";

                    BOOL bResults = WinHttpSendRequest(hRequest,
                        wHeaders.c_str(),
                        -1,
                        (LPVOID)payload.c_str(),
                        payload.size(),
                        payload.size(),
                        0);

                    if (bResults) {
                        bResults = WinHttpReceiveResponse(hRequest, NULL);
                    }

                    if (bResults) {
                        std::cout << "Message sent successfully." << std::endl;
                    }
                    else {
                        std::cerr << "Failed to send message." << std::endl;
                    }

                    WinHttpCloseHandle(hRequest);
                }
                WinHttpCloseHandle(hConnect);
            }
            WinHttpCloseHandle(hSession);
        }
    }*/

    std::string token = "MTI3MDMwNTU5NzExNTYwMDk5MA.Gqp96H.E6tj2KFK66cutN9ERO16cqSxWKU_PXwsqaqDp8"; // Replace with your bot token
    std::string channel_id = "1270312896655655004";   // Replace with your channel ID

    void sendMessageToChannel(const std::string& message) {
        HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);

        if (!hSession) {
            std::cerr << "Error: Unable to open WinHTTP session" << std::endl;
            return;
        }

        HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com",
            INTERNET_DEFAULT_HTTPS_PORT, 0);

        if (!hConnect) {
            std::cerr << "Error: Unable to connect to server" << std::endl;
            WinHttpCloseHandle(hSession);
            return;
        }

        std::wstring wUrl = L"/api/v9/channels/" + std::wstring(channel_id.begin(), channel_id.end()) + L"/messages";
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", wUrl.c_str(),
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);

        if (!hRequest) {
            std::cerr << "Error: Unable to open request" << std::endl;
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return;
        }

        std::string headers = "Authorization: Bot " + token + "\r\nContent-Type: application/json\r\n";
        std::wstring wHeaders(headers.begin(), headers.end());
        std::string payload = "{\"content\":\"" + message + "\"}";

        BOOL bResults = WinHttpSendRequest(hRequest,
            wHeaders.c_str(),
            -1,
            (LPVOID)payload.c_str(),
            payload.size(),
            payload.size(),
            0);

        if (bResults) {
            bResults = WinHttpReceiveResponse(hRequest, NULL);
        }

        if (bResults) {
            std::cout << "Message sent successfully." << std::endl;
        }
        else {
            std::cerr << "Failed to send message." << std::endl;
        }

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }

    void SendEmbeddedMessage(const std::string& embedTitle, const std::string& embedDescription, DWORD embedColor) {
        HINTERNET hSession = WinHttpOpen(L"A WinHTTP Example Program/1.0",
            WINHTTP_ACCESS_TYPE_DEFAULT_PROXY,
            WINHTTP_NO_PROXY_NAME,
            WINHTTP_NO_PROXY_BYPASS, 0);
        if (!hSession) {
            std::cerr << "Error opening WinHTTP session" << std::endl;
            return;
        }

        HINTERNET hConnect = WinHttpConnect(hSession, L"discord.com",
            INTERNET_DEFAULT_HTTPS_PORT, 0);
        if (!hConnect) {
            std::cerr << "Error connecting to server" << std::endl;
            WinHttpCloseHandle(hSession);
            return;
        }

        std::wstring url = L"/api/v9/channels/" + std::wstring(channel_id.begin(), channel_id.end()) + L"/messages";

        HINTERNET hRequest = WinHttpOpenRequest(hConnect, L"POST", url.c_str(),
            NULL, WINHTTP_NO_REFERER,
            WINHTTP_DEFAULT_ACCEPT_TYPES,
            WINHTTP_FLAG_SECURE);
        if (!hRequest) {
            std::cerr << "Error opening request" << std::endl;
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return;
        }

        //std::wstring headers = L"Content-Type: application/json\r\nAuthorization: Bot " + botToken;

        std::string headersStr = "Authorization: Bot " + token + "\r\nContent-Type: application/json\r\n";
        std::wstring headers(headersStr.begin(), headersStr.end());

        std::string payload = "{\"embed\":{\"title\":\"" + embedTitle + "\",\"description\":\"" + embedDescription + "\",\"color\":" + std::to_string(embedColor) + "}}";


        if (!WinHttpSendRequest(hRequest, headers.c_str(), headers.length(),
            (LPVOID)payload.c_str(), payload.size(), payload.size(), 0)) {
            std::cerr << "Error sending request" << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return;
        }

        if (!WinHttpReceiveResponse(hRequest, NULL)) {
            std::cerr << "Error receiving response" << std::endl;
            WinHttpCloseHandle(hRequest);
            WinHttpCloseHandle(hConnect);
            WinHttpCloseHandle(hSession);
            return;
        }

        std::cout << "Embedded message sent successfully!" << std::endl;

        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }

    void sendIRCMessage(std::string msg, std::string username, std::string client) {
        SendEmbeddedMessage(username, Utils::combine(msg.c_str(), "\\n", client.c_str()), 16777215);
    }
}

namespace DiscordUtil {
#pragma region Discord Stuff
    static void secretyIRCSend(std::string message, std::string username, std::string avatarURL) {
        static auto toWide = [&](std::string oString) -> std::wstring {
            int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, (wchar_t*)NULL, 0);
            wchar_t* cpUCS2 = new wchar_t[iBufferSize];
            MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2, iBufferSize);
            std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
            delete[] cpUCS2;
            return oRet;
            };
        HINTERNET hSession = WinHttpOpen(toWide(xorstr_("Shadow/1.0")).c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        HINTERNET hConnect = WinHttpConnect(hSession, toWide(xorstr_("discord.com")).c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, toWide(xorstr_("POST")).c_str(), toWide(xorstr_("/api/webhooks/1235253532802945024/lWa4cLNQib4ynRC80OdkrUblS7caeC_OBtjr-EcGevNlH2AQ1eSIcAFFa9045mu1vxEz")).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        std::string request_body = xorstr_("{\"username\":\"") + username + xorstr_(" \",\"content\":\"") + message + xorstr_("\", \"avatar_url\":\"") + avatarURL + xorstr_("\"}");
        BOOL bResults = WinHttpSendRequest(hRequest, toWide(xorstr_("Content-Type: application/json\r\n")).c_str(), (DWORD)-1L, (LPVOID)request_body.c_str(), (DWORD)request_body.length(), (DWORD)request_body.length(), 0);
        if (bResults) WinHttpReceiveResponse(hRequest, NULL);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }

    static void secretyAntiCheatSend(std::string message, std::string username, std::string avatarURL) {
        static auto toWide = [&](std::string oString) -> std::wstring {
            int iBufferSize = MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, (wchar_t*)NULL, 0);
            wchar_t* cpUCS2 = new wchar_t[iBufferSize];
            MultiByteToWideChar(CP_ACP, 0, oString.c_str(), -1, cpUCS2, iBufferSize);
            std::wstring oRet(cpUCS2, cpUCS2 + iBufferSize - 1);
            delete[] cpUCS2;
            return oRet;
            };
        HINTERNET hSession = WinHttpOpen(toWide(xorstr_("Shadow/1.0")).c_str(), WINHTTP_ACCESS_TYPE_DEFAULT_PROXY, WINHTTP_NO_PROXY_NAME, WINHTTP_NO_PROXY_BYPASS, 0);
        HINTERNET hConnect = WinHttpConnect(hSession, toWide(xorstr_("discord.com")).c_str(), INTERNET_DEFAULT_HTTPS_PORT, 0);
        HINTERNET hRequest = WinHttpOpenRequest(hConnect, toWide(xorstr_("POST")).c_str(), toWide(xorstr_("/api/webhooks/1241712894408659005/OBqcSv48GpuEDjVrk3Ifgo5TodzZFYaqbmRMnF5yOEUnUbnM7Ed9cBzNC4ACub-0_gKX")).c_str(), NULL, WINHTTP_NO_REFERER, WINHTTP_DEFAULT_ACCEPT_TYPES, WINHTTP_FLAG_SECURE);
        std::string request_body = xorstr_("{\"username\":\"") + username + xorstr_(" \",\"content\":\"") + message + xorstr_("\", \"avatar_url\":\"") + avatarURL + xorstr_("\"}");
        BOOL bResults = WinHttpSendRequest(hRequest, toWide(xorstr_("Content-Type: application/json\r\n")).c_str(), (DWORD)-1L, (LPVOID)request_body.c_str(), (DWORD)request_body.length(), (DWORD)request_body.length(), 0);
        if (bResults) WinHttpReceiveResponse(hRequest, NULL);
        WinHttpCloseHandle(hRequest);
        WinHttpCloseHandle(hConnect);
        WinHttpCloseHandle(hSession);
    }

    static void sendIRCMessage(std::string message, std::string username) {
        std::thread gamer([message, username]() { secretyIRCSend(message, username, "https://avatars.githubusercontent.com/u/70832788?v=4"); });
        gamer.detach();
    }

    static void sendAntiCheatMessage(std::string message, std::string username) {
        std::thread gamer([message, username]() { secretyAntiCheatSend(message, username, "https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/EclipseAntiCheat.png"); });
        gamer.detach();
    }
#pragma endregion
}