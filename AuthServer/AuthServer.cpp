#include "AuthServer.h"
#include <memory>
#include <thread>
#include <iostream>
#include "CommonUtil.h"

namespace Adoter
{

void AuthServer::OnAccept(const boost::system::error_code& ec)
{
}

void AuthServer::AddTicket(const std::string& ticket, Auth::Account* account)
{
	std::unique_lock<std::mutex> lock(_ticket_mutex);
	account->set_expiry_time(time(nullptr) + 10);
	if (_valid_tickets.find(ticket) == _valid_tickets.end())
		_valid_tickets.emplace(ticket, std::shared_ptr<Auth::Account>(std::move(account)));
}

bool AuthServer::Start(boost::asio::io_service& io_service)
{
	_ticket_cleanup_timer = new boost::asio::deadline_timer(io_service);	
	_ticket_cleanup_timer->expires_from_now(boost::posix_time::seconds(10));    
	_ticket_cleanup_timer->async_wait(std::bind(&AuthServer::CleanLoginTickets, this, std::placeholders::_1));

	_thread = std::thread(std::bind(&AuthServer::Run, this));

	return true;
}

void AuthServer::Run()
{
}

void AuthServer::Stop()
{
	_stopped = true;
	_thread.join(); //释放资源
	_ticket_cleanup_timer->cancel(); //停止定时器
}

void AuthServer::CleanLoginTickets(const boost::system::error_code& error)
{
	if (error) return;
	time_t now = time(nullptr);
	{
		std::unique_lock<std::mutex> lock(_ticket_mutex);
		for (auto it = _valid_tickets.begin(); it != _valid_tickets.end(); ++it)
		{
			if (it->second->expiry_time() < now)	 //超时
				it = _valid_tickets.erase(it);
			else
				++it;
		}
	}
	//每10秒检查一次
	_ticket_cleanup_timer->expires_from_now(boost::posix_time::seconds(10));    
	_ticket_cleanup_timer->async_wait(std::bind(&AuthServer::CleanLoginTickets, this, std::placeholders::_1));
}

}

