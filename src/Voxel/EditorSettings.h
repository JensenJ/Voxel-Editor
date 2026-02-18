#pragma once
#include <Voxel/pch.h>
#include <Voxel/Core.h>

class EditorSettings {
  public:
    static void Initialise(const std::filesystem::path& path) {
        s_FilePath = path;
        std::unordered_map<std::string, std::unordered_map<std::string, std::string>> loadedData;
        bool loaded = Load(path, loadedData);

        if (loaded) {
            s_Data = std::move(loadedData);
        } else {
            s_Data.clear();
        }

        EnsureDefaults();
        Save();
    }

    static void Save() {
        if (!s_Dirty)
            return;

        SaveInternal();
        s_Dirty = false;
    }

    static std::string GetString(const std::string& section, const std::string& key,
                                 const std::string& defaultValue = "") {
        auto sec = s_Data.find(section);
        if (sec == s_Data.end())
            return defaultValue;

        auto it = sec->second.find(key);
        if (it == sec->second.end())
            return defaultValue;

        return it->second;
    }

    static int GetInt(const std::string& section, const std::string& key, int defaultValue = 0) {
        try {
            return std::stoi(GetString(section, key));
        } catch (...) {
            return defaultValue;
        }
    }

    static float GetFloat(const std::string& section, const std::string& key,
                          float defaultValue = 0.0f) {
        try {
            return std::stof(GetString(section, key));
        } catch (...) {
            return defaultValue;
        }
    }

    static bool GetBool(const std::string& section, const std::string& key,
                        bool defaultValue = false) {
        auto value = GetString(section, key);
        if (value.empty())
            return defaultValue;

        std::string lower = value;
        std::transform(lower.begin(), lower.end(), lower.begin(), ::tolower);

        return (lower == "true" || lower == "1");
    }

    static void SetString(const std::string& section, const std::string& key,
                          const std::string& value) {
        if (s_Data[section][key] != value) {
            s_Data[section][key] = value;
            s_Dirty = true;
        }
    }

    static void SetInt(const std::string& section, const std::string& key, int value) {
        SetString(section, key, std::to_string(value));
    }

    static void SetFloat(const std::string& section, const std::string& key, float value) {
        SetString(section, key, std::to_string(value));
    }

    static void SetBool(const std::string& section, const std::string& key, bool value) {
        SetString(section, key, value ? "true" : "false");
    }

  private:
    static inline std::unordered_map<std::string, std::unordered_map<std::string, std::string>>
        s_Data;

    static inline std::filesystem::path s_FilePath;
    static inline bool s_Dirty = false;

    static void Trim(std::string& str) {
        auto notSpace = [](int ch) { return !std::isspace(ch); };

        str.erase(str.begin(), std::find_if(str.begin(), str.end(), notSpace));

        str.erase(std::find_if(str.rbegin(), str.rend(), notSpace).base(), str.end());
    }

    static void EnsureDefaults() { SetDefault("Editor", "UIScale", "1.0"); }

    static bool Has(const std::string& section, const std::string& key) {
        auto sec = s_Data.find(section);
        if (sec == s_Data.end())
            return false;

        return sec->second.find(key) != sec->second.end();
    }

    static void SetDefault(const std::string& section, const std::string& key,
                           const std::string& value) {
        if (!Has(section, key)) {
            s_Dirty = true;
            s_Data[section][key] = value;
        }
    }

    static bool
    Load(const std::filesystem::path& path,
         std::unordered_map<std::string, std::unordered_map<std::string, std::string>>& outData) {
        s_FilePath = path;
        s_Data.clear();

        std::ifstream file(path);
        if (!file.is_open())
            return false;

        std::string line;
        std::string currentSection;

        try {
            while (std::getline(file, line)) {
                Trim(line);

                if (line.empty())
                    continue;

                if (line[0] == ';' || line[0] == '#')
                    continue;

                if (line.front() == '[' && line.back() == ']') {
                    currentSection = line.substr(1, line.size() - 2);
                    Trim(currentSection);
                    continue;
                }

                auto equalsPos = line.find('=');
                if (equalsPos == std::string::npos)
                    continue;

                std::string key = line.substr(0, equalsPos);
                std::string value = line.substr(equalsPos + 1);

                Trim(key);
                Trim(value);

                outData[currentSection][key] = value;
            }
        } catch (...) {
            LOG_ERROR("Failed to load editor settings, restoring to default.");
            return false;
        }
        return true;
    }

    static void SaveInternal() {
        std::filesystem::path tempPath = s_FilePath;
        tempPath += ".tmp";

        std::ofstream file(tempPath, std::ios::out | std::ios::trunc);
        if (!file.is_open())
            return;

        std::vector<std::string> sections;
        sections.reserve(s_Data.size());

        for (const auto& [section, _] : s_Data)
            sections.push_back(section);

        std::sort(sections.begin(), sections.end());

        for (const auto& section : sections) {
            file << "[" << section << "]\n";

            const auto& keys = s_Data[section];

            std::vector<std::string> keyNames;
            keyNames.reserve(keys.size());

            for (const auto& [key, _] : keys)
                keyNames.push_back(key);

            std::sort(keyNames.begin(), keyNames.end());

            for (const auto& key : keyNames) {
                file << key << "=" << keys.at(key) << "\n";
            }

            file << "\n";
        }

        file.close();
        std::filesystem::rename(tempPath, s_FilePath);
    }
};
