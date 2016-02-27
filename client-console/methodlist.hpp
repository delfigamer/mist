#pragma once

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
	bool(*utils_cbase_geterror)(char const** result) noexcept;
	bool(*utils_cbase_seterror)(char const* error) noexcept;
	bool(*utils_cbase_yield)() noexcept;
	bool(*utils_cbase_write)(char const* str) noexcept;
	bool(*utils_cbase_getchar)(char* str) noexcept;
	bool(*utils_configset_integer)(::utils::ConfigSet const* self, char const* expr, int def, int* result) noexcept;
	bool(*utils_configset_number)(::utils::ConfigSet const* self, char const* expr, double def, double* result) noexcept;
	bool(*utils_configset_lstring)(::utils::ConfigSet const* self, char const* expr, char* buffer, int buflen, int* result) noexcept;
	bool(*utils_configset_boolean)(::utils::ConfigSet const* self, char const* expr, bool def, bool* result) noexcept;
	bool(*utils_databuffer_pcreate)(int length, int capacity, void const* data, ::utils::DataBuffer** result) noexcept;
	bool(*utils_databuffer_getdata)(::utils::DataBuffer* self, void** result) noexcept;
	bool(*utils_databuffer_getlength)(::utils::DataBuffer* self, int* result) noexcept;
	bool(*utils_databuffer_setlength)(::utils::DataBuffer* self, int length) noexcept;
	bool(*utils_databuffer_getcapacity)(::utils::DataBuffer* self, int* result) noexcept;
	bool(*utils_encoding_getencoding)(int index, ::utils::encoding_t const** result) noexcept;
	bool(*utils_encoding_translatestr)(::utils::translation_t* translation, int* result) noexcept;
	bool(*utils_refobject_addref)(::utils::RefObject const* self) noexcept;
	bool(*utils_refobject_release)(::utils::RefObject const* self) noexcept;
	bool(*rsbin_rsbin_isfileavailable)(char const* path, bool* result) noexcept;
	bool(*rsbin_fstask_promote)(::rsbin::FsTask* self) noexcept;
	bool(*rsbin_fstask_isfinished)(::rsbin::FsTask* self, bool* result) noexcept;
	bool(*rsbin_fstask_getresult)(::rsbin::FsTask* self, int* result) noexcept;
	bool(*rsbin_fstask_geterror)(::rsbin::FsTask* self, char const** result) noexcept;
	bool(*rsbin_fileio_create)(char const* path, int mode, ::rsbin::FileIo** result) noexcept;
	bool(*rsbin_fileio_startread)(::rsbin::FileIo* self, uint64_t offset, int length, void* buffer, ::rsbin::FsTask** result) noexcept;
	bool(*rsbin_fileio_startwrite)(::rsbin::FileIo* self, uint64_t offset, int length, void const* buffer, ::rsbin::FsTask** result) noexcept;
	bool(*rsbin_fileio_setsize)(::rsbin::FileIo* self, uint64_t size) noexcept;
	bool(*rsbin_pngreader_create)(int format, ::rsbin::PngReader** result) noexcept;
	bool(*rsbin_pngreader_release)(::rsbin::PngReader* self) noexcept;
	bool(*rsbin_pngreader_feed)(::rsbin::PngReader* self, int length, void const* buffer) noexcept;
	bool(*rsbin_pngreader_isfinished)(::rsbin::PngReader* self, bool* result) noexcept;
	bool(*rsbin_pngreader_getwidth)(::rsbin::PngReader* self, int* result) noexcept;
	bool(*rsbin_pngreader_getheight)(::rsbin::PngReader* self, int* result) noexcept;
	bool(*rsbin_pngreader_getdata)(::rsbin::PngReader* self, ::utils::DataBuffer** result) noexcept;
	bool(*rsbin_pngwriter_create)(int format, int width, int height, ::utils::DataBuffer* data, ::rsbin::PngWriter** result) noexcept;
	bool(*rsbin_pngwriter_release)(::rsbin::PngWriter* self) noexcept;
	bool(*rsbin_pngwriter_grab)(::rsbin::PngWriter* self, int length, void* buffer, int* result) noexcept;
	bool(*rsbin_pngwriter_isfinished)(::rsbin::PngWriter* self, bool* result) noexcept;
	bool(*window_window_finish)() noexcept;
};

extern methodlist_t const methodlist;
