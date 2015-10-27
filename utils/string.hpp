#ifndef UTILS_STRING_HPP__
#define UTILS_STRING_HPP__ 1

#include "databuffer.hpp"
#include "ref.hpp"

namespace utils
{
	class String
	{
	public:
		Ref< DataBuffer > m_payload;

	public:
		String();
		String( const char* newchars, int length = 0 );
		String( const String& other ) noexcept;
		String( String&& other ) noexcept;
		String( Ref< DataBuffer > const& db );
		String( Ref< DataBuffer >&& db );
		~String() noexcept;
		String& operator=( const String& other ) noexcept;
		String& operator=( String&& other ) noexcept;

		operator bool() const noexcept;
		operator char const*() const noexcept;
		int getlength() const noexcept;
		const char* getchars() const noexcept;
		void setchars( const char* newchars, int length = 0 );
	};
}

#endif
