#pragma once

#include <common.hpp>
#include <cinttypes>

namespace utils
{
	struct cbase;
	class ConfigSet;
	struct encoding_t;
	struct translation_t;
	struct Encoding;
	class RefObject;
	class DataBuffer;
}
namespace rsbin
{
	class Common;
	class FsTask;
	class FileIo;
	class PngReader;
	class PngWriter;
}
namespace window
{
	class Window;
}

struct methodlist_t
{
	bool(*utils_cbase_geterror)(char const** result) NOEXCEPT;
	bool(*utils_cbase_seterror)(char const* error) NOEXCEPT;
	bool(*utils_cbase_yield)() NOEXCEPT;
	bool(*utils_cbase_write)(char const* str) NOEXCEPT;
	bool(*utils_cbase_getchar)(char* str) NOEXCEPT;
	bool(*utils_configset_integer)(::utils::ConfigSet const* self, char const* expr, int def, int* result) NOEXCEPT;
	bool(*utils_configset_number)(::utils::ConfigSet const* self, char const* expr, double def, double* result) NOEXCEPT;
	bool(*utils_configset_lstring)(::utils::ConfigSet const* self, char const* expr, char* buffer, int buflen, int* result) NOEXCEPT;
	bool(*utils_configset_boolean)(::utils::ConfigSet const* self, char const* expr, bool def, bool* result) NOEXCEPT;
	bool(*utils_databuffer_pcreate)(int length, int capacity, void const* data, ::utils::DataBuffer** result) NOEXCEPT;
	bool(*utils_databuffer_getdata)(::utils::DataBuffer* self, void** result) NOEXCEPT;
	bool(*utils_databuffer_getlength)(::utils::DataBuffer* self, int* result) NOEXCEPT;
	bool(*utils_databuffer_setlength)(::utils::DataBuffer* self, int length) NOEXCEPT;
	bool(*utils_databuffer_getcapacity)(::utils::DataBuffer* self, int* result) NOEXCEPT;
	bool(*utils_encoding_getencoding)(int index, ::utils::encoding_t const** result) NOEXCEPT;
	bool(*utils_encoding_translatestr)(::utils::translation_t* translation, int* result) NOEXCEPT;
	bool(*utils_refobject_addref)(::utils::RefObject const* self) NOEXCEPT;
	bool(*utils_refobject_release)(::utils::RefObject const* self) NOEXCEPT;
	bool(*rsbin_rsbin_isfileavailable)(char const* path, bool* result) NOEXCEPT;
	bool(*rsbin_fstask_promote)(::rsbin::FsTask* self) NOEXCEPT;
	bool(*rsbin_fstask_isfinished)(::rsbin::FsTask* self, bool* result) NOEXCEPT;
	bool(*rsbin_fstask_getresult)(::rsbin::FsTask* self, int* result) NOEXCEPT;
	bool(*rsbin_fstask_geterror)(::rsbin::FsTask* self, char const** result) NOEXCEPT;
	bool(*rsbin_fileio_create)(char const* path, int mode, ::rsbin::FileIo** result) NOEXCEPT;
	bool(*rsbin_fileio_startread)(::rsbin::FileIo* self, uint64_t offset, int length, void* buffer, ::rsbin::FsTask** result) NOEXCEPT;
	bool(*rsbin_fileio_startwrite)(::rsbin::FileIo* self, uint64_t offset, int length, void const* buffer, ::rsbin::FsTask** result) NOEXCEPT;
	bool(*rsbin_fileio_setsize)(::rsbin::FileIo* self, uint64_t size) NOEXCEPT;
	bool(*rsbin_pngreader_create)(int format, ::rsbin::PngReader** result) NOEXCEPT;
	bool(*rsbin_pngreader_release)(::rsbin::PngReader* self) NOEXCEPT;
	bool(*rsbin_pngreader_feed)(::rsbin::PngReader* self, int length, void const* buffer) NOEXCEPT;
	bool(*rsbin_pngreader_isfinished)(::rsbin::PngReader* self, bool* result) NOEXCEPT;
	bool(*rsbin_pngreader_getwidth)(::rsbin::PngReader* self, int* result) NOEXCEPT;
	bool(*rsbin_pngreader_getheight)(::rsbin::PngReader* self, int* result) NOEXCEPT;
	bool(*rsbin_pngreader_getdata)(::rsbin::PngReader* self, ::utils::DataBuffer** result) NOEXCEPT;
	bool(*rsbin_pngwriter_create)(int format, int width, int height, ::utils::DataBuffer* data, ::rsbin::PngWriter** result) NOEXCEPT;
	bool(*rsbin_pngwriter_release)(::rsbin::PngWriter* self) NOEXCEPT;
	bool(*rsbin_pngwriter_grab)(::rsbin::PngWriter* self, int length, void* buffer, int* result) NOEXCEPT;
	bool(*rsbin_pngwriter_isfinished)(::rsbin::PngWriter* self, bool* result) NOEXCEPT;
};

extern methodlist_t const methodlist;
