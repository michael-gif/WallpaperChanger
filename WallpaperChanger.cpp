#include <iostream>
#include <windows.h>
#include <filesystem>
#include <vector>
#include <sstream>
#include <fstream>
#include <time.h>


std::string getCurrentWallpaperPath() {
    char currentWallpaperPath[MAX_PATH];
    bool result = SystemParametersInfoA(SPI_GETDESKWALLPAPER, MAX_PATH, currentWallpaperPath, 0);
    if (result) {
        return std::string(currentWallpaperPath);
    }
    else {
        std::cout << "Failed to get wallpaper." << std::endl;
    }
}

void getWallpapersFromFolder(std::vector<std::string>& out, std::string& currentWallpaperPath, std::string& folderPath) {
    for (const auto& entry : std::filesystem::directory_iterator(folderPath)) {
        std::string path = entry.path().string();
        std::string extension = entry.path().extension().string();
        if ((extension == ".jpg" || extension == ".png") && path != currentWallpaperPath)
            out.emplace_back(path);
    }
}

std::string getRandomWallpaper(std::vector<std::string>& wallpapers) {
    srand(time(0));
    int randomIndex = rand() % wallpapers.size();
    return wallpapers[randomIndex];
}

bool setWallpaper(std::string& wallpaperPath) {
    bool result = SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    if (result) {
        std::cout << "Wallpaper changed successfully!" << std::endl;
        return true;
    }
    else {
        std::cout << "Failed to change wallpaper." << std::endl;
        return false;
    }
}

void createLogFile(bool success, std::string& retrievedWallpaperPath, std::vector<std::string>& wallpapers, std::string& newWallpaper) {
    auto now = std::chrono::system_clock::now();
    std::time_t currentTime = std::chrono::system_clock::to_time_t(now);

    // Format the date and time to create a file name
    struct tm timeInfo;
    localtime_s(&timeInfo, &currentTime);
    std::stringstream ss;
    ss << std::put_time(&timeInfo, "%Y-%m-%d_%H-%M-%S"); // e.g., 2024-10-09_14-30-00
    std::string timestamp = ss.str();

    std::string folderPath;
    char* userPath = nullptr;
    size_t sz = 0;
    if (_dupenv_s(&userPath, &sz, "USERPROFILE") == 0 && userPath != nullptr)
    {
        folderPath = std::string(userPath) + "\\Downloads\\WallpaperChangerLogs";
        free(userPath);
    }
    std::filesystem::create_directories(folderPath);
    std::string logFileName = folderPath + "\\WallpaperLog_" + timestamp + ".txt";
    std::ofstream logFile(logFileName);
    if (logFile.is_open()) {
        logFile << "Wallpaper change: " << (success ? "Success!" : "Failed") << "\n";
        logFile << "Current wallpaper: " << retrievedWallpaperPath << "\n";
        logFile << "Potential wallpapers:\n";
        for (size_t i = 0; i < wallpapers.size(); ++i) 
            logFile << "- " << wallpapers[i] << "\n";
        logFile << "New wallpaper: " << newWallpaper << "\n";
        logFile.close();
        std::cout << "Log file created at: " << logFileName << std::endl;
    }
    else {
        std::cerr << "Error opening file: " << logFileName << std::endl;
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    int argc = __argc;
    char** argv = __argv;

    if (argc == 1) {
        MessageBoxA(NULL, "Error: Please provide wallpaper folder path as an argument", "Error", MB_OK | MB_ICONERROR);
        return 1;
    }
    std::string currentWallpaperPath = getCurrentWallpaperPath();
    std::vector<std::string> wallpapers;
    std::string wallpaperFolderPath = argv[1];
    getWallpapersFromFolder(wallpapers, currentWallpaperPath, wallpaperFolderPath);
    std::string newWallpaper = getRandomWallpaper(wallpapers);
    bool success = setWallpaper(newWallpaper);
    createLogFile(success, currentWallpaperPath, wallpapers, newWallpaper);
    return 0;
}