#include "WorldSession.h"
#include "RedisManager.h"
#include "Player.h"

namespace Adoter
{

//////////////////////////////////////////////////////////////////////////////////////
std::shared_ptr<Player> g_player = nullptr; //全局玩家定义，唯一的一个Player对象
//////////////////////////////////////////////////////////////////////////////////////

WorldSession::WorldSession(boost::asio::ip::tcp::socket&& socket) : Socket(std::move(socket))
{
}

void WorldSession::InitializeHandler(const boost::system::error_code error, const std::size_t bytes_transferred)
{
	try
	{
		std::cout << "------------------------------" << _socket.remote_endpoint().address() << std::endl;

		if (error)
		{
			Close();
			std::cout << "Remote client disconnect, RemoteIp:" << _socket.remote_endpoint().address() << std::endl;
			return;
		}
		else
		{
				{
					std::cout << "发送数据给:" << _socket.remote_endpoint().address() << std::endl;
					Asset::S_CreateRoom enter_game;
					enter_game.set_type_t(Asset::META_TYPE_S2C_CREATE_ROOM);
					enter_game.mutable_room()->set_room_id(100000);

					Asset::Meta smeta;
					smeta.set_type_t(Asset::META_TYPE_C2S_CREATE_ROOM);
					smeta.set_stuff(enter_game.SerializeAsString());

					std::string content = smeta.SerializeAsString();
					AsyncSend(content.c_str(), content.size());
					const char* buff = content.c_str();
					for (size_t i = 0; i < content.size(); ++i)
					{
						std::cout << buff[i] << " ";
					}	
					std::cout << std::endl;
				}
			Asset::Meta meta;
			bool result = meta.ParseFromArray(_buffer.data(), bytes_transferred);
			if (!result) 
			{
				std::cout << __func__ << ":Meta parse error." << std::endl;	
				Close();
				return;		//非法协议
			}
			
			/////////////////////////////////////////////////////////////////////////////打印收到协议提示信息
		
			const pb::FieldDescriptor* type_field = meta.GetDescriptor()->FindFieldByName("type_t");
			if (!type_field) return;

			const pb::EnumValueDescriptor* enum_value = meta.GetReflection()->GetEnum(meta, type_field);
			if (!enum_value) return;

			const std::string& enum_name = enum_value->name();
			std::cout << "Received message type is: " << enum_name.c_str() << std::endl;
			
			google::protobuf::Message* message = ProtocolInstance.GetMessage(meta.type_t());	
			if (!message) 
			{
				std::cout << __func__ << ":Could not found message of type:" << meta.type_t() << std::endl;
				Close();
				return;		//非法协议
			}
			
			result = message->ParseFromString(meta.stuff());
			if (!result) 
			{
				std::cout << __func__ << ":Messge parse error." << std::endl;	
				Close();
				return;		//非法协议
			}
			
			/////////////////////////////////////////////////////////////////////////////游戏逻辑处理流程
			
			if (Asset::META_TYPE_C2S_CREATE_PLAYER == meta.type_t()) //创建角色
			{
				Asset::CreatePlayer* create_player = dynamic_cast<Asset::CreatePlayer*>(message);
				if (!create_player) return; 
				
			 	std::shared_ptr<Redis> redis = std::make_shared<Redis>();
				int64_t player_id = redis->CreatePlayer();
				if (player_id == 0) return; //创建失败

				g_player = std::make_shared<Player>(player_id, shared_from_this());
				g_player->Save(); //存盘，防止数据库无数据
				
				//返回结果
				//create_player->set_player_id(player_id);
				//g_player->SendResponse(create_player);
				g_player->OnCreatePlayer(player_id);
			}
			else if (Asset::META_TYPE_C2S_ENTER_GAME == meta.type_t()) //进入游戏
			{
				if (g_player) return; //已经在登录状态

				const Asset::EnterGame* enter_game = dynamic_cast<Asset::EnterGame*>(message);
				if (!enter_game) return; 

				if (!g_player) g_player = std::make_shared<Player>(enter_game->player_id(), shared_from_this());
				g_player->OnEnterGame(); //加载数据
			}
			else
			{
				if (!g_player) 
				{
					std::cerr << "Player has not inited. " << std::endl;
					return; //未初始化的Player
				}
				//其他协议的调用规则
				g_player->HandleProtocol(meta.type_t(), message);
			}
		}
	}
	catch (std::exception& e)
	{
		std::cerr << __func__ << ":Exception: " << e.what() << std::endl;
		//g_player->Logout(nullptr); //网络断开，释放对象
		return;
	}
	//递归持续接收	
	AsyncReceiveWithCallback(&WorldSession::InitializeHandler);
}

void WorldSession::Start()
{
	AsyncReceiveWithCallback(&WorldSession::InitializeHandler);
}
	
bool WorldSession::Update() 
{ 
	if (!g_player) return true;
	g_player->Update(); 
	return true;
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
