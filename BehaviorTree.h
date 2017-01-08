#pragma once

#include <behaviac.h>
#include "Entity.h"

namespace Adoter
{

/*
 * 行为树，使用腾讯开源项目 behaviac 进行的扩展，自动绑定ASSET中的HOSTER...
 *
 * 从而实现强大的AI系统.
 *
 * */
class BehaviorTree : public behaviac::Agent
{
public:
	DECLARE_BEHAVIAC_AGENT(BehaviorTree,Agent);
public:
};


class BehaviorTreeManager
{
public:
	BehaviorTreeManager();

	static BehaviorTreeManager& Instance()
	{
		static BehaviorTreeManager _instance;
		return _instance;
	}
public:
	bool InitBehaviac()
	{
		behaviac::Config::SetSocketBlocking(false);
		behaviac::Config::SetSocketPort(40002);
		return true;
	}

};

#define BehaviorTreeInstance BehaviorTreeManager::Instance()

}
