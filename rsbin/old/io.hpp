#ifndef RSBIN_IO_HPP__
#define RSBIN_IO_HPP__ 1

#include <utils/refobject.hpp>
#include <common.hpp>
#include <cinttypes>

namespace rsbin
{
	class IoTask;

	R_CLASS( name = io )
	class Io: public utils::RefObject
	{
	public:
		Io() = default;
		virtual ~Io() override = default;
		Io( Io const& other ) = delete;
		Io& operator=( Io const& other ) = delete;

		virtual R_METHOD() IoTask* startread(
			uint64_t offset, int length, void* buffer ) = 0;
		virtual R_METHOD() IoTask* startwrite(
			uint64_t offset, int length, void const* buffer ) = 0;
		virtual R_METHOD() void setsize( uint64_t size ) = 0;
	};
}

#endif
