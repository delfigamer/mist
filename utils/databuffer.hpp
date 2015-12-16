#ifndef UTILS_DATABUFFER_HPP__
#define UTILS_DATABUFFER_HPP__ 1

#include "refobject.hpp"
#include "ref.hpp"
#include <cinttypes>

namespace utils
{
	class DataBuffer: public RefObject
	{
	public:
		int m_length;
		int const m_capacity;
		uint8_t m_data[];

		static Ref< DataBuffer > create(
			int length, int capacity, void const* data );

	private:
		DataBuffer( int length, int capacity, void const* data );

	public:
		virtual ~DataBuffer() override;
		virtual void destroy() override;
		DataBuffer() = delete;
		DataBuffer( DataBuffer const& other ) = delete;
		DataBuffer( DataBuffer&& other ) = delete;
		DataBuffer& operator=( DataBuffer const& other ) = delete;
		DataBuffer& operator=( DataBuffer&& other ) = delete;
	};

	DataBuffer* utils_databuffer_new(
		int length, int capacity, void const* data ) noexcept;
	void* utils_databuffer_getdata( DataBuffer* db ) noexcept;
	int utils_databuffer_getlength( DataBuffer* db ) noexcept;
	bool utils_databuffer_setlength( DataBuffer* db, int length ) noexcept;
	int utils_databuffer_getcapacity( DataBuffer* db ) noexcept;
}

#endif
