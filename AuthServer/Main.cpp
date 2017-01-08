#include "AuthServer.h"
#include "Session.h"
#include <iostream>

using namespace Adoter;

static boost::asio::io_service* _io_service;

void SignalHandler(const boost::system::error_code& error, int number)
{    
	//if (!error)        
	//	_io_service->stop();
}

int main(int argc, const char* argv[])
{
	std::cout << "Service starting..." << std::endl;
	
	try 
	{
		_io_service = new boost::asio::io_service();
		
		//认证服务器，获取登录TOCKEN	
		if (!AuthInstance.Start(*_io_service))
		{
			std::cout << "Service start failed..." << std::endl;
			delete _io_service;
			return 1;
		}
		std::cout << "Service start success..." << std::endl;
		//boost::asio::signal_set signals(*_io_service, SIGINT, SIGTERM);
		//signals.async_wait(SignalHandler);
		
		SessionInstance.StartNetwork(*_io_service, "0.0.0.0", 60000);

		_io_service->run();		
		//signals.cancel();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	return 0;
}
