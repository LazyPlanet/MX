#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <iostream>
#include <queue>
#include <unordered_map>

#include <boost/asio.hpp>

#include "Protocol.h"
#include "AsyncAcceptor.h"
#include "NetThread.h"
#include "CommonUtil.h"

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
		_closed = true;

		boost::system::error_code error;
		_socket.close(error);
	}
	
	virtual bool Update() 
	{
		if (_closed) return false;

		//发送可以放到消息队列里面处理
		if (_is_async || (_write_queue.empty() && !_closing)) return true;

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

	virtual bool IsConnect() { return _socket.is_open(); }

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

		/*
		for (int i = 0; i < size; ++i)
		{
			std::cout << (int)buff[i] << " ";
		}
		*/
	}

	virtual void OnSend(const boost::system::error_code& error, std::size_t bytes_transferred)
	{
		std::cout << __func__ << ":bytes_transferred:" << bytes_transferred << " has error:" << error << std::endl;
	}

	void QueuePacket(std::string content)
	{
		_write_queue.push(content);
	}

	bool IsOpen() const { return !_closed && !_closing; }

	void CloseSocket()
	{
		if (_closed.exchange(true)) return;

		boost::system::error_code shutdown_error;
		_socket.shutdown(boost::asio::socket_base::shutdown_send, shutdown_error);

		OnClose();
	}

	void DelayedCloseSocket() { _closing = true; }

protected:
	virtual void OnClose() { }

	bool AsyncProcessQueue()
	{
		if (_is_async) return false;

		_is_async = true;

		_socket.async_write_some(boost::asio::null_buffers(), std::bind(&Socket<T, S>::WriteHandlerWrapper,
					this->shared_from_this(), std::placeholders::_1, std::placeholders::_2));

		return true;
	}

	void SetNoDelay(bool enable)
	{
		boost::system::error_code error;
		_socket.set_option(tcp::no_delay(enable), error);

		if (error) { }
	}

	S& UnderlyingStream() { return _socket; }

	void WriteHandlerWrapper(boost::system::error_code /*error*/, std::size_t /*transfered*/)
	{
		_is_async = false;

		HandleQueue();
	}

	bool HandleQueue()
	{
		if (_write_queue.empty()) return false;

		auto content = _write_queue.front();

		std::size_t bytes_to_send = content.size();

		boost::system::error_code error;
		std::size_t bytes_sent = _socket.write_some(boost::asio::buffer(content.c_str(), bytes_to_send), error);

		if (error)
		{
			if (error == boost::asio::error::would_block || error == boost::asio::error::try_again) return AsyncProcessQueue();

			_write_queue.pop();

			if (_closing && _write_queue.empty()) CloseSocket();

			return false;
		}
		else if (bytes_sent == 0)
		{
			_write_queue.pop();

			if (_closing && _write_queue.empty()) CloseSocket();

			return false;
		}
		else if (bytes_sent < bytes_to_send) 
		{
			return AsyncProcessQueue();
		}

		_write_queue.pop();

		if (_closing && _write_queue.empty()) CloseSocket();

		return !_write_queue.empty();
	}

protected:

	std::queue<std::string> _write_queue;

	std::atomic<bool> _closed;    
	std::atomic<bool> _closing;
	//接收缓存
	std::array<unsigned char, 4096> _buffer;

	bool _is_async = false; //异步发送
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
