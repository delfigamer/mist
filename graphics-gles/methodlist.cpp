#include "methodlist.hpp"
// #include "texture.hpp"
// #include "statictexture.hpp"
// #include "rendertarget.hpp"
// #include "shader.hpp"
// #include "meshbuffer.hpp"
// #include "meshdata.hpp"
// #include "meshshape.hpp"
// #include "shapegroup.hpp"
#include "clearshape.hpp"

namespace graphics
{
	MethodList const* getmethodlist()
	{
		static MethodList const List = {
			0, // graphics_texture_setminfilter,
			0, // graphics_texture_setmagfilter,
			0, // graphics_texture_setwrapmode,
			0, // graphics_statictexture_new,
			0, // graphics_statictexture_assign,
			0, // graphics_rendertarget_new,
			0, // graphics_rendertarget_setdepthstenciluse,
			0, // graphics_rendertarget_setshape,
			0, // graphics_rendertarget_setclearcolor,
			0, // graphics_rendertarget_setcleardepth,
			0, // graphics_rendertarget_setclearstencil,
			0, // graphics_shader_new,
			0, // graphics_shader_settexture,
			0, // graphics_shader_setshadersources,
			0, // graphics_meshbuffer_getvertexdata,
			0, // graphics_meshbuffer_setvertexdata,
			0, // graphics_meshbuffer_getindexdata,
			0, // graphics_meshbuffer_setindexdata,
			0, // graphics_meshdata_new,
			0, // graphics_meshdata_trylock,
			0, // graphics_meshdata_unlock,
			0, // graphics_meshshape_new,
			0, // graphics_meshshape_setmeshdata,
			0, // graphics_meshshape_setshader,
			0, // graphics_meshshape_setblendmethod,
			0, // graphics_meshshape_settexture,
			0, // graphics_meshshape_setmatrix,
			graphics_clearshape_new,
			graphics_clearshape_getcolor,
			graphics_clearshape_setcolor,
			0, // graphics_shapegroup_new,
			0, // graphics_shapegroup_insert,
			0, // graphics_shapegroup_remove,
			0, // graphics_shapegroup_setactive,
		};
		return &List;
	}
}
