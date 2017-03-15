#include <algorithm>
#include <string>
#include <stdarg.h>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

//#include <google/protobuf/stubs/common.h>
//#include <google/protobuf/descriptor.h>
//#include <google/protobuf/message.h>

#include "MXLog.h"
#include "Player.h"
#include "Config.h"
#include "MessageFormat.h"

namespace Adoter
{

extern std::shared_ptr<Player> g_player;

void vutf8printf(FILE* out, const char *str, va_list* ap)
{
	vfprintf(out, str, *ap);
}

void utf8printf(FILE* out, const char *str, ...)
{
	va_list ap;
	va_start(ap, str);
	vutf8printf(out, str, &ap);
	va_end(ap);
}

void MXLog::Print(Asset::LogMessage* message)
{
	if (!message) return;

	if (g_player) message->set_player_id(g_player->GetID());

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
	std::string filename = "mx." + level_name;
	std::string fullname(_dir + filename);

	if (FILE* logfile = fopen(fullname.c_str(), "aw"))
	{
		fprintf(logfile, "%s|%ld|%s|%s\n", curr_time.c_str(), _server_id, _server_name.c_str(), output.c_str());
		fflush(logfile); //写入文件
	}
}

MXLog::MXLog() : _colored(false)
{
    for (int32_t i = 0; i < Asset::MAX_LOG_LEVEL; ++i) _colors[i] = ColorTypes(MAX_COLORS);

    InitColors("1 2");
}

void MXLog::InitColors(std::string const& str)
{
    if (str.empty())
    {
        _colored = false;
        return;
    }

    int color[Asset::MAX_LOG_LEVEL];

    std::istringstream ss(str);

    for (int32_t i = 0; i < Asset::MAX_LOG_LEVEL; ++i)
    {
        ss >> color[i];

        if (!ss) return;

        if (color[i] < 0 || color[i] >= MAX_COLORS) return;
    }

    for (int32_t i = 0; i < Asset::MAX_LOG_LEVEL; ++i) _colors[i] = ColorTypes(color[i]);

    _colored = true;
}

void MXLog::SetColor(bool stdout_stream, ColorTypes color)
{
    enum ANSITextAttr
    {
        TA_NORMAL                                = 0,
        TA_BOLD                                  = 1,
        TA_BLINK                                 = 5,
        TA_REVERSE                               = 7
    };

    enum ANSIFgTextAttr
    {
        FG_BLACK                                 = 30,
        FG_RED,
        FG_GREEN,
        FG_BROWN,
        FG_BLUE,
        FG_MAGENTA,
        FG_CYAN,
        FG_WHITE,
        FG_YELLOW
    };

    enum ANSIBgTextAttr
    {
        BG_BLACK                                 = 40,
        BG_RED,
        BG_GREEN,
        BG_BROWN,
        BG_BLUE,
        BG_MAGENTA,
        BG_CYAN,
        BG_WHITE
    };

    static int32_t UnixColorFG[MAX_COLORS] =
    {
        FG_BLACK,                                          // BLACK
        FG_RED,                                            // RED
        FG_GREEN,                                          // GREEN
        FG_BROWN,                                          // BROWN
        FG_BLUE,                                           // BLUE
        FG_MAGENTA,                                        // MAGENTA
        FG_CYAN,                                           // CYAN
        FG_WHITE,                                          // WHITE
        FG_YELLOW,                                         // YELLOW
        FG_RED,                                            // LRED
        FG_GREEN,                                          // LGREEN
        FG_BLUE,                                           // LBLUE
        FG_MAGENTA,                                        // LMAGENTA
        FG_CYAN,                                           // LCYAN
        FG_WHITE                                           // LWHITE
    };

    fprintf((stdout_stream? stdout : stderr), "\x1b[%d%sm", UnixColorFG[color], (color >= YELLOW && color < MAX_COLORS ? ";1" : ""));
}

void MXLog::ResetColor(bool stdout_stream)
{
    fprintf((stdout_stream ? stdout : stderr), "\x1b[0m");
}
	
void MXLog::ConsolePrint(Asset::LogMessage* message)
{
	if (!message) return;
	
	if (g_player) message->set_player_id(g_player->GetID());

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

    if (_colored)
    {
        SetColor(stdout_stream, _colors[message->level()]);
		utf8printf(stdout_stream ? stdout : stderr, "%s|%ld|%s|%s\n", curr_time.c_str(), _server_id, _server_name.c_str(), output.c_str());
        ResetColor(stdout_stream);
    }
    else
	{
		utf8printf(stdout_stream ? stdout : stderr, "%s|%ld|%s|%s\n", curr_time.c_str(), _server_id, _server_name.c_str(), output.c_str());
	}
}

void MXLog::Load()
{
	_dir = ConfigInstance.GetString("LogDirectory", "");

	if (!_dir.empty())
	{
		if ((_dir.at(_dir.length() - 1) != '/') && (_dir.at(_dir.length() - 1) != '\\'))
		{
			_dir.push_back('/');
		}
	}
}

}
