#include "Session.h"
#include "Auth.pb.h"

namespace Adoter
{

Session::Session(boost::asio::ip::tcp::socket&& socket) : Socket(std::move(socket))
{
}

void Session::ProtocolHandler(const boost::system::error_code error, const std::size_t bytes_transferred)
{
	std::cout << "--------------------------" << std::endl;

	try
	{
		if (error)
		{
			std::cout << "Remote client disconnect, RemoteIp:" << _socket.remote_endpoint().address() << std::endl;
			return;
		}
		else
		{
			{
				Auth::Login login;
				login.set_type_t(Auth::META_TYPE_C2S_LOGIN);
				login.mutable_account()->set_username("zhenyunheng@zulong.com");
				login.mutable_account()->set_password("123456");

				Auth::Meta smeta;
				smeta.set_type_t(Auth::META_TYPE_C2S_LOGIN);
				smeta.set_stuff(login.SerializeAsString());
	
				std::string content = smeta.SerializeAsString();
				AsyncSend(content.c_str(), content.size());
			}
			Auth::Meta meta;
			bool result = meta.ParseFromArray(_buffer.data(), bytes_transferred);
			if (!result) 
			{
				std::cout << __func__ << ":Meta parse error." << std::endl;	
				return;		//非法协议
			}
			
			/*
			google::protobuf::Message* message = ProtocolInstance.GetMessage(meta.type_t());	
			
			result = message->ParseFromString(meta.stuff());
			if (!result) 
			{
				std::cout << __func__ << ":Messge parse error." << std::endl;	
				return;		//非法协议
			}
			*/
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
		return;
	}
	//递归持续接收	
	AsyncReceiveWithCallback(&Session::ProtocolHandler);
}

void Session::Start()
{
	AsyncReceiveWithCallback(&Session::ProtocolHandler);
}

NetworkThread<Session>* SessionManager::CreateThreads() const
{    
	return new NetworkThread<Session>[GetNetworkThreadCount()];
}

void SessionManager::OnSocketAccept(tcp::socket&& socket, int32_t thread_index)
{    
	SessionInstance.OnSocketOpen(std::forward<tcp::socket>(socket), thread_index);
}

bool SessionManager::StartNetwork(boost::asio::io_service& io_service, const std::string& bind_ip, int32_t port, int thread_count)
{
	if (!SuperSocketManager::StartNetwork(io_service, bind_ip, port, thread_count)) return false;
	_acceptor->SetSocketFactory(std::bind(&SuperSocketManager::GetSocketForAccept, this));    
	_acceptor->AsyncAcceptWithCallback<&OnSocketAccept>();    
	return true;
}

}
