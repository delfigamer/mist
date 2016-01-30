#include "common.hpp"
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#include <unordered_map>
#include <GLES2/gl2.h>

namespace graphics
{
#define ENTRY( name ) { name, #name }
	static std::unordered_map< GLenum, char const* > const errmap = {
		ENTRY( GL_NO_ERROR ),
		ENTRY( GL_INVALID_ENUM ),
		ENTRY( GL_INVALID_FRAMEBUFFER_OPERATION ),
		ENTRY( GL_INVALID_VALUE ),
		ENTRY( GL_INVALID_OPERATION ),
		ENTRY( GL_OUT_OF_MEMORY ),
	};

	void checkerror_pos( char const* filename, char const* function, int line )
	{
		GLenum err = glGetError();
		if( err != GL_NO_ERROR )
		{
			char buffer[ 1024 ];
			auto it = errmap.find( err );
			if( it != errmap.end() )
			{
				snprintf( buffer, sizeof( buffer ), "OpenGL ES error: %s", it->second );
			}
			else
			{
				snprintf( buffer, sizeof( buffer ), "OpenGL ES error: %#x", uint32_t( err ) );
			}
			utils::Console->writeln(
				"[%48s:%24s@%4i]\t%s", filename, function, line, buffer );
			throw utils::StrException( buffer );
		}
	}
}
