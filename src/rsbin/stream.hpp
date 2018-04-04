#pragma once

#include <rsbin/storage.hpp>
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

		[[ r::method, r::name( "startadvance" ) ]] virtual StorageMap* advance(
			unsigned length ) = 0;
		[[ r::method ]] virtual void skip( unsigned length ) = 0;
		[[ r::method ]] virtual uint64_t getposition() = 0;
	};
}

r_emit(<<
	function rsbin_stream:advance(length)
		local mptr = ffi.new('uint8_t*[1]')
		local mlen = ffi.new('unsigned[1]')
		local task = self:startadvance(length)
		if not task then
			return nil, nil, 0
		end
		while not task:poll() do
			coroutine.yield()
		end
		task:getmap(mptr, mlen)
		if mptr[0] == nil or mlen[0] == 0 then
			return task, nil, 0
		else
			return task, mptr[0], mlen[0]
		end
	end
>>)
