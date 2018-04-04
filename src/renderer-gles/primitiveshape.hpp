#pragma once

#include <renderer-gles/shape.hpp>
#include <renderer-gles/vertexbuffer.hpp>
// #include <renderer-gles/indexbuffer.hpp>
#include <renderer-gles/program.hpp>
// #include <renderer-gles/texture.hpp>
#include <common/ref.hpp>
#include <common/flaglock.hpp>
#include <common.hpp>
#include <mutex>

namespace graphics
{
	R_ENUM()
	enum class blendfunc
	{
		add = 0,
		subtract = 1,
		invsubtract = 2,
		invalid = 3,
	};

	R_ENUM()
	enum class blendfactor
	{
		zero = 0,
		one = 1,
		source = 2,
		invsource = 3,
		dest = 4,
		invdest = 5,
		sourcealpha = 6,
		invsourcealpha = 7,
		destalpha = 8,
		invdestalpha = 9,
		invalid = 10,
	};

	R_ENUM()
	enum class primitivetype
	{
		pointlist = 0,
		linestrip = 1,
		linelist = 2,
		trianglestrip = 3,
		trianglefan = 4,
		trianglelist = 5,
		invalid = 6,
	};

	R_CLASS( name = primitiveshape )
	class PrimitiveShape: public Shape
	{
	private:
		typedef FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		AtomicRef< VertexBuffer > m_vertexbuffer;
		// Ref< IndexBuffer > m_indexbuffer;
		AtomicRef< Program > m_program;
		// Ref< Texture > m_textures[ 8 ];
		std::atomic< int > m_type;
		int m_blendsf;
		int m_blenddf;
		int m_blendop;
		mutex_t m_mutex;
		float m_matrix[ 16 ];

	protected:
		virtual void doadvance() override;

	public:
		PrimitiveShape(
			int type, int sourceblend, int destblend, int blendmethod );
		~PrimitiveShape();

		virtual void paint() override;

		R_METHOD() static PrimitiveShape* create(
			int type, int sourceblend, int destblend, int blendmethod )
		{
			return new PrimitiveShape( type, sourceblend, destblend, blendmethod );
		}
		R_METHOD() void setvertexbuffer( VertexBuffer* value );
		// R_METHOD() void setindexbuffer( IndexBuffer* value );
		R_METHOD() void setprogram( Program* value );
		// R_METHOD() void settexture( int index, Texture* texture );
		R_METHOD() void setmatrix( float const* value );
	};
}
