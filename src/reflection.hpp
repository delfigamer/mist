#include <common.hpp>
#include <exception>
#include <memory>

namespace r
{
	static_assert( sizeof( std::exception_ptr ) <= sizeof( void*[ 4 ] ) );

	struct exceptionbox;

	extern "C" typedef void exceptionbox_free_t(
		exceptionbox* self );
	extern "C" typedef char const* exceptionbox_what_t(
		exceptionbox* self );

	struct exceptionbox
	{
		exceptionbox_free_t* freeptr;
		exceptionbox_what_t* whatptr;
		union
		{
			std::exception_ptr eptr;
			void* data[ 4 ];
		};

		static void free( exceptionbox* self ) noexcept;
		static char const* what( exceptionbox* self ) noexcept;
		static void make( exceptionbox* self ) noexcept;
	};

	struct module
	{
		void const* ptr;
		void const* chunkbytes;
		size_t chunksize;
		char const* chunkname;
	};

	extern "C"
	{
		inline void exceptionbox::free( exceptionbox* self ) noexcept
		{
			self->eptr.~exception_ptr();
		}

		inline char const* exceptionbox::what( exceptionbox* self ) noexcept
		{
			try
			{
				std::rethrow_exception( self->eptr );
			}
			catch( std::exception const& e )
			{
				return e.what();
			}
		}
	}

	inline void exceptionbox::make( exceptionbox* self ) noexcept
	{
		self->freeptr = &exceptionbox::free;
		self->whatptr = &exceptionbox::what;
		new( &self->eptr )std::exception_ptr();
		self->eptr = std::current_exception();
	}
}
