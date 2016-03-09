#include "methodlist.hpp"

namespace graphics {
	MethodList const* getmethodlist() {
		static MethodList const List = {
			graphics_texture_setminfilter,
			graphics_texture_setmagfilter,
			graphics_texture_setwrapmode,
			graphics_statictexture_new,
			graphics_statictexture_assign,
			graphics_coloredshape_getcolor,
			graphics_coloredshape_setcolor,
			graphics_clearshape_new,
			graphics_lineshape_new,
			graphics_lineshape_getpos,
			graphics_lineshape_setpos,
			graphics_shapegroup_new,
			graphics_shapegroup_insert,
			graphics_shapegroup_remove,
			graphics_shapegroup_begin,
			graphics_shapegroup_end,
			graphics_shapegroup_iterator_inc,
			graphics_shapegroup_iterator_eq,
			graphics_shapegroup_iterator_deref,
			graphics_shapegroup_iterator_close,
			graphics_shapegroup_setprogram,
			graphics_sprite_setposition,
			graphics_sprite_setextent,
			graphics_sprite_settexregion,
			graphics_spritefield_new,
			graphics_spritefield_gettexture,
			graphics_spritefield_settexture,
			graphics_spritefield_newsprite,
			graphics_spritefield_removesprite
		};
		return &List;
	}
}
