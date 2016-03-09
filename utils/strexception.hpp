#ifndef UTILS_STREXCEPTION_HPP__
#define UTILS_STREXCEPTION_HPP__ 1

#include <utils/string.hpp>
#include <common.hpp>
#include <exception>

namespace utils
{
	class StrException: public std::exception
	{
	private:
		String m_message;

	public:
		StrException();
		StrException( char const* format, ... );
		StrException( String const& message );
		StrException( StrException const& other );
		StrException( StrException&& other );
		StrException& operator=( StrException const& other );
		StrException& operator=( StrException&& other );
		virtual char const* what() const NOEXCEPT override;
	};
}

#endif
