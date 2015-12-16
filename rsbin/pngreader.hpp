#ifndef RSBIN_PNGREADER_HPP__
#define RSBIN_PNGREADER_HPP__ 1

#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/databuffer.hpp>
#include <utils/console.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
	class PngReader
	{
	public:
		enum BitmapFormat
		{
			BitmapFormat_Int8_Gamma = 0,
			BitmapFormat_Invalid = 1,
		};

	private:
		BitmapFormat m_format;
		bool m_finished;
		uint32_t m_width;
		uint32_t m_height;
		int m_stride;
		utils::Ref< utils::DataBuffer > m_data;
		png_structp m_png;
		png_infop m_info;
		utils::String m_error;
		jmp_buf m_jmpbuf;
		utils::SingletonRef< utils::ConsoleClass > m_console;

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
		PngReader() = delete;
		PngReader( int format );
		~PngReader();
		PngReader( PngReader const& other ) = delete;
		PngReader( PngReader&& other ) = delete;
		PngReader& operator=( PngReader const& other ) = delete;
		PngReader& operator=( PngReader&& other ) = delete;

		void feed( int length, void const* buffer );
		bool isfinished();
		int getwidth();
		int getheight();
		utils::Ref< utils::DataBuffer > const& getdata();
	};

	PngReader* rsbin_pngreader_new( int format ) noexcept;
	bool rsbin_pngreader_delete( PngReader* reader ) noexcept;
	bool rsbin_pngreader_feed(
		PngReader* reader,
		int length, void const* buffer ) noexcept;
	int rsbin_pngreader_isfinished( PngReader* reader ) noexcept;
	int rsbin_pngreader_getwidth( PngReader* reader ) noexcept;
	int rsbin_pngreader_getheight( PngReader* reader ) noexcept;
	utils::DataBuffer* rsbin_pngreader_getdata(
		PngReader* reader ) noexcept;
}

#endif
