#include <mutex>
#include <algorithm>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>

#include "Config.h"

namespace Adoter
{

bool ConfigManager::LoadInitial(std::string const& file)
{
    std::lock_guard<std::mutex> lock(_config_lock);

    _filename = file;

    try
    {
		boost::property_tree::ptree fulltree;
		boost::property_tree::ini_parser::read_ini(file, fulltree);

        if (fulltree.empty())
        {
            printf("%s:empty file (%s)", __func__, file.c_str());
            return false;
        }

        _config = fulltree.begin()->second;
    }
    catch (boost::property_tree::ini_parser::ini_parser_error const& e)
    {
        if (e.line() == 0)
		{
            printf("%s:message:%s filename:%s", __func__, e.message().c_str(), e.filename().c_str());
		}
        else
		{
            printf("%s:message:%s filename:%s error line:%ld", __func__, e.message().c_str(), e.filename().c_str(), e.line());

		}

        return false;
    }

    return true;
}

ConfigManager& ConfigManager::Instance()
{
    static ConfigManager _instance;
    return _instance;
}

bool ConfigManager::Reload(std::string& error)
{
    return LoadInitial(_filename);
}

template<class T>
T ConfigManager::GetValue(std::string const& name, T def) const
{
    try
    {
        return _config.get<T>(boost::property_tree::ptree::path_type(name, '/'));
    }
    catch (boost::property_tree::ptree_bad_path)
    {
		 //LOG(ERROR, "Missing Name %s in config file %s, add \"%s = %s\" to this file",
		//		 name.c_str(), _filename.c_str(), name.c_str(), std::to_string(def).c_str());
    }
    catch (boost::property_tree::ptree_bad_data)
    {
		//LOG(ERROR, "Bad value defined for name %s in config file %s, going to use %s instead",
        //    name.c_str(), _filename.c_str(), std::to_string(def).c_str());
    }

    return def;
}

template<>
std::string ConfigManager::GetValue<std::string>(std::string const& name, std::string def) const
{
    try
    {
        return _config.get<std::string>(boost::property_tree::ptree::path_type(name, '/'));
    }
    catch (boost::property_tree::ptree_bad_path)
    {
        //TC_LOG_WARN("server.loading", "Missing name %s in config file %s, add \"%s = %s\" to this file",
        //    name.c_str(), _filename.c_str(), name.c_str(), def.c_str());
    }
    catch (boost::property_tree::ptree_bad_data)
    {
        //TC_LOG_ERROR("server.loading", "Bad value defined for name %s in config file %s, going to use %s instead",
        //    name.c_str(), _filename.c_str(), def.c_str());
    }

    return def;
}

std::string ConfigManager::GetString(std::string const& name, const std::string& def) const
{
    std::string val = GetValue(name, def);
    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
    return val;
}

bool ConfigManager::GetBool(std::string const& name, bool def) const
{
    std::string val = GetValue(name, std::string(def ? "1" : "0"));
    val.erase(std::remove(val.begin(), val.end(), '"'), val.end());
    return (val == "1" || val == "true" || val == "TRUE" || val == "yes" || val == "YES");
}

int ConfigManager::GetInt(std::string const& name, int def) const
{
    return GetValue(name, def);
}

float ConfigManager::GetFloat(std::string const& name, float def) const
{
    return GetValue(name, def);
}

std::string const& ConfigManager::GetFilename()
{
    std::lock_guard<std::mutex> lock(_config_lock);
    return _filename;
}

std::list<std::string> ConfigManager::GetKeysByString(std::string const& name)
{
    std::lock_guard<std::mutex> lock(_config_lock);

    std::list<std::string> keys;

    for (const boost::property_tree::ptree::value_type& child : _config)
        if (child.first.compare(0, name.length(), name) == 0)
            keys.push_back(child.first);

    return keys;
}

}
