#ifndef GRAPHICS_MESHSHAPE_HPP__
#define GRAPHICS_MESHSHAPE_HPP__ 1

#include "shape.hpp"
#include "meshdata.hpp"
#include "shader.hpp"
#include "texture.hpp"
#include <utils/ref.hpp>
#include <utils/flaglock.hpp>
#include <mutex>

namespace graphics
{
	class MeshShape: public Shape
	{
	public:
		enum
		{
			Method_Replace = 0,
			Method_AlphaBlend = 1,
			Method_Add = 2,
			Method_AddSaturated = 3,
			Method_Invalid = 4,
		};

	private:
		typedef utils::FlagLock mutex_t;
		typedef std::lock_guard< mutex_t > lock_t;

	private:
		utils::Ref< MeshData > m_meshdata;
		utils::Ref< Shader > m_shader;
		mutex_t m_mutex;
		GLenum m_blendsf;
		GLenum m_blenddf;
		GLenum m_blendop;
		utils::Ref< Texture > m_textures[ 8 ];
		GLfloat m_matrix[ 16 ];

	protected:
		virtual void doadvance( int framecount ) override;

	public:
		MeshShape();
		virtual ~MeshShape() override;
		MeshShape( MeshShape const& ) = delete;
		MeshShape& operator=( MeshShape const& ) = delete;

		virtual void paint() override;
		void setmeshdata( MeshData* md );
		void setshader( Shader* sh );
		void setblendmethod( int method );
		void settexture( int stage, Texture* texture );
		void setmatrix( float const* data );
	};

	MeshShape* graphics_meshshape_new() noexcept;
	bool graphics_meshshape_setmeshdata(
		MeshShape* shape, MeshData* md ) noexcept;
	bool graphics_meshshape_setshader(
		MeshShape* shape, Shader* sh ) noexcept;
	bool graphics_meshshape_setblendmethod(
		MeshShape* shape, int method ) noexcept;
	bool graphics_meshshape_settexture(
		MeshShape* shape, int stage, Texture* texture ) noexcept;
	bool graphics_meshshape_setmatrix(
		MeshShape* shape, float const* data ) noexcept;
}

#endif
