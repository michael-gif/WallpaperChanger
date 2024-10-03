#include <iostream>
#include <windows.h>
#include <filesystem>
#include <vector>

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

void setWallpaper(std::string& wallpaperPath) {
    bool result = SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)wallpaperPath.c_str(), SPIF_UPDATEINIFILE | SPIF_SENDWININICHANGE);
    if (result) {
        std::cout << "Wallpaper changed successfully!" << std::endl;
    }
    else {
        std::cout << "Failed to change wallpaper." << std::endl;
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
    setWallpaper(newWallpaper);
    return 0;
}