#include "meshshape.hpp"
#include "common.hpp"
#include <utils/cbase.hpp>

namespace graphics
{
	static int const blendsftable[] =
	{
		GL_ONE,
		GL_SRC_ALPHA,
		GL_ONE,
		GL_ONE,
	};
	static int const blenddftable[] =
	{
		GL_ZERO,
		GL_ONE_MINUS_SRC_ALPHA,
		GL_ONE,
		GL_ONE_MINUS_SRC_COLOR,
	};
	static int const blendoptable[] =
	{
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
		GL_FUNC_ADD,
	};

	void MeshShape::doadvance( int framecount )
	{
		MeshData* md = m_meshdata;
		if( md )
		{
			md->advance( framecount );
		}
		Shader* sh = m_shader;
		if( sh )
		{
			sh->advance( framecount );
		}
	}

	MeshShape::MeshShape()
		: m_blendsf( GL_ONE )
		, m_blenddf( GL_ZERO )
		, m_blendop( GL_FUNC_ADD )
		, m_matrix{
			1, 0, 0, 0,
			0, 1, 0, 0,
			0, 0, 1, 0,
			0, 0, 0, 1,
		}
	{
	}

	MeshShape::~MeshShape()
	{
	}

	void MeshShape::paint()
	{
		MeshData* md = m_meshdata;
		Shader* sh = m_shader;
		if( !md || !sh )
		{
			return;
		}
		int vertexcount;
		int indexcount;
		if( !md->bind( &vertexcount, &indexcount ) )
		{
			return;
		}
		int wvmindex;
		if( !sh->bind( m_textures, &wvmindex ) )
		{
			return;
		}
		{
			lock_t lock( m_mutex );
			glBlendEquation( m_blendop );
			checkerror();
			glBlendFunc( m_blendsf, m_blenddf );
			checkerror();
			glUniformMatrix4fv( wvmindex, 1, false, m_matrix );
			checkerror();
		}
		glDrawElements( GL_TRIANGLES, indexcount, GL_UNSIGNED_SHORT, 0 );
		checkerror();
	}

	void MeshShape::setmeshdata( MeshData* md )
	{
		m_meshdata = md;
	}

	void MeshShape::setshader( Shader* sh )
	{
		m_shader = sh;
	}

	void MeshShape::setblendmethod( int method )
	{
		lock_t lock( m_mutex );
		m_blendsf = TABLE_ASSERT( blendsftable, method, "blend method" );
		m_blenddf = TABLE_ASSERT( blenddftable, method, "blend method" );
		m_blendop = TABLE_ASSERT( blendoptable, method, "blend method" );
	}

	void MeshShape::settexture( int stage, Texture* texture )
	{
		if( stage < 0 || stage >= 8 )
		{
			throw std::runtime_error( "invalid stage index" );
		}
		m_textures[ stage ] = texture;
	}

	void MeshShape::setmatrix( float const* data )
	{
		lock_t lock( m_mutex );
		bool inverse = data[ 11 ] < 0;
		for( int row = 0; row < 4; ++row )
		{
			for( int col = 0; col < 4; ++col )
			{
				m_matrix[ col * 4 + col ] =
					inverse ?
					( - data[ row * 4 + col ] ) :
					data[ row * 4 + col ];
			}
		}
	}

	MeshShape* graphics_meshshape_new() noexcept
	{
	CBASE_PROTECT(
		return new MeshShape();
	)
	}

	bool graphics_meshshape_setmeshdata(
		MeshShape* shape, MeshData* md ) noexcept
	{
	CBASE_PROTECT(
		shape->setmeshdata( md );
		return 1;
	)
	}

	bool graphics_meshshape_setshader(
		MeshShape* shape, Shader* sh ) noexcept
	{
	CBASE_PROTECT(
		shape->setshader( sh );
		return 1;
	)
	}

	bool graphics_meshshape_setblendmethod(
		MeshShape* shape, int method ) noexcept
	{
	CBASE_PROTECT(
		shape->setblendmethod( method );
		return 1;
	)
	}

	bool graphics_meshshape_settexture(
		MeshShape* shape, int stage, Texture* texture ) noexcept
	{
	CBASE_PROTECT(
		shape->settexture( stage, texture );
		return 1;
	)
	}

	bool graphics_meshshape_setmatrix(
		MeshShape* shape, float const* data ) noexcept
	{
	CBASE_PROTECT(
		shape->setmatrix( data );
		return 1;
	)
	}
}
