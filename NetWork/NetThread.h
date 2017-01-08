#include <mutex>
#include <vector>
#include <memory>
#include <atomic>
#include <thread>
#include <boost/asio.hpp>
#include <boost/asio/deadline_timer.hpp>

namespace Adoter 
{
/*
 * 网络线程模型
 *
 * */

using boost::asio::ip::tcp;

template<class SOCKET_TYPE>
class NetworkThread
{
public:    
	NetworkThread() : _connections(0), _stopped(false), _thread(nullptr), _accept_socket(_io_service), _update_timer(_io_service)    {    }
	virtual ~NetworkThread()    
	{        
		Stop();        
		
		if (_thread)        
		{            
			Wait();            
			_thread.reset();        
		}
	}

	virtual void Stop() { 
		_stopped = true;
		_io_service.stop();
	}

	virtual bool Start() { 
		
		_thread = std::make_shared<std::thread>(std::bind(&NetworkThread::Run, this));
		
		if (!_thread) return false;
	
		return true;
	}
	
	virtual void Wait() { 
		_thread->join();        
		_thread.reset();        
	}

	virtual int32_t GetConnectionCount() const { return _connections; }

	virtual void AddSocket(std::shared_ptr<SOCKET_TYPE> socket)
	{
		std::lock_guard<std::mutex> lock(_fresh_mutex);

		++_connections;
		_fresh_list.push_back(socket);
		SocketAdded(socket); //目前无用途
	}

	tcp::socket* GetSocketForAccept() { return &_accept_socket; }

	//把添加的SOCKET都加载进来
	virtual void AddSockets()
	{
		std::lock_guard<std::mutex> lock(_fresh_mutex);
		if (_fresh_list.empty()) return;
		for (auto socket : _fresh_list)
		{
			if (socket->IsOpen())
			{
				_socket_list.push_back(socket);
			}
			else
			{
				--_connections;
				SocketRemoved(socket); //目前无用途
			}
		}
		_fresh_list.clear();
	}

	virtual void Run()
	{
		std::cout << "Starting network thread..." << std::endl;
		_update_timer.expires_from_now(boost::posix_time::milliseconds(10));
		_update_timer.async_wait(std::bind(&NetworkThread<SOCKET_TYPE>::Update, this));        
		
		_io_service.run();

		_socket_list.clear();        
		_fresh_list.clear();
	}
	
	virtual void Update() //每0.01秒检查一次，删除不活跃的连接
	{
		if (_stopped) return;
		_update_timer.expires_from_now(boost::posix_time::milliseconds(10));
		_update_timer.async_wait(std::bind(&NetworkThread<SOCKET_TYPE>::Update, this));        
		//删除不连接的SOCKET，同时更新新连接的SOCKET
		AddSockets();
		_socket_list.erase(std::remove_if(_socket_list.begin(), _socket_list.end(), [this] (std::shared_ptr<SOCKET_TYPE> socket) {
			if (!socket->Update())
			{
				if (socket->IsOpen()) socket->Close();
				this->SocketRemoved(socket);
				--this->_connections;
				return true;
			}
			return false;
		}), _socket_list.end()); 
	}
protected:
	virtual void SocketAdded(std::shared_ptr<SOCKET_TYPE>) { }    
	virtual void SocketRemoved(std::shared_ptr<SOCKET_TYPE>) { }
private:
	std::vector<std::shared_ptr<SOCKET_TYPE>> _socket_list; //连接的SOCKET列表
	std::atomic<int32_t> _connections;    
	std::atomic<bool> _stopped;
	
	std::shared_ptr<std::thread> _thread;	
	
	std::mutex _fresh_mutex;
	std::vector<std::shared_ptr<SOCKET_TYPE>> _fresh_list; //新连接的SOCKET列表

	boost::asio::io_service _io_service;
	tcp::socket _accept_socket;
	boost::asio::deadline_timer _update_timer;
};

}
