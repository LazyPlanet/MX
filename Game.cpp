#include <vector>
#include <algorithm>

#include "Game.h"
#include "Asset.h"

namespace Adoter
{

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

bool Game::Start(std::unordered_map<int64_t, std::shared_ptr<Player>>& players)
{
	_players = players; //复制下玩家数据

	if (_players.size() != 4) return false; //做下检查，是否满足开局条件

	for (size_t i = 0; i < _players.size(); ++i)
	{
		int32_t card_count = 13; //正常开启，普通玩家牌数量

		if (_banker_index % 4 == i) card_count = 14; //庄家牌数量
		
		auto cards = FaPai(card_count);
		_players[i]->OnFaPai(std::move(cards));  //各个玩家发牌
	}
	return true;
}

bool Game::Over()
{
	_hupai_players.push_back(1);
	++_banker_index; //换庄家
	return true;
}

std::vector<int32_t> Game::FaPai(size_t card_count)
{
	std::vector<int32_t> cards;
	
	if (card_count > _cards.size()) return cards;

	for (size_t i = 0; i < card_count; ++i)
	{
		int32_t value = _cards.front();	

		cards.push_back(value);

		_cards.pop_front();
	}
	
	return cards;
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
		{
			int32_t cards_count = std::min(asset_card->group_count(), asset_card->cards_size());

			for (int i = 0; i < cards_count; ++i)
			{
				Asset::Pai card;
				card.set_card_type(asset_card->card_type());
				card.set_card_value(asset_card->cards(i).value());

				_cards.emplace(_cards.size() + 1, card);

			}
		}
	}

	//if (_cards.size() != CARDS_COUNT) return false;
	return true;
}

}
