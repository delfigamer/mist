#pragma once

#include <common/taskstate.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace rsbin
{
	R_CLASS( name = inputstream )
	class InputStream: public RefObject
	{
	public:
		InputStream() = default;
		~InputStream() = default;
		InputStream( InputStream const& other ) = delete;
		InputStream& operator=( InputStream const& other ) = delete;

/*
	Reads data from the stream into the given buffer.
	'length' - the requested number of bytes to read.
	'buffer' must be at least 'length' bytes long.
	Returns the number of bytes read, which may be less than 'length'.
	When the end of stream is encountered, reads 0 bytes.
*/
		virtual R_METHOD() int read( int length, void* buffer ) = 0;
		virtual R_METHOD() uint64_t getoffset() = 0;
	};

	R_CLASS( name = outputstream )
	class OutputStream: public RefObject
	{
	public:
		OutputStream() = default;
		~OutputStream() = default;
		OutputStream( OutputStream const& other ) = delete;
		OutputStream& operator=( OutputStream const& other ) = delete;

/*
	Writes data from the given buffer into the stream.
	'length' - the requested number of bytes to write.
	'buffer' must be at least 'length' bytes long.
	Returns the number of bytes written, which may be less than 'length'.
*/
		virtual R_METHOD() int write( int length, void const* buffer ) = 0;
		virtual R_METHOD() void flush() = 0;
		virtual R_METHOD() uint64_t getoffset() = 0;
	};
}
