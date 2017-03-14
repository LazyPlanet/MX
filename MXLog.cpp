
#include "boost/date_time/posix_time/posix_time.hpp"
#include "boost/date_time/posix_time/conversion.hpp"

#include <google/protobuf/stubs/common.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>

#include "MXLog.h"
#include "MessageFormat.h"

namespace Adoter
{
	
void MXLog::Print(Asset::LogMessage* message)
{
	if (!message) return;

	boost::posix_time::ptime ptime(boost::posix_time::second_clock::local_time());
	std::string curr_time = boost::posix_time::to_iso_extended_string(ptime);

	message->set_time(curr_time);

	const pb::FieldDescriptor* field = message->GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;

	const pb::EnumValueDescriptor* enum_value = message->GetReflection()->GetEnum(*message, field);
	if (!enum_value) return;

	const std::string& enum_name = enum_value->name();

	std::cout << __func__ << ":Message type is: " << enum_name.c_str() << std::endl;

	auto type = message->type();
	std::cout << std::to_string(type) << std::endl;
	
	auto level = message->level();

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

	::google::protobuf::MessageFormat::Printer printer;
	printer.SetSingleLineMode(true); //整行打印
	printer.SetUseUtf8StringEscaping(true);

	std::string output;
	printer.PrintToString(*message, &output);

	std::cout << "output:" << output << std::endl;

}

}
