#ifndef GRAPHICS_METHODLIST_HPP__
#define GRAPHICS_METHODLIST_HPP__ 1

namespace utils
{
	class DataBuffer;
}

namespace graphics
{
	class Texture;
	class StaticTexture;
	class RenderTarget;
	class Shader;
	class MeshBuffer;
	class MeshData;
	class Shape;
	class MeshShape;
	class ClearShape;
	class ShapeGroup;
	class ShapeGroup_iterator;

	struct MethodList
	{
		typedef utils::DataBuffer DataBuffer;

		bool( *texture_setminfilter )( Texture* t, int value );
		bool( *texture_setmagfilter )( Texture* t, int value );
		bool( *texture_setwrapmode )( Texture* t, int ws, int wt );
		StaticTexture*( *statictexture_new )();
		bool( *statictexture_assign )(
			StaticTexture* st,
			int format, DataBuffer* data, int width, int height );
		RenderTarget*( *rendertarget_new )( int format, int width, int height );
		bool( *rendertarget_setdepthstenciluse )(
			RenderTarget* rt, bool use );
		bool( *rendertarget_setshape )(
			RenderTarget* rt, Shape* shape );
		bool( *rendertarget_setclearcolor )(
			RenderTarget* rt, bool flag, float const* value );
		bool( *rendertarget_setcleardepth )(
			RenderTarget* rt, bool flag, float value );
		bool( *rendertarget_setclearstencil )(
			RenderTarget* rt, bool flag, int value );
		Shader*( *shader_new )();
		bool( *shader_settexture )( Shader* sh, int stage, Texture* texture );
		bool( *shader_setshadersources )(
			Shader* sh,
			int format, char const* vert, char const* frag, char const* texnames );
		DataBuffer*( *meshbuffer_getvertexdata )( MeshBuffer* mb );
		bool( *meshbuffer_setvertexdata )( MeshBuffer* mb, DataBuffer* db );
		DataBuffer*( *meshbuffer_getindexdata )( MeshBuffer* mb );
		bool( *meshbuffer_setindexdata )( MeshBuffer* mb, DataBuffer* db );
		MeshData*( *meshdata_new )();
		int( *meshdata_trylock )( MeshData* md, MeshBuffer** pmb );
		bool( *meshdata_unlock )( MeshData* md, MeshBuffer* mb );
		MeshShape*( *meshshape_new )();
		bool( *meshshape_setmeshdata )( MeshShape* shape, MeshData* md );
		bool( *meshshape_setshader )( MeshShape* shape, Shader* sh );
		bool( *meshshape_setblendmethod )( MeshShape* shape, int method );
		bool( *meshshape_settexture )(
			MeshShape* shape, int stage, Texture* texture );
		bool( *meshshape_setmatrix )( MeshShape* shape, float const* data );
		ClearShape*( *clearshape_new )();
		bool( *clearshape_getcolor )( ClearShape* shape, float* color );
		bool( *clearshape_setcolor )( ClearShape* shape, float const* color );
		ShapeGroup*( *shapegroup_new )();
		bool( *shapegroup_insert )(
			ShapeGroup* shape, Shape* item, int order, ShapeGroup_iterator* it );
		bool( *shapegroup_remove )( ShapeGroup* shape, ShapeGroup_iterator* it );
		bool( *shapegroup_setactive )( ShapeGroup* shape, bool active );
	};

	MethodList const* getmethodlist();
}

#endif
