#pragma once

class String
{
public:
    static std::string ReplaceAll(std::string str, const char* c1, const char* c2) {
        size_t pos = 0;
        while ((pos = str.find(c1, pos)) != std::string::npos) {
            str.replace(pos, strlen(c1), c2);
            pos += strlen(c2);
        }
        return str;
    }

    static std::string ReplaceAll(const std::string& str, const std::string& from, const std::string& to) {
        std::string result = str;
        size_t start_pos = 0;
        while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
            result.replace(start_pos, from.length(), to);
            start_pos += to.length();
        }
        return result;
    }

    static std::string ReplaceAll(const std::string& str, const std::vector<std::pair<std::string, std::string>>& replacements) {
        std::string result = str;
        for (const auto& [from, to] : replacements) {
            result = ReplaceAll(result, from, to);
        }
        return result;
    }

    static std::string Trim(std::string& str) {
        str.erase(0, str.find_first_not_of(" \t\n\r\f\v"));
        str.erase(str.find_last_not_of(" \t\n\r\f\v") + 1);
        return str;
    }

    static std::string RemoveNonAscii(std::string& str) {
        // make sure to exclude symbols like ampersands and the § char here (so they dont get removed)
        std::erase_if(str, [](unsigned char c) { return c > 127 && c != '&'; });
        return str;
    }

};