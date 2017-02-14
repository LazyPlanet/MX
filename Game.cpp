#include <vector>
#include <algorithm>

#include "Game.h"
#include "Asset.h"

namespace Adoter
{

int32_t Game::_banker_index = 0; //庄家索引
/////////////////////////////////////////////////////
//一场游戏
/////////////////////////////////////////////////////
void Game::Init()
{
	std::iota(_cards.begin(), _cards.end(), 1);

	std::vector<int32_t> cards(_cards.begin(), _cards.end());
	std::random_shuffle(cards.begin(), cards.end()); //洗牌
	_cards = std::list<int32_t>(cards.begin(), cards.end());

}

bool Game::Start()
{
	if (_players.size() != 4) return false;

	for (size_t i = 0; i < _players.size(); ++i)
	{
		auto player = _players[i];
		
		if ((size_t)_banker_index % 4 == i)
		{
			player->OnFaPai(14); //庄家发牌
		}
		else
		{
			player->OnFaPai(13);
		}
	}
	return true;
}

bool Game::Over()
{
	_hupai_players.push_back(1);
	++_banker_index; //换庄家
	return true;
}

void Game::FaPai(uint32_t card_count, std::vector<int32_t>& cards)
{
	if (card_count > _cards.size()) return;

	for (size_t i = 0; i < card_count; ++i)
	{
		int32_t value = _cards.front();	
		cards.push_back(value);
		_cards.pop_front();
	}
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
