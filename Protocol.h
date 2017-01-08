#pragma once

#include <google/protobuf/message.h>
#include <google/protobuf/compiler/importer.h>
#include <google/protobuf/compiler/parser.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>
#include <google/protobuf/io/tokenizer.h>
#include <unordered_map>
#include <string>
#include <iostream>
#include <functional>
#include "P_Header.h"

namespace Adoter {

namespace pb = google::protobuf;

/*
 * 类功能：
 * 
 * 1.自动注册(.proto)文件中的所有协议;
 *
 * 2.代码风格倾向于使用C++11新特性，其他功能补充使用Boost开源库;
 *
 * 说明：此处的协议数据指谷歌ProtocolBuffer内容中的Message，用于区分消息队列中的Message，所以命名如此.
 *
 * */

class ProtocolManager : public std::enable_shared_from_this<ProtocolManager>
{
private:
	bool _parse_sucess;
	std::string _proto_file_path;
	
	std::unordered_map<int32_t, pb::Message*>  _messages;

public:
	ProtocolManager();

	static ProtocolManager& Instance()
	{
		static ProtocolManager _instance;
		return _instance;
	}
	
	pb::Message* GetMessage(int32_t message_type)
	{
		auto it = _messages.find(message_type);
		if (it == _messages.end()) return nullptr;
		return it->second;
	}
	
	//加载协议	
	bool Load();
};

#define ProtocolInstance ProtocolManager::Instance()

}
