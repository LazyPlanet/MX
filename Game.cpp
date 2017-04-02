#include <vector>
#include <algorithm>

#include "Game.h"
#include "Timer.h"
#include "Asset.h"
#include "MXLog.h"
#include "CommonUtil.h"

namespace Adoter
{

/////////////////////////////////////////////////////
//一场游戏
/////////////////////////////////////////////////////
void Game::Init(std::shared_ptr<Room> room)
{
	_cards.resize(136);

	std::iota(_cards.begin(), _cards.end(), 1);

	std::vector<int32_t> cards(_cards.begin(), _cards.end());

	std::random_shuffle(cards.begin(), cards.end()); //洗牌

	_cards = std::list<int32_t>(cards.begin(), cards.end());

	_room = room;
}

bool Game::Start(std::vector<std::shared_ptr<Player>> players)
{
	if (MAX_PLAYER_COUNT != players.size()) return false; //做下检查，是否满足开局条件

	int32_t player_index = 0;

	for (auto player : players)
	{
		_players[player_index++] = player; //复制成员
		player->SetPosition(Asset::POSITION_TYPE(player_index));
	}

	_banker_index = _room->GetBankerIndex();

	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];

		DEBUG("%s:line:%d player_id:%ld player_index:%d\n", __func__, __LINE__, player->GetID(), i);

		int32_t card_count = 13; //正常开启，普通玩家牌数量

		if (_banker_index % 4 == i) 
		{
			card_count = 14; //庄家牌数量
			_curr_player_index = i; //当前操作玩家
		}

		auto cards = FaPai(card_count);

		player->OnFaPai(cards);  //各个玩家发牌
		player->SetGame(shared_from_this());
	}

	OnStart();

	return true;
}
	
void Game::OnStart()
{
	if (!_room) return;

}

bool Game::OnOver()
{
	_hupai_players.push_back(1);
	//清理牌
	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];
		player->ClearCards();
	}
	return true;
}

/////////////////////////////////////////////////////
//
//玩家可操作的状态只有2种，顺序不可变：
//
//(1) 碰、杠、胡牌；
//
//(2) 轮到玩家；
//
/////////////////////////////////////////////////////
	
bool Game::CanPaiOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (/*_oper_limit.time_out() < CommonTimerInstance.GetTime() 
			&& */_oper_limit.player_id() == player->GetID()) 
	{
		return true; //玩家操作：碰、杠、胡牌
	}

	auto player_index = GetPlayerOrder(player->GetID());
	if (_curr_player_index == player_index) 
	{
		return true; //轮到该玩家
	}

	DEBUG("%s:line:%d curr_player_index:%d player_index:%d player_id:%ld oper_limit_player_id:%ld\n", 
			__func__, __LINE__, _curr_player_index, player_index, player->GetID(), _oper_limit.player_id());
	return false;
}

