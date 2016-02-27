#ifndef RSBIN_PNGREADER_HPP__
#define RSBIN_PNGREADER_HPP__ 1

#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/databuffer.hpp>
#include <common.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin
{
	R_CLASS( name = pngreader )
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
		PngReader( PngReader const& ) = delete;
		PngReader& operator=( PngReader const& ) = delete;

		R_METHOD() static PngReader* create( int format );
		R_METHOD( release ) void release();
		R_METHOD() void feed( int length, void const* buffer );
		R_METHOD() bool isfinished() noexcept { return m_finished; }
		R_METHOD() int getwidth() noexcept { return m_width; }
		R_METHOD() int getheight() noexcept { return m_height; }
		R_METHOD( addref = result[0] ) utils::DataBuffer* getdata() noexcept {
			return m_data;
		}
	};
}

#endif
