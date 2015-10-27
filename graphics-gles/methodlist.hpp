#ifndef GRAPHICS_METHODLIST_HPP__
#define GRAPHICS_METHODLIST_HPP__ 1

#include "texture.hpp"
#include "statictexture.hpp"
#include "rendertarget.hpp"
#include "pixelprogram.hpp"
#include "shape.hpp"
#include "shapegroup.hpp"
#include "coloredshape.hpp"
#include "lineshape.hpp"
#include "clearshape.hpp"
#include "sprite.hpp"
#include "spritefield.hpp"
#include <utils/stringinstance.hpp>

namespace graphics {
	struct MethodList {
		typedef utils::StringInstance StringInstance;
		
		bool( *texture_setminfilter )( Texture* t, int value );
		bool( *texture_setmagfilter )( Texture* t, int value );
		bool( *texture_setwrapmode )( Texture* t, int ws, int wt );
		StaticTexture*( *statictexture_new )();
		bool( *statictexture_assign )( StaticTexture* st, StringInstance* data, int width, int height );
		RenderTarget*( *rendertarget_new )( int width, int height );
		bool( *rendertarget_setdepthstenciluse )( RenderTarget* rt, bool use );
		bool( *rendertarget_setshape )( RenderTarget* rt, Shape* shape );
		bool( *rendertarget_setclearcolor )( RenderTarget* rt, bool flag, float const* value );
		bool( *rendertarget_setcleardepth )( RenderTarget* rt, bool flag, float value );
		bool( *rendertarget_setclearstencil )( RenderTarget* rt, bool flag, int value );
		PixelProgram*( *pixelprogram_new )();
		bool( *pixelprogram_setstagetexture )( PixelProgram* pp, int stage, Texture* texture );
		bool( *pixelprogram_setstageconstant )( PixelProgram* pp, int stage, float const* constant );
		bool( *pixelprogram_setstagecolor )( PixelProgram* pp, int stage, int op, int arg1, int arg2 );
		bool( *pixelprogram_setstagealpha )( PixelProgram* pp, int stage, int op, int arg1, int arg2 );
		bool( *pixelprogram_setstagetexcoord )( PixelProgram* pp, int stage, int index );
		bool( *pixelprogram_setstagematrix )( PixelProgram* pp, int stage, int index );
		bool( *pixelprogram_setmatrix )( PixelProgram* pp, int index, float const* data );
		bool( *coloredshape_getcolor )( ColoredShape* shape, float* color );
		bool( *coloredshape_setcolor )( ColoredShape* shape, float const* color );
		ClearShape*( *clearshape_new )();
		LineShape*( *lineshape_new )();
		bool( *lineshape_getpos )( LineShape* shape, float* pos );
		bool( *lineshape_setpos )( LineShape* shape, float const* pos );
		ShapeGroup*( *shapegroup_new )();
		bool( *shapegroup_insert )( ShapeGroup* shape, Shape* item, int order, ShapeGroup_iterator* it );
		bool( *shapegroup_remove )( ShapeGroup* shape, ShapeGroup_iterator* it );
// 		bool( *shapegroup_begin )( ShapeGroup* shape, ShapeGroup_iterator* it );
// 		bool( *shapegroup_end )( ShapeGroup* shape, ShapeGroup_iterator* it );
// 		bool( *shapegroup_iterator_inc )( ShapeGroup_iterator* it );
// 		int( *shapegroup_iterator_eq )( ShapeGroup_iterator* a, ShapeGroup_iterator* b );
// 		bool( *shapegroup_iterator_deref )( ShapeGroup_iterator* it, Shape** item, int* order );
// 		bool( *shapegroup_iterator_close )( ShapeGroup_iterator* it );
		bool( *shapegroup_setactive )( ShapeGroup* shape, bool active );
		bool( *sprite_setposition )( Sprite* sprite, float x, float y );
		bool( *sprite_setextent )( Sprite* sprite, float x1, float y1, float x2, float y2 );
		bool( *sprite_settexregion1 )( Sprite* sprite, float x1, float y1, float x2, float y2 );
		bool( *sprite_settexregion2 )( Sprite* sprite, float x1, float y1, float x2, float y2 );
		bool( *sprite_setcolor )( Sprite* sprite, float const* color );
		SpriteField*( *spritefield_new )();
		bool( *spritefield_setpixelprogram )( SpriteField* sf, PixelProgram* pixelprogram );
		Sprite*( *spritefield_newsprite )( SpriteField* sf, int order );
		bool( *spritefield_removesprite )( SpriteField* sf, Sprite* sprite );
		bool( *spritefield_setblendmethod )( SpriteField* sf, int method );
		bool( *spritefield_settexture )( SpriteField* sf, int stage, Texture* texture );
		bool( *spritefield_setmatrix )( SpriteField* sf, float const* data );
	};
	
	MethodList const* getmethodlist();
}

#endif
