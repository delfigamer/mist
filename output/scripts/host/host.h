	typedef int RefObject;
	typedef int DataBuffer;

	typedef int FileIo;
	typedef int FsTask;
	typedef int PngReader;
	typedef int PngWriter;

	typedef int Texture;
	typedef int StaticTexture;
	typedef int RenderTarget;
	typedef int Shader;
	typedef int MeshBuffer;
	typedef int MeshData;
	typedef int Shape;
	typedef int MeshShape;
	typedef int ClearShape;
	typedef int ShapeGroup;

	typedef int Window;

	typedef struct { ptrdiff_t p[ 8 ]; } ShapeGroup_iterator;

	typedef struct
	{
		bool( *encode )( void* dest, uint32_t charcode, size_t destsize, size_t* pointlength );
		bool( *decode )( void const* source, uint32_t* charcode, size_t sourcesize, size_t* pointlength );
	} encoding_t;

	typedef struct
	{
		float pos_x;
		float pos_y;
		float pos_z;
		float tex1_x;
		float tex1_y;
		float tex2_x;
		float tex2_y;
		uint32_t color;
	} Vertex;

	typedef struct
	{
		int width;
		int height;
		float texelsoffset;
		float texeltoffset;
		char const* cmdline;
		bool acceleratorinput;
		bool pointinput;
		bool keyboardinput;
		bool silent;
	} WindowInfo;

	typedef struct
	{
		char const*( *cbase_geterror )();
		void( *cbase_seterror )( char const* error );
		bool( *cbase_write )( char const* str );
		bool( *cbase_getchar )( char* str );
		void( *cbase_yield )();
		encoding_t const*( *encoding_getencoding )( int index );
		size_t( *encoding_translatestr )(
			encoding_t const* senc, encoding_t const* denc,
			void const* source, void* dest, size_t bufsize );
		int( *refobject_addref )( RefObject* ro );
		int( *refobject_release )( RefObject* ro );
		DataBuffer*( *databuffer_new )(
			int length, int capacity, void const* data );
		void*( *databuffer_getdata )( DataBuffer* db );
		int( *databuffer_getlength )( DataBuffer* db );
		bool( *databuffer_setlength )( DataBuffer* db, int length );
		int( *databuffer_getcapacity )( DataBuffer* db );
	} UtilsMethodList;

	typedef struct
	{
		FileIo*( *fileio_new )( char const* path, int mode );
		FsTask*( *fileio_startread )(
			FileIo* io, uint64_t offset, int length, void* buffer );
		FsTask*( *fileio_startwrite )(
			FileIo* io, uint64_t offset, int length,
			void const* buffer );
		bool( *fileio_setsize )( FileIo* fileio, uint64_t size );
		bool( *fstask_promote )( FsTask* task );
		int( *fstask_isfinished )( FsTask* task );
		int( *fstask_getresult )( FsTask* task );
		char const*( *fstask_geterror )( FsTask* task );
		PngReader*( *pngreader_new )( int format );
		bool( *pngreader_delete )( PngReader* reader );
		bool( *pngreader_feed )(
			PngReader* reader,
			int length, void const* buffer );
		int( *pngreader_isfinished )( PngReader* reader );
		int( *pngreader_getwidth )( PngReader* reader );
		int( *pngreader_getheight )( PngReader* reader );
		DataBuffer*( *pngreader_getdata )( PngReader* reader );
		PngWriter*( *pngwriter_new )(
			int format, int width, int height,
			DataBuffer* data );
		bool( *pngwriter_delete )( PngWriter* writer );
		bool( *pngwriter_grab )(
			PngWriter* writer, int length, void* buffer,
			int* result );
		int( *pngwriter_isfinished )( PngWriter* writer );
		int( *fileexists )( char const* path );
		bool( *filesize )( char const* path, uint64_t* size );
	} RsbinMethodList;

	typedef struct
	{
		bool( *texture_setminfilter )( Texture* t, int value );
		bool( *texture_setmagfilter )( Texture* t, int value );
		bool( *texture_setwrapmode )( Texture* t, int ws, int wt );
		StaticTexture*( *statictexture_new )();
		bool( *statictexture_assign )(
			StaticTexture* st,
			int format, DataBuffer* data, int width, int height );
		RenderTarget*( *rendertarget_new )( int format, int width, int height );
		bool( *rendertarget_setdepthstenciluse )( RenderTarget* rt, bool use );
		bool( *rendertarget_setshape )( RenderTarget* rt, Shape* shape );
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
	} GraphicsMethodList;

	typedef struct
	{
		bool( *window_setshape )( Window* window, Shape* shape );
		WindowInfo const*( *window_getinfo )( Window* window );
		bool( *window_finish )( Window* window );
	} WindowMethodList;

	typedef struct
	{
		UtilsMethodList const* utils;
		RsbinMethodList const* rsbin;
		GraphicsMethodList const* graphics;
		WindowMethodList const* window;
	} HostMethodList;
