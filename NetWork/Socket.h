#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <queue>
#include <unordered_map>
#include <sstream>

#include <boost/asio.hpp>
#include <spdlog/spdlog.h>

#include "AsyncAcceptor.h"
#include "NetThread.h"

namespace Adoter
{

template<class T, class S = boost::asio::ip::tcp::socket>
class Socket : public std::enable_shared_from_this<T>
{
public:
	S _socket; 
public:
	explicit Socket(boost::asio::ip::tcp::socket&& socket) : _socket(std::move(socket)), _closed(false), _closing(false) { }
	virtual ~Socket() { }
	
	virtual bool Update() 
	{
		if (_closed) 
		{
			spdlog::get("console")->error("{0} Line:{1} client_id:{2} has closed.", __func__, __LINE__, _socket.remote_endpoint().address().to_string().c_str());
			return false;
		}

		//发送可以放到消息队列里面处理
		if (_is_writing_async || (_write_queue.empty() && !_closing)) 
		{
			//spdlog::get("console")->error("{0} Line:{1} client_id:{2} has closed.", __func__, __LINE__, _socket.remote_endpoint().address().to_string().c_str());
			return true;
		}

		for (; HandleQueue(); ) {}

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

	bool AsyncProcessQueue()    
	{
		if (_is_writing_async) return false;
		_is_writing_async = true;

		_socket.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T, S>::WriteHandlerWrapper, 
					this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));
		return false;
	}

	void WriteHandlerWrapper(boost::system::error_code /*error*/, std::size_t /*bytes_transferred*/)
	{
		_is_writing_async = false;
		HandleQueue();
	}

	void EnterQueue(std::string&& meta)    
	{        
		auto content = std::move(meta);
		//////////////////////////////////////////////////////数据包头
		unsigned short body_size = content.size();
		unsigned char header[2] = { 0 };

		header[0] = (body_size >> 8) & 0xff;
		header[1] = body_size & 0xff;
		//////////////////////////////////////////////////////包数据体
		auto body = content.c_str(); 

		//////////////////////////////////////////////////////数据整理发送
		char buffer[4096] = { 0 }; //发送数据缓存
		for (int i = 0; i < 2; ++i) buffer[i] = header[i];
		for (int i = 0; i < body_size; ++i) buffer[i + 2] = body[i];

		//std::lock_guard<std::mutex> lock(_mutex);
		_write_queue.push(std::string(buffer, body_size + 2));
	}

	bool HandleQueue()
	{
		//std::lock_guard<std::mutex> lock(_mutex);

		if (!_socket.is_open()) 
		{
			spdlog::get("console")->error("{0} Line:{1} has disconnected from server.", __func__, __LINE__);
			return false;
		}

		if (_write_queue.empty()) return false;
		std::string& meta = _write_queue.front();  //其实是META数据

		std::size_t bytes_to_send = meta.size();

		boost::system::error_code error;
		std::size_t bytes_sent = _socket.write_some(boost::asio::buffer(meta.c_str(), bytes_to_send), error);

		if (error == boost::asio::error::would_block || error == boost::asio::error::try_again)
		{
			spdlog::get("console")->error("{0} Line:{1} bytes_to_send:{2} bytes_sent:{3}", __func__, __LINE__, bytes_to_send, bytes_sent);
			return AsyncProcessQueue();

			_write_queue.pop();

			if (_closing && _write_queue.empty()) Close();

			return false;
		}
		else if (bytes_sent == 0)
		{
			spdlog::get("console")->error("{0} Line:{1} bytes_to_send:{2} bytes_sent:{3}", __func__, __LINE__, bytes_to_send, bytes_sent);

			_write_queue.pop();

			if (_closing && _write_queue.empty()) Close();

			return false;
		}
		else if (bytes_sent < bytes_to_send) //一般不会出现这个情况，重新发送，记个ERROR
		{
			spdlog::get("console")->error("{0} Line:{1} bytes_to_send:{2} bytes_sent:{3}", __func__, __LINE__, bytes_to_send, bytes_sent);
			return AsyncProcessQueue();
		}

		spdlog::get("console")->debug("{0} Line:{1} bytes_to_send:{2} bytes_sent:{3}", __func__, __LINE__, bytes_to_send, bytes_sent);
		_write_queue.pop();

		if (_closing && _write_queue.empty()) Close();

		return !_write_queue.empty();
	}

protected:
	virtual void OnClose() { }
protected:
	std::atomic<bool> _closed;    
	std::atomic<bool> _closing;
	bool _is_writing_async = false;
	//std::mutex _mutex;
	//接收缓存
	std::array<unsigned char, 4096> _buffer;
	//发送队列
	std::queue<std::string> _write_queue;
};

template <class SOCKET_TYPE> //各种类型的SOCKET，比如Session-其本质也要继承至Socket
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
		
		_threads = CreateThreads(); //继承类所实现的Socket

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
			spdlog::get("console")->critical("{0} Line:{1} error:{2}", __func__, __LINE__, error.what());
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
