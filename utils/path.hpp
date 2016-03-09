#ifndef UTILS_PATH_HPP__
#define UTILS_PATH_HPP__ 1

#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <utils/refobject.hpp>
#include <common.hpp>

namespace utils
{
	class Path: public RefObject
	{
	public:
#if defined( _WIN32 ) || defined( _WIN64 )
		typedef wchar_t char_t;
#else
		typedef char char_t;
#endif

	private:
		Ref< DataBuffer > m_target;
		Ref< Path > m_base;
		Ref< DataBuffer > m_result;

	public:
		Path() = delete;
		Path( DataBuffer* target, Path* base );
		Path( Path const& other );
		Path( Path&& other );
		virtual ~Path();
		Path& operator=( Path const& other ) = delete;

		char_t const* combine();

	public:
		static Ref< Path > create( void const* path );
	};
}

#endif
