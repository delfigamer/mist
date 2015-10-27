#include "name.hpp"

#include <math/crc32.hpp>
#include <utility>

namespace utils {
	Name::Name() :
		m_string( 0 ) ,
		m_hash( 0 ) {
	}
	
	Name::Name( String const& string ) :
		m_string( string ) ,
		m_hash( strhash( string ) ) {
	}
	
	Name::Name( Name const& other ) :
		m_string( other.m_string ) ,
		m_hash( other.m_hash ) {
	}
	
	Name::Name( Name&& other ) :
		m_string( std::move( other.m_string ) ) ,
		m_hash( other.m_hash ) {
	}
	
	Name::~Name() {
	}
	
	int Name::hash() const  noexcept {
		return m_hash;
	}
	
	String const& Name::string() const noexcept {
		return m_string;
	}
	
	String const& Name::string( String const& nv ) {
		return m_string = nv;
	}
	
	Name& Name::operator=( Name const& other ) {
		m_string = other.m_string;
		m_hash = other.m_hash;
		return *this;
	}
	
	Name& Name::operator=( Name&& other ) {
		m_string = std::move( other.m_string );
		m_hash = other.m_hash;
		return *this;
	}
	
	bool Name::operator<( Name const& other ) const noexcept {
		return m_hash < other.m_hash;
	}
	
	bool Name::operator>( Name const& other ) const noexcept {
		return m_hash > other.m_hash;
	}
	
	bool Name::operator<=( Name const& other ) const noexcept {
		return m_hash <= other.m_hash;
	}
	
	bool Name::operator>=( Name const& other ) const noexcept {
		return m_hash >= other.m_hash;
	}
	
	bool Name::operator==( Name const& other ) const noexcept {
		return m_hash == other.m_hash;
	}
	
	bool Name::operator!=( Name const& other ) const noexcept {
		return m_hash != other.m_hash;
	}
	
	Name::operator String const&() const noexcept {
		return m_string;
	}
	
	Name::operator int() const noexcept {
		return m_hash;
	}
	
	Name::operator bool() const noexcept {
		return m_string;
	}
	
	int Name::strhash( String const& str ) noexcept {
		if( str ) {
			return (int)math::crc32( str.chars(), str.length() );
		} else {
			return 0;
		}
	}
}
