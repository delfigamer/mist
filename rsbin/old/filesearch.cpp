#include "filesearch.hpp"
#include <utils/encoding.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>
#ifdef _WIN32
#elif defined( __ANDROID__ )
#endif

namespace rsbin {
	
	bool FileSearch::query( int id, void** target ) {
		return queryobject( this, id, target );
	}
	
	FileSearch::FileSearch( utils::String const& target ) :
		m_target( target ) {
#ifdef _WIN32
		wchar_t buffer[ 1024 ];
		utils::translatestr( utils::encoding::utf8, utils::encoding::utf16,
				( uint8_t const* )target.chars(), ( uint8_t* )buffer, sizeof( buffer ) );
		m_handle = FindFirstFileW( buffer, &m_finddata );
		if( m_handle == INVALID_HANDLE_VALUE ) {
			int le = GetLastError();
			if( le == ERROR_FILE_NOT_FOUND ) {
				m_hasentry = false;
			} else {
				throw std::runtime_error( "windows error" );
			}
		} else {
			m_hasentry = true;
		}
#elif defined( __ANDROID__ )
#endif
	}
	
	FileSearch::~FileSearch() {
#ifdef _WIN32
		FindClose( m_handle );
#elif defined( __ANDROID__ )
#endif
	}
	
	bool FileSearch::hasentry() {
#ifdef _WIN32
		return m_hasentry;
#elif defined( __ANDROID__ )
	return false;
#endif
	}
	
	void FileSearch::getentry( FileSearchEntry* entry ) {
#ifdef _WIN32
		if( !m_hasentry ) {
			return;
		}
		utils::translatestr( utils::encoding::utf16, utils::encoding::utf8,
			( uint8_t const* )m_finddata.cFileName, ( uint8_t* )m_filename, sizeof( m_filename ) );
		entry->filename = m_filename;
		entry->size = uint64_t( m_finddata.nFileSizeHigh ) << 32 | m_finddata.nFileSizeLow;
		SYSTEMTIME st;
		FileTimeToSystemTime( &m_finddata.ftLastWriteTime, &st );
		entry->savetime[ 0 ] = st.wYear;
		entry->savetime[ 1 ] = st.wMonth;
		entry->savetime[ 2 ] = st.wDay;
		entry->savetime[ 3 ] = st.wHour;
		entry->savetime[ 4 ] = st.wMinute;
		entry->savetime[ 5 ] = st.wSecond * 1000 + st.wMilliseconds;
#elif defined( __ANDROID__ )
#endif
	}
	
	void FileSearch::findnext() {
#ifdef _WIN32
		if( !m_hasentry ) {
			return;
		}
		bool m_hasentry = FindNextFileW( m_handle, &m_finddata );
		if( !m_hasentry ) {
			int le = GetLastError();
			if( le == ERROR_NO_MORE_FILES ) {
				m_hasentry = false;
			} else {
				throw std::runtime_error( "windows error" );
			}
		}
#elif defined( __ANDROID__ )
#endif
	}
	
	extern "C" {
		FileSearch* rsbin_filesearch_new( char const* target ) noexcept {
			CBASE_PROTECT(
#ifdef _WIN32
				return new FileSearch( target );
#elif defined( __ANDROID__ )
				cbase_seterror( "not implemented" );
				return 0;
#endif
			)
		}
		
		int rsbin_filesearch_hasentry( FileSearch* fs ) noexcept {
			CBASE_PROTECT(
				return fs->hasentry() ? 1 : 2;
			)
		}
		
		bool rsbin_filesearch_getentry( FileSearch* fs, FileSearchEntry* entry ) noexcept {
			CBASE_PROTECT(
				fs->getentry( entry );
				return 1;
			)
		}
		
		bool rsbin_filesearch_findnext( FileSearch* fs ) noexcept {
			CBASE_PROTECT(
				fs->findnext();
				return 1;
			)
		}
	}
}
