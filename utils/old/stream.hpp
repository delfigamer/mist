#ifndef UTILS_STREAM_HPP__
#define UTILS_STREAM_HPP__ 1

#include <utils/taskbase.hpp>
#include <utils/refobject.hpp>
#include <cinttypes>

namespace utils {
	class Stream: public RefObject {
	public:
		virtual bool query( int id, void** target ) override;
		Stream();
		virtual ~Stream() override;
		Stream( Stream const& other ) = delete;
		Stream( Stream&& other ) = delete;
		Stream& operator=( Stream const& other ) = delete;
		Stream& operator=( Stream&& other ) = delete;
		
		virtual TaskBase* dostarttransfer( int length, void* buffer, int* result, int taskpriority ) = 0;
		Ref< TaskBase > starttransfer( int length, void* buffer, int* result, int taskpriority );
		int synctransfer( int length, void* buffer );
	};
}

DEFINE_REFID( utils::Stream, utils::RefObject, 0x8bea74b5 )

namespace utils {
	extern "C" {
		TaskBase* utils_stream_starttransfer( Stream* stream, int length, void* buffer, int* result, int taskpriority ) noexcept;
	}
}

#endif
