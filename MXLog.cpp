#include <algorithm>
#include <string>

#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

//#include <google/protobuf/stubs/common.h>
//#include <google/protobuf/descriptor.h>
//#include <google/protobuf/message.h>

#include "MXLog.h"
#include "MessageFormat.h"

namespace Adoter
{
	
void MXLog::Print(Asset::LogMessage* message)
{
	if (!message) return;

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

	auto level = message->level();

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

}
