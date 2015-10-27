#ifndef RSBIN_PACKAGEHEAP_HPP__
#define RSBIN_PACKAGEHEAP_HPP__ 1

#include "io.hpp"
#include <utils/taskqueuebase.hpp>
#include <map>

namespace rsbin {
	struct PackageHeader;
	
	class PackageHeap {
	public:
		friend struct HeapLoadTask;
		
	public:
		struct chunk_t {
			uint64_t offset;
			uint64_t length;
			chunk_t* next;
			
			chunk_t() = default;
			chunk_t( uint64_t _offset, uint64_t _length, chunk_t* _next );
		};
		
	private:
		typedef std::map< uint64_t, chunk_t* > map_t;
		
	private:
		map_t m_chunks;
		chunk_t* m_emptyspace;
		uint64_t m_boundary;
		bool m_ready;
		
		chunk_t* alloc( uint64_t length );
		chunk_t* normchain( chunk_t const* first );
		static void read_cb( void* ud, int result, char const* error );
		
	public:
		PackageHeap();
		PackageHeap( uint64_t firstlength );
		~PackageHeap();
		PackageHeap( PackageHeap const& other ) = delete;
		PackageHeap( PackageHeap&& other ) = delete;
		PackageHeap& operator=( PackageHeap const& other ) = delete;
		PackageHeap& operator=( PackageHeap&& other ) = delete;
		
		chunk_t const* find( uint64_t offset );
		uint64_t extendchain( chunk_t const** first, uint64_t length );
		void freechain( chunk_t const* first );
		
		utils::TaskBase* startload( Io* io, PackageHeader const* header, int taskpriority, utils::TaskQueueBase* queue );
		void write( Io* io, uint64_t offset );
		uint32_t getlength();
		uint64_t getemptyspace();
		bool isready();
	};
}

#endif
