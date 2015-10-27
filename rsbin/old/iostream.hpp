#ifndef RSBIN_IOSTREAM_HPP__
#define RSBIN_IOSTREAM_HPP__ 1

#include "io.hpp"
#include <utils/stream.hpp>

namespace rsbin {
	class IoStream: public utils::Stream {
	private:
		utils::Ref< Io > m_io;
		uint64_t m_offset;
		bool m_direction;
		
	public:
		virtual bool query( int id, void** target ) override;
		IoStream() = delete;
		explicit IoStream( Io* io, uint64_t offset, bool direction );
		virtual ~IoStream() override;
		IoStream( IoStream const& other ) = delete;
		IoStream( IoStream&& other ) = delete;
		IoStream& operator=( IoStream const& other ) = delete;
		IoStream& operator=( IoStream&& other ) = delete;
		
		virtual utils::TaskBase* dostarttransfer( int length, void* buffer, int* result, int taskpriority ) override;
	};
}

DEFINE_REFID( rsbin::IoStream, utils::Stream, 0x3071814d )

namespace rsbin {
	extern "C" {
		IoStream* rsbin_iostream_new( Io* io, uint64_t offset, bool readdir ) noexcept;
	}
}

#endif
