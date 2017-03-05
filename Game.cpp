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
	_cards.resize(136);

	std::iota(_cards.begin(), _cards.end(), 1);

	std::vector<int32_t> cards(_cards.begin(), _cards.end());

	std::random_shuffle(cards.begin(), cards.end()); //洗牌

	_cards = std::list<int32_t>(cards.begin(), cards.end());

	std::cout << __func__ << ": size: " << _cards.size() << std::endl;
}

bool Game::Start(std::unordered_map<int64_t, std::shared_ptr<Player>> players)
{
	_players = players; //复制下玩家数据

	//if (_players.size() != 4) return false; //做下检查，是否满足开局条件

	//for (size_t i = 0; i < _players.size(); ++i)
	for (auto player : _players)
	{
		int32_t card_count = 14; //正常开启，普通玩家牌数量

		//if (_banker_index % 4 == i) card_count = 14; //庄家牌数量
		
		auto cards = FaPai(card_count);

		for (auto card : cards)
			std::cout << card << " ";
		std::cout << std::endl;

		player.second->OnFaPai(cards);  //各个玩家发牌
		player.second->SetGame(shared_from_this());
	}

	OnStart();

	return true;
}
	
void Game::OnStart()
{

}

bool Game::Over()
{
	_hupai_players.push_back(1);
	++_banker_index; //换庄家
	for (auto player : _players) player.second->SetGame(nullptr);
	return true;
}

bool Game::CanPaiOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player || !_room) return false;

	if (_player != nullptr && _player != player) return false;

	return true;
}

void Game::OnPaiOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!CanPaiOperate(player, message)) return; //没到玩家操作

	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return; 

	_room->BroadCast(message); //广播玩家操作

	switch (pai_operate->oper_type())
	{
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_DAPAI: //打牌
		{
			const auto& pai = pai_operate->pais(0);

			CheckPai(pai, player->GetID()); //检查各个玩家手里的牌是否满足胡、杠、碰、吃
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GANGPAI: //杠牌
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_PENGPAI: //碰牌
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_CHIPAI: //吃牌
		{
			if (_player->GetID() != player->GetID()) return; //不是该玩家的操作
			
			ClearOperation(); //清理玩家操作状态
		}
		break;

		default:
		{

		}
		break;
	}

	_operation_index = _operation_index % 4;
}

void Game::ClearOperation()
{
	_timer.cancel();

	_player = nullptr;
}
	
int32_t Game::CheckPai(const Asset::Pai& pai, int64_t from_player_id)
{
	for (auto player : _players)
	{
		if (from_player_id == player.second->GetID()) continue;

		auto result = player.second->CheckPai(pai);
		if (result == Asset::PAI_CHECK_RETURN_NULL) continue;

		Asset::PaiOperationAlter alter;
		alter.mutable_pai()->CopyFrom(pai);

		_player = player.second; //当前可以进行操作的玩家
		_player->SendProtocol(alter); //提示给Client

		//定时器，玩家是否操作超时
		boost::asio::io_service io_service;
		_timer.expires_from_now(boost::posix_time::seconds(8)); //8秒的操作倒计时
		_timer.async_wait(std::bind(&Game::OnOperateTimeOut, shared_from_this()));
		io_service.run();
	}
	return 0;
}

void Game::OnOperateTimeOut()
{
	_player = nullptr;
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

	std::cout << "Load MJ Size:" << messages.size() << std::endl; 

	for (auto message : messages)
	{
		Asset::MJCard* asset_card = dynamic_cast<Asset::MJCard*>(message); 
		if (!asset_card) return false;
		
		for (int k = 0; k < asset_card->group_count(); ++k)
		{
			//std::cout << "group_count:" << asset_card->group_count() << "cards_size" << asset_card->cards_size() << std::endl;

			int32_t cards_count = std::min(asset_card->cards_count(), asset_card->cards_size());

			for (int i = 0; i < cards_count; ++i)
			{
				Asset::Pai card;
				card.set_card_type(asset_card->card_type());
				card.set_card_value(asset_card->cards(i).value());

				_cards.emplace(_cards.size() + 1, card); //从1开始的索引

			}
		}
	}

	//if (_cards.size() != CARDS_COUNT) return false;
	return true;
}

void GameManager::OnCreateGame(std::shared_ptr<Game> game)
{
	_games.push_back(game);
}

}
