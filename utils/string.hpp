#ifndef UTILS_STRING_HPP__
#define UTILS_STRING_HPP__ 1

#include <utils/databuffer.hpp>
#include <utils/ref.hpp>
#include <common.hpp>
#include <atomic>
#include <utility>
#include <cstring>

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

	inline String::String()
		: m_payload( nullptr )
	{
	}

	inline String::String( const char* newchars, int length )
		: m_payload( nullptr )
	{
		setchars( newchars, length );
	}

	inline String::String( const String& other ) NOEXCEPT
		: m_payload( other.m_payload )
	{
	}

	inline String::String( String&& other ) NOEXCEPT
		: m_payload( std::move( other.m_payload ) )
	{
	}

	inline String::String( Ref< DataBuffer > const& db )
		: m_payload( db )
	{
	}

	inline String::String( Ref< DataBuffer >&& db )
		: m_payload( std::move( db ) )
	{
	}

	inline String::~String() NOEXCEPT
	{
	}

	inline String& String::operator=( const String& other ) NOEXCEPT
	{
		m_payload = other.m_payload;
		return *this;
	}

	inline String& String::operator=( String&& other ) NOEXCEPT
	{
		m_payload = std::move( other.m_payload );
		return *this;
	}

	inline String::operator bool() const NOEXCEPT
	{
		return m_payload;
	}

	inline String::operator char const*() const NOEXCEPT
	{
		return getchars();
	}

	inline int String::getlength() const NOEXCEPT
	{
		DataBuffer* e = m_payload;
		return ( e ? e->m_length : 0 );
	}

	inline const char* String::getchars() const NOEXCEPT
	{
		DataBuffer* e = m_payload;
		std::atomic_thread_fence( std::memory_order_acquire );
		return ( e ? ( char const* )e->m_data : "" );
	}

	inline void String::setchars( const char* newchars, int length )
	{
		if( !newchars )
		{
			m_payload = nullptr;
			return;
		}
		if( length == 0 )
		{
			length = strlen( newchars );
			if( length == 0 )
			{
				m_payload = nullptr;
				return;
			}
		}
		Ref< DataBuffer > payload = DataBuffer::create(
			length, length + 1, newchars );
		payload->m_data[ length ] = 0;
		payload->m_length = length + 1;
		std::atomic_thread_fence( std::memory_order_release );
		m_payload = payload;
	}
}

#endif
