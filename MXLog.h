#pragma once

#include <string>
#include <memory>
#include <functional>

#include "P_Header.h"

namespace Adoter
{

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

	template<typename T, typename ...Args>
	std::unique_ptr<T> make_unique(Args&& ...args)
	{
			return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
	}

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

	void Load();

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

}

