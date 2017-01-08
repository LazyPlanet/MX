#pragma once
/*
 * 认证服务器
 *
 * 主要用来生成TOCKEN
 *
 * 账号认证，其本质是一个WEB服务器，此处为了实现方便，做一个普通服务器，
 *
 * 主要由两部分组成：
 *
 * (1) 账号认证 Auth
 * 	所有客户端查询账号和密码.
 *
 * (2) 游戏服务器列表 RealmList
 * 	所有游戏服务器，注册到该模块.
 *
 * */
#include <string>
#include <mutex>
#include <thread>
#include <atomic>
#include <unordered_map>
#include <boost/asio.hpp>
#include "Auth.pb.h"

namespace Adoter
{

class AuthServer 
{
private:
	int32_t _bind_port;
	std::string _bind_ip;
	
	boost::asio::ip::tcp::endpoint _local_address;
	boost::asio::ip::tcp::endpoint _external_address;    

	std::thread _thread;
	std::mutex _ticket_mutex;    
	std::atomic<bool> _stopped;
	boost::asio::deadline_timer* _ticket_cleanup_timer; //登录认证清理超时
	std::unordered_map<std::string, std::shared_ptr<Auth::Account>> _valid_tickets;    
	
	//std::vector<std::shared_ptr<Session>> _client_sessions;	 
	//std::vector<std::shared_ptr<Session>> _server_sessions;	//游戏服务器连接会话
public:
	~AuthServer() 
	{
		_stopped = true;
		_thread.join(); //释放资源
		_ticket_cleanup_timer->cancel(); //停止定时器
	}
	virtual void OnAccept(const boost::system::error_code& ec);
	virtual bool Start(boost::asio::io_service& io_service);
	virtual void Run();
	virtual void Stop();
	virtual void CleanLoginTickets(const boost::system::error_code& error);
public:
	static AuthServer& Instance()
	{
		static AuthServer _instance;
		return _instance;
	}

	void AddTicket(const std::string& ticket, Auth::Account* account);
	bool CheckAccount() { return true; }
};

#define AuthInstance AuthServer::Instance()

}
