#ifndef RSBIN_PNGWRITER_HPP__
#define RSBIN_PNGWRITER_HPP__ 1

#include <utils/string.hpp>
#include <utils/cyclicbuffer.hpp>
#include <utils/ref.hpp>
#include <utils/databuffer.hpp>
#include <utils/console.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
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
		utils::SingletonRef< utils::ConsoleClass > m_console;

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
		PngWriter( PngWriter const& other ) = delete;
		PngWriter( PngWriter&& other ) = delete;
		PngWriter& operator=( PngWriter const& other ) = delete;
		PngWriter& operator=( PngWriter&& other ) = delete;

		void grab( int length, void* buffer, int* result );
		bool isfinished();
	};

	PngWriter* rsbin_pngwriter_new(
		int format, int width, int height,
		utils::DataBuffer* data ) noexcept;
	bool rsbin_pngwriter_delete( PngWriter* writer ) noexcept;
	bool rsbin_pngwriter_grab(
		PngWriter* writer, int length, void* buffer,
		int* result ) noexcept;
	int rsbin_pngwriter_isfinished( PngWriter* writer ) noexcept;
}

#endif
