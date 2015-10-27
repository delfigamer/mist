#include "databuffer.hpp"
#include "console.hpp"
#include "cbase.hpp"
// #include <algorithm>
// #include <utility>
#include <cstring>

#undef LOG
#define LOG( ... )

namespace utils {
	Ref< DataBuffer > DataBuffer::create(
		int length, int capacity, void const* data )
	{
		LOG( "%#x %#x %s", length, capacity,
			( data && length < 256 ? ( char const* )data : "" ) );
		DataBuffer* e;
		e = ( DataBuffer* )operator new(
			sizeof( DataBuffer ) + capacity );
		try {
			new( e )DataBuffer( length, capacity, data );
			return Ref< DataBuffer >( e, 0 );
		} catch( ... ) {
			operator delete( e );
			throw;
		}
	}

	DataBuffer::DataBuffer(
		int length, int capacity, void const* data )
		: m_length( length )
		, m_capacity( capacity )
	{
		LOG( "%#x %#x %s", length, capacity,
			( chars && length < 256 ? chars : "" ) );
		if( data ) {
			memcpy( m_data, data, length );
		}
	}

	DataBuffer::~DataBuffer()
	{
		LOG( "%#x %#x %s", m_length, m_capacity,
			( m_data && m_length < 256 ?
				( char const* )m_data : "" ) );
	}

	void DataBuffer::destroy()
	{
		LOG( "%#x %#x %s", m_length, m_capacity,
			( m_data && m_length < 256 ?
				( char const* )m_data : "" ) );
		this->~DataBuffer();
		operator delete( this );
	}

	extern "C" {
		DataBuffer* utils_databuffer_new(
			int length, int capacity, void const* data ) noexcept
		{
		CBASE_PROTECT(
			Ref< DataBuffer > si = DataBuffer::create( length, capacity, data );
			si->addref();
			return si;
		)
		}

		void* utils_databuffer_getdata( DataBuffer* db ) noexcept
		{
		CBASE_PROTECT(
			return db->m_data;
		)
		}

		int utils_databuffer_getlength( DataBuffer* db ) noexcept
		{
		CBASE_PROTECT(
			return db->m_length;
		)
		}

		bool utils_databuffer_setlength( DataBuffer* db, int length ) noexcept
		{
		CBASE_PROTECT(
			db->m_length = length;
			return 1;
		)
		}

		int utils_databuffer_getcapacity( DataBuffer* db ) noexcept
		{
		CBASE_PROTECT(
			return db->m_capacity;
		)
		}
	}
}
