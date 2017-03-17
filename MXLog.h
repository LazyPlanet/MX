#pragma once

#include <string>
#include <memory>
#include <functional>
#include <stdarg.h>

#include "P_Header.h"

namespace Adoter
{

//void vutf8printf(FILE* out, const char *str, va_list* ap);
//void utf8printf(FILE* out, const char *str, ...);
void CP(const char *str, ...); //控制台日志

enum ColorTypes
{
    BLACK,
    RED,
    GREEN,
    BROWN,
    BLUE,
    MAGENTA,
    CYAN,
    GREY,
    YELLOW,
    LRED,
    LGREEN,
    LBLUE,
    LMAGENTA,
    LCYAN,
    WHITE,
	MAX_COLORS
};

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

	static MXLog& Instance()
	{
		static MXLog _instance;
		return _instance;
	}

	void Load(); //加载日志配置

    void InitColors(const std::string& init_str);

	void Print(Asset::LogMessage* message); //日志输出
	void ConsolePrint(Asset::LogMessage* message); //控制台输出
    
private:
	void SetColor(bool stdout_stream, ColorTypes color);
	void ResetColor(bool stdout_stream);

	bool _colored; //控制台日志是否具有颜色
	ColorTypes _colors[Asset::MAX_LOG_LEVEL];
};

#define MXLogInstance MXLog::Instance()

//存储日志
#define P(message) MXLog::Instance().Print(message)

//按级别日志
#define LOG(level, message) if (!message) return; \
message->set_level(Asset::level); \
MXLog::Instance().Print(message); \

}
