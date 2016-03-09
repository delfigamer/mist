#ifndef UTILS_STRING_HPP__
#define UTILS_STRING_HPP__ 1

#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>

namespace utils
{
	class String
	{
	public:
		Ref< DataBuffer > m_payload;

	public:
		String();
		String( const char* newchars, int length = 0 );
		String( const String& other ) NOEXCEPT;
		String( String&& other ) NOEXCEPT;
		String( Ref< DataBuffer > const& db );
		String( Ref< DataBuffer >&& db );
		~String() NOEXCEPT;
		String& operator=( const String& other ) NOEXCEPT;
		String& operator=( String&& other ) NOEXCEPT;

		operator bool() const NOEXCEPT;
		operator char const*() const NOEXCEPT;
		int getlength() const NOEXCEPT;
		const char* getchars() const NOEXCEPT;
		void setchars( const char* newchars, int length = 0 );
	};
}

#endif
