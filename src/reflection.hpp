#include <common/ref.hpp>
#include <common.hpp>
#include <exception>
#include <memory>

namespace r
{
	struct module
	{
		void const* ptr;
		void const* chunkbytes;
		size_t chunksize;
		char const* chunkname;
	};

	static_assert( sizeof( std::exception_ptr ) <= sizeof( void*[ 4 ] ) );

	struct exceptionbox;

	extern "C" typedef void exceptionbox_free_t( exceptionbox* self );
	extern "C" typedef char const* exceptionbox_what_t( exceptionbox* self );

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

	struct objectbox;

	extern "C" typedef void objectbox_cfree_t( objectbox* self );
	typedef void objectbox_free_t( objectbox* self );

	struct objectbox
	{
		void* object;
		objectbox_cfree_t* cfreeptr;
		objectbox_free_t* freeptr;

		static void cfree( objectbox* self ) noexcept;
	};

	extern "C"
	{
		inline void objectbox::cfree( objectbox* self ) noexcept
		{
			self->freeptr( self );
		}
	}

	template< typename T >
	objectbox maketrivialbox( T* ptr )
	{
		return objectbox{
			ptr,
			nullptr,
			nullptr };
	}

	template< typename T >
	objectbox makeuptrbox( std::unique_ptr< T >&& ptr )
	{
		if( ptr )
		{
			return objectbox{
				ptr.release(),
				&objectbox::cfree,
				[]( objectbox* self )
				{
					delete ( T* )self->object;
				} };
		}
		else
		{
			return objectbox{ nullptr };
		}
	}

	template< typename T >
	objectbox makerefbox( Ref< T >&& ref )
	{
		if( ref )
		{
			return objectbox{
				ref.detach(),
				&objectbox::cfree,
				[]( objectbox* self )
				{
					::release( ( T* )self->object );
				} };
		}
		else
		{
			return objectbox{ nullptr };
		}
	}

	template< typename T >
	objectbox makerefbox( Ref< T > const& ref )
	{
		if( ref )
		{
			return objectbox{
				ref.share(),
				&objectbox::cfree,
				[]( objectbox* self )
				{
					::release( ( T* )self->object );
				} };
		}
		else
		{
			return objectbox{ nullptr };
		}
	}
}
