#include "opengl.hpp"
#include <utils/strexception.hpp>
#include <utils/console.hpp>
#ifdef _WIN32
#include "EGL/egl.h"
#elif defined(__ANDROID__)
// #include <GLES/gl.h>
#endif

// PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
// PFNGLATTACHSHADERPROC glAttachShader;
// PFNGLCOMPILESHADERPROC glCompileShader;
// PFNGLCREATEPROGRAMPROC glCreateProgram;
// PFNGLCREATESHADERPROC glCreateShader;
// PFNGLDELETEPROGRAMPROC glDeleteProgram;
// PFNGLDELETESHADERPROC glDeleteShader;
// PFNGLDETACHSHADERPROC glDetachShader;
// PFNGLGETPROGRAMIVPROC glGetProgramiv;
// PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
// PFNGLGETSHADERIVPROC glGetShaderiv;
// PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
// PFNGLISPROGRAMPROC glIsProgram;
// PFNGLISSHADERPROC glIsShader;
// PFNGLLINKPROGRAMPROC glLinkProgram;
// PFNGLSHADERSOURCEPROC glShaderSource;
// PFNGLUSEPROGRAMPROC glUseProgram;
// PFNGLVALIDATEPROGRAMPROC glValidateProgram;

namespace graphics {
	bool Shaders_Supported;

	static void const* GetProcAddress( char const* name ) {
		void const* ptr;
		ptr = ( void const* )eglGetProcAddress( name );
		ptrdiff_t ptri = ptrdiff_t( ptr );
		if( ptri == 1 || ptri == 2 || ptri == 3 || ptri == -1 ) {
			ptr = 0;
		}
		return ptr;
	}

#define LoadFunc( type, name ) ( \
	name = ( type )GetProcAddress( #name ), \
	LOG( "%30s: %#10x", #name, uint32_t( name ) ) \
)

	void initializeglstate() {
		LOG( "%#x", uint32_t( ( void const* )glGetString ) );
		auto glGetStringptr = ( char const*( * )( GLenum ) )GetProcAddress( "glGetString" );
		LOG( "%#x", uint32_t( ( void const* )glGetStringptr ) );
		LOG( "OpenGL version: %s", glGetStringptr( GL_VERSION ) );
		LOG( "OpenGL extensions: %s", glGetStringptr( GL_EXTENSIONS ) );
// 		LoadFunc( PFNGLGENERATEMIPMAPPROC, glGenerateMipmap );
// 		LoadFunc( PFNGLATTACHSHADERPROC, glAttachShader );
// 		LoadFunc( PFNGLCOMPILESHADERPROC, glCompileShader );
// 		LoadFunc( PFNGLCREATEPROGRAMPROC, glCreateProgram );
// 		LoadFunc( PFNGLCREATESHADERPROC, glCreateShader );
// 		LoadFunc( PFNGLDELETEPROGRAMPROC, glDeleteProgram );
// 		LoadFunc( PFNGLDELETESHADERPROC, glDeleteShader );
// 		LoadFunc( PFNGLDETACHSHADERPROC, glDetachShader );
// 		LoadFunc( PFNGLGETPROGRAMIVPROC, glGetProgramiv );
// 		LoadFunc( PFNGLGETPROGRAMINFOLOGPROC, glGetProgramInfoLog );
// 		LoadFunc( PFNGLGETSHADERIVPROC, glGetShaderiv );
// 		LoadFunc( PFNGLGETSHADERINFOLOGPROC, glGetShaderInfoLog );
// 		LoadFunc( PFNGLISPROGRAMPROC, glIsProgram );
// 		LoadFunc( PFNGLISSHADERPROC, glIsShader );
// 		LoadFunc( PFNGLLINKPROGRAMPROC, glLinkProgram );
// 		LoadFunc( PFNGLSHADERSOURCEPROC, glShaderSource );
// 		LoadFunc( PFNGLUSEPROGRAMPROC, glUseProgram );
// 		LoadFunc( PFNGLVALIDATEPROGRAMPROC, glValidateProgram );

// 		Shaders_Supported =
// 			glAttachShader;
	}
}
