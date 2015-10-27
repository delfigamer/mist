#ifndef RSBIN_PNGREADTASK_HPP__
#define RSBIN_PNGREADTASK_HPP__ 1

#include <utils/stream.hpp>
#include <utils/functionaltask.hpp>
#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/stringinstance.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin {
	class PngReadTask: public utils::FunctionalTask {
	private:
		utils::Ref< utils::Stream > m_source;
		int m_stage;
		int m_width;
		int m_height;
		int m_stride;
		utils::Ref< utils::StringInstance > m_data;
		uint8_t* m_buffer;
		int m_ioresult;
		png_structp m_png;
		png_infop m_info;
		utils::String m_error;
		jmp_buf m_jmpbuf;
		
		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void info_callback( png_structp png, png_infop info );
		static void row_callback(  png_structp png, png_bytep row,
			png_uint_32 rowpos, int pass  );
		static void end_callback( png_structp png, png_infop info );
		void doreadsig();
		void doreadheader();
		void doprocesschunk();
		
	public:
		virtual bool query( int id, void** target ) override;
		PngReadTask() = delete;
		PngReadTask( int taskpriority, utils::Stream* source );
		virtual ~PngReadTask() override;
		PngReadTask( PngReadTask const& other ) = delete;
		PngReadTask( PngReadTask&& other ) = delete;
		PngReadTask& operator=( PngReadTask const& other ) = delete;
		PngReadTask& operator=( PngReadTask&& other ) = delete;
		
		virtual void doaction() override;
		int getwidth() const;
		int getheight() const;
		utils::Ref< utils::StringInstance > const& getdata() const;
	};
}

DEFINE_REFID( rsbin::PngReadTask, utils::FunctionalTask, 0x93acccc3 )

namespace rsbin {
	extern "C" {
		PngReadTask* rsbin_pngreadtask_new( int taskpriority, utils::Stream* source ) noexcept;
		int rsbin_pngreadtask_getwidth( PngReadTask* task ) noexcept;
		int rsbin_pngreadtask_getheight( PngReadTask* task ) noexcept;
		utils::StringInstance* rsbin_pngreadtask_getdata( PngReadTask* task ) noexcept;
	}
}

#endif
