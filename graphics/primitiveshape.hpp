#ifndef GRAPHICS_PRIMITIVESHAPE_HPP__
#define GRAPHICS_PRIMITIVESHAPE_HPP__ 1

#include <graphics/shape.hpp>
#include <graphics/vertexbuffer.hpp>
// #include <graphics/indexbuffer.hpp>
// #include <graphics/program.hpp>
// #include <graphics/texture.hpp>
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
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
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		utils::Ref< VertexBuffer > m_vertexbuffer;
		// utils::Ref< IndexBuffer > m_indexbuffer;
		// utils::Ref< Program > m_program;
		// utils::Ref< Texture > m_textures[ 8 ];
		std::atomic< int > m_type;
		int m_blendsf;
		int m_blenddf;
		int m_blendop;
		mutex_t m_mutex;
		D3DMATRIX m_matrix;

	protected:
		virtual void doadvance() override;

	public:
		PrimitiveShape(
			int type, int sourceblend, int destblend, int blendmethod );
		virtual ~PrimitiveShape() override;
		PrimitiveShape( PrimitiveShape const& ) = delete;
		PrimitiveShape& operator=( PrimitiveShape const& ) = delete;

		virtual void paint() override;

		R_METHOD() static PrimitiveShape* create(
			int type, int sourceblend, int destblend, int blendmethod )
		{
			return new PrimitiveShape( type, sourceblend, destblend, blendmethod );
		}
		R_METHOD() void setvertexbuffer( VertexBuffer* value );
		// R _METHOD() void setindexbuffer( IndexBuffer* value );
		// R _METHOD() void setprogram( Program* value );
		// R _METHOD() void settexture( int index, Texture* texture );
		R_METHOD() void setmatrix( float const* value );
	};
}

#endif
