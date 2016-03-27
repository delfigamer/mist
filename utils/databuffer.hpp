#ifndef UTILS_DATABUFFER_HPP__
#define UTILS_DATABUFFER_HPP__ 1

#include <utils/refobject.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <cinttypes>

namespace utils
{
	R_CLASS( name = databuffer )
	class DataBuffer: public RefObject
	{
	public:
		int m_length;
		int const m_capacity;
		uint8_t m_data[];

		R_METHOD( name = create ) static DataBuffer* newinstance(
			int length, int capacity, void const* data )
		{
			DataBuffer* db;
			db = ( DataBuffer* )operator new(
				sizeof( DataBuffer ) + capacity );
			try
			{
				new( db )DataBuffer( length, capacity, data );
				return db;
			}
			catch( ... )
			{
				operator delete( db );
				throw;
			}
		}

		static Ref< DataBuffer > create(
			int length, int capacity, void const* data )
		{
			return Ref< DataBuffer >( newinstance( length, capacity, data ), 0 );
		}

	private:
		DataBuffer( int length, int capacity, void const* data )
			: m_length( length )
			, m_capacity( capacity )
		{
			if( data )
			{
				memcpy( m_data, data, length );
			}
		}

	public:
		virtual ~DataBuffer() override
		{
		}

		virtual void destroy() override
		{
			this->~DataBuffer();
			operator delete( this );
		}

		DataBuffer( DataBuffer const& ) = delete;
		DataBuffer& operator=( DataBuffer const& ) = delete;

		R_METHOD() void* getdata() NOEXCEPT { return m_data; }
		R_METHOD() int getlength() NOEXCEPT { return m_length; }
		R_METHOD() void setlength( int length ) NOEXCEPT { m_length = length; }
		R_METHOD() int getcapacity() NOEXCEPT { return m_capacity; }
	};
}

#endif
