#pragma once

#include <common/task.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace rsbin
{
	struct [[ r::struct, r::name( "storagemap" ) ]] StorageMap
	{
		uint8_t* ptr;
		uint32_t length;
	};

	struct [[ r::class, r::name( "maptask" ) ]] MapTask: Task
	{
		MapTask() = default;
		~MapTask() = default;
		[[ r::method ]]
		virtual StorageMap getmap() = 0;
	};

	struct [[ r::class, r::name( "getlimittask" ) ]] GetLimitTask: Task
	{
		GetLimitTask() = default;
		~GetLimitTask() = default;
		[[ r::method ]]
		virtual uint64_t getlimit() = 0;
	};

	class [[ r::class, r::name( "storage" ) ]] Storage: public RefObject
	{
	public:
		Storage() = default;
		~Storage() = default;
		Storage( Storage const& other ) = delete;
		Storage& operator=( Storage const& other ) = delete;

		[[ r::method ]]
		virtual Ref< MapTask > startmap(
			uint64_t offset, uint32_t length,
			bool flagread, bool flagwrite ) = 0;
		[[ r::method ]]
		virtual Ref< GetLimitTask > startgetlimit( uint64_t* plimit ) = 0;
		[[ r::method ]]
		virtual Ref< Task > startsetlimit( uint64_t limit ) = 0;
		[[ r::method ]]
		virtual Ref< Task > startflush() = 0;
		[[ r::method ]]
		virtual Ref< Task > startclose() = 0;
	};
}

r_emit(<<
	function rsbin_storage:map(offset, length, flagread, flagwrite)
		local task = self:startmap(offset, length, flagread, flagwrite)
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

	function rsbin_storage:getlimit()
		local limit = ffi.new('uint64_t[1]')
		local task = self:startgetlimit(limit)
		if task then
			while not task:poll() do
				coroutine.yield()
			end
			local result = task:getlimit()
			task:release()
			return result
		else
			return limit[0]
		end
	end

	function rsbin_storage:setlimit(limit)
		local task = self:startsetlimit(limit)
		if task then
			while not task:poll() do
				coroutine.yield()
			end
			task:release()
		end
	end

	function rsbin_storage:flush()
		local task = self:startflush()
		if task then
			while not task:poll() do
				coroutine.yield()
			end
			task:release()
		end
	end

	function rsbin_storage:close()
		local task = self:startclose()
		if task then
			while not task:poll() do
				coroutine.yield()
			end
			task:release()
		end
	end
>>)
