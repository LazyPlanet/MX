#include "Protocol.h"

namespace Adoter {

ProtocolManager::ProtocolManager() : _parse_sucess(false)
{
}

bool ProtocolManager::Load()
{
	_proto_file_path = "P_Protocol.proto";
	
	const pb::DescriptorPool* _pool = pb::DescriptorPool::generated_pool();
	if (!_pool) return false;

	const pb::FileDescriptor* _file_descriptor = _pool->FindFileByName(_proto_file_path);
	if (!_file_descriptor) return false;

	const pb::EnumDescriptor* meta_type = _file_descriptor->FindEnumTypeByName("META_TYPE");	
	if (!meta_type) std::cout << __func__ << ":could not found typename:META_TYPE" << std::endl;

	for (int i = 0; i < _file_descriptor->message_type_count(); ++i)
	{
		const pb::Descriptor* descriptor = _file_descriptor->message_type(i);
		if (!descriptor) return false;
	
		if (descriptor->name() == "Meta") continue;		//Meta不作为协议处理流程一员
			
		const pb::FieldDescriptor* field = descriptor->FindFieldByNumber(1);	//所有MESSAGE的第一个变量必须是类型
		if(!field || field->enum_type() != meta_type) continue;

		int type_t = field->default_value_enum()->number();
		if (type_t > Asset::META_TYPE_C2S_COUNT) continue; 	//只加载C2S的协议处理
		
		const pb::Message* msg = pb::MessageFactory::generated_factory()->GetPrototype(descriptor);
		pb::Message* message = msg->New();
		_messages.emplace(field->default_value_enum()->number(), message);	//合法协议，如果已经存在则忽略，即只加载第一个协议
	}

	std::cout << __func__ << ":Load protocol success，total：" << _messages.size() << std::endl;
	_parse_sucess = true;
	return true;
}

}
