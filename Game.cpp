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
	_cards.clear();
	_cards.resize(136);

	std::iota(_cards.begin(), _cards.end(), 1);
	std::vector<int32_t> cards(_cards.begin(), _cards.end());

	std::random_shuffle(cards.begin(), cards.end()); //洗牌

	_cards = std::list<int32_t>(cards.begin(), cards.end());

	auto log = make_unique<Asset::LogMessage>();
	log->set_type(Asset::GAME_CARDS);
	for (auto card : _cards) log->mutable_cards()->Add(card);
	LOG(INFO, log.get()); 

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

	auto banker_index = _room->GetBankerIndex();

	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];

		DEBUG("%s:line:%d player_id:%ld player_index:%d\n", __func__, __LINE__, player->GetID(), i);

		int32_t card_count = 13; //正常开启，普通玩家牌数量

		if (banker_index % MAX_PLAYER_COUNT == i) 
		{
			card_count = 14; //庄家牌数量
			_curr_player_index = i; //当前操作玩家
			_banker_player_id = player->GetID(); //庄家
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

	_room->SetBanker(_banker_player_id); //设置庄家

	Asset::GameInformation message;
	message.set_banker_player_id(_banker_player_id);
	BroadCast(message);
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
	if (Asset::PAI_OPER_TYPE_GIVEUP != pai_operate->oper_type())
	{
		_curr_player_index = GetPlayerOrder(player->GetID()); //上面检查过，就说明当前该玩家可操作
		BroadCast(message); //广播玩家操作，玩家放弃操作不能广播
	}

	//const auto& pai = _oper_limit.pai(); //缓存的牌
	const auto& pai = pai_operate->pai(); //玩家发上来的牌

	//一个人打牌之后，要检查其余每个玩家手中的牌，且等待他们的操作，直到超时
	switch (pai_operate->oper_type())
	{
		case Asset::PAI_OPER_TYPE_DAPAI: //打牌
		{
			//const auto& pai = pai_operate->pai(); //玩家发上来的牌
			//检查各个玩家手里的牌是否满足胡、杠、碰、吃
			if (CheckPai(pai, player->GetID())) //有满足要求的玩家
			{
				SendCheckRtn();
			}
			else //没有玩家需要操作：给当前玩家的下家继续发牌
			{
				auto next_player_index = (_curr_player_index + 1) % MAX_PLAYER_COUNT; 

				auto player_next = GetPlayerByOrder(next_player_index);
				
				if (!player_next) 
				{
					DEBUG_ASSERT(false);
					return; 
				}
				
				DEBUG("%s:line:%d next_player_id:%ld _curr_player_index:%d next_player_index:%d\n", 
						__func__, __LINE__, player_next->GetID(), _curr_player_index, next_player_index);

				auto cards = FaPai(1); 

				auto card = GameInstance.GetCard(cards[0]); //玩家待抓的牌

				Asset::PaiOperationAlert alert;

				//胡牌检查
				std::vector<Asset::FAN_TYPE> fan_list;
				if (player_next->CheckHuPai(card, fan_list)) 
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_pai()->CopyFrom(card);
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_HUPAI);
					if (player_next->IsTingPai()) pai_perator->mutable_oper_list()->Add(Asset::FAN_TYPE_SHANG_TING);  //听牌玩家胡牌翻番
				}
				else if (player_next->CheckBaoHu(pai))
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_pai()->CopyFrom(card);
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_HUPAI);
					fan_list.push_back(Asset::FAN_TYPE_LOU_BAO); //宝胡
				}

				//听牌检查:TODO 打牌后的一次
				if (player_next->CheckTingPai())
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_pai()->CopyFrom(card);
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_TINGPAI);
				}

				//旋风杠检查，只检查第一次发牌之前
				if (player_next->CheckFengGangPai()) 
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_XUANFENG_FENG);
				}
				if (player_next->CheckJianGangPai()) 
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_XUANFENG_JIAN);
				}
				
				player_next->OnFaPai(cards); //放入玩家牌里面
				
				//杠检查：包括明杠和暗杠
				std::vector<Asset::PaiElement> pais;
				if (player_next->CheckAllGangPai(pais)) 
				{
					for (auto pai : pais) 
					{
						auto pai_perator = alert.mutable_pais()->Add();
						pai_perator->mutable_pai()->CopyFrom(pai);
						pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_ANGANGPAI);
					}
				}

				if (alert.pais().size()) 
				{
					player_next->SendProtocol(alert); //提示Client

					_oper_limit.set_player_id(player_next->GetID()); //当前可操作玩家
					_oper_limit.set_from_player_id(player_next->GetID()); //当前牌来自玩家，自己抓牌
					_oper_limit.set_time_out(CommonTimerInstance.GetTime() + 30); //8秒后超时
				}
				else 
				{
					_curr_player_index = next_player_index;
				}

			}
		}
		break;
		
		case Asset::PAI_OPER_TYPE_HUPAI: //胡牌
		{
			std::vector<Asset::FAN_TYPE> fan_list;

			if (!player->CheckHuPai(pai, fan_list)) //无法胡牌
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				
				auto player_next = GetNextPlayer(player->GetID());
				if (!player_next) return; 
				
				auto cards = FaPai(1); 
				player_next->OnFaPai(cards);
				
				_curr_player_index = (_curr_player_index + 1) % MAX_PLAYER_COUNT;
			}
			else
			{
				Calculate(player->GetID(), _oper_limit.from_player_id(), fan_list); //结算
				_room->GameOver(player->GetID()); //胡牌

				OnOver();
			}
		}
		break;
		
		case Asset::PAI_OPER_TYPE_GANGPAI: //杠牌
		case Asset::PAI_OPER_TYPE_ANGANGPAI: //杠牌
		{
			bool ret = player->CheckGangPai(pai, _oper_limit.from_player_id());
			if (!ret) 
			{
				DEBUG_ASSERT(false);
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnGangPai(pai, _oper_limit.from_player_id());
				_curr_player_index = GetPlayerOrder(player->GetID()); //重置当前玩家索引
				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;

		case Asset::PAI_OPER_TYPE_PENGPAI: //碰牌
		{
			bool ret = player->CheckPengPai(pai);
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnPengPai(pai);
				_curr_player_index = GetPlayerOrder(player->GetID()); //重置当前玩家索引
				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;

		case Asset::PAI_OPER_TYPE_CHIPAI: //吃牌
		{
			bool ret = player->CheckChiPai(pai);
			if (!ret) 
			{
				player->AlertMessage(Asset::ERROR_GAME_PAI_UNSATISFIED); //没有牌满足条件
				return; 
			}
			else
			{
				player->OnChiPai(pai, message);
				ClearOperation(); //清理缓存以及等待玩家操作的状态
			}
		}
		break;
		
		case Asset::PAI_OPER_TYPE_GIVEUP: //放弃
		{
			if (SendCheckRtn()) return;
			
			auto next_player_index = (_curr_player_index + 1) % MAX_PLAYER_COUNT; //如果有玩家放弃操作，则继续下个玩家

			auto player_next = GetPlayerByOrder(next_player_index);
			DEBUG("%s:line:%d _oper_limit.player_id:%ld next_player_id:%ld _curr_player_index:%d next_player_index:%d\n", 
					__func__, __LINE__, _oper_limit.player_id(), player_next->GetID(), _curr_player_index, next_player_index);
			if (!player_next) return; 

			//如果是其他玩家放弃了操作(比如，对门不碰)，则检查下家还能不能要这张牌，来吃
			DEBUG("%s:line:%d _oper_limit.player_id:%ld next_player_id:%ld _curr_player_index:%d next_player_index:%d\n", 
					__func__, __LINE__, _oper_limit.player_id(), player_next->GetID(), _curr_player_index, next_player_index);
			
			auto cards = FaPai(1); 

			auto card = GameInstance.GetCard(cards[0]); //玩家待抓的牌

			Asset::PaiOperationAlert alert;

			//胡牌检查
			std::vector<Asset::FAN_TYPE> fan_list;
			if (player_next->CheckHuPai(card, fan_list)) 
			{
				auto pai_perator = alert.mutable_pais()->Add();
				pai_perator->mutable_pai()->CopyFrom(card);
				pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_HUPAI);
				if (player_next->IsTingPai()) pai_perator->mutable_oper_list()->Add(Asset::FAN_TYPE_SHANG_TING);  //听牌玩家胡牌翻番
			}
			else if (player_next->CheckBaoHu(pai))
			{
				auto pai_perator = alert.mutable_pais()->Add();
				pai_perator->mutable_pai()->CopyFrom(card);
				pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_HUPAI);
				fan_list.push_back(Asset::FAN_TYPE_LOU_BAO); //宝胡
			}

			//听牌检查:TODO 打牌后的一次
			if (player_next->CheckTingPai())
			{
				auto pai_perator = alert.mutable_pais()->Add();
				pai_perator->mutable_pai()->CopyFrom(card);
				pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_TINGPAI);
			}

			//旋风杠检查，只检查第一次发牌之前
			if (player_next->CheckFengGangPai()) 
			{
				auto pai_perator = alert.mutable_pais()->Add();
				pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_XUANFENG_FENG);
			}
			if (player_next->CheckJianGangPai()) 
			{
				auto pai_perator = alert.mutable_pais()->Add();
				pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_XUANFENG_JIAN);
			}
			
			player_next->OnFaPai(cards); //放入玩家牌里面
			
			//杠检查：包括明杠和暗杠
			std::vector<Asset::PaiElement> pais;
			if (player_next->CheckAllGangPai(pais)) 
			{
				for (auto pai : pais) 
				{
					auto pai_perator = alert.mutable_pais()->Add();
					pai_perator->mutable_pai()->CopyFrom(pai);
					pai_perator->mutable_oper_list()->Add(Asset::PAI_OPER_TYPE_GANGPAI);
				}
			}

			if (_oper_limit.player_id() == player_next->GetID()) 
			{
				ClearOperation(); //清理缓存以及等待玩家操作的状态

				_curr_player_index = next_player_index;
			}
			else if (alert.pais().size()) 
			{
				player_next->SendProtocol(alert); //提示Client

				_oper_limit.set_player_id(player_next->GetID()); //当前可操作玩家
				_oper_limit.set_from_player_id((player_next->GetID())); //当前牌来自玩家，自己抓牌，所以是自己
				_oper_limit.set_time_out(CommonTimerInstance.GetTime() + 30); //8秒后超时
			}
			else 
			{
				_curr_player_index = next_player_index;
			}
		}
		break;

		default:
		{
			DEBUG("%s:line:%d 服务器接收未能处理的协议，玩家角色:player_id:%ld, 操作类型:%d\n", __func__, __LINE__, player->GetID(), pai_operate->oper_type());
			DEBUG_ASSERT(false);
			return; //直接退出
		}
		break;
	}
}

