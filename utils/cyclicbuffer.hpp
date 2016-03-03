#ifndef UTILS_CYCLICBUFFER_HPP__
#define UTILS_CYCLICBUFFER_HPP__ 1

#include <common.hpp>
#include <cinttypes>

namespace utils
{
	class CyclicBuffer
	{
	private:
		uint8_t* m_data;
		int m_pos;
		int m_fill;
		int m_size;

	public:
		CyclicBuffer();
		CyclicBuffer( int size );
		~CyclicBuffer();
		CyclicBuffer( CyclicBuffer const& ) = delete;
		CyclicBuffer& operator=( CyclicBuffer const& ) = delete;

		void grow( int minsize );
		int push_fit( int length, void const* buffer );
		void push( int length, void const* buffer );
		int pop( int length, void* buffer );
		bool isempty();
	};
};

#endif
