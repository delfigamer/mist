#include <utils/cyclicbuffer.hpp>
#include <cstring>

namespace utils
{
	inline size_t p2align( size_t x )
	{
		if( x == 0 )
		{
			return 1;
		}
		x -= 1;
		size_t r = 1;
		while( x > 0 )
		{
			r <<= 1;
			x >>= 1;
		}
		return r;
	}

	CyclicBuffer::CyclicBuffer()
		: m_data( 0 )
		, m_pos( 0 )
		, m_fill( 0 )
		, m_size( 0 )
	{
	}

	CyclicBuffer::CyclicBuffer( size_t size )
		: CyclicBuffer()
	{
		grow( size );
	}

	CyclicBuffer::~CyclicBuffer()
	{
		delete[] m_data;
	}

	void CyclicBuffer::grow( size_t minsize )
	{
		size_t size = p2align( minsize );
		if( size > m_size )
		{
			uint8_t* newdata = new uint8_t[ size ];
			if( m_pos + m_fill > m_size )
			{
				size_t plen = m_size - m_pos;
				memcpy( newdata, m_data + m_pos, plen );
				memcpy( newdata + plen, m_data, m_fill - plen );
			}
			else
			{
				memcpy( newdata, m_data + m_pos, m_fill );
			}
			delete[] m_data;
			m_data = newdata;
			m_pos = 0;
			m_size = size;
		}
	}

	size_t CyclicBuffer::push_fit( size_t length, void const* buffer )
	{
		if( m_fill + length > m_size )
		{
			length = m_size - m_fill;
		}
		size_t pos = m_pos + m_fill;
		if( pos >= m_size )
		{
			pos -= m_size;
		}
		if( pos + length > m_size )
		{
			size_t plen = m_size - pos;
			memcpy( m_data + pos, buffer, plen );
			memcpy( m_data, ( uint8_t const* )buffer + plen,
				length - plen );
		}
		else
		{
			memcpy( m_data + pos, buffer, length );
		}
		m_fill += length;
		return length;
	}

	void CyclicBuffer::push( size_t length, void const* buffer )
	{
		grow( m_fill + length );
		push_fit( length, buffer );
	}

	size_t CyclicBuffer::pop( size_t length, void* buffer )
	{
		if( length > m_fill )
		{
			length = m_fill;
		}
		if( m_pos + length > m_size )
		{
			size_t plen = m_size - m_pos;
			memcpy( buffer, m_data + m_pos, plen );
			memcpy( ( uint8_t* )buffer + plen, m_data,
				length - plen );
			m_pos = length - plen;
		}
		else
		{
			memcpy( buffer, m_data + m_pos, length );
			m_pos += length;
		}
		m_fill -= length;
		return length;
	}

	bool CyclicBuffer::isempty()
	{
		return m_fill == 0;
	}
}
