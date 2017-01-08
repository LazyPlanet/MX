#pragma once

#include "P_Header.h"
#include "PriorityQueue.h"

namespace Adoter
{

using namespace std::chrono;

class PriorityQueueItem : public std::enable_shared_from_this<PriorityQueueItem>
{
private:
	Asset::MsgItem _item;
public:

	explicit PriorityQueueItem(Asset::MsgItem& item)
	{
		this->_item.CopyFrom(item);
	}

	bool operator < (const PriorityQueueItem& other) const
	{
		return _item.priority() > other._item.priority();	//最小值优先，优先级级别：1~10，默认为10
	}

	const Asset::MsgItem& Get() { return _item; }
	const Asset::MsgItem& Get() const { return _item; }
};

class MessageDispatcher : public std::enable_shared_from_this<MessageDispatcher>
{
private:
	std::mutex _mutex;
	std::shared_ptr<std::thread> _thread;
	PriorityQueue<PriorityQueueItem> _messages;
public:
	~MessageDispatcher() { _thread->join();	}
	MessageDispatcher() { _thread = std::make_shared<std::thread>(std::bind(&MessageDispatcher::Dispatcher, this));	}
	
	//消息队列存盘(一般游戏逻辑不需做此操作)
	void Save() 
	{
		//Asset::MsgItems items;	//用于存盘
	}	

	//消息队列加载(一般游戏逻辑不需做此操作)
	void Load() 
	{
		//Asset::MsgItems items;	//用于加载
	}		


	static MessageDispatcher& Instance()
	{
		static MessageDispatcher _instance;
		return _instance;
	}
public:
	bool Empty(); //队列是否为空
	void Dispatcher(); //分发消息
	void SendMessage(Asset::MsgItem msg/*消息COPY*/); //发消息
	bool Discharge(int64_t receiver, const Asset::MsgItem& message); //发送给接收者
};

#define DispatcherInstance MessageDispatcher::Instance()

}
