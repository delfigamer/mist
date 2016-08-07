#pragma once

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
		Io();
		virtual ~Io() override;
		Io( Io const& other ) = delete;
		Io& operator=( Io const& other ) = delete;

		virtual R_METHOD() IoTask* startread(
			uint64_t offset, size_t length, void* buffer, bool promote ) = 0;
		virtual R_METHOD() IoTask* startwrite(
			uint64_t offset, size_t length, void const* buffer, bool promote ) = 0;
		virtual R_METHOD() void setsize( uint64_t size ) = 0;

		uint64_t blockread(
			uint64_t offset, size_t length, void* buffer,
			bool promote = false );
		uint64_t blockwrite(
			uint64_t offset, size_t length, void const* buffer,
			bool promote = false );
	};
}
