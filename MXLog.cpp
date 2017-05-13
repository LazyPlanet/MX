#include <algorithm>
#include <string>
#include <stdarg.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

#include "MXLog.h"
#include "Player.h"
#include "Config.h"
#include "CommonUtil.h"
#include "MessageFormat.h"

namespace Adoter
{

//extern std::shared_ptr<Player> g_player;

void vutf8printf(FILE* out, const char *str, va_list* ap)
{
	vfprintf(out, str, *ap);
}

void utf8printf(FILE* out, const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vutf8printf(out, str, &ap);
}

void DEBUG(const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vutf8printf(stdout, str, &ap);
}

void P(Asset::LOG_LEVEL level, const char *format, ...)
{
	va_list args;
	va_start (args, format);
	va_end (args);

	char string_content[1024];
	int ret = vsprintf(string_content, format, args);
	if (ret != 0) return;

	auto log = make_unique<Asset::LogMessage>(); 
	log->set_level(level); 

	log->set_content(string_content); 

	MXLog::Instance().Print(log.get());
}

void MXLog::Print(Asset::LogMessage* message)
{
	if (!message) return;

	//if (g_player) message->set_player_id(g_player->GetID());

	//时间
	boost::posix_time::ptime ptime(boost::posix_time::second_clock::local_time());
	std::string curr_time = boost::posix_time::to_iso_extended_string(ptime);

	std::replace(curr_time.begin(), curr_time.end(), 'T', ' ');

	/////日志类型
	//
	if (!message->has_type()) message->set_type(Asset::NORMAL);

	const pb::FieldDescriptor* type_field = message->GetDescriptor()->FindFieldByName("type");
	if (!type_field) return;

	const pb::EnumValueDescriptor* type_value = message->GetReflection()->GetEnum(*message, type_field);
	if (!type_value) return;

	auto type_name = type_value->name();
	std::transform(type_name.begin(), type_name.end(), type_name.begin(), ::tolower);
	
	/////日志级别
	//
	if (!message->has_level()) message->set_level(Asset::TRACE);

	const pb::FieldDescriptor* level_field = message->GetDescriptor()->FindFieldByName("level");
	if (!level_field) return;

	const pb::EnumValueDescriptor* level_value = message->GetReflection()->GetEnum(*message, level_field);
	if (!level_value) return;

	auto level_name = level_value->name();
	std::transform(level_name.begin(), level_name.end(), level_name.begin(), ::tolower);

	//auto level = message->level();

	/*
	switch (level)
	{
		case Asset::LOG_LEVEL_INFO:
		{
			
		}
		break;
		
		case Asset::LOG_LEVEL_TRACE:
		{
			
		}
		break;
		
		case Asset::LOG_LEVEL_WARNNING:
		{
			
		}
		break;
	}
	*/

	::google::protobuf::MessageFormat::Printer printer;
	printer.SetSingleLineMode(true); //整行打印
	printer.SetUseUtf8StringEscaping(true);

	std::string output;
	printer.PrintToString(*message, &output);

	std::transform(output.begin(), output.end(), output.begin(), ::tolower);
	std::cout << curr_time << "|" << output << std::endl;

	//std::string subfix = level_name.replace(level_name.begin(), level_name.begin() + 10, "");        
	//文件名称
	std::string filename = "mx." + level_name;
	std::string sub_dir = boost::gregorian::to_iso_extended_string(ptime.date());
	std::string fullname(_dir + sub_dir + "/"+ filename);

	boost::filesystem::path dir(_dir + sub_dir);

	if (!boost::filesystem::exists(dir))
	{
		if (boost::filesystem::create_directory(dir))
		{
			std::cout << "Success" << "\n";
		}
	}

	if (FILE* logfile = fopen(fullname.c_str(), "aw"))
	{
		utf8printf(logfile, "%s|%ld|%s|%s\n", curr_time.c_str(), _server_id, _server_name.c_str(), output.c_str());
		fflush(logfile); //写入文件
		//fclose(logfile);
	}
}

MXLog::MXLog()
{
	////////////////////日志格式////////////////////
	spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%f] [logname:%n] [level:%l] [thread:%t] %v");

	////////////////////日志定义////////////////////
	//
	//控制台日志
	auto console = spdlog::stdout_color_mt("console");
	console->set_level(spdlog::level::trace);
	//玩家日志
	auto players = spdlog::basic_logger_mt("players", "logs/players");
	players->flush_on(spdlog::level::trace);
	//异步日志
	spdlog::set_async_mode(4096); //队列大小必须是2的整数倍
	auto player = spdlog::daily_logger_st("player", "logs/player");
	player->flush_on(spdlog::level::trace);
}

void MXLog::ConsolePrint(Asset::LogMessage* message)
{
	if (!message) return;
	
	//if (g_player) message->set_player_id(g_player->GetID());

	/////时间
	//
	boost::posix_time::ptime ptime(boost::posix_time::second_clock::local_time());
	std::string curr_time = boost::posix_time::to_iso_extended_string(ptime);

	std::replace(curr_time.begin(), curr_time.end(), 'T', ' ');

	/////日志类型
	//
	if (!message->has_type()) message->set_type(Asset::NORMAL);

	const pb::FieldDescriptor* type_field = message->GetDescriptor()->FindFieldByName("type");
	if (!type_field) return;

	const pb::EnumValueDescriptor* type_value = message->GetReflection()->GetEnum(*message, type_field);
	if (!type_value) return;

	auto type_name = type_value->name();
	std::transform(type_name.begin(), type_name.end(), type_name.begin(), ::tolower);
	
	/////日志级别
	//
	if (!message->has_level()) message->set_level(Asset::TRACE);

	const pb::FieldDescriptor* level_field = message->GetDescriptor()->FindFieldByName("level");
	if (!level_field) return;

	const pb::EnumValueDescriptor* level_value = message->GetReflection()->GetEnum(*message, level_field);
	if (!level_value) return;

	auto level_name = level_value->name();
	std::transform(level_name.begin(), level_name.end(), level_name.begin(), ::tolower);
	
	::google::protobuf::MessageFormat::Printer printer;
	printer.SetSingleLineMode(true); //整行打印
	printer.SetUseUtf8StringEscaping(true);

	std::string output;
	printer.PrintToString(*message, &output);

	std::transform(output.begin(), output.end(), output.begin(), ::tolower);

    bool stdout_stream = message->level() != Asset::ERROR && message->level() != Asset::FATAL;

	utf8printf(stdout_stream ? stdout : stderr, "%s|%ld|%s|%s\n", curr_time.c_str(), _server_id, _server_name.c_str(), output.c_str());
}

void MXLog::Load()
{
	_dir = ConfigInstance.GetString("LogDirectory", "logs");

	if (!_dir.empty())
	{
		if ((_dir.at(_dir.length() - 1) != '/') && (_dir.at(_dir.length() - 1) != '\\'))
		{
			_dir.push_back('/');
		}
	}
}

}
