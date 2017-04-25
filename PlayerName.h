#pragma once

#include <memory>
#include <functional>
#include <fstream>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include "P_Header.h"
#include "CommonUtil.h"

namespace Adoter
{
namespace pb = google::protobuf;

class PlayerName : public std::enable_shared_from_this<PlayerName>   
{
	Asset::FamilyName _family_name;
	Asset::WomanName _woman_name;
	Asset::ManName _man_name;
public:
	static PlayerName& Instance()
	{
		static PlayerName _instance;
		return _instance;
	}

	bool Load()
	{
		std::ifstream fi("FamilyName", std::ifstream::in);
		pb::io::IstreamInputStream fa_iis(&fi);
		auto result = pb::TextFormat::Parse(&fa_iis, &_family_name);
		fi.close();

		if (!result) return false;
		
		std::ifstream wi("WomanName", std::ifstream::in);
		pb::io::IstreamInputStream woman_iis(&wi);
		result = pb::TextFormat::Parse(&fa_iis, &_woman_name);
		wi.close();
		
		std::ifstream mi("ManName", std::ifstream::in);
		pb::io::IstreamInputStream man_iis(&mi);
		result = pb::TextFormat::Parse(&fa_iis, &_man_name);
		mi.close();
		
		if (!result) return false;

		_family_name.PrintDebugString();
		_man_name.PrintDebugString();
		_woman_name.PrintDebugString();

		return true;
	}

	std::string Get()
	{
		auto family_index = CommonUtil::Random(0, _family_name.name().size() - 1);
		auto woman_index = CommonUtil::Random(0, _woman_name.name().size() - 1);
		auto man_index = CommonUtil::Random(0, _man_name.name().size() - 1);

		std::string name = woman_index > man_index ? _woman_name.name(woman_index) : _man_name.name(man_index);

		return _family_name.name(family_index) + name; 
	}
};

#define NameInstance PlayerName::Instance()

}
