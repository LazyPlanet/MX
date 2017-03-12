#include <vector>
#include <algorithm>

#include "Game.h"
#include "Timer.h"
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
		player.second->ClearCards(); 

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
	if (_operation_limit.time_out() > CommonTimerInstance.GetTime() && _operation_limit.player_id() != player->GetID()) return false; //没到该玩家的操作

	return true;
}

void Game::OnPaiOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player || !message) return;

	if (!CanPaiOperate(player, message)) 
	{
		player->AlertMessage(Asset::ERROR_GAME_NO_PERMISSION); //没有权限，没到玩家操作
		return; 
	}

	if (CommonTimerInstance.GetTime() < _operation_limit.time_out()) ClearOperation(); //已经超时，清理缓存以及等待玩家操作的状态
			
	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return; 

	_room->BroadCast(message); //广播玩家操作
	
	const auto& pai = _operation_limit.pai(); //缓存的牌

	//一个人打牌之后，要检查其余每个玩家手中的牌，且等待他们的操作，直到超时
	switch (pai_operate->oper_type())
	{
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_DAPAI: //打牌
		{
			const auto& pai = pai_operate->pai(); //玩家发上来的牌

			//检查各个玩家手里的牌是否满足胡、杠、碰、吃
			auto player_id = CheckPai(pai, player->GetID()); 

			if (player_id) //第一个满足要求的玩家
			{
				_operation_limit.set_player_id(player_id); //当前可以进行操作的玩家
				_operation_limit.set_time_out(CommonTimerInstance.GetTime() + 8); //时间：8s后超时
				_operation_limit.mutable_pai()->CopyFrom(pai); //缓存这张牌
				
				//发送给Client
				Asset::PaiOperationAlert alert;
				alert.mutable_pai()->CopyFrom(pai);
				if (auto player_to = GetPlayer(player_id)) player_to->SendProtocol(alert);
			}
			else //没有玩家需要操作：给当前玩家的下家继续发牌
			{
				auto player_next = GetNextPlayer(player_id);
				if (!player_next) return; 
				
				auto cards = FaPai(1); 
				player_next->OnFaPai(cards);
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_HUPAI: //胡牌
		{
			bool ret = player->CheckHuPai(_operation_limit.pai());
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				//Caculate(); //结算
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GANGPAI: //杠牌
		{
			bool ret = player->CheckGangPai(_operation_limit.pai());
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnGangPai(_operation_limit.pai());

				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;

		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_PENGPAI: //碰牌
		{
			bool ret = player->CheckPengPai(pai);
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnPengPai(_operation_limit.pai());

				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;

		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_CHIPAI: //吃牌
		{
			bool ret = player->CheckChiPai(pai);
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnChiPai(_operation_limit.pai(), message);

				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GIVEUP: //放弃
		{
			auto player_next = GetNextPlayer(player->GetID());
			if (!player_next) return; 
			
			auto cards = FaPai(1); //发牌 
			player_next->OnFaPai(cards);

			ClearOperation(); //清理缓存以及等待玩家操作的状态
		}
		break;

		default:
		{
			return; //直接退出
		}
		break;
	}

	_operation_index = _operation_index % 4;
}

void Game::ClearOperation()
{
	_operation_limit.Clear(); //清理状态
}
	
int64_t Game::CheckPai(const Asset::PaiElement& pai, int64_t from_player_id)
{
	for (auto player : _players)
	{
		if (from_player_id == player.second->GetID()) continue;

		auto result = player.second->CheckPai(pai);
		if (result == Asset::PAI_CHECK_RETURN_NULL) continue;

		return player.second->GetID(); //当前可以进行操作的玩家
	}
	return 0;
}

void Game::OnOperateTimeOut()
{
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
	
std::shared_ptr<Player> Game::GetNextPlayer(int64_t player_id)
{
	if (!_room) return nullptr;

	return _room->GetNextPlayer(player_id);
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
				Asset::PaiElement card;
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
