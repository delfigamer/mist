#ifndef RSBIN_METHODLIST_HPP__
#define RSBIN_METHODLIST_HPP__ 1
#include <cinttypes>

namespace utils
{
	class DataBuffer;
}

namespace rsbin
{
	class FileIo;
	class FsTask;
	class PngReader;
	class PngWriter;

	struct MethodList
	{
		typedef utils::DataBuffer DataBuffer;

		FileIo*( *fileio_new )( char const* path, int mode );
		FsTask*( *fileio_startread )(
			FileIo* io, uint64_t offset, int length, void* buffer );
		FsTask*( *fileio_startwrite )(
			FileIo* io, uint64_t offset, int length,
			void const* buffer );
		bool( *fileio_setsize )( FileIo* fileio, uint64_t size );
		bool( *fstask_promote )( FsTask* task );
		int( *fstask_isfinished )( FsTask* task );
		int( *fstask_getresult )( FsTask* task );
		char const*( *fstask_geterror )( FsTask* task );
		PngReader*( *pngreader_new )( int format );
		bool( *pngreader_delete )( PngReader* reader );
		bool( *pngreader_feed )(
			PngReader* reader,
			int length, void const* buffer );
		int( *pngreader_isfinished )( PngReader* reader );
		int( *pngreader_getwidth )( PngReader* reader );
		int( *pngreader_getheight )( PngReader* reader );
		DataBuffer*( *pngreader_getdata )( PngReader* reader );
		PngWriter*( *pngwriter_new )(
			int format, int width, int height,
			DataBuffer* data );
		bool( *pngwriter_delete )( PngWriter* writer );
		bool( *pngwriter_grab )(
			PngWriter* writer, int length, void* buffer,
			int* result );
		int( *pngwriter_isfinished )( PngWriter* writer );
		int( *fileexists )( char const* path );
		bool( *filesize )( char const* path, uint64_t* size );
	};

	MethodList const* getmethodlist();
}

#endif
