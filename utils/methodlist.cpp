#include "methodlist.hpp"
#include "cbase.hpp"
#include "encoding.hpp"
#include "refobject.hpp"
#include "databuffer.hpp"

namespace utils {
	MethodList const* getmethodlist() {
		static MethodList const List = {
			cbase_geterror,
			cbase_seterror,
			cbase_write,
			cbase_getchar,
			cbase_yield,
			utils_encoding_getencoding,
			utils_encoding_translatestr,
			utils_refobject_addref,
			utils_refobject_release,
			utils_databuffer_new,
			utils_databuffer_getdata,
			utils_databuffer_getlength,
			utils_databuffer_setlength,
			utils_databuffer_getcapacity,
		};
		return &List;
	}
}
