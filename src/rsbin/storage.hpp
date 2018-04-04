#pragma once

#include <common/task.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace rsbin
{
	struct [[ r::class, r::name( "storagemap" ) ]] StorageMap: Task
	{
		StorageMap() = default;
		~StorageMap() = default;
		[[ r::method ]] virtual void getmap(
			uint8_t** pmapptr [[ r::required ]],
			unsigned* pmaplength [[ r::required ]] ) = 0;
	};

	struct [[ r::class, r::name( "getlimittask" ) ]] GetLimitTask: Task
	{
		GetLimitTask() = default;
		~GetLimitTask() = default;
		[[ r::method ]] virtual uint64_t getlimit() = 0;
	};

	class [[ r::class, r::name( "storage" ) ]] Storage: public RefObject
	{
	public:
		Storage() = default;
		~Storage() = default;
		Storage( Storage const& other ) = delete;
		Storage& operator=( Storage const& other ) = delete;

		[[ r::method, r::name( "startmap" ) ]] virtual StorageMap* map(
			uint64_t offset, uint32_t length,
			bool flagread, bool flagwrite ) = 0;
		[[ r::method, r::name( "startgetlimit" ) ]]
			virtual GetLimitTask* getlimit(
			uint64_t* plimit [[ r::required ]] ) = 0;
		[[ r::method, r::name( "startsetlimit" ) ]] virtual Task* setlimit(
			uint64_t limit ) = 0;
		[[ r::method, r::name( "startflush" ) ]] virtual Task* flush() = 0;
		[[ r::method, r::name( "startclose" ) ]] virtual Task* close() = 0;
	};
}

r_emit(<<
	function rsbin_storage:map(offset, length, flagread, flagwrite)
		local mptr = ffi.new('uint8_t*[1]')
		local mlen = ffi.new('unsigned[1]')
		local task = self:startmap(offset, length, flagread, flagwrite)
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

	function rsbin_storage:getlimit()
		local limit = ffi.new('uint64_t[1]')
		local task = self:startgetlimit(limit)
		if task then
			while not task:poll() do
				coroutine.yield()
			end
			return task:getlimit()
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
		end
	end

	function rsbin_storage:flush()
		local task = self:startflush()
		if task then
			while not task:poll() do
				coroutine.yield()
			end
		end
	end

	function rsbin_storage:close()
		local task = self:startclose()
		if task then
			while not task:poll() do
				coroutine.yield()
			end
		end
	end
>>)
