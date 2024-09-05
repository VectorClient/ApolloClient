#pragma once

std::map<uint8_t, std::string> flagNames = {
    { 0, "VerticalMotion" },
    { 2, "Highjump" },
    { 3, "Fly" },
    { 4, "Highjump" },
    { 7, "Phase" },
    { 9, "Speed" },
    { 10, "Bunnyhop" },
    { 14, "Combat" },
    { 15, "NoSwing" },
    { 25, "BadPackets" },
    { 27, "VClip" },
    { 28, "Autoclicker" },
    { 31, "Instabreak" },
    { 35, "ChestStealer" },
    { 36, "Blink" },
    { 37, "Spider" },
    { 40, "Scaffold" },
    { 42, "TimerLimit (Some weird yDelta check)"}, // dk fs
    { 43, "FastBreak" },
    { 11, "Strafe" },
    { 21, "ScaffoldRot" },
    { 16, "Reach" },
    { 8, "DownMotion" },
    { 5, "Motion" },
    { 13, "Motion_2_" },
    { 34, "InvalidGround (Step)"},
    { 44, "Mining" },
    { 45, "BadTick/Timer" },
    { 46, "Scaffold (Invalid face)" },
    {48, "Timer"}
};

std::map<uint8_t, std::string> typeMap = {
    { 0, "Single" },
    { 1, "A" },
    { 2, "B" },
    { 3, "C" },
    { 4, "D" },
    { 5, "E" },
    { 6, "F" },
    { 7, "G" },
    { 8, "H" },
    { 9, "I" },
    { 10, "J" },
    { 11, "K" },
    { 12, "L" },
    { 13, "M" },
    { 14, "N" },
    { 15, "O" },
    { 16, "P" },
    { 17, "Q" },
    { 18, "R" },
    { 19, "S" },
    { 20, "T" },
    { 21, "U" },
    { 22, "V" },
    { 23, "W" },
    { 24, "X" },
    { 25, "Y" },
    { 26, "Z" },
};

const std::string& GetFlagName(uint8_t flag)
{
    if (flagNames.contains(flag))
    {
        return flagNames[flag];
    }
    else
    {
        static std::string unmappedFlag = "Unmapped flag";
        return unmappedFlag;
    }
}

const std::string& GetType(uint8_t type)
{
    return typeMap[type];
}

// Base64 decoding table
static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

// Function to check if a character is a valid Base64 character
inline bool is_base64(unsigned char c) {
    return (isalnum(c) || (c == '+') || (c == '/'));
}

// Function to decode a Base64 encoded string
std::vector<uint8_t> base64_decode(const std::string& encoded_string) {
    int in_len = encoded_string.size();
    int i = 0, j = 0, in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::vector<uint8_t> decoded_bytes;

    while (in_len-- && (encoded_string[in_] != '=') && is_base64(encoded_string[in_])) {
        char_array_4[i++] = encoded_string[in_]; in_++;
        if (i == 4) {
            for (i = 0; i < 4; i++) {
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            }

            char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

            for (i = 0; (i < 3); i++) {
                decoded_bytes.push_back(char_array_3[i]);
            }
            i = 0;
        }
    }

    if (i) {
        for (j = i; j < 4; j++) {
            char_array_4[j] = 0;
        }

        for (j = 0; j < 4; j++) {
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        }

        char_array_3[0] = (char_array_4[0] << 2) + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4) + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];

        for (j = 0; (j < i - 1); j++) {
            decoded_bytes.push_back(char_array_3[j]);
        }
    }

    return decoded_bytes;
}