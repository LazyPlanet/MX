#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <boost/asio.hpp>
#include "Socket.h"

namespace Adoter
{

using namespace google::protobuf;

/*
 * 类说明：
 *
 * 会话：每个玩家和服务器保持的一个连接，称为会话.
 *
 * 说明：有的游戏也称为连接(Connection).
 *
 * */

class Player;

class WorldSession : public Socket<WorldSession>
{
public:
	//std::shared_ptr<Player> g_player = nullptr; //全局玩家定义，唯一的一个Player对象
	typedef std::function<int32_t(Message*)> CallBack;
public:
	virtual ~WorldSession() { }
	WorldSession(boost::asio::ip::tcp::socket&& socket);
	WorldSession(WorldSession const& right) = delete;    
	WorldSession& operator=(WorldSession const& right) = delete;
	
	virtual void Start() override;
	bool Update() override; 
	void InitializeHandler(const boost::system::error_code error, const std::size_t bytes_transferred);
};

class WorldSessionManager : public SocketManager<WorldSession> 
{
	typedef SocketManager<WorldSession> SuperSocketManager;
private:
	std::mutex _mutex;
	std::vector<std::shared_ptr<WorldSession>> _list; //定时清理断开的会话
	std::unordered_map<int64_t, std::shared_ptr<WorldSession>> _sessions; //根据玩家状态变化处理	
public:
	static WorldSessionManager& Instance()
	{
		static WorldSessionManager _instance;
		return _instance;
	}

	size_t GetCount();
	void Add(std::shared_ptr<WorldSession> session);
	void Emplace(int64_t player_id, std::shared_ptr<WorldSession> session);
	bool StartNetwork(boost::asio::io_service& io_service, const std::string& bind_ip, int32_t port, int thread_count = 1) override;
protected:        
	NetworkThread<WorldSession>* CreateThreads() const override;
private:        
	static void OnSocketAccept(tcp::socket&& socket, int32_t thread_index);
};

#define WorldSessionInstance WorldSessionManager::Instance()

}
