#ifndef UTILS_METHODLIST_HPP__
#define UTILS_METHODLIST_HPP__ 1

namespace utils
{
	class encoding_t;
	class translation_t;
	class RefObject;
	class DataBuffer;

	struct MethodList
	{
		char const*( *cbase_geterror )();
		void( *cbase_seterror )( char const* error );
		bool( *cbase_write )( char const* str );
		bool( *cbase_getchar )( char* str );
		void( *cbase_yield )();
		encoding_t const*( *encoding_getencoding )( int index );
		int( *encoding_translatestr )( translation_t* translation );
		int( *refobject_addref )( RefObject* ro );
		int( *refobject_release )( RefObject* ro );
		DataBuffer*( *databuffer_new )(
			int length, int capacity, void const* data );
		void*( *databuffer_getdata )( DataBuffer* db );
		int( *databuffer_getlength )( DataBuffer* db );
		bool( *databuffer_setlength )( DataBuffer* db, int length );
		int( *databuffer_getcapacity )( DataBuffer* db );
	};

	MethodList const* getmethodlist();
}

#endif
