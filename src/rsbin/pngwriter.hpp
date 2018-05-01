#pragma once

#include <rsbin/pngcommon.hpp>
#include <rsbin/formattask.hpp>
#include <rsbin/stream.hpp>
#include <common/ref.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
	class [[ r::class, r::name( "pngwriter" ) ]] PngWriter: public FormatTask
	{
	private:
		Ref< DataBuffer > m_pixels;
		uint32_t m_width;
		uint32_t m_height;
		uint32_t m_stride;
		int m_format;
		jmp_buf m_jmpbuf;

		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void write_callback(
			png_structp png, png_bytep data, png_size_t length );
		static void flush_callback( png_structp png );
		void writedata( png_structp png, png_infop info );
		virtual void process() override;

	public:
		PngWriter(
			Stream* stream, bitmapformat format,
			uint32_t width, uint32_t height, DataBuffer* pixels );
		~PngWriter();
		PngWriter( PngWriter const& ) = delete;
		PngWriter& operator=( PngWriter const& ) = delete;

	[[ r::method ]]
		static Ref< PngWriter > create(
			Stream* stream, bitmapformat format,
			uint32_t width, uint32_t height, DataBuffer* pixels )
		{
			return Ref< PngWriter >::create(
				stream, format, width, height, pixels );
		}
	};
}
