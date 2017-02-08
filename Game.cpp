#include "Game.h"
#include "Asset.h"

#include <vector>
#include <algorithm>

namespace Adoter
{

void Game::Init()
{
	_cards.reserve(CARDS_COUNT);

	std::iota(_cards.begin(), _cards.end(), 1);

	std::random_shuffle(_cards.begin(), _cards.end()); //洗牌
}

bool Game::Start()
{
	return true;
}

int32_t Game::GetCard()
{
	return 0;
}

/////////////////////////////////////////////////////
//游戏通用管理类
/////////////////////////////////////////////////////
bool GameManager::Load()
{
	std::unordered_set<pb::Message*> messages = AssetInstance.GetMessagesByType(Asset::ASSET_TYPE_MJ_CARD);
	for (auto message : messages)
	{
		Asset::MJCard* asset_card = dynamic_cast<Asset::MJCard*>(message); 
		if (!asset_card) return false;
		
		for (int k = 0; k < asset_card->group_count(); ++k)
			for (int i = 0; i < asset_card->cards_count(); ++i)
				_cards.emplace(_cards.size() + 1, asset_card->mutable_cards(i));
	}

	//if (_cards.size() != CARDS_COUNT) return false;
	return true;
}

}
