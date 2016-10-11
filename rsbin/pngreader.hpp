#pragma once

#include <rsbin/pngcommon.hpp>
#include <common/string.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = pngreader )
	class PngReader: public RefObject
	{
	private:
		int m_format;
		bool m_finished;
		uint32_t m_width;
		uint32_t m_height;
		ptrdiff_t m_stride;
		Ref< DataBuffer > m_data;
		png_structp m_png;
		png_infop m_info;
		String m_error;
		jmp_buf m_jmpbuf;

		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void info_callback( png_structp png, png_infop info );
		static void row_callback( png_structp png, png_bytep row,
			png_uint_32 rowpos, int pass );
		static void end_callback( png_structp png, png_infop info );
		void doreadsig();
		void doreadheader();
		void doprocesschunk();

	public:
		PngReader( bitmapformat format );
		~PngReader();
		PngReader( PngReader const& ) = delete;
		PngReader& operator=( PngReader const& ) = delete;

		R_METHOD() static PngReader* create( bitmapformat format )
		{
			return new PngReader( format );
		}
		R_METHOD() void feed( size_t length, void const* buffer );
		R_METHOD() bool isfinished() NOEXCEPT { return m_finished; }
		R_METHOD() uint32_t getwidth() NOEXCEPT { return m_width; }
		R_METHOD() uint32_t getheight() NOEXCEPT { return m_height; }
		R_METHOD() DataBuffer* getdata() NOEXCEPT
		{
			::addref( m_data );
			return m_data;
		}
	};
}
