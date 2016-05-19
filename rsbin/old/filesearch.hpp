#ifndef RSBIN_FILESEARCH_HPP__
#define RSBIN_FILESEARCH_HPP__ 1

#include <utils/refobject.hpp>
#include <utils/string.hpp>
#ifdef _WIN32
#include <windows.h>
#elif defined( __ANDROID__ )
#else
#error Unsupported platform
#endif

namespace rsbin {
	struct FileSearchEntry {
		char const* filename;
		uint64_t size;
		int savetime[ 6 ];
	};

	class FileSearch: public utils::RefObject {
	private:
		utils::String m_target;
#ifdef _WIN32
		HANDLE m_handle;
		WIN32_FIND_DATAW m_finddata;
		bool m_hasentry;
		char m_filename[ 1024 ];
#elif defined( __ANDROID__ )
#endif

	public:
		virtual bool query( int id, void** target ) override;
		FileSearch() = delete;
		explicit FileSearch( utils::String const& target );
		virtual ~FileSearch() override;
		FileSearch( FileSearch const& other ) = delete;
		FileSearch( FileSearch&& other ) = delete;
		FileSearch& operator=( FileSearch const& other ) = delete;
		FileSearch& operator=( FileSearch&& other ) = delete;

		bool hasentry();
		void getentry( FileSearchEntry* entry );
		void findnext();
	};
}

DEFINE_REFID( rsbin::FileSearch, utils::RefObject, 0x7cc7d817 )

namespace rsbin {
	extern "C" {
		FileSearch* rsbin_filesearch_new( char const* target ) noexcept;
		int rsbin_filesearch_hasentry( FileSearch* fs ) noexcept;
		bool rsbin_filesearch_getentry( FileSearch* fs, FileSearchEntry* entry ) noexcept;
		bool rsbin_filesearch_findnext( FileSearch* fs ) noexcept;
	}
}

#endif
