#pragma once

#include <common.hpp>

namespace utils
{
	class CyclicBuffer
	{
	private:
		uint8_t* m_data;
		size_t m_pos;
		size_t m_fill;
		size_t m_size;

	public:
		CyclicBuffer();
		CyclicBuffer( size_t size );
		~CyclicBuffer();
		CyclicBuffer( CyclicBuffer const& ) = delete;
		CyclicBuffer& operator=( CyclicBuffer const& ) = delete;

		void grow( size_t minsize );
		size_t push_fit( size_t length, void const* buffer );
		void push( size_t length, void const* buffer );
		size_t pop( size_t length, void* buffer );
		bool isempty();
	};
};

