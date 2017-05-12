
#include <spdlog/spdlog.h>

#include "WorldSession.h"
#include "RedisManager.h"
#include "CommonUtil.h"
#include "Player.h"
#include "MXLog.h"
#include "Protocol.h"
#include "PlayerName.h"

namespace Adoter
{

namespace spd = spdlog;

WorldSession::~WorldSession()
{
    KillOutPlayer();
}

WorldSession::WorldSession(boost::asio::ip::tcp::socket&& socket) : Socket(std::move(socket))
{
}

void WorldSession::InitializeHandler(const boost::system::error_code error, const std::size_t bytes_transferred)
{
	try
	{
		auto log = make_unique<Asset::LogMessage>();
		log->set_client_ip(_socket.remote_endpoint().address().to_string());
		LOG(ACTION, log.get());

		if (error)
		{
			Close(); ////断开网络连接
			return;
		}
		else
		{
			Asset::Meta meta;
			bool result = meta.ParseFromArray(_buffer.data(), bytes_transferred);

			if (!result) return;		//非法协议
			
			//std::cout << "接收数据：";
			//meta.PrintDebugString(); //打印出来Message.
			//std::cout << std::endl;
			
			/////////////////////////////////////////////////////////////////////////////打印收到协议提示信息
		
			const pb::FieldDescriptor* type_field = meta.GetDescriptor()->FindFieldByName("type_t");
			if (!type_field) return;

			const pb::EnumValueDescriptor* enum_value = meta.GetReflection()->GetEnum(meta, type_field);
			if (!enum_value) return;

			const std::string& enum_name = enum_value->name();
			
			if (g_player)
				DEBUG("%s:line:%d, 玩家:%ld 接收客户端发送的协议数据:%s\n", __func__, __LINE__, g_player->GetID(), enum_name.c_str());
			else
				DEBUG("%s:line:%d, 接收客户端发送的协议数据:%s\n", __func__, __LINE__, enum_name.c_str());
			
			pb::Message* msg = ProtocolInstance.GetMessage(meta.type_t());	
			if (!msg) 
			{
				DEBUG("Could not found message of type:%d", meta.type_t());
				return;		//非法协议
			}

			auto message = msg->New();
			
			//result = message->ParseFromString(meta.stuff());
			result = message->ParseFromArray(meta.stuff().c_str(), meta.stuff().size());
			if (!result) return;		//非法协议
		
			message->PrintDebugString(); //打印出来Message.

			/////////////////////////////////////////////////////////////////////////////游戏逻辑处理流程
			
			if (Asset::META_TYPE_C2S_LOGIN == meta.type_t()) //账号登陆
			{
				Asset::Login* login = dynamic_cast<Asset::Login*>(message);
				if (!login) return; 
				
			 	auto redis = std::make_shared<Redis>();
				std::string stuff = redis->GetUser(login->account().username());

				Asset::User user;

				if (stuff.empty()) //没有数据
				{
					user.mutable_account()->CopyFrom(login->account());

					///////如果账号下没有角色，创建一个给Client

					int64_t player_id = redis->CreatePlayer();
					if (player_id == 0) 
					{
						DEBUG("Create player failed.");
						return; //创建失败
					}

					user.mutable_player_list()->Add(player_id);

					auto stuff = user.SerializeAsString();
					redis->SaveUser(login->account().username(), stuff); //账号数据存盘

					g_player = std::make_shared<Player>(player_id, shared_from_this());
					std::string player_name = NameInstance.Get();
					DEBUG("%s:line:%d, player_id:%ld, player_name:%s\n", __func__, __LINE__, player_id, player_name.c_str());
					g_player->SetName(player_name);
					g_player->Save(); //存盘，防止数据库无数据
				}
				else
				{
					user.ParseFromString(stuff);
				}

				///////清理状态
				_account.Clear(); _player_list.clear();
				//账号信息
				_account.CopyFrom(login->account());
				//玩家数据
				for (auto player_id : user.player_list())
				{
					_player_list.emplace(player_id);
				}
				///////发送给Client当前的角色信息
				Asset::PlayerList player_list;
				player_list.mutable_player_list()->CopyFrom(user.player_list());
				SendProtocol(player_list); //传给Client，带有角色ID
				
				//记录日志
				//log->set_player_id(g_player->GetID());
				log->set_type(Asset::PLAYER_LOGIN);
				LOG(ACTION, log.get());
			}
			else if (Asset::META_TYPE_C2S_LOGOUT == meta.type_t()) //账号登出
			{
				Asset::Logout* logout = dynamic_cast<Asset::Logout*>(message);
				if (!logout) return; 

				KillOutPlayer();
			}
			else if (Asset::META_TYPE_SHARE_CREATE_PLAYER == meta.type_t()) //创建角色
			{
				return;
				Asset::CreatePlayer* create_player = dynamic_cast<Asset::CreatePlayer*>(message);
				if (!create_player) return; 
				
			 	std::shared_ptr<Redis> redis = std::make_shared<Redis>();
				int64_t player_id = redis->CreatePlayer();
				if (player_id == 0) return; //创建失败

				g_player = std::make_shared<Player>(player_id, shared_from_this());
				g_player->Save(); //存盘，防止数据库无数据
				
				//返回结果
				create_player->set_player_id(player_id);
				g_player->SendProtocol(create_player);
			}
			else if (Asset::META_TYPE_C2S_ENTER_GAME == meta.type_t()) //进入游戏
			{
				const Asset::EnterGame* enter_game = dynamic_cast<Asset::EnterGame*>(message);
				if (!enter_game) return; 

				if (_player_list.find(enter_game->player_id()) == _player_list.end())
				{
					Close();
					DEBUG("Player has not found.");
					return; //账号下没有该角色数据
				}

				if (!g_player) g_player = std::make_shared<Player>(enter_game->player_id(), shared_from_this());
				g_player->OnEnterGame(); //加载数据
			}
			else
			{
				if (!g_player) 
				{
					DEBUG("Player has not inited");
					return; //未初始化的Player
				}
				//其他协议的调用规则
				g_player->HandleProtocol(meta.type_t(), message);
			}
		}
	}
	catch (std::exception& e)
	{
		Close();
		return;
	}
	//递归持续接收	
	AsyncReceiveWithCallback(&WorldSession::InitializeHandler);
}

void WorldSession::KillOutPlayer()
{
	Close();
}

void WorldSession::Start()
{
	AsyncReceiveWithCallback(&WorldSession::InitializeHandler);
}
	
bool WorldSession::Update() 
{ 
	if (!Socket::Update()) return false;

	if (!g_player) 
	{
		return true; //长时间未能上线
	}

	g_player->Update(); 

	//std::cout << "-------------update" << std::endl;

	return true;
}

void WorldSession::OnClose()
{
	spdlog::get("console")->error("{0} Line:{1} Remote client disconnect, remote_ip:{2}, player_id:{3}", 
			__func__, __LINE__, _socket.remote_endpoint().address().to_string().c_str(), g_player == nullptr ? 0 : g_player->GetID());

	if (g_player) //网络断开
	{
		g_player->OnLogout(nullptr);

		g_player.reset();

		g_player = nullptr;
	}
}

void WorldSession::SendProtocol(pb::Message* message)
{
	SendProtocol(*message);
}

void WorldSession::SendProtocol(pb::Message& message)
{
	spdlog::get("console")->warn(message.DebugString());

	const pb::FieldDescriptor* field = message.GetDescriptor()->FindFieldByName("type_t");
	if (!field) return;
	
	int type_t = field->default_value_enum()->number();
	if (!Asset::META_TYPE_IsValid(type_t)) return;	//如果不合法，不检查会宕线
	
	Asset::Meta meta;
	meta.set_type_t((Asset::META_TYPE)type_t);
	meta.set_stuff(message.SerializeAsString());

	std::string content = meta.SerializeAsString();
	//AsyncSend(content);

	EnterQueue(std::move(content));
	const pb::EnumValueDescriptor* enum_value = message.GetReflection()->GetEnum(message, field);
	if (!enum_value) return;
	DEBUG("%s:line:%d, protocol_name:%s, content:%s\n", __func__, __LINE__, enum_value->name().c_str(), message.ShortDebugString().c_str());
}

void WorldSessionManager::Add(std::shared_ptr<WorldSession> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	_list.push_back(session);
}	

void WorldSessionManager::Emplace(int64_t player_id, std::shared_ptr<WorldSession> session)
{
	std::lock_guard<std::mutex> lock(_mutex);
	if (_sessions.find(player_id) == _sessions.end())
		_sessions.emplace(player_id, session);
}

size_t WorldSessionManager::GetCount()
{
	std::lock_guard<std::mutex> lock(_mutex);
	return _list.size();
}

NetworkThread<WorldSession>* WorldSessionManager::CreateThreads() const
{    
	return new NetworkThread<WorldSession>[GetNetworkThreadCount()];
}

void WorldSessionManager::OnSocketAccept(tcp::socket&& socket, int32_t thread_index)
{    
	WorldSessionInstance.OnSocketOpen(std::forward<tcp::socket>(socket), thread_index);
}

bool WorldSessionManager::StartNetwork(boost::asio::io_service& io_service, const std::string& bind_ip, int32_t port, int thread_count)
{
	if (!SuperSocketManager::StartNetwork(io_service, bind_ip, port, thread_count)) return false;
	_acceptor->SetSocketFactory(std::bind(&SuperSocketManager::GetSocketForAccept, this));    
	_acceptor->AsyncAcceptWithCallback<&OnSocketAccept>();    
	return true;
}

}
