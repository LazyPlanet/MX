#pragma once

#include <hiredis.h>
#include <string>
#include <iostream>

#include <Player.h>

/*
 * 数据库管理
 *
 * 所有表名称都要小写
 *
 *
 * */

namespace Adoter
{

class Redis 
{
private:
	std::string _hostname = "127.0.0.1";
	int32_t _port = 6379;
	struct timeval _timeout = {1, 500000}; //1.5秒 

	redisContext* _client;
public:
	~Redis() { redisFree(_client); }

	Redis() 
	{ 
		_client = redisConnectWithTimeout(_hostname.c_str(), _port, _timeout);
	}

	Redis(std::string& hostname, int32_t port) : _hostname(hostname), _port(port)
	{ 
		_client = redisConnectWithTimeout(hostname.c_str(), port, _timeout);
	}

	redisContext* GetClient() { return _client; }

	redisReply* ExcuteCommand(std::string command)
	{
		redisReply* reply = (redisReply*)redisCommand(_client, command.c_str());
		return reply;
	}

	void GetEntity(int64_t entity_id)
	{
	}
	
	int64_t CreatePlayer()
	{
		redisReply* reply = (redisReply*)redisCommand(_client, "Incr player_counter");
		if (!reply) return 0;

		if (reply->type != REDIS_REPLY_INTEGER) return 0;
		
		int64_t player_id = reply->integer;
		freeReplyObject(reply);
		
		std::cout << __func__ << " success, player_id:" << player_id << std::endl;

		return player_id;
	}

	std::string GetPlayer(int64_t player_id)
	{
		std::string value = "";

		std::string command = "Get player:" + std::to_string(player_id);
		redisReply* reply = (redisReply*)redisCommand(_client, command.c_str());

		if (!reply) return value;

		if (reply->type == REDIS_REPLY_NIL) return value;
		
		if (reply->type != REDIS_REPLY_STRING) return value;

		value = reply->str;
		freeReplyObject(reply);

		std::cout << __func__ << " success, player_id:" << player_id << std::endl;

		return value;
	}
	
	void SavePlayer(int64_t player_id, std::string& stuff)
	{
		std::string key = "player:" + std::to_string(player_id);

		redisReply* reply = (redisReply*)redisCommand(_client, "Set %s %s", key.c_str(), stuff.c_str());
		if (!reply) return;
		
		if (reply->type != REDIS_REPLY_STRING) return;

		freeReplyObject(reply);
		
		std::cout << __func__ << " success, player_id:" << player_id << std::endl;
	}
	
	int64_t CreateRoom()
	{
		redisReply* reply = (redisReply*)redisCommand(_client, "Incr room_counter");
		if (!reply) return 0;

		if (reply->type != REDIS_REPLY_INTEGER) return 0;
		
		int64_t room_id = reply->integer;
		freeReplyObject(reply);
		
		std::cout << __func__ << " success, room_id:" << room_id << std::endl;

		return room_id;
	}

	std::string GetUser(std::string username)
	{
		std::string value = "";

		std::string command = "Get user:" + username;
		redisReply* reply = (redisReply*)redisCommand(_client, command.c_str());

		if (!reply) return value;

		if (reply->type == REDIS_REPLY_NIL) return value;
		
		if (reply->type != REDIS_REPLY_STRING) return value;

		value = reply->str;
		freeReplyObject(reply);

		std::cout << __func__ << " success, username:" << username << std::endl;

		return value;
	}
	
	void SaveUser(std::string username, std::string& stuff)
	{
		std::string key = "user:" + username;

		redisReply* reply = (redisReply*)redisCommand(_client, "Set %s %s", key.c_str(), stuff.c_str());
		if (!reply) return;
		
		if (reply->type != REDIS_REPLY_STRING) return;

		freeReplyObject(reply);
		
		std::cout << __func__ << " success, user:" << username << std::endl;
	}
};

}
