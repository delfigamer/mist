#ifndef UTILS_DELEGATE_HPP__
#define UTILS_DELEGATE_HPP__ 1

#include <atomic>
#include <thread>
#include <functional>

namespace utils
{
	template< typename T >
	class Delegate
	{
	};

	template< typename Ret, typename... Args >
	class Delegate< Ret( Args... ) >
	{
	public:
		typedef Ret( *code_t )( void* self, Args... args );

	private:
		std::atomic_flag mutable m_aflag;
		code_t m_code;
		void* m_self;

	public:
		Delegate( code_t code = 0, void* self = 0 )
			: m_code( code )
			, m_self( self )
		{
			m_aflag.clear( std::memory_order_release );
		}

		Delegate( Delegate const& other )
		{
			other.get( &m_code, &m_self );
			m_aflag.clear( std::memory_order_release );
		}

		~Delegate()
		{
		}

		Delegate& operator=( Delegate const& other )
		{
			code_t code;
			void* self;
			other.get( &code, &self );
			set( code, self );
			return *this;
		}

		void get( code_t* code, void** self ) const
		{
			while( m_aflag.test_and_set( std::memory_order_acquire ) )
			{
			}
			*code = m_code;
			*self = m_self;
			m_aflag.clear( std::memory_order_release );
		}

		void set( code_t code, void* self = 0 )
		{
			while( m_aflag.test_and_set( std::memory_order_acquire ) )
			{
			}
			m_code = code;
			m_self = self;
			m_aflag.clear( std::memory_order_release );
		}

		Ret operator()( bool* success, Args... args )
		{
			code_t code;
			void* self;
			get( &code, &self );
			if( code )
			{
				if( success )
				{
					*success = true;
				}
				return code( self, std::forward< Args >( args )... );
			}
			else
			{
				if( success )
				{
					*success = false;
				}
				return Ret();
			}
		}

		Ret operator()( Args... args )
		{
			return this->operator()( 0, std::forward< Args >( args )... );
		}

		template< Ret( target )( Args... ) >
		static Ret bind_static( void* self, Args... args )
		{
			return target( std::forward< Args >( args )... );
		}

		template< typename T, Ret( T::*target )( Args... ) >
		static Ret bind_method( void* self, Args... args )
		{
			return ( ( ( T* )self )->*target )( std::forward< Args >( args )... );
		}
	};
}

#endif
