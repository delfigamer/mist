#ifndef UTILS_CLOSEDNAMETABLE_HPP_
#define UTILS_CLOSEDNAMETABLE_HPP_ 1

#include "name.hpp"
#include "smalloc.hpp"
#include <cstdlib>

namespace utils {
	class ClosedNameTable {
	private:
		struct Bucket {
			unsigned int m_hash;
			void* m_data;
			
			Bucket();
		};
		
		Bucket* m_data;
		int m_capacity;
		
		static Bucket* newblock( int capacity = 256 );
		Bucket* find( unsigned int hash ) const;
		
	public:
		ClosedNameTable( int capacity );
		ClosedNameTable( ClosedNameTable& ) = delete;
		ClosedNameTable( ClosedNameTable&& ) = delete;
		~ClosedNameTable();
		
		void const*& operator[]( const Name& name ) const;
		void*& operator[]( const Name& name );
	};
}

#endif
