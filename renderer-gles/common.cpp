#include <renderer-gles/common.hpp>
#include <renderer-gles/gl2.hpp>
#include <unordered_map>

namespace graphics
{
#define ENTRY( name ) { name, #name }
	static std::unordered_map< int, char const* > const errmap = {
		ENTRY( GL_NO_ERROR ),
		ENTRY( GL_INVALID_ENUM ),
		ENTRY( GL_INVALID_VALUE ),
		ENTRY( GL_INVALID_OPERATION ),
		ENTRY( GL_INVALID_FRAMEBUFFER_OPERATION ),
		ENTRY( GL_OUT_OF_MEMORY ),
	};

	void checkerror_pos(
		char const* filename, char const* function, int line )
	{
		int hr = glGetError();
		if( hr )
		{
			char buffer[ 1024 ];
			auto it = errmap.find( hr );
			if( it != errmap.end() )
			{
				snprintf(
					buffer, sizeof( buffer ),
					"GL ES error: %s", it->second );
			}
			else
			{
				snprintf(
					buffer, sizeof( buffer ),
					"GL ES error: %#x", uint32_t( hr ) );
			}
			throw utils::StrException(
				"[%48s:%24s@%4i]\t%s", filename, function, line, buffer );
		}
	}
}
