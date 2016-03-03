#ifndef UTILS_DATABUFFER_HPP__
#define UTILS_DATABUFFER_HPP__ 1

#include "refobject.hpp"
#include "ref.hpp"
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
		uint8_t m_data[ 1 ];

		static Ref< DataBuffer > create(
			int length, int capacity, void const* data );

	private:
		DataBuffer( int length, int capacity, void const* data );

	public:
		virtual ~DataBuffer() override;
		virtual void destroy() override;
		DataBuffer() = delete;
		DataBuffer( DataBuffer const& ) = delete;
		DataBuffer& operator=( DataBuffer const& ) = delete;

		R_METHOD() static DataBuffer* pcreate(
			int length, int capacity, void const* data );
		R_METHOD() void* getdata() NOEXCEPT { return m_data; }
		R_METHOD() int getlength() NOEXCEPT { return m_length; }
		R_METHOD() void setlength( int length ) NOEXCEPT { m_length = length; }
		R_METHOD() int getcapacity() NOEXCEPT { return m_capacity; }
	};
}

#endif
