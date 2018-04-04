#pragma once

#include <rsbin/storage.hpp>
#include <common/flaglock.hpp>
#include <common.hpp>
#include <mutex>
#include <vector>
#include <memory>

namespace rsbin
{
	class [[ r::class, r::name( "memorystorage" ) ]] MemoryStorage:
		public Storage
	{
	private:
		typedef FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		class page_t
		{
		private:
			uint8_t* m_buffer;

		public:
			page_t();
			page_t( page_t&& other );
			~page_t();
			page_t& operator=( page_t&& other );
			uint8_t* get( bool flagwrite );
		};

	private:
		mutex_t m_mutex;
		std::vector< page_t > m_pages;
		uint64_t m_limit;
		int m_mapcount;

	public:
		[[ r::method ]] MemoryStorage();
		~MemoryStorage();

		virtual StorageMap* map(
			uint64_t offset, uint32_t length,
			bool flagread, bool flagwrite ) override;
		virtual GetLimitTask* getlimit( uint64_t* plimit ) override;
		virtual Task* setlimit( uint64_t limit ) override;
		virtual Task* flush() override;
		virtual Task* close() override;

	public:
		friend class MemoryStorageMap;
	};
}
