
namespace utils
{
	class DataBuffer;
	struct encoding_t;
	struct translation_t;
	class RefObject;
}

struct utils_methodlist_t
{
	bool(*utils_cbase_geterror)(char const** result) noexcept;
	bool(*utils_cbase_seterror)(char const* error) noexcept;
	bool(*utils_cbase_yield)() noexcept;
	bool(*utils_cbase_write)(char const* str) noexcept;
	bool(*utils_cbase_getchar)(char* str) noexcept;
	bool(*utils_databuffer_new)(int length, int capacity, void const* data, utils::DataBuffer** result) noexcept;
	bool(*utils_databuffer_getdata)(utils::DataBuffer* self, void** result) noexcept;
	bool(*utils_databuffer_getlength)(utils::DataBuffer* self, int* result) noexcept;
	bool(*utils_databuffer_setlength)(utils::DataBuffer* self, int length) noexcept;
	bool(*utils_databuffer_getcapacity)(utils::DataBuffer* self, int* result) noexcept;
	bool(*utils_getencoding)(int index, utils::encoding_t const** result) noexcept;
	bool(*utils_translatestr)(utils::translation_t* translation, int* result) noexcept;
	bool(*utils_refobject_addref)(utils::RefObject const* self, int* result) noexcept;
	bool(*utils_refobject_release)(utils::RefObject const* self, int* result) noexcept;
};

extern utils_methodlist_t const utils_methodlist;
