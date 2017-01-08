/*
 * C++版本：C++11
 *
 * 说明：倾向于用标准类库中接口
 *
 */

#include <thread>
#include <vector>
#include <memory>
#include <functional>
#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include "Timer.h"
#include "World.h"
#include "WorldSession.h"

const int const_world_sleep = 50;

/*
 * 游戏逻辑处理接口
 *
 * */

using namespace Adoter;

boost::asio::io_service _io_service;
std::shared_ptr<boost::asio::io_service::work> _io_service_work;

void SignalHandler(const boost::system::error_code& error, int)
{    
	//if (!error) World::StopNow(SHUTDOWN_EXIT_CODE);
}

void WorldUpdateLoop()
{
	int32_t curr_time = 0, prev_sleep_time = 0;
	
	int32_t prev_time = GetTime();
	
	while (!WorldInstance.IsStopped())
	{
		curr_time = GetTime();
		
		int32_t diff = GetTimeDiff(prev_time, curr_time);
		
		WorldInstance.Update(diff);        
		
		prev_time = curr_time;
		
		if (diff <= const_world_sleep + prev_sleep_time) //50MS
		{            
			prev_sleep_time = const_world_sleep + prev_sleep_time - diff;            
			std::this_thread::sleep_for(std::chrono::milliseconds(prev_sleep_time));        
			//std::cout << __func__ << ":" << prev_sleep_time << std::endl;
		}        
		else    
		{	
			prev_sleep_time = 0;
		}
	}
}

void ShutdownThreadPool(std::vector<std::shared_ptr<std::thread>>& threads)
{    
	_io_service.stop();    
	
	for (auto& thread : threads)    
	{        
		thread->join();    
	}
}

/*
 *
 * 函数入口
 *
 * */
int main(int argc, const char* argv[])
{
		/*	
		Asset::Item_Potion* message = new Asset::Item_Potion();
		message->mutable_item_common_prop()->set_quality(5);
		Item* item = new Item(message);	
		bool re = item->CanUse();
		Item* item_potion = new Item_Potion(message);	
		*/
	std::cout << "Service starting..." << std::endl;
	try 
	{
		//世界初始化，涵盖所有....
		if (!WorldInstance.Load()) return 1;

		//网络初始化
		_io_service_work = std::make_shared<boost::asio::io_service::work>(_io_service);

		int _thread_nums = 5;
		std::vector<std::shared_ptr<std::thread>> _threads;	
		for (int i = 0; i < _thread_nums; ++i)
		{
			std::shared_ptr<std::thread> pthread(new std::thread(boost::bind(&boost::asio::io_service::run, &_io_service)));	//IO <-> Multi Threads
			_threads.push_back(pthread);
		}

		//boost::asio::signal_set signals(_io_service, SIGINT, SIGTERM);
		//signals.async_wait(SignalHandler);

		WorldSessionInstance.StartNetwork(_io_service, "0.0.0.0", 50000, 5);

		//世界循环
		WorldUpdateLoop();
	
		std::cout << "Service stop..." << std::endl;

		ShutdownThreadPool(_threads);
	}
	catch (std::exception& e)
	{
		std::cerr << __func__ << ":Exception: " << e.what() << std::endl;
	}
	
	std::cout << "Service stoped." << std::endl;
	return 0;
}
