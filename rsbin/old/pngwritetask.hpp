#ifndef RSBIN_PNGWRITETASK_HPP__
#define RSBIN_PNGWRITETASK_HPP__ 1

#include <utils/stream.hpp>
#include <utils/functionaltask.hpp>
#include <utils/string.hpp>
#include <utils/ref.hpp>
#include <utils/stringinstance.hpp>
#include <png/png.hpp>
#include <csetjmp>

namespace rsbin {
	class PngWriteTask: public utils::FunctionalTask {
	private:
		utils::Ref< utils::Stream > m_target;
		int m_stage;
		int m_width;
		int m_height;
		int m_stride;
		int m_rowpos;
		utils::Ref< utils::StringInstance > m_data;
		uint8_t* m_buffer;
		int m_bufferfill;
		int m_ioresult;
		png_structp m_png;
		png_infop m_info;
		utils::String m_error;
		jmp_buf m_jmpbuf;

		static void error_handler( png_structp png, png_const_charp msg );
		static void warning_handler( png_structp png, png_const_charp msg );
		static void write_callback( png_structp png, png_bytep data, png_size_t length );
		static void flush_callback( png_structp png );
		void dowriteheader();
		void dowritechunk();
		void dowriteend();
		bool flushbuffer();

	public:
		virtual bool query( int id, void** target ) override;
		PngWriteTask() = delete;
		PngWriteTask( int taskpriority, utils::Stream* target, int width, int height, utils::StringInstance* data );
		virtual ~PngWriteTask() override;
		PngWriteTask( PngWriteTask const& other ) = delete;
		PngWriteTask( PngWriteTask&& other ) = delete;
		PngWriteTask& operator=( PngWriteTask const& other ) = delete;
		PngWriteTask& operator=( PngWriteTask&& other ) = delete;

		virtual void doaction() override;
	};
}

DEFINE_REFID( rsbin::PngWriteTask, utils::FunctionalTask, 0x09225f39 )

namespace rsbin {
	extern "C" {
		PngWriteTask* rsbin_pngwritetask_new( int taskpriority, utils::Stream* target,
			int width, int height, utils::StringInstance* data ) noexcept;
	}
}

#endif
