#pragma once

#include <rsbin/pngcommon.hpp>
#include <rsbin/stream.hpp>
#include <common/ref.hpp>
#include <common/task.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <exception>
#include <atomic>
#include <thread>
#include <csetjmp>

namespace rsbin
{
	class [[ r::class, r::name( "pngreader" ) ]] PngReader: public Task
	{
	private:
		std::thread m_thread;
		Ref< Stream > m_stream;
		Ref< DataBuffer > m_pixels;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_stride;
		int m_format;
		std::exception_ptr m_error;
		jmp_buf m_jmpbuf;
		bool m_streamend;
		std::atomic< bool > m_aborted;
		std::atomic< bool > m_finished;

		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void info_callback( png_structp png, png_infop info );
		static void row_callback(
			png_structp png, png_bytep row, png_uint_32 rowpos, int pass );
		static void end_callback( png_structp png, png_infop info );
		void feed( size_t length, void const* buffer );
		void readdata( png_structp png, png_infop info );
		void threadfunc();

	public:
		PngReader( bitmapformat format, Stream* stream );
		~PngReader();
		PngReader( PngReader const& ) = delete;
		PngReader& operator=( PngReader const& ) = delete;

		virtual bool poll() override;

	[[ r::method ]]
		static Ref< PngReader > create( bitmapformat format, Stream* stream )
		{
			return Ref< PngReader >::create( format, stream );
		}
	[[ r::method ]]
		Ref< DataBuffer > getpixels();
	[[ r::method ]]
		uint32_t getwidth();
	[[ r::method ]]
		uint32_t getheight();
	};
}
