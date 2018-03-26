#pragma once

#include <common/refobject.hpp>
#include <common.hpp>

struct [[ r::class, r::name( "task" ) ]] Task: RefObject
{
	Task() = default;
	~Task() = default;
	Task( Task const& ) = delete;
	Task& operator=( Task const& ) = delete;
	[[ r::method ]] virtual bool poll() = 0;
};
