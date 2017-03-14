#pragma once

#include <string>
#include <memory>
#include <functional>

#include "P_Header.h"

namespace Adoter
{
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

public:

	static MXLog& Instance()
	{
		static MXLog _instance;
		return _instance;
	}

	void Print(Asset::LogMessage* message);
};

#define MXLogInstance MXLog::Instance()

}