void Game::ClearOperation()
{
	DEBUG("%s:line:%d player_id:%ld\n", __func__, __LINE__, _oper_limit.player_id());
	_oper_limit.Clear(); //清理状态
}
	
void Game::Calculate(int64_t hupai_player_id/*胡牌玩家*/, int64_t dianpao_player_id/*胡牌玩家*/, std::vector<Asset::FAN_TYPE>& fan_list)
{
	int32_t base_score = 1, total_score = 0;

	//番数由于玩家角色性检查(比如庄家胡牌翻番)
	if (IsBanker(hupai_player_id)) 
	{
		fan_list.push_back(Asset::FAN_TYPE_ZHUANG);
	}

	Asset::GameCalculate message;

	/////////////////////////////////////////////////////////////////胡牌积分
	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];
		if (!player) return;
		
		auto player_id = player->GetID();
		
		auto record = message.mutable_record()->mutable_list()->Add();
		record->set_player_id(player_id);

		if (hupai_player_id == player_id) continue;

		int32_t score = base_score;

		////////牌型基础分值计算
		for (const auto& fan : fan_list)
		{
			if (Asset::FAN_TYPE_ZHUANG == fan)
				score *= 2; //是否庄家
			else if (Asset::FAN_TYPE_ZHAN_LI == fan)
				score *= 2; //是否站立胡
			else if (Asset::FAN_TYPE_DUAN_MEN == fan)
				score *= 2; //是否缺门
			else if (Asset::FAN_TYPE_QING_YI_SE == fan)
				score *= 2; //是否清一色  
			else if (Asset::FAN_TYPE_LOU_BAO == fan)
				score *= 2; //是否搂宝  
			else if (Asset::FAN_TYPE_PIAO_HU == fan)
				score *= 2; //是否飘胡
			else if (Asset::FAN_TYPE_JIA_HU_NORMAL == fan)
				score *= 2; //夹胡
			else if (Asset::FAN_TYPE_JIA_HU_MIDDLE == fan)
				score *= 4; //中番夹胡
			else if (Asset::FAN_TYPE_JIA_HU_HIGHER == fan)
				score *= 8; //高番夹胡
			else if (Asset::FAN_TYPE_LOU_BAO == fan)
				score *= 2; //宝胡
			else if (Asset::FAN_TYPE_SHANG_TING == fan)
				score *= 2; //听牌
			
			//推送列表
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(fan);
			detail->set_score(score);
		}
		
		////////操作和玩家牌状态分值计算
		if (dianpao_player_id == hupai_player_id)
		{
			score *= 2; //自摸
			
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_ZI_MO);
			detail->set_score(score);
		}

		if (player_id == dianpao_player_id) 
		{
			score *= 2; //点炮翻番
			
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_DIAN_PAO);
			detail->set_score(score);
		}

		if (player->IsBimen()) 
		{
			score *= 2; //闭门翻番
			
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_BIMEN);
			detail->set_score(score);
		}

		if (dianpao_player_id == player_id)
		{
			score *= 2; //庄点炮
		
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_DIAN_PAO);
			detail->set_score(score);
		}
		
		record->set_score(-score); //玩家总共所输积分
		total_score += score; //胡牌玩家赢了该玩家积分
	}

	auto record = std::find_if(message.mutable_record()->mutable_list()->begin(), message.mutable_record()->mutable_list()->end(), 
			[hupai_player_id](const Asset::GameRecord_GameElement& ele){
				return hupai_player_id == ele.player_id();
			});
	if (record == message.mutable_record()->mutable_list()->end()) return;
	record->set_score(total_score); //胡牌玩家赢积分
	
	/////////////////////////////////////////////////////////////////杠牌积分
	for (int i = 0; i < MAX_PLAYER_COUNT; ++i)
	{
		auto player = _players[i];
		if (!player) return;
		
		auto ming_count = player->GetMingGangCount();
		
		auto an_count = player->GetAnGangCount();

		int32_t ming_score = ming_count * 1, an_score = an_count * 2;
		auto score = ming_score + an_score;
				
		DEBUG("%s:line:%d player_id:%ld, ming_count:%d, an_count:%d, score:%d\n", __func__, __LINE__, player->GetID(), ming_count, an_count, score);

		auto record = message.mutable_record()->mutable_list(i);
		record->set_score((record->score() + score) * (MAX_PLAYER_COUNT - 1)); //增加杠分

		////////杠牌玩家所赢积分
		if (ming_count)
		{
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_MING_GANG);
			detail->set_score(ming_score * (MAX_PLAYER_COUNT - 1));
		}

		if (an_count)
		{
			auto detail = record->mutable_details()->Add();
			detail->set_fan_type(Asset::FAN_TYPE_AN_GANG);
			detail->set_score(an_score * (MAX_PLAYER_COUNT - 1));
		}

		for (int index = 0; index < MAX_PLAYER_COUNT; ++index)
		{
			if (index == i) continue;

			auto record = message.mutable_record()->mutable_list(index);
			record->set_score(record->score() - score); //扣除杠分

			////////被杠牌玩家所输积分
			if (ming_count)
			{
				auto detail = record->mutable_details()->Add();
				detail->set_fan_type(Asset::FAN_TYPE_MING_GANG);
				detail->set_score(-ming_score * (MAX_PLAYER_COUNT - 1));
			}

			if (an_count)
			{
				auto detail = record->mutable_details()->Add();
				detail->set_fan_type(Asset::FAN_TYPE_AN_GANG);
				detail->set_score(-an_score * (MAX_PLAYER_COUNT - 1));
			}
		}
	}

	message.PrintDebugString();
		
	BroadCast(message);
}
	
