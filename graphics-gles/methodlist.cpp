#include "methodlist.hpp"

namespace graphics
{
	MethodList const* getmethodlist()
	{
		static MethodList const List = {
			graphics_texture_setminfilter,
			graphics_texture_setmagfilter,
			graphics_texture_setwrapmode,
			graphics_statictexture_new,
			graphics_statictexture_assign,
			graphics_rendertarget_new,
			graphics_rendertarget_setdepthstenciluse,
			graphics_rendertarget_setshape,
			graphics_rendertarget_setclearcolor,
			graphics_rendertarget_setcleardepth,
			graphics_rendertarget_setclearstencil,
			graphics_pixelprogram_new,
			graphics_pixelprogram_setstagetexture,
			graphics_pixelprogram_setstageconstant,
			graphics_pixelprogram_setstagecolor,
			graphics_pixelprogram_setstagealpha,
			graphics_pixelprogram_setstagetexcoord,
			graphics_pixelprogram_setstagematrix,
			graphics_pixelprogram_setmatrix,
			graphics_coloredshape_getcolor,
			graphics_coloredshape_setcolor,
			graphics_clearshape_new,
			graphics_lineshape_new,
			graphics_lineshape_getpos,
			graphics_lineshape_setpos,
			graphics_shapegroup_new,
			graphics_shapegroup_insert,
			graphics_shapegroup_remove,
			graphics_shapegroup_setactive,
			graphics_sprite_setposition,
			graphics_sprite_setextent,
			graphics_sprite_settexregion1,
			graphics_sprite_settexregion2,
			graphics_sprite_setcolor,
			graphics_spritefield_new,
			graphics_spritefield_setpixelprogram,
			graphics_spritefield_newsprite,
			graphics_spritefield_removesprite,
			graphics_spritefield_setblendmethod,
			graphics_spritefield_settexture,
			graphics_spritefield_setmatrix,
		};
		return &List;
	}
}
