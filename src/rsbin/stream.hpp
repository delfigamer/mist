#pragma once

#include <rsbin/storage.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace rsbin
{
	class [[ r::class, r::name( "stream" ) ]] Stream: public RefObject
	{
	public:
		Stream() = default;
		~Stream() = default;
		Stream( Stream const& other ) = delete;
		Stream& operator=( Stream const& other ) = delete;

		[[ r::method ]] virtual Ref< MapTask > startadvance(
			uint32_t length ) = 0;
		[[ r::method ]] virtual void skip( uint32_t length ) = 0;
		[[ r::method ]] virtual uint64_t getposition() = 0;
	};
}

r_emit(<<
	function rsbin_stream:advance(length)
		local task = self:startadvance(length)
		if not task then
			return nil, nil, 0
		end
		while not task:poll() do
			coroutine.yield()
		end
		local map = task:getmap()
		if map.ptr == nil or map.length == 0 then
			return task, nil, 0
		else
			return task, map.ptr, map.length
		end
	end
>>)
