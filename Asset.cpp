#include "Asset.h"

#include <fstream>

namespace Adoter {

AssetManager::AssetManager() : _parse_sucess(false)
{

}

bool AssetManager::Load()
{
	this->_asset_path = "./Asset/Asset/";
	this->_proto_file_path = "P_Asset.proto";

	this->_pool = pb::DescriptorPool::generated_pool();
	if (!this->_pool) return false;

	this-> _file_descriptor = _pool->FindFileByName(_proto_file_path);
	if (!this->_file_descriptor) return false;

	const pb::EnumDescriptor* asset_type = _file_descriptor->FindEnumTypeByName("ASSET_TYPE");	
	if (!asset_type) std::cout << __func__ << ":could not found typename:ASSET_TYPE" << std::endl;

	//加载所有资源结构
	for (int i = 0; i < _file_descriptor->message_type_count(); ++i)
	{
		const pb::Descriptor* descriptor = _file_descriptor->message_type(i);
		if (!descriptor) return false;
	
		const pb::FieldDescriptor* field = descriptor->FindFieldByNumber(1);	//所有MESSAGE的第一个变量必须是类型
		if(!field || field->enum_type() != asset_type) continue;

		const pb::Message* msg = pb::MessageFactory::generated_factory()->GetPrototype(descriptor);
		pb::Message* message = msg->New();

		if (_messages.find(field->default_value_enum()->number()) == _messages.end())
		{
			_messages.emplace(field->default_value_enum()->number(), message);	//合法协议，如果已经存在则忽略，即只加载第一个协议
		}
		else
		{
		       std::cout << "Load asset error, reduplicate message name：" << msg->GetTypeName() << std::endl;
		}
	}
	//加载所有资源数据
	fs::path full_path(_asset_path);
	if (!LoadAssets(full_path)) return false;

	std::cout << __func__ << ":Load asset data success，asset total：" << _assets.size() << ", types total:" << _assets_bytypes.size() << std::endl;

	this->_parse_sucess = true;
	return true;
}

bool AssetManager::LoadAssets(fs::path& full_path)
{
    	if (fs::exists(full_path))
    	{
        	fs::directory_iterator item_begin(full_path);
        	fs::directory_iterator item_end;

        	for ( ; item_begin != item_end; item_begin++)
        	{
            	if (fs::is_directory(*item_begin))
            	{
					std::string sub_dir_str(item_begin->path().string());
					fs::path sub_dir(sub_dir_str);
					LoadAssets(sub_dir);
            	}
            	else
            	{
					const std::string& filename = item_begin->path().string();
					//////打开文件
					std::fstream file(filename.c_str(), std::ios::in | std::ios::binary);
					if (!file) return false; 	//如果一个有问题就退出

					int32_t size = 0;

					file >> size;
					if (size == 0 || size > 1024) return false;	//理论上单个文件不会超过1024字节

					char content[1024];
					file.readsome(content, size);

					std::string directory_string = item_begin->path().parent_path().string();
					if (directory_string == "") return false;

					int32_t found_pos = directory_string.find_last_of("/");
					const std::string& message_name = directory_string.substr(found_pos + 1);	//MESSAGE名称即为文件夹名称

					const pb::Descriptor* descriptor = this->_file_descriptor->FindMessageTypeByName(message_name);
					if (!descriptor) return false;

					const pb::Message* msg = pb::MessageFactory::generated_factory()->GetPrototype(descriptor);
					if (!msg) return false;

					pb::Message* message = msg->New();
					message->ParseFromArray(content, size);

					//////关闭文件
					file.close();

					////////////////////////////////////////////
					const pb::FieldDescriptor* type_field = message->GetDescriptor()->FindFieldByName("type_t");
					if (!type_field) return false; //如果一个有问题就退出

					int64_t global_id = 0;

					const pb::FieldDescriptor* prop_field = message->GetDescriptor()->FindFieldByName("common_prop");
					if (prop_field) //普通资源
					{
						const pb::Message& prop_message = message->GetReflection()->GetMessage(*message, prop_field);
						const pb::FieldDescriptor* global_id_field = prop_message.GetDescriptor()->FindFieldByName("global_id");
						if (!global_id_field) return false;
						global_id = prop_message.GetReflection()->GetInt64(prop_message, global_id_field);
					}
					else //物品资源
					{
						const pb::FieldDescriptor* item_prop_field = message->GetDescriptor()->FindFieldByName("item_common_prop");
						if (!item_prop_field) return false;

						const pb::Message& item_prop_message = message->GetReflection()->GetMessage(*message, item_prop_field);
						prop_field = item_prop_message.GetDescriptor()->FindFieldByName("common_prop");
						if (!prop_field) return false;

						const pb::Message& prop_message = item_prop_message.GetReflection()->GetMessage(item_prop_message, prop_field);
						const pb::FieldDescriptor* global_id_field = prop_message.GetDescriptor()->FindFieldByName("global_id");
						if (!global_id_field) return false;
						global_id = prop_message.GetReflection()->GetInt64(prop_message, global_id_field);
					}
					////////////////////////////////////////////加载到全局唯一表
					_assets.emplace(global_id, message);

					////////////////////////////////////////////加载到类型表
					int32_t type_t = type_field->default_value_enum()->number();
					_assets_bytypes[type_t].emplace(message);
            	}
        	}
    	}

	return true;
} 

pb::Message* AssetManager::GetMessage(int32_t message_type)
{
	auto it = _messages.find(message_type);
	if (it == _messages.end()) return nullptr;
	return it->second;
}

std::unordered_set<pb::Message*> AssetManager::GetMessagesByType(int32_t message_type)
{
	return _assets_bytypes[message_type];
}

pb::Message* AssetManager::Get(int64_t global_id)
{
	auto it = _assets.find(global_id);
	if (it == _assets.end()) return nullptr;
	return it->second;
}

}
