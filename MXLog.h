#pragma once

#include <string>
#include <memory>
#include <functional>
#include <stdarg.h>

#include "spdlog/spdlog.h"

#include "P_Header.h"

namespace Adoter
{

void DEBUG(const char *str, ...);
void P(Asset::LOG_LEVEL level, const char *format, ...);

namespace pb = google::protobuf;

////////////////////////////////////////////////////////////////////////////////
// 日志管理系统
////////////////////////////////////////////////////////////////////////////////
class MXLog : public std::enable_shared_from_this<MXLog> 
{
	std::string _dir; //存储路径
	int64_t _server_id; //服务器ID
	std::string _server_name; //服务器名称

public:

	MXLog();
	~MXLog() { 
		DEBUG("%s:line:%d Delete all logs.\n", __func__, __LINE__);
		spdlog::drop_all(); 
	}

	static MXLog& Instance()
	{
		static MXLog _instance;
		return _instance;
	}

	void Load(); //加载日志配置

    void InitColors(const std::string& init_str);

	void Print(Asset::LogMessage* message); //日志输出
	void ConsolePrint(Asset::LogMessage* message); //控制台输出
};

#define MXLogInstance MXLog::Instance()

//按级别日志
#define LOG(level, message) \
	message->set_level(Asset::level); \
	MXLog::Instance().Print(message); \

}
