#ifndef UTILS_NAMETABLE_HPP_
#define UTILS_NAMETABLE_HPP_ 1

#include "name.hpp"
#include "smalloc.hpp"
#include <cstdlib>

namespace utils {
	class NameTable {
	private:
		typedef void* Block[256];
		static PoolAllocator* m_blockallocator;
		mutable Block* m_data;
		
		static Block* newblock();
		void** find( unsigned int hash ) const;
		
	public:
		NameTable();
		NameTable( NameTable& ) = delete;
		NameTable( NameTable&& ) = delete;
		~NameTable();
		
		void const*& operator[]( const Name& name ) const;
		void*& operator[]( const Name& name );
	};
}

#endif
