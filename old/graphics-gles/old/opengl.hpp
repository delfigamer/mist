#ifndef GRAPHICS_OPENGL_H__
#define GRAPHICS_OPENGL_H__ 1

#ifdef _WIN32
#include "GLES2/gl2.h"
#include "GLES2/gl2ext.h"
#elif defined(__ANDROID__)
#include <GLES2/gl.h>
#include <GLES2/glext.h>
#else
#error "Unsupported target platform"
#endif

// extern PFNGLGENERATEMIPMAPPROC glGenerateMipmap;
// extern PFNGLATTACHSHADERPROC glAttachShader;
// extern PFNGLCOMPILESHADERPROC glCompileShader;
// extern PFNGLCREATEPROGRAMPROC glCreateProgram;
// extern PFNGLCREATESHADERPROC glCreateShader;
// extern PFNGLDELETEPROGRAMPROC glDeleteProgram;
// extern PFNGLDELETESHADERPROC glDeleteShader;
// extern PFNGLDETACHSHADERPROC glDetachShader;
// extern PFNGLGETPROGRAMIVPROC glGetProgramiv;
// extern PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog;
// extern PFNGLGETSHADERIVPROC glGetShaderiv;
// extern PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog;
// extern PFNGLISPROGRAMPROC glIsProgram;
// extern PFNGLISSHADERPROC glIsShader;
// extern PFNGLLINKPROGRAMPROC glLinkProgram;
// extern PFNGLSHADERSOURCEPROC glShaderSource;
// extern PFNGLUSEPROGRAMPROC glUseProgram;
// extern PFNGLVALIDATEPROGRAMPROC glValidateProgram;

namespace graphics {
	extern bool Shaders_Supported;
	void initializeglstate();
}

#endif