void Game::OnPaiOperate(std::shared_ptr<Player> player, pb::Message* message)
{
	if (!player || !message || !_room) return;
	
	DEBUG("%s:line:%d player_id:%ld, 当前可操作的牌:%s\n", __func__, __LINE__, player->GetID(), _oper_limit.DebugString().c_str());

	if (!CanPaiOperate(player, message)) 
	{
		player->AlertMessage(Asset::ERROR_GAME_NO_PERMISSION); //没有权限，没到玩家操作，防止外挂
		DEBUG_ASSERT(false); 
	}

	//if (CommonTimerInstance.GetTime() < _oper_limit.time_out()) ClearOperation(); //已经超时，清理缓存以及等待玩家操作的状态
			
	Asset::PaiOperation* pai_operate = dynamic_cast<Asset::PaiOperation*>(message);
	if (!pai_operate) return; 

	//如果不是放弃，才是当前玩家的操作
	if (Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GIVEUP != pai_operate->oper_type())
		_curr_player_index = GetPlayerOrder(player->GetID()); //上面检查过，就说明当前该玩家可操作

	_room->BroadCast(message); //广播玩家操作
	
	const auto& pai = _oper_limit.pai(); //缓存的牌

	//一个人打牌之后，要检查其余每个玩家手中的牌，且等待他们的操作，直到超时
	switch (pai_operate->oper_type())
	{
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_DAPAI: //打牌
		{
			const auto& pai = pai_operate->pai(); //玩家发上来的牌

			//检查各个玩家手里的牌是否满足胡、杠、碰、吃
			std::vector<Asset::PAI_CHECK_RETURN> pai_rtn;
			auto player_id = CheckPai(pai, player->GetID(), pai_rtn); 

			DEBUG("满足操作的玩家数据:%s:line:%d player_id:%ld can PaiOperate\n", __func__, __LINE__, player_id);

			if (player_id) //第一个满足要求的玩家
			{
				_oper_limit.set_player_id(player_id); //当前可操作玩家
				_oper_limit.mutable_pai()->CopyFrom(pai); //缓存这张牌
				_oper_limit.set_time_out(CommonTimerInstance.GetTime() + 30); //8秒后超时
				
				//发送给Client
				Asset::PaiOperationAlert alert;
				alert.mutable_pai()->CopyFrom(pai);
				for (auto rtn : pai_rtn) alert.mutable_check_return()->Add(rtn); //可操作牌类型
				if (auto player_to = GetPlayer(player_id)) player_to->SendProtocol(alert);
			}
			else //没有玩家需要操作：给当前玩家的下家继续发牌
			{
				auto player_next = GetNextPlayer(player->GetID());
				if (!player_next) return; 

				DEBUG("没有操作玩家，顺序发牌:%s:line:%d player_id:%ld next_player_id:%ld can PaiOperate _curr_player_index:%d\n", __func__, __LINE__, 
						player_id, player_next->GetID(), _curr_player_index);
				
				auto cards = FaPai(1); 
				player_next->OnFaPai(cards);

				_curr_player_index = (_curr_player_index + 1) % 4;
				DEBUG("设置当前操作数据数据:%s:line:%d player_id:%ld can PaiOperate _curr_player_index:%d\n", __func__, __LINE__, player_id, _curr_player_index);
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_HUPAI: //胡牌
		{
			bool ret = player->CheckHuPai(_oper_limit.pai());
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				
				auto player_next = GetNextPlayer(player->GetID());
				if (!player_next) return; 
				
				auto cards = FaPai(1); 
				player_next->OnFaPai(cards);
				
				_curr_player_index = (_curr_player_index + 1) % 4;

				return; 
			}
			else
			{
				//Caculate(); //结算
				_room->GameOver(player->GetID()); //胡牌

				OnOver();
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GANGPAI: //杠牌
		{
			bool ret = player->CheckGangPai(_oper_limit.pai());
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnGangPai(_oper_limit.pai());
				
				auto cards = FaPai(1);  //理论上应该给他从后面发一张，现在就顺序发一张吧
				player->OnFaPai(cards);
				
				_curr_player_index = GetPlayerOrder(player->GetID()); //重置当前玩家索引

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
				player->OnPengPai(_oper_limit.pai());
				
				_curr_player_index = GetPlayerOrder(player->GetID()); //重置当前玩家索引

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
				player->OnChiPai(_oper_limit.pai(), message);
				
				//_curr_player_index = (_curr_player_index + 1) % 4; //吃完牌,还是当前玩家操作

				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;
		
		case Asset::PaiOperation_PAI_OPER_TYPE_PAI_OPER_TYPE_GIVEUP: //放弃
		{
			auto next_player_index = (_curr_player_index + 1) % 4; //如果有玩家放弃操作，则继续下个玩家

			auto player_next = GetPlayerByOrder(next_player_index);
			CP("%s:line:%d _oper_limit.player_id:%ld next_player_id:%ld _curr_player_index:%d next_player_index:%d\n", 
					__func__, __LINE__, _oper_limit.player_id(), player_next->GetID(), _curr_player_index, next_player_index);
			if (!player_next) return; 

			//如果是其他玩家放弃了操作(比如，对门不碰)，则检查下家还能不能要这张牌，来吃
			CP("%s:line:%d _oper_limit.player_id:%ld next_player_id:%ld _curr_player_index:%d next_player_index:%d\n", 
					__func__, __LINE__, _oper_limit.player_id(), player_next->GetID(), _curr_player_index, next_player_index);

			if (_oper_limit.player_id() != player_next->GetID()) 
			{
				auto rtn_check = player_next->CheckPai(pai);

				if (rtn_check.size() > 0)
				{
					//发送给Client
					Asset::PaiOperationAlert alert;
					alert.mutable_pai()->CopyFrom(pai);
					for (auto rtn : rtn_check) alert.mutable_check_return()->Add(rtn); //可操作牌类型
					player_next->SendProtocol(alert);
				}
				else
				{
					auto cards = FaPai(1); //发牌 
					player_next->OnFaPai(cards);

					_curr_player_index = next_player_index;
					ClearOperation(); //清理缓存以及等待玩家操作的状态
				}
			}
			else
			{
				auto cards = FaPai(1); //发牌 
				player_next->OnFaPai(cards);

				_curr_player_index = next_player_index;
				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;

		default:
		{
			return; //直接退出
		}
		break;
	}
}

void Game::ClearOperation()
{
	CP("%s:line:%d player_id:%ld\n", __func__, __LINE__, _oper_limit.player_id());
	_oper_limit.Clear(); //清理状态
}
	
/////////////////////////////////////////////////////
//
//检查各个玩家能否对该牌进行操作
//
//返回可操作玩家的索引
//
/////////////////////////////////////////////////////

int64_t Game::CheckPai(const Asset::PaiElement& pai, int64_t from_player_id, std::vector<Asset::PAI_CHECK_RETURN>& pai_rt)
{
	int32_t player_index = GetPlayerOrder(from_player_id); //当前玩家索引
	if (player_index == -1) return 0; //理论上不会出现

	//assert(_curr_player_index == player_index); //理论上一定相同：错误，如果碰牌的玩家出牌就不一定
	CP("%s!!!:line:%d _curr_player_index:%d player_index:%d\n", __func__, __LINE__, _curr_player_index, player_index);

	int32_t next_player_index = (_curr_player_index + 1) % 4;

	int64_t rtn_player_id = 0; //返回可操作的玩家ID

	for (int32_t i = next_player_index; i < 3 + next_player_index; ++i)
	{
		auto cur_index = i % 4;

		auto player = GetPlayerByOrder(cur_index);
		if (!player) 
		{
			CP("%s!!!:line:%d cur_index:%d player_index:%d\n", __func__, __LINE__, cur_index, player->GetID());
			return rtn_player_id; //理论上不会出现
		}

		if (from_player_id == player->GetID())
		{
			CP("%s!!!:line:%d cur_index:%d player_index:%d\n", __func__, __LINE__, cur_index, player->GetID());
			continue; //自己不能对自己的牌进行操作
		}

		auto rtn_check = player->CheckPai(pai);
		if (rtn_check.size() == 0) 
		{
			CP("%s!!!:line:%d _curr_player_index:%d player_index:%d\n", __func__, __LINE__, _curr_player_index, player_index);
			continue; //不能吃、碰、杠和胡牌
		}

		auto it_chi = std::find(rtn_check.begin(), rtn_check.end(), Asset::PAI_CHECK_RETURN_CHI);
		if (it_chi != rtn_check.end() && rtn_check.size() == 1 && cur_index != next_player_index) 
		{
			CP("%s!!!:line:%d cur_index:%d player_index:%d\n", __func__, __LINE__, cur_index, player->GetID());
			continue; //吃牌只能是下家
		}
		
		rtn_player_id = player->GetID(); //只获取可操作的玩家ID
		pai_rt = rtn_check;
			
		CP("%s!!!:line:%d rtn_player_id:%ld rtn_player_id:%ld\n", __func__, __LINE__, rtn_player_id, player->GetID());
		
		auto it_hu = std::find(rtn_check.begin(), rtn_check.end(), Asset::PAI_CHECK_RETURN_HU);
		if (it_hu != rtn_check.end()) break; //胡牌则终止检查
	}

	return rtn_player_id;
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

	int32_t order = GetPlayerOrder(player_id);
	if (order == -1) return nullptr;

	return GetPlayerByOrder((order + 1) % 4);
}

int32_t Game::GetPlayerOrder(int32_t player_id)
{
	if (!_room) return -1;

	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];

		if (!player) continue;

		if (player->GetID() == player_id) return i; //序号
	}

	return -1;
}

std::shared_ptr<Player> Game::GetPlayerByOrder(int32_t player_index)
{
	if (!_room) return nullptr;

	if (player_index < 0 || player_index >= MAX_PLAYER_COUNT) return nullptr;

	return _players[player_index];
}

std::shared_ptr<Player> Game::GetPlayer(int64_t player_id)
{
	for (auto player : _players)
	{
		if (!player) continue;

		if (player->GetID() == player_id) return player;
	}
	
	return nullptr;
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
