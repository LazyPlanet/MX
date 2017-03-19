#include "MessageDispatcher.h"
#include "Player.h"

namespace Adoter
{

void MessageDispatcher::SendMessage(Asset::MsgItem msg/*消息COPY*/)
{
	//std::lock_guard<std::mutex> lock(_mutex);
	//auto item = std::make_shared<PriorityQueueItem>(msg);
	PriorityQueueItem item(msg);
	system_clock::time_point curr_time = system_clock::now();
	msg.set_time(system_clock::to_time_t(curr_time));	//发送时间
	_messages.Emplace(item);
}

bool MessageDispatcher::Empty()
{
	//std::lock_guard<std::mutex> lock(_mutex);
	return _messages.Empty();
}

void MessageDispatcher::Dispatcher()
{
	while(true)
	{
		if (Empty()) 
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(20));
			continue;
		}
		//std::lock_guard<std::mutex> lock(_mutex);
		const Asset::MsgItem& message = _messages.GetNext().Get();
		if (message.delay()) 
		{
			//延时发送
			int32_t delay = message.delay();
			system_clock::time_point curr_time = system_clock::now();
			if (delay > 0 && system_clock::to_time_t(curr_time) < message.time() + delay) continue;	//还没到发送时间
		}
		//发给接收者
		int64_t receiver = message.receiver();
		Discharge(receiver, message);
		std::this_thread::sleep_for(std::chrono::milliseconds(20)); //有消息就尽快发送，不要延时
	}
}

bool MessageDispatcher::Discharge(int64_t receiver, const Asset::MsgItem& message)
{
	auto player = PlayerInstance.GetPlayer(receiver); //接收者可能不在线
	if (!player) return false;
	player->HandleMessage(message);		//交给各个接受者处理
	return true;
}

}
