#pragma once

#include <string>
#include <list>
#include <vector>
#include <mutex>

#include <boost/property_tree/ptree.hpp>

namespace Adoter
{

class ConfigManager
{
    ConfigManager() = default;
    ConfigManager(ConfigManager const&) = delete;
    ConfigManager& operator=(ConfigManager const&) = delete;
    ~ConfigManager() = default;

public:
    bool LoadInitial(std::string const& file);

    static ConfigManager& Instance();

    bool Reload(std::string& error);

    std::string GetString(std::string const& name, const std::string& def) const;

    bool GetBool(std::string const& name, bool def) const;

    int GetInt(std::string const& name, int def) const;

    float GetFloat(std::string const& name, float def) const;

    std::string const& GetFilename();

    std::list<std::string> GetKeysByString(std::string const& name);

private:
    std::string _filename;
    boost::property_tree::ptree _config;
    std::mutex _config_lock;

    template<class T>
    T GetValue(std::string const& name, T def) const;
};

#define ConfigInstance ConfigManager::Instance()

}
