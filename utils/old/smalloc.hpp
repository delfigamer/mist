#ifndef UTILS_SMALLOC_HPP_
#define UTILS_SMALLOC_HPP_ 1

#include <cstdlib>

namespace utils {
	void* page_alloc( int size );
	void page_free( void* ptr, int size );
	void* heap_alloc( int size );
	void heap_free( void* ptr, int size );
	
	class StackAllocator {
	public:
		typedef int Marker;
		static const int DefaultSize = 0x10000;
		
	private:
		void* m_base;
		Marker m_pos;
		int m_size;
		
	public:
		StackAllocator();
		StackAllocator( int size );
		StackAllocator( StackAllocator& ) = delete;
		StackAllocator( StackAllocator&& ) = delete;
		~StackAllocator();
		
		void* alloc( int size );
		Marker marker();
		Marker marker( Marker newmarker );
		void clear();
	};
	
	class PoolAllocator {
	public:
		static const int DefaultCount = 0x100;
		
	private:
		void* m_base;
		int m_itemsize;
		int m_itemcount;
		void* m_link;
		
		static void* getlink( void* ptr );
		static void setlink( void* ptr, void* link );
		void grow();
		
	public:
		PoolAllocator() = delete;
		PoolAllocator( int itemsize );
		PoolAllocator( int itemsize, int itemcount );
		PoolAllocator( PoolAllocator& ) = delete;
		PoolAllocator( PoolAllocator&& ) = delete;
		~PoolAllocator();
		
		void* alloc();
		void free( void* ptr );
	};
	
	template< size_t datasize >
	PoolAllocator* requirepool() {
		static PoolAllocator allocator( datasize );
		return &allocator;
	}
	
	template< typename datatype >
	PoolAllocator* requirepool() {
		return requirepool< sizeof( datatype ) >();
	}
	
	extern StackAllocator GlobalStackAllocator;
}

#endif
