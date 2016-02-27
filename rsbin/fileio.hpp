#ifndef RSBIN_FILEIO_HPP__
#define RSBIN_FILEIO_HPP__ 1

#include <utils/refobject.hpp>
#include <utils/string.hpp>
#include <common.hpp>
#include <cinttypes>

namespace rsbin
{
	class FsTask;

	R_CLASS( name = fileio )
	class FileIo: public utils::RefObject
	{
	public:
		enum
		{
			FileMode_Read = 0, // "r", open for read only, fail if doesn't exist
			FileMode_Update = 1, // "r+", open for read and write, fail if doesn't exist
			FileMode_Create = 2, // "w", create or truncate for write only
			FileMode_Clear = 3, // "w+", create or truncate for read and write
			FileMode_Provide = 4, // "r+"/"w+", create or open for read and write
		};
	public:
#if defined( _WIN32 ) || defined( _WIN64 )
		void* m_handle;
		void* m_mapping;
#elif defined( __ANDROID__ )
		int m_handle;
#endif
		uint8_t* m_view;
		uint64_t m_viewsize;

	public:
		FileIo() = delete;
		FileIo( char const* path, int mode );
		virtual ~FileIo() override;
		FileIo( FileIo const& other ) = delete;
		FileIo( FileIo&& other ) = delete;
		FileIo& operator=( FileIo const& other ) = delete;
		FileIo& operator=( FileIo&& other ) = delete;

		R_METHOD() static FileIo* create( char const* path, int mode );
		R_METHOD() FsTask* startread(
			uint64_t offset, int length, void* buffer );
		R_METHOD() FsTask* startwrite(
			uint64_t offset, int length, void const* buffer );
		R_METHOD() void setsize( uint64_t size );
	};
}

#endif