void Game::BroadCast(pb::Message* message, int64_t exclude_player_id)
{
	if (!_room) return;
	_room->BroadCast(message, exclude_player_id);
}

void Game::BroadCast(pb::Message& message, int64_t exclude_player_id)
{
	if (!_room) return;
	_room->BroadCast(message, exclude_player_id);
}

bool Game::SendCheckRtn()
{
	ClearOperation();

	if (_oper_list.size() == 0) return false;

	auto check = [this](Asset::PAI_OPER_TYPE rtn_type, Asset::PaiOperationList& operation)->bool{

		for (const auto& oper : _oper_list)
		{
			auto it = std::find(oper.oper_list().begin(), oper.oper_list().end(), rtn_type);

			if (it != oper.oper_list().end()) 
			{
				operation = oper;

				return true;
			}
		}
		return false;
	};

	Asset::PaiOperationList operation;
	for (int32_t i = Asset::PAI_OPER_TYPE_HUPAI; i <= Asset::PAI_OPER_TYPE_CHIPAI; ++i)
	{
		auto result = check((Asset::PAI_OPER_TYPE)i, operation);
		if (result) break;
	}
	if (operation.oper_list().size() == 0) 
	{
		DEBUG("%s:line%d 没有可操作的牌值.\n", __func__, __LINE__);
		return false;
	}

	int64_t player_id = operation.player_id(); 

	_oper_limit.set_player_id(player_id); //当前可操作玩家
	_oper_limit.set_from_player_id(operation.from_player_id()); //当前牌来自玩家
	_oper_limit.mutable_pai()->CopyFrom(operation.pai()); //缓存这张牌
	_oper_limit.set_time_out(CommonTimerInstance.GetTime() + 30); //8秒后超时
	
	Asset::PaiOperationAlert alert;

	auto pai_perator = alert.mutable_pais()->Add();
	pai_perator->mutable_pai()->CopyFrom(operation.pai());

	for (auto rtn : operation.oper_list()) 
		pai_perator->mutable_oper_list()->Add(rtn);
		//alert.mutable_oper_list()->Add(rtn); //可操作牌类型
	if (auto player_to = GetPlayer(player_id)) 
		player_to->SendProtocol(alert); //发给目标玩家

	auto it = std::find_if(_oper_list.begin(), _oper_list.end(), [player_id](const Asset::PaiOperationList& operation){
				return player_id == operation.player_id();
			});
	if (it != _oper_list.end()) 
	{
		DEBUG("%s:line%d 删除玩家%ld操作.\n", __func__, __LINE__, player_id);
		_oper_list.erase(it);
	}

	return true;
}
	
