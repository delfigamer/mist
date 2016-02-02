#include "methodlist.hpp"
#include "texture.hpp"
#include "statictexture.hpp"
#include "rendertarget.hpp"
#include "shader.hpp"
#include "meshbuffer.hpp"
#include "meshdata.hpp"
#include "meshshape.hpp"
#include "shapegroup.hpp"
#include "clearshape.hpp"

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
			graphics_shader_new,
			graphics_shader_settexture,
			graphics_shader_setshadersources,
			graphics_meshbuffer_getvertexdata,
			graphics_meshbuffer_setvertexdata,
			graphics_meshbuffer_getindexdata,
			graphics_meshbuffer_setindexdata,
			graphics_meshdata_new,
			graphics_meshdata_trylock,
			graphics_meshdata_unlock,
			graphics_meshshape_new,
			graphics_meshshape_setmeshdata,
			graphics_meshshape_setshader,
			graphics_meshshape_setblendmethod,
			graphics_meshshape_settexture,
			graphics_meshshape_setmatrix,
			graphics_clearshape_new,
			graphics_clearshape_getcolor,
			graphics_clearshape_setcolor,
			graphics_shapegroup_new,
			graphics_shapegroup_insert,
			graphics_shapegroup_remove,
			graphics_shapegroup_setactive,
		};
		return &List;
	}
}
