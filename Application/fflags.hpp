/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * ░▒█▀▀▀░▒█▀▀▀░█░░█▀▀▄░█▀▀▀░░░▒█░░▒█░█▀▀▄░█▀▀▄░▄▀▀▄░▄▀▀▄░█▀▀░█▀▀▄
 * ░▒█▀▀░░▒█▀▀░░█░░█▄▄█░█░▀▄░░░▒█▒█▒█░█▄▄▀░█▄▄█░█▄▄█░█▄▄█░█▀▀░█▄▄▀
 * ░▒█░░░░▒█░░░░▀▀░▀░░▀░▀▀▀▀░░░▒▀▄▀▄▀░▀░▀▀░▀░░▀░█░░░░█░░░░▀▀▀░▀░▀▀
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Written by Excel. FFlag-Wrapper under the MIT License.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * https://github.com/ItzzExcel/FFlag-Wrapper/
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * Yes, it's that simple.
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING

#define AUTOCREATE_JSON_FILE

#include <Windows.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <atlstr.h>
#include <shlobj.h>
#include <experimental/filesystem>
#include <nlohmann/json.hpp>

namespace fs = std::experimental::filesystem;
using json = nlohmann::json;

namespace FFlags {

    bool ALLOW_BLOXSTRAP = false;

    bool __DirectoryExists(const std::string folderPath) {
        DWORD attributes = GetFileAttributesA(folderPath.c_str());

        if (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY)) {
            return true;
        }
        else {
            return false;
        }
    }

    std::string __GetUserFolderPath() {
        wchar_t path[MAX_PATH];
        if (SHGetFolderPathW(NULL, CSIDL_PROFILE, NULL, 0, path) == S_OK) {
            size_t convertedChars = 0;
            char narrowPath[MAX_PATH] = { NULL };

            if (wcstombs_s(&convertedChars, narrowPath, path, MAX_PATH) == 0) {
                return std::string(narrowPath);
            }

            else {
                return "";
            }
        }
        else {
            return "";
        }
    }

    bool __IsFile(const std::string filePath) {
        DWORD attributes = GetFileAttributesA(filePath.c_str());

        if (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY))
            return true;
        else
            return false;
    }

    bool __DoFile(const std::string filePath, const std::string content) {
        try {
            std::ofstream outFile(filePath);

            if (outFile.is_open()) {
                outFile << content;
                outFile.close();
                return true;
            }
            else {
                return false;
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error: " << ex.what() << std::endl;
            return false;
        }
    }

    bool __DoDirectory(const std::string& path) {
        if (CreateDirectoryA(path.c_str(), NULL) == 0) {
            DWORD error = GetLastError();
            if (error == ERROR_ALREADY_EXISTS) {
                return true;
            }
            else {
                std::cerr << "Error: Failed to create directory. Error code: " << error << std::endl;
                return false;
            }
        }
        else {
            return true;
        }
    }

    std::string __GetRobloxFolder() {

        if (ALLOW_BLOXSTRAP == true) {
            std::string bloxstrapPath = __GetUserFolderPath() + "\\AppData\\Local\\Bloxstrap\\Modifications\\ClientSettings";
            if (__DirectoryExists(bloxstrapPath))
                return bloxstrapPath;
        }

        std::string robloxPath = __GetUserFolderPath() + "\\AppData\\Local\\Roblox\\Versions";

        if (fs::exists(robloxPath) && fs::is_directory(robloxPath)) {
            for (const auto& entry : fs::directory_iterator(robloxPath)) {
                if (fs::is_directory(entry.path()) && fs::exists(entry.path() / "RobloxPlayerBeta.exe")) {
                    return entry.path().string() + "\\ClientSettings";
                }
            }
        }
        return "\0";
    }

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - */

    bool Write(std::string FFlag, std::string Value) {
        std::string filePath = __GetRobloxFolder() + "\\ClientAppSettings.json";

        try {
            json jsonData;

#ifdef AUTOCREATE_JSON_FILE
            if (!__DirectoryExists(__GetRobloxFolder()))
                __DoDirectory(__GetRobloxFolder());

            if (!__IsFile(filePath))
                __DoFile(filePath, "{}");
#endif

            std::ifstream inputFile(filePath);
            inputFile >> jsonData;
            inputFile.close();

            jsonData[FFlag] = Value;

            std::ofstream outputFile(filePath);
            outputFile << std::setw(2) << jsonData << std::endl;
            outputFile.close();

            return true;
        }
        catch (const std::exception& ex) {
            throw std::runtime_error("Error updating file: " + std::string(ex.what()));
        }
    }

    std::string Read(std::string FFlag) {
        std::string filePath = __GetRobloxFolder() + "\\ClientAppSettings.json";

#ifdef AUTOCREATE_JSON_FILE
        if (!__DirectoryExists(__GetRobloxFolder()))
            __DoDirectory(__GetRobloxFolder());

        if (!__IsFile(filePath))
            __DoFile(filePath, "{}");
#endif

        try {
            std::ifstream inputFile(filePath);
            json jsonData;
            inputFile >> jsonData;
            inputFile.close();

            if (jsonData.contains(FFlag)) {
                return jsonData[FFlag].get<std::string>();
            }
            else {
                return "null";
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error:\t" << ex.what() << std::endl;
            return "null";
        }
    }

    bool Delete(std::string FFlag) {
        std::string filePath = __GetRobloxFolder() + "\\ClientAppSettings.json";

#ifdef AUTOCREATE_JSON_FILE
        if (!__DirectoryExists(__GetRobloxFolder()))
            __DoDirectory(__GetRobloxFolder());

        if (!__IsFile(filePath))
            __DoFile(filePath, "{}");
#endif

        try {
            std::ifstream inputFile(filePath);
            json jsonData;
            inputFile >> jsonData;
            inputFile.close();

            if (jsonData.contains(FFlag)) {
                jsonData.erase(FFlag);

                std::ofstream outputFile(filePath);
                outputFile << std::setw(2) << jsonData << std::endl;
                outputFile.close();

                return true;
            }
            else {
                std::cerr << "Error: The value does not exist." << std::endl;
                return false;
            }
        }
        catch (const std::exception& ex) {
            std::cerr << "Error:\t" << ex.what() << std::endl;
            return false;
        }
    }
}