/////////////////////////////////////////////////////
//
//检查各个玩家能否对该牌进行操作
//
//返回可操作玩家的索引
//
/////////////////////////////////////////////////////

bool Game::CheckPai(const Asset::PaiElement& pai, int64_t from_player_id)
{
	_oper_list.clear();

	int32_t player_index = GetPlayerOrder(from_player_id); //当前玩家索引
	if (player_index == -1) return false; //理论上不会出现

	//assert(_curr_player_index == player_index); //理论上一定相同：错误，如果碰牌的玩家出牌就不一定
	DEBUG("%s:line:%d _curr_player_index:%d player_index:%d\n", __func__, __LINE__, _curr_player_index, player_index);

	int32_t next_player_index = (_curr_player_index + 1) % MAX_PLAYER_COUNT;

	for (int32_t i = next_player_index; i < MAX_PLAYER_COUNT - 1 + next_player_index; ++i)
	{
		auto cur_index = i % MAX_PLAYER_COUNT;

		auto player = GetPlayerByOrder(cur_index);
		if (!player) return false; //理论上不会出现

		if (from_player_id == player->GetID()) continue; //自己不能对自己的牌进行操作

		auto rtn_check = player->CheckPai(pai, from_player_id); //TODO：其他玩家打的宝牌，已经听的玩家可以胡，理论上只有自摸宝牌才能胡
		if (rtn_check.size() == 0) 
		{
			DEBUG("%s:line:%d _curr_player_index:%d player_index:%d\n", __func__, __LINE__, _curr_player_index, player_index);
			continue; //不能吃、碰、杠和胡牌
		}

		for (auto value : rtn_check)
			DEBUG("玩家可以进行的操作: %s:line:%d cur_index:%d next_player_index:%d player_id:%ld value:%d\n", __func__, __LINE__, cur_index, next_player_index, player->GetID(),value);
		
		auto it_chi = std::find(rtn_check.begin(), rtn_check.end(), Asset::PAI_OPER_TYPE_CHIPAI);
		if (it_chi != rtn_check.end() && cur_index != next_player_index) rtn_check.erase(it_chi);
		
		if (rtn_check.size() == 0) continue; 

		///////////////////////////////////////////////////缓存所有操作
		Asset::PaiOperationList pai_operation;
		pai_operation.set_player_id(player->GetID());
		pai_operation.set_from_player_id(from_player_id);
		pai_operation.mutable_pai()->CopyFrom(pai);
		for (auto result : rtn_check) 
		{
			pai_operation.mutable_oper_list()->Add(result);
			DEBUG("%s:line:%d 可操作玩家:%ld 可以操作类型:%d\n", __func__, __LINE__, player->GetID(), result);
		}
		_oper_list.push_back(pai_operation);
	}

	return _oper_list.size() > 0;
}

void Game::OnOperateTimeOut()
{
}

std::vector<int32_t> Game::TailPai(size_t card_count)
{
	std::vector<int32_t> cards;
	
	if (_cards.size() < card_count) return cards;

	for (size_t i = 0; i < card_count; ++i)
	{
		int32_t value = _cards.back();	
		cards.push_back(value);
		_cards.pop_back();
	}

	return cards;
}

std::vector<int32_t> Game::FaPai(size_t card_count)
{
	std::vector<int32_t> cards;

	if (_cards.size() / 2 <= 12) //可以分牌，不能继续抓牌
	{
		DEBUG("%s:line:%d, size:%u\n", __func__, __LINE__, _cards.size());
		return cards;
	}
	
	if (_cards.size() < card_count) return cards;

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

	return GetPlayerByOrder((order + 1) % MAX_PLAYER_COUNT);
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

bool Game::IsBanker(int64_t player_id) 
{ 
	if (!_room) return false;
	return _room->IsBanker(player_id); 
}

Asset::PaiElement Game::GetBaopai(int32_t tail_index)
{
	std::vector<int32_t> list(_cards.begin(), _cards.end());

	auto card_index = list.size() - tail_index + 1; 

	return GameInstance.GetCard(list[card_index]);
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
