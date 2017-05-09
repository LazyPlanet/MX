#pragma once

#include <memory>
#include <functional>
#include <fstream>

#include <google/protobuf/text_format.h>
#include <google/protobuf/io/zero_copy_stream_impl.h>

#include <boost/algorithm/string.hpp>

#include <pbjson.hpp>

#include "P_Header.h"
#include "CommonUtil.h"
#include "MXLog.h"

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
		//加载姓
		std::ifstream fi("FamilyName", std::ifstream::in);
		pb::io::IstreamInputStream fa_iis(&fi);
		auto result = pb::TextFormat::Parse(&fa_iis, &_family_name);
		fi.close();

		if (!result) return false;
		
		//女子名字
		std::ifstream wi("WomanName", std::ifstream::in);
		pb::io::IstreamInputStream woman_iis(&wi);
		result = pb::TextFormat::Parse(&woman_iis, &_woman_name);
		wi.close();
		
		//男子名字
		std::ifstream mi("ManName", std::ifstream::in);
		pb::io::IstreamInputStream man_iis(&mi);
		result = pb::TextFormat::Parse(&man_iis, &_man_name);
		mi.close();
		
		if (!result) return false;

		DEBUG("%s:Load FamilyName size:%d, WomanName size:%d, ManName size:%d\n", __func__, _family_name.name().size(), _woman_name.name().size(), _man_name.name().size());

		return true;
	}

	std::string Get()
	{
		auto family_index = CommonUtil::Random(0, _family_name.name().size() - 1);
		auto woman_index = CommonUtil::Random(0, _woman_name.name().size() - 1);
		auto man_index = CommonUtil::Random(0, _man_name.name().size() - 1);

		std::string family_name = _family_name.name(family_index);
		std::string name = woman_index > man_index ? _woman_name.name(woman_index) : _man_name.name(man_index);

		boost::trim(family_name); //姓
		boost::trim(name); //名字

		std::string full_name = family_name + name; //全名

		return full_name; 
	}
};

#define NameInstance PlayerName::Instance()

}
