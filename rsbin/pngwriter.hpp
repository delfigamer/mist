#pragma once

#include <rsbin/pngcommon.hpp>
#include <common/string.hpp>
#include <utils/cyclicbuffer.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>
#include <cinttypes>

namespace rsbin
{
	R_CLASS( name = pngwriter )
	class PngWriter: public RefObject
	{
	private:
		int m_format;
		int m_stage;
		uint32_t m_width;
		uint32_t m_height;
		ptrdiff_t m_stride;
		uint32_t m_rowpos;
		Ref< DataBuffer > m_data;
		utils::CyclicBuffer m_buffer;
		png_structp m_png;
		png_infop m_info;
		String m_error;
		jmp_buf m_jmpbuf;

		static void error_handler(
			png_structp png, png_const_charp msg );
		static void warning_handler(
			png_structp png, png_const_charp msg );
		static void write_callback(
			png_structp png, png_bytep data, png_size_t length );
		static void flush_callback( png_structp png );
		void writeheader();
		void writerow();
		void writeend();
		bool feedbuffer();

	public:
		PngWriter() = delete;
		PngWriter(
			bitmapformat format, uint32_t width, uint32_t height,
			DataBuffer* data );
		~PngWriter();
		PngWriter( PngWriter const& ) = delete;
		PngWriter& operator=( PngWriter const& ) = delete;

		R_METHOD() static PngWriter* create(
			bitmapformat format, uint32_t width, uint32_t height,
			DataBuffer* data )
		{
			return new PngWriter( format, width, height, data );
		}
		R_METHOD() size_t grab( size_t length, void* buffer );
		R_METHOD() bool isfinished();
	};
}
