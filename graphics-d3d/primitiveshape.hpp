#ifndef GRAPHICS_PRIMITIVESHAPE_HPP__
#define GRAPHICS_PRIMITIVESHAPE_HPP__ 1

#include "shape.hpp"
#include "vertexbuffer.hpp"
#include "indexbuffer.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
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
		utils::Ref< Shader > m_shader;
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
		MeshShape();
		virtual ~MeshShape() override;
		MeshShape( MeshShape const& ) = delete;
		MeshShape& operator=( MeshShape const& ) = delete;

		virtual void paint() override;
		void setvertexbuffer( VertexBuffer* value );
		void setindexbuffer( IndexBuffer* value );
		void setshader( Shader* value );
		void settexture( int index, Texture* texture );
		void settype( int value );
		void setblend( int sfactor, int dfactor, int method );
		void setmatrix( float const* value );
	};

	MeshShape* graphics_meshshape_new() noexcept;
	bool graphics_meshshape_setvertexbuffer(
		MeshShape* shape, VertexBuffer* value ) noexcept;
	bool graphics_meshshape_setindexbuffer(
		MeshShape* shape, IndexBuffer* value ) noexcept;
	bool graphics_meshshape_setshader(
		MeshShape* shape, Shader* value ) noexcept;
	bool graphics_meshshape_settexture(
		MeshShape* shape, int index, Texture* texture ) noexcept;
	bool graphics_meshshape_settype(
		MeshShape* shape, int value ) noexcept;
	bool graphics_meshshape_setblend(
		MeshShape* shape, int sfactor, int dfactor, int method ) noexcept;
	bool graphics_meshshape_setmatrix(
		MeshShape* shape, float const* value ) noexcept;
}

#endif
