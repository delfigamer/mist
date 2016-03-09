#ifndef GRAPHICS_METHODLIST_HPP__
#define GRAPHICS_METHODLIST_HPP__ 1

#include "texture.hpp"
#include "statictexture.hpp"
#include "shader.hpp"
#include "program.hpp"
#include "shape.hpp"
#include "shapegroup.hpp"
#include "coloredshape.hpp"
#include "lineshape.hpp"
#include "clearshape.hpp"
#include "sprite.hpp"
#include "spritefield.hpp"

namespace graphics {
	struct MethodList {
		typedef utils::StringInstance StringInstance;
		
		bool( *texture_setminfilter )( Texture* t, int value );
		bool( *texture_setmagfilter )( Texture* t, int value );
		bool( *texture_setwrapmode )( Texture* t, int ws, int wt );
		StaticTexture*( *statictexture_new )();
		bool( *statictexture_assign )( StaticTexture* st, StringInstance* data, int width, int height );
		bool( *coloredshape_getcolor )( ColoredShape* shape, GLfloat* color );
		bool( *coloredshape_setcolor )( ColoredShape* shape, GLfloat const* color );
		ClearShape*( *clearshape_new )();
		LineShape*( *lineshape_new )();
		bool( *lineshape_getpos )( LineShape* shape, GLfloat* pos );
		bool( *lineshape_setpos )( LineShape* shape, GLfloat const* pos );
		ShapeGroup*( *shapegroup_new )();
		bool( *shapegroup_insert )( ShapeGroup* shape, Shape* item, int order, ShapeGroup_iterator* it );
		bool( *shapegroup_remove )( ShapeGroup* shape, ShapeGroup_iterator* it );
		bool( *shapegroup_begin )( ShapeGroup* shape, ShapeGroup_iterator* it );
		bool( *shapegroup_end )( ShapeGroup* shape, ShapeGroup_iterator* it );
		bool( *shapegroup_iterator_inc )( ShapeGroup_iterator* it );
		int( *shapegroup_iterator_eq )( ShapeGroup_iterator* a, ShapeGroup_iterator* b );
		bool( *shapegroup_iterator_deref )( ShapeGroup_iterator* it, Shape** item, int* order );
		bool( *shapegroup_iterator_close )( ShapeGroup_iterator* it );
		bool( *shapegroup_setprogram )( ShapeGroup* shape, Program* program );
		bool( *sprite_setposition )( Sprite* sprite, GLfloat x, GLfloat y );
		bool( *sprite_setextent )( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
		bool( *sprite_settexregion )( Sprite* sprite, GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2 );
		SpriteField*( *spritefield_new )();
		Texture*( *spritefield_gettexture )( SpriteField* sf );
		bool( *spritefield_settexture )( SpriteField* sf, Texture* texture );
		Sprite*( *spritefield_newsprite )( SpriteField* sf, int order );
		bool( *spritefield_removesprite )( SpriteField* sf, Sprite* sprite );
	};
	
	MethodList const* getmethodlist();
}

#endif
