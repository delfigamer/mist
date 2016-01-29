#ifndef UTILS_DELEGATE_HPP__
#define UTILS_DELEGATE_HPP__ 1

#include "flaglock.hpp"
#include <mutex>
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
		typedef FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		mutex_t mutable m_mutex;
		code_t m_code;
		void* m_self;

	public:
		Delegate( code_t code = 0, void* self = 0 )
			: m_code( code )
			, m_self( self )
		{
		}

		Delegate( Delegate const& other )
		{
			other.get( &m_code, &m_self );
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
			lock_t lock( m_mutex );
			*code = m_code;
			*self = m_self;
		}

		void set( code_t code, void* self = 0 )
		{
			lock_t lock( m_mutex );
			m_code = code;
			m_self = self;
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
	};
}

#endif
