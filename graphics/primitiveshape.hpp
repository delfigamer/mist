#ifndef GRAPHICS_PRIMITIVESHAPE_HPP__
#define GRAPHICS_PRIMITIVESHAPE_HPP__ 1

#include <graphics/shape.hpp>
#include <graphics/vertexbuffer.hpp>
#include <graphics/indexbuffer.hpp>
#include <graphics/program.hpp>
#include <graphics/texture.hpp>
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
#include <common.hpp>
#include <mutex>

namespace graphics
{
	R_CLASS( name = primitiveshape )
	class PrimitiveShape: public Shape
	{
	public:
		enum
		{
			Method_Add = 0,
			Method_Subtract = 1,
			Method_RevSubtract = 2,
			Method_Invalid = 3,
			Factor_Zero = 0,
			Factor_One = 1,
			Factor_Source = 2,
			Factor_InvSource = 3,
			Factor_Dest = 4,
			Factor_InvDest = 5,
			Factor_SourceAlpha = 6,
			Factor_InvSourceAlpha = 7,
			Factor_DestAlpha = 8,
			Factor_InvDestAlpha = 9,
			Factor_Invalid = 10,
			Primitive_PointList = 0,
			Primitive_LineStrip = 1,
			Primitive_LineList = 2,
			Primitive_TriangleStrip = 3,
			Primitive_TriangleFan = 4,
			Primitive_TriangleList = 5,
			Primitive_Invalid = 6,
		};

	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		utils::Ref< VertexBuffer > m_vertexbuffer;
		utils::Ref< IndexBuffer > m_indexbuffer;
		utils::Ref< Program > m_program;
		utils::Ref< Texture > m_textures[ 8 ];
		mutex_t m_mutex;
		int m_type;
		int m_blendsf;
		int m_blenddf;
		int m_blendop;
		D3DMATRIX m_matrix;

	protected:
		virtual void doadvance() override;

	public:
		PrimitiveShape();
		virtual ~PrimitiveShape() override;
		PrimitiveShape( PrimitiveShape const& ) = delete;
		PrimitiveShape& operator=( PrimitiveShape const& ) = delete;

		virtual void paint() override;

		R_METHOD() static PrimitiveShape* create()
		{
			return new PrimitiveShape();
		}
		R_METHOD() void setvertexbuffer( VertexBuffer* value );
		R_METHOD() void setindexbuffer( IndexBuffer* value );
		R_METHOD() void setprogram( Program* value );
		// R _METHOD() void settexture( int index, Texture* texture );
		R_METHOD() void settype( int value );
		R_METHOD() void setblend( int sfactor, int dfactor, int method );
		R_METHOD() void setmatrix( float const* value );
	};
}

#endif
