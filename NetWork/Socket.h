#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <unordered_map>
#include <boost/asio.hpp>
#include "Protocol.h"
#include "AsyncAcceptor.h"
#include "NetThread.h"

namespace Adoter
{

namespace pb = google::protobuf;

template<class T, class S = boost::asio::ip::tcp::socket>
class Socket : public std::enable_shared_from_this<T>
{
public:
	S _socket; 
public:
	explicit Socket(boost::asio::ip::tcp::socket&& socket) : _socket(std::move(socket)), _closed(false), _closing(false) { }
	virtual ~Socket() 
	{
		std::cout << __func__ <<  _socket.remote_endpoint().address() << std::endl;
		_socket.close();
	}
	
	virtual bool Update() {
		if (_closed) return false;
		//发送可以放到消息队列里面处理
		//if (_isWritingAsync || (_writeQueue.empty() && !_closing)) return true;
		//for (; HandleQueue();) {}
		return true;
	}

	virtual S& GetStream() { return _socket; } //当前字节流
	virtual boost::asio::ip::tcp::socket::endpoint_type GetRemoteEndPoint() const { return _socket.remote_endpoint(); }
	virtual void Start() = 0; 
	virtual void Close()
	{
		if (_closed.exchange(true)) return;

		boost::system::error_code error;
		_socket.shutdown(boost::asio::socket_base::shutdown_send, error);

		OnClose();
	}
	virtual void DelayedClose() { _closing = true; } //发送队列为空时再进行关闭
	virtual bool IsConnect() { return _socket.is_open(); }
	virtual bool IsOpen() const { return !_closed && !_closing; }

	virtual void AsyncReceive()
	{
		_socket.async_read_some(boost::asio::buffer(_buffer), std::bind(&Socket<T, S>::OnReceive, this, std::placeholders::_1, std::placeholders::_2));
	}
	virtual void OnReceive(const boost::system::error_code& error, const std::size_t bytes_transferred)
	{
		AsyncReceive();
	}
	virtual void AsyncReceiveWithCallback(void(T::*callback)(boost::system::error_code, std::size_t))
	{
		_socket.async_read_some(boost::asio::buffer(_buffer), std::bind(callback, this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
	}
	virtual void AsyncSend(std::string& content)
	{
		AsyncSend(content.c_str(), content.size());
	}
	virtual void AsyncSend(const char* buff, size_t size)
	{
		boost::asio::async_write(_socket, boost::asio::buffer(buff, size), std::bind(&Socket::OnSend, this, std::placeholders::_1, std::placeholders::_2));
	}
	virtual void OnSend(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		std::cout << __func__ << ":bytes_transferred:" << bytes_transferred << " has error:" << error << std::endl;
	}

protected:
	virtual void OnClose() { }
protected:
	std::atomic<bool> _closed;    
	std::atomic<bool> _closing;
	//接收缓存
	std::array<char, 4096> _buffer;
};

template <class SOCKET_TYPE> //各种类型的SOCKET，比如Session
class SocketManager 
{
protected:
	AsyncAcceptor* _acceptor; //接收连接
	NetworkThread<SOCKET_TYPE>* _threads; //网络线程池，每个SOCKET有N个NetworkThread进行网络管理
	int32_t _thread_count;
protected:
	SocketManager() : _acceptor(nullptr), _threads(nullptr), _thread_count(1) {	}
	
	virtual NetworkThread<SOCKET_TYPE>* CreateThreads() const = 0;
public:
	virtual ~SocketManager() { }

	virtual bool StartNetwork(boost::asio::io_service& io_service, const std::string& bind_ip, int port, int thread_count)
	{
		try
		{
			_acceptor = new AsyncAcceptor(io_service, bind_ip, port);
		}
		catch (const boost::system::system_error& error)
		{
			std::cout << __func__ << ":Start Server IP:" << bind_ip << " PORT:" << port << " Error:" << error.what() << std::endl;
			return false;
		}

		_thread_count = thread_count;
		
		_threads = CreateThreads();

		for (int32_t i = 0; i < _thread_count; ++i)            
			_threads[i].Start();
		
		return true;
	}

	//释放资源、清理内存
	virtual void StopNetwork()
	{
		_acceptor->Close();
		for (int32_t i = 0; i < _thread_count; ++i)
			_threads[i].Stop();

		Wait();

		_thread_count = 0;
		delete _acceptor;
		_acceptor = nullptr;
		delete[] _threads;
		_threads = nullptr;
	}

	//直到数据发送完成
	virtual void Wait()    
	{        
		if (_thread_count != 0)            
			for (int32_t i = 0; i < _thread_count; ++i)                
				_threads[i].Wait();    
	}

	virtual void OnSocketOpen(tcp::socket&& socket, int32_t thread_index)
	{
		try        
		{            
			std::shared_ptr<SOCKET_TYPE> newSocket = std::make_shared<SOCKET_TYPE>(std::move(socket));            
			newSocket->Start();            
			_threads[thread_index].AddSocket(newSocket);        //网络线程
		}        
		catch (const boost::system::system_error& error)        
		{            
			std::cout << __func__ << " Error:" << error.what() << std::endl;
		}
	}
	
	virtual int32_t GetNetworkThreadCount() const { return _thread_count; }

	int32_t SelectThreadWithMinConnections() const    
	{        
		int32_t min = 0;        
		for (int32_t i = 1; i < _thread_count; ++i)            
			if (_threads[i].GetConnectionCount() < _threads[min].GetConnectionCount())                
				min = i;        
		return min;    
	}    
	
	std::pair<tcp::socket*, int32_t> GetSocketForAccept()    
	{        
		int32_t thread_index = SelectThreadWithMinConnections();        
		return std::make_pair(_threads[thread_index].GetSocketForAccept(), thread_index);    
	}
};

}
