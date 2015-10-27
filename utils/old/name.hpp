#ifndef UTILS_NAME_HPP_
#define UTILS_NAME_HPP_ 1

#include "string.hpp"

namespace utils {
	class Name {
	private:
		String m_string;
		int m_hash;
		
	public:
		Name();
		Name( String const& string );
		Name( Name const& other );
		Name( Name&& other );
		~Name() noexcept;
		
		int hash() const noexcept;
		String const& string() const noexcept;
		String const& string( String const& nv );
		
		Name& operator=( Name const& other );
		Name& operator=( Name&& other );
		bool operator<( Name const& other ) const noexcept;
		bool operator>( Name const& other ) const noexcept;
		bool operator<=( Name const& other ) const noexcept;
		bool operator>=( Name const& other ) const noexcept;
		bool operator==( Name const& other ) const noexcept;
		bool operator!=( Name const& other ) const noexcept;
		operator String const&() const noexcept;
		operator int() const noexcept;
		operator bool() const noexcept;
		
		static int strhash( String const& str ) noexcept;
	};
}

#endif
