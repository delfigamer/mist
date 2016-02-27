#ifndef RSBIN_PNGWRITER_HPP__
#define RSBIN_PNGWRITER_HPP__ 1

#include <utils/string.hpp>
#include <utils/cyclicbuffer.hpp>
#include <utils/ref.hpp>
#include <utils/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
	R_CLASS( name = pngwriter )
	class PngWriter
	{
	public:
		enum
		{
			BitmapFormat_Int8_Gamma = 0,
			BitmapFormat_Invalid = 1,
		};

	private:
		int m_format;
		int m_stage;
		int m_width;
		int m_height;
		int m_stride;
		int m_rowpos;
		utils::Ref< utils::DataBuffer > m_data;
		utils::CyclicBuffer m_buffer;
		png_structp m_png;
		png_infop m_info;
		utils::String m_error;
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
			int format, int width, int height,
			utils::DataBuffer* data );
		~PngWriter();
		PngWriter( PngWriter const& ) = delete;
		PngWriter& operator=( PngWriter const& ) = delete;

		R_METHOD() static PngWriter* create(
			int format, int width, int height,
			utils::DataBuffer* data );
		R_METHOD( release ) void release();
		R_METHOD() void grab( int length, void* buffer, int* result );
		R_METHOD() bool isfinished();
	};
}

#endif
