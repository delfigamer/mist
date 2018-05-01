#pragma once

#include <rsbin/pngcommon.hpp>
#include <rsbin/stream.hpp>
#include <rsbin/formattask.hpp>
#include <common/ref.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
	class [[ r::class, r::name( "pngreader" ) ]] PngReader: public FormatTask
	{
	private:
		Ref< DataBuffer > m_pixels;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_stride;
		int m_format;
		jmp_buf m_jmpbuf;
		bool m_streamend;

		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void read_callback(
			png_structp png, png_bytep data, png_size_t length );
		void feed( size_t length, void const* buffer );
		void readdata( png_structp png, png_infop info );
		virtual void process() override;

	public:
		PngReader( Stream* stream, bitmapformat format );
		~PngReader();
		PngReader( PngReader const& ) = delete;
		PngReader& operator=( PngReader const& ) = delete;

	[[ r::method ]]
		static Ref< PngReader > create( Stream* stream, bitmapformat format )
		{
			return Ref< PngReader >::create( stream, format );
		}
	[[ r::method ]]
		Ref< DataBuffer > getpixels();
	[[ r::method ]]
		uint32_t getwidth();
	[[ r::method ]]
		uint32_t getheight();
	};
}
