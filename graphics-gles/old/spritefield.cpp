#include "spritefield.hpp"
#include "common.hpp"
#include <utils/profile.hpp>
#include <utils/cbase.hpp>
#include <stdexcept>

namespace graphics {
	static void fillvertexquad( Sprite::Data const& sdata, SpriteField::Vertex* vertices ) {
		vertices[ 0 ].pos_x = sdata.pos_x - sdata.ext_x1 - sdata.ext_x2;
		vertices[ 0 ].pos_y = sdata.pos_y - sdata.ext_y1 - sdata.ext_y2;
		vertices[ 0 ].tex_x = sdata.tex_x1;
		vertices[ 0 ].tex_y = sdata.tex_y1;

		vertices[ 1 ].pos_x = sdata.pos_x + sdata.ext_x1 - sdata.ext_x2;
		vertices[ 1 ].pos_y = sdata.pos_y + sdata.ext_y1 - sdata.ext_y2;
		vertices[ 1 ].tex_x = sdata.tex_x2;
		vertices[ 1 ].tex_y = sdata.tex_y1;

		vertices[ 2 ].pos_x = sdata.pos_x + sdata.ext_x1 + sdata.ext_x2;
		vertices[ 2 ].pos_y = sdata.pos_y + sdata.ext_y1 + sdata.ext_y2;
		vertices[ 2 ].tex_x = sdata.tex_x2;
		vertices[ 2 ].tex_y = sdata.tex_y2;

		vertices[ 3 ].pos_x = sdata.pos_x - sdata.ext_x1 + sdata.ext_x2;
		vertices[ 3 ].pos_y = sdata.pos_y - sdata.ext_y1 + sdata.ext_y2;
		vertices[ 3 ].tex_x = sdata.tex_x1;
		vertices[ 3 ].tex_y = sdata.tex_y2;
	}

	bool SpriteField::SpriteComparer::operator()( Sprite* lhs, Sprite* rhs ) const {
		return *lhs < *rhs;
	}

	bool SpriteField::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	SpriteField::SpriteField() {
	}

	SpriteField::~SpriteField() {
	}

	void SpriteField::paint() {
		std::lock_guard< Monitor > _lock_guard( *this );
	PROFILE( "paint",
		Texture* texture = m_texture;
		if( !texture ) {
			return;
		}
		glEnableVertexAttribArray( 0 );	checkerror();
		glEnableVertexAttribArray( 1 );	checkerror();
		glEnable( GL_TEXTURE_2D );	checkerror();
		texture->bind();
// 		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );	checkerror();
		Vertex* vertices = m_vertices.data();
		glVertexAttribPointer( 0, 2, GL_FLOAT, false, sizeof( Vertex ), &vertices[ 0 ].pos_x );	checkerror();
		glVertexAttribPointer( 1, 2, GL_FLOAT, false, sizeof( Vertex ), &vertices[ 0 ].tex_x );	checkerror();
		GLshort* indices = m_indices.data();
		glDrawElements( GL_TRIANGLES, m_indices.size(), GL_UNSIGNED_SHORT, indices );	checkerror();
		texture->unbind();
		glDisable( GL_TEXTURE_2D );	checkerror();
		glDisableVertexAttribArray( 0 );	checkerror();
		glDisableVertexAttribArray( 1 );	checkerror();
	)
	}

	void SpriteField::advance() {
		std::lock_guard< Monitor > _lock_guard( *this );
	PROFILE( "advance",
		{
			lock_t _lock_guard( m_mutex );
			while( !m_newsprites.empty() ) {
				m_sprites.insert( m_newsprites.top() );
				m_newsprites.pop();
			}
		}
		Texture* texture = m_texture;
		if( texture ) {
			texture->advance();
		}
		int count = m_sprites.size();
		m_vertices.resize( count * 4 );
		m_indices.resize( count * 6 );
		Vertex* vertices = m_vertices.data();
		GLshort* indices = m_indices.data();
		auto it = m_sprites.begin();
		Sprite::Data data;
		for( int i = 0; i < count; ++i ) {
			( *it )->getdata( &data );
			fillvertexquad( data, vertices + ( i << 2 ) );
			GLshort* qi = indices + i * 6;
			qi[ 0 ] = ( i << 2 );
			qi[ 1 ] = ( i << 2 ) | 1;
			qi[ 2 ] = ( i << 2 ) | 2;
			qi[ 3 ] = ( i << 2 );
			qi[ 4 ] = ( i << 2 ) | 2;
			qi[ 5 ] = ( i << 2 ) | 3;
			++it;
		}
	)
	}

	Texture* SpriteField::gettexture() const {
		return m_texture;
	}

	void SpriteField::settexture( Texture* texture ) {
		m_texture = texture;
	}

	Sprite* SpriteField::newsprite( int order ) {
	PROFILE( "newsprite",
		lock_t _lock_guard( m_mutex );
		Sprite* sprite = new Sprite( order );
		m_newsprites.push( sprite );
		return sprite;
	)
	}

	void SpriteField::removesprite( Sprite* sprite ) {
		bool success;
		{
			std::lock_guard< Monitor > _lock_guard( *this );
			success = m_sprites.erase( sprite ) != 0;
		}
		if( !success ) {
			throw std::runtime_error( "passed sprite does not belong to this field" );
		}
		delete sprite;
	}

	extern "C" {
		SpriteField* graphics_spritefield_new() noexcept {
		CBASE_PROTECT(
			return new SpriteField();
		)
		}

		Texture* graphics_spritefield_gettexture( SpriteField* sf ) noexcept {
		CBASE_PROTECT(
			return sf->gettexture();
		)
		}

		bool graphics_spritefield_settexture( SpriteField* sf, Texture* texture ) noexcept {
		CBASE_PROTECT(
			sf->settexture( texture );
			return 1;
		)
		}

		Sprite* graphics_spritefield_newsprite( SpriteField* sf, int order ) noexcept {
		CBASE_PROTECT(
			return sf->newsprite( order );
		)
		}

		bool graphics_spritefield_removesprite( SpriteField* sf, Sprite* sprite ) noexcept {
		CBASE_PROTECT(
			sf->removesprite( sprite );
			return 1;
		)
		}
	}
}
