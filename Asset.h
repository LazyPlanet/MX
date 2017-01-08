#pragma once

#include <memory>
#include <string>
#include <iostream>
#include <functional>
#include <unordered_map>
#include <unordered_set>

#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <boost/filesystem.hpp>

#include "P_Header.h"

namespace Adoter {

namespace pb = google::protobuf;
namespace fs = boost::filesystem;

/*
 * 类功能：
 * 
 * 自动注册(.proto)文件中的所有合法配置;
 *
 * */

class AssetManager : public std::enable_shared_from_this<AssetManager>
{
private:
	bool _parse_sucess;
	std::string _proto_file_path;
	std::string _asset_path;
	
	//各个ASSET_TYPE对应的MESSAGE结构
	std::unordered_map<int32_t /*type_t*/, pb::Message*>  _messages;
	//各个ASSET_TYPE对应的所有数据
	std::unordered_map<int32_t /*type_t*/, std::unordered_set<pb::Message*>>  _assets_bytypes;
	//各个全局ID对应的数据
	std::unordered_map<int64_t /*global_id*/, pb::Message*>  _assets;
	
	const pb::DescriptorPool* _pool = nullptr; 
	const pb::FileDescriptor* _file_descriptor = nullptr;
private:
	bool LoadAssets(fs::path& full_path);
public:
	AssetManager();

	static AssetManager& Instance()
	{
		static AssetManager _instance;
		return _instance;
	}
	//获取MESSAGE
	pb::Message* Get(int64_t global_id); //根据ID获取数据：常用
	pb::Message* GetMessage(int32_t message_type); //获取MESSAGE对象实体
	std::unordered_set<pb::Message*> GetMessagesByType(int32_t message_type); //所有类型的资源数据
	//通过全局ID获取类型ID
	int32_t GetMessageTypeFrom(int64_t global_id)
	{
		int32_t message_type = global_id >> 16;
		return message_type;
	}
	//加载数据	
	bool Load();
};

#define AssetInstance AssetManager::Instance()

}
