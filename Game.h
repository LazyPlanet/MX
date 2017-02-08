#pragma once

#include "P_Header.h"

#include <memory>
#include <vector>
#include <numeric>
#include <algorithm>
#include <unordered_map>

namespace Adoter
{

#define CARDS_COUNT 136

class Game : public std::enable_shared_from_this<Game>
{
private:
	std::vector<int32_t> _cards;
public:
	virtual void Init();
	virtual bool Start();
	virtual int32_t GetCard();
};

/////////////////////////////////////////////////////
//游戏通用管理类
/////////////////////////////////////////////////////
class GameManager
{
private:
	std::unordered_map<int32_t, Asset::MJCard_Card*> _cards;
public:
	static GameManager& Instance()
	{
		static GameManager _instance;
		return _instance;
	}

	bool Load();
};

#define GameInstance GameManager::Instance()

}
