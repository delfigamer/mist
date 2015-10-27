#include "common.hpp"
#include <stdexcept>

namespace graphics {
	void checkerror() {
		char const* error = 0;
		switch( glGetError() ) {
		case GL_NO_ERROR:
			return;
			
		case GL_INVALID_ENUM:
			error = "invalid enum";
			break;
			
		case GL_INVALID_VALUE:
			error = "invalid value";
			break;
			
		case GL_INVALID_OPERATION:
			error = "invalid operation";
			break;
			
		case GL_STACK_OVERFLOW:
			error = "stack overflow";
			break;
			
		case GL_STACK_UNDERFLOW:
			error = "stack underflow";
			break;
			
		case GL_OUT_OF_MEMORY:
			error = "out of memory";
			break;
			
		default:
			error = "unknown error";
		}
		while( glGetError() ) {
		}
		if( error ) {
			throw std::runtime_error( error );
		}
	}
}
