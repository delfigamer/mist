#ifndef UTILS_METHOD_HPP__
#define UTILS_METHOD_HPP__ 1

#include <atomic>
#include <thread>
#include <functional>

namespace utils {
	template< typename T >
	class Method {
	};

	template< typename Ret, typename... Args >
	class Method< Ret( Args... ) > {
	public:
		class Dummy { };
		typedef Ret( Dummy::*code_t )( Args... );

	private:
		std::atomic_flag mutable m_aflag;
		code_t m_code;
		Dummy* m_object;

	public:
		Method() :
			m_code( 0 ) ,
			m_object( 0 ) {
			// LOG( "-" );
			m_aflag.clear( std::memory_order_release );
		}

		template< typename T >
		Method( Ret( T::*code )( Args... ), T* object ) :
			m_code( ( code_t )code ) ,
			m_object( ( Dummy* )object ) {
			// LOG( "-" );
			m_aflag.clear( std::memory_order_release );
		}

		Method( Method const& other ) :
			m_code( 0 ) ,
			m_object( 0 ) {
			// LOG( "-" );
			other.get( &m_code, &m_object );
			m_aflag.clear( std::memory_order_release );
		}

		~Method() {
			// LOG( "-" );
		}

		Method& operator=( Method const& other ) {
			code_t code;
			Dummy* object;
			other.get( &code, &object );
			set( code, object );
			return *this;
		}

		void get( code_t* code, Dummy** object ) const {
			while( m_aflag.test_and_set(
					std::memory_order_acquire ) ) {
			}
			*code = m_code;
			*object = m_object;
			m_aflag.clear( std::memory_order_release );
		}

		void set( code_t code, Dummy* object ) {
			while( m_aflag.test_and_set(
					std::memory_order_acquire ) ) {
			}
			m_code = code;
			m_object = object;
			m_aflag.clear( std::memory_order_release );
		}

		Ret operator()( bool* success, Args... args ) {
			code_t code;
			Dummy* object;
			get( &code, &object );
			if( code ) {
				if( success ) {
					*success = true;
				}
				return ( object->*code )(
					std::forward< Args >( args )... );
			} else {
				if( success ) {
					*success = false;
				}
				return Ret();
			}
		}

		Ret operator()( Args... args ) {
			return this->operator()(
				0, std::forward< Args >( args )... );
		}
	};
}

#endif
