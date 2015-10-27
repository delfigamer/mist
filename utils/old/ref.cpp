#include "ref.hpp"
#include "console.hpp"
#include <atomic>
#include <exception>
#include <stdexcept>
#include <utility>

#undef LOG
#define LOG( ... )

namespace utils {
	RefBase::RefBase() noexcept :
		m_ref( ( RefObject* )0 ) {
		LOG( "< Ref: %#10x : %#10x >\tRef()",
			uint32_t( this ), 0 );
	}

	RefBase::RefBase( RefObject* ref ) noexcept :
		m_ref( ( RefObject* )0 ) {
		LOG( "< Ref: %#10x : %#10x >\tRef( %#10x )",
			uint32_t( this ), 0, uint32_t( ref ) );
		assign_silent( ref );
	}

	RefBase::RefBase( RefObject* ref, int ) noexcept :
		m_ref( ref ) {
		LOG( "< Ref: %#10x : %#10x >\tRef( %#10x, int() )",
			uint32_t( this ), 0, uint32_t( ref ) );
	}

	RefBase::RefBase( RefObject* ref, int, int id ) noexcept :
		m_ref( ( RefObject* )0 ) {
		LOG( "< Ref: %#10x : %#10x >\tRef( %#10x, int(), %#.8x )",
			uint32_t( this ), 0, uint32_t( ref ), id );
		assign_silent( ref, id );
	}

	RefBase::RefBase( RefBase&& other ) noexcept :
		m_ref( ( RefObject* )0 ) {
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tRef( std::move( < Ref: %#10x : %#10x > ) )",
			uint32_t( this ), 0, uint32_t( &other ), uint32_t( ref ) );
	}

	RefBase::RefBase( RefBase const& other ) noexcept :
		m_ref( ( RefObject* )0 ) {
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tRef( < Ref: %#10x : %#10x > )",
			uint32_t( this ), 0, uint32_t( &other ), uint32_t( ref ) );
		assign_silent( ref );
	}

	RefBase::RefBase( RefBase const& other, int id ) noexcept :
		m_ref( ( RefObject* )0 ) {
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tRef( < Ref: %#10x : %#10x >, %#.8x )",
			uint32_t( this ), 0, uint32_t( &other ), uint32_t( ref ), id );
		assign_silent( ref, id );
	}

	RefBase::~RefBase() noexcept {
		RefObject* oldref = m_ref.exchange( 0, std::memory_order_relaxed );
		if( oldref ) {
			oldref->release();
		}
		LOG( "< Ref: %#10x : %#10x >\t~Ref()",
			uint32_t( this ), uint32_t( oldref ) );
	}

	RefObject* RefBase::possess_silent( RefObject* ref ) noexcept {
		RefObject* oldref;
		oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref ) {
			oldref->release();
		}
		return oldref;
	}

	RefObject* RefBase::assign_silent( RefObject* ref ) noexcept {
		if( ref ) {
			ref->addref();
		}
		RefObject* oldref = m_ref.exchange( ref, std::memory_order_relaxed );
		if( oldref ) {
			oldref->release();
		}
		return oldref;
	}

	RefObject* RefBase::assign_silent( RefObject* ref, int id ) noexcept {
		RefObject* newref;
		if( ref ) {
			if( !ref->query( id, ( void** )&newref ) ) {
				throw std::runtime_error( "RefObject cast failure" );
			}
		} else {
			newref = 0;
		}
		return possess_silent( newref );
	}

	RefObject* RefBase::possess( RefObject* ref ) noexcept {
		RefObject* oldref = possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tpossess( %#10x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( ref ) );
		return oldref;
	}

	RefObject* RefBase::assign( RefObject* ref ) noexcept {
		RefObject* oldref = assign_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( %#10x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( ref ) );
		return oldref;
	}

	RefObject* RefBase::assign( RefObject* ref, int id ) noexcept {
		RefObject* oldref = assign_silent( ref, id );
		LOG( "< Ref: %#10x : %#10x >\tassign( %#10x, %#.8x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( ref ), id );
		return oldref;
	}

	RefBase& RefBase::assign( RefBase&& other ) noexcept {
		RefObject* ref = other.m_ref.exchange( 0, std::memory_order_relaxed );
		RefObject* oldref = possess_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( std::move( < Ref: %#10x : %#10x > ) )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( &other ), uint32_t( ref ) );
		( void )oldref;
		return *this;
	}

	RefBase& RefBase::assign( RefBase const& other ) noexcept {
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		RefObject* oldref = assign_silent( ref );
		LOG( "< Ref: %#10x : %#10x >\tassign( < Ref: %#10x : %#10x > )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( &other ), uint32_t( ref ) );
		( void )oldref;
		return *this;
	}

	RefBase& RefBase::assign( RefBase const& other, int id ) noexcept {
		RefObject* ref = other.m_ref.load( std::memory_order_relaxed );
		RefObject* oldref = assign_silent( ref, id );
		LOG( "< Ref: %#10x : %#10x >\tassign( < Ref: %#10x : %#10x >, %#.8x )",
			uint32_t( this ), uint32_t( oldref ), uint32_t( &other ), uint32_t( ref ), id );
		( void )oldref;
		return *this;
	}

	RefObject* RefBase::get() const noexcept {
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tget()", uint32_t( this ), uint32_t( ref ) );
		return ref;
	}

	RefObject& RefBase::deref() const {
		RefObject* ref = m_ref.load( std::memory_order_relaxed );
		LOG( "< Ref: %#10x : %#10x >\tderef()", uint32_t( this ), uint32_t( ref ) );
		if( ref ) {
			return *ref;
		} else {
			throw std::runtime_error( "dereferencing an empty Ref" );
		}
	}

	bool RefBase::operator==( RefBase const& other ) {
		return m_ref.load( std::memory_order_relaxed ) == other.m_ref.load( std::memory_order_relaxed );
	}

	bool RefBase::operator!=( RefBase const& other ) {
		return !( *this == other );
	}

	Ref< RefObject >::Ref() noexcept :
		RefBase() {
	}

	Ref< RefObject >::Ref( RefObject* ref ) noexcept :
		RefBase( ref ) {
	}

	Ref< RefObject >::Ref( RefObject* ref, int ) noexcept :
		RefBase( ref, 0 ) {
	}

	Ref< RefObject >::Ref( Ref< RefObject > const& other ) noexcept :
		RefBase( other ) {
	}

	Ref< RefObject >::Ref( RefBase const& other ) noexcept :
		RefBase( other ) {
	}

	Ref< RefObject >::Ref( Ref< RefObject >&& other ) noexcept :
		RefBase( std::move( other ) ) {
	}

	Ref< RefObject >::Ref( RefBase&& other ) noexcept :
		RefBase( std::move( other ) ) {
	}

	Ref< RefObject >::~Ref() noexcept {
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefObject* ref ) noexcept {
		RefBase::assign( ref );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( Ref< RefObject > const& other ) noexcept {
		RefBase::assign( other );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefBase const& other ) noexcept {
		RefBase::assign( other );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( Ref< RefObject >&& other ) noexcept {
		RefBase::assign( std::move( other ) );
		return *this;
	}

	Ref< RefObject >& Ref< RefObject >::operator=( RefBase&& other ) noexcept {
		RefBase::assign( std::move( other ) );
		return *this;
	}

	RefObject& Ref< RefObject >::operator*() const {
		return RefBase::deref();
	}

	RefObject* Ref< RefObject >::operator->() const {
		return &RefBase::deref();
	}

	Ref< RefObject >::operator RefObject*() const noexcept {
		return RefBase::get();
	}
}
