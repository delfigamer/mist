#include <utils/databuffer.hpp>
#include <utils/cbase.hpp>
#include <utils/console.hpp>
#include <cstring>

namespace utils {
	Ref< DataBuffer > DataBuffer::create(
		int length, int capacity, void const* data )
	{
		DataBuffer* e;
		e = ( DataBuffer* )operator new(
			sizeof( DataBuffer ) + capacity );
		try
		{
			new( e )DataBuffer( length, capacity, data );
			return Ref< DataBuffer >( e, 0 );
		}
		catch( ... )
		{
			operator delete( e );
			throw;
		}
	}

	DataBuffer::DataBuffer(
		int length, int capacity, void const* data )
		: m_length( length )
		, m_capacity( capacity )
	{
		if( data )
		{
			memcpy( m_data, data, length );
		}
	}

	DataBuffer::~DataBuffer()
	{
	}

	void DataBuffer::destroy()
	{
		this->~DataBuffer();
		operator delete( this );
	}

	DataBuffer* DataBuffer::pcreate(
		int length, int capacity, void const* data )
	{
		Ref< DataBuffer > db = create( length, capacity, data );
		db->addref();
		return db;
	}
}
