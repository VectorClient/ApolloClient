#pragma once

namespace FileUtils {
#pragma region Folders and Assets
    // Get Minecraft's RoamingState folder location.
    std::string getRoamingStatePath(void) {
        char* path = NULL;
        size_t length;

        // _dupenv_s Is used to retrieve an environment variable’s value.
        _dupenv_s(&path, &length, "appdata");

        // Return Minecraft's "RoamingState" path.
        return std::string(path) + xorstr_("\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\RoamingState");
    };

    // Get Minecraft's LocalState folder location.
    std::string getLocalStatePath(void) {
        char* path = NULL;
        size_t length;

        // _dupenv_s Is used to retrieve an environment variable’s value.
        _dupenv_s(&path, &length, "appdata");

        // Return Minecraft's "RoamingState" path.
        return std::string(path) + xorstr_("\\..\\Local\\Packages\\Microsoft.MinecraftUWP_8wekyb3d8bbwe\\LocalState");
    };

    // Get the client's folder or create one if it doesn't exist.
    std::string getAuthPath(void) {
        // The Client's file location.
        std::string ClientFilePath = getLocalStatePath() + xorstr_("\\logs\\gua256.txt");

        // Convert ClientFilePath to a filesystem::path.
        auto folderpath = std::filesystem::path(ClientFilePath);

        // Return the Client's folder location.
        return ClientFilePath;
    }

    // Get the client's folder or create one if it doesn't exist.
    std::string getClientPath(void) {
        // The Client's file location.
        std::string ClientFilePath = getRoamingStatePath() + xorstr_("\\Apollo");

        // Convert ClientFilePath to a filesystem::path.
        auto folderpath = std::filesystem::path(ClientFilePath);

        // Check if FolderPath exists.
        if (!std::filesystem::exists(folderpath)) { // If folder doesn't exist.
            std::filesystem::create_directories(folderpath); // Create a new folder.
        }

        // Return the Client's folder location.
        return ClientFilePath;
    }

    // Get the resource folder inside the client's folder.
    std::string getResourcePath(void) {
        // The Resource's file location.
        std::string ResourcePath = getClientPath() + xorstr_("\\Resource");

        // Convert ResourcePath to a filesystem::path.
        auto ResourceFolder = std::filesystem::path(ResourcePath);

        // Check if ResourcePath exists.
        if (!std::filesystem::exists(ResourceFolder)) // If folder doesn't exist.
            std::filesystem::create_directories(ResourceFolder); // Create a new folder.

        // Return the Resource folder location.
        return ResourcePath;
    }

    // Checks if the path exist or no.
    bool doesPathExist(std::string path) {
        // Returns a bool to check if path exists.
        return std::filesystem::exists(path);
    }

    void createPath(const std::string& path)
    {
        std::filesystem::create_directory(path);
    }

    void deletePath(const std::string& path)
    {
        std::filesystem::remove(path);
    }

    // Download a file using urlmon.
    void downloadFile(std::string url, std::string path) {
        // If the path/location already exists.
        if (doesPathExist(path)) return; // Return to stop it from downloading.

        // Downloads a file from a URL link.
        URLDownloadToFileA(nullptr, url.c_str(), path.c_str(), 0, NULL);
    }

    static void writeText(std::string filePath, std::string Message) {
        std::ifstream fileCheck(filePath.c_str());
        if (!fileCheck.good()) {
            fileCheck.close();
            std::ofstream fileCreate(filePath.c_str());
            if (fileCreate.is_open()) {
                fileCreate << Message;
                fileCreate.close();
            }
        }
    }

    void InitializeClientFolders() {
        if (!doesPathExist(getClientPath() + xorstr_("\\Authentication.txt"))) {
            writeText(getClientPath() + xorstr_("\\Authentication.txt"), xorstr_("Username: \nPassword: \nKey: "));
        }
        
        if (!doesPathExist(getClientPath() + xorstr_("\\configurations\\"))) {
            createPath(getClientPath() + xorstr_("\\configurations\\"));
        }

        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/fire.png", getResourcePath() + "\\Combat.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/run.png", getResourcePath() + "\\Movement.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/user.png", getResourcePath() + "\\Player.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/eye.png", getResourcePath() + "\\Visual.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/flask.png", getResourcePath() + "\\Misc.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Shadow-Assets/main/Title.png", getResourcePath() + "\\Title.png");
        downloadFile("https://raw.githubusercontent.com/xNotTozic/Apollo-Assets/main/TargetHead.png", getResourcePath() + "\\TargetHead.png");
    }

    void debugOutput(std::string output) {
        static bool init = false;
        static auto path = getClientPath();

        auto file = std::string(path + xorstr_("\\Output.txt"));

        if (!init) {
            auto f = std::filesystem::path(file);

            if (std::filesystem::exists(f))
                std::filesystem::remove(f);

            init = true;
        };

        CloseHandle(CreateFileA(file.c_str(), GENERIC_WRITE | GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL));

        std::ofstream fStream;
        fStream.open(file.c_str(), std::ios::app);

        if (fStream.is_open())
            fStream << output << "" << std::endl;

        return fStream.close();

    };
#pragma endregion
}