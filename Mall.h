#pragma once

#include <memory>
#include <functional>

namespace Adoter
{
namespace pb = google::protobuf;

class Mall : public std::enable_shared_from_this<Mall> 
{

};

#define ActivityInstance Activity::Instance()

}


