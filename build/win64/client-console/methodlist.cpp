#include <build\win64\client-console\methodlist.hpp>
#include <utils\cbase.hpp>
#include <utils\configset.hpp>
#include <utils\databuffer.hpp>
#include <utils\encoding.hpp>
#include <utils\refobject.hpp>
#include <rsbin\common.hpp>
#include <rsbin\fsthread.hpp>
#include <rsbin\fileio.hpp>
#include <rsbin\pngreader.hpp>
#include <rsbin\pngwriter.hpp>
#include <client-console\window.hpp>
#include <utils/cbase.hpp>
#include <exception>

static bool utils_cbase_geterror(char const** result) NOEXCEPT
{
	{
		*result = ::utils::cbase::geterror();
		return true;
	}
}

static bool utils_cbase_seterror(char const* error) NOEXCEPT
{
	{
		::utils::cbase::seterror(error);
		return true;
	}
}

static bool utils_cbase_yield() NOEXCEPT
{
	{
		::utils::cbase::yield();
		return true;
	}
}

static bool utils_cbase_write(char const* str) NOEXCEPT
{
	try
	{
		::utils::cbase::write(str);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_cbase_getchar(char* str) NOEXCEPT
{
	try
	{
		::utils::cbase::getchar(str);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_configset_integer(::utils::ConfigSet const* self, char const* expr, int def, int* result) NOEXCEPT
{
	try
	{
		*result = self->linteger(expr, def);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_configset_number(::utils::ConfigSet const* self, char const* expr, double def, double* result) NOEXCEPT
{
	try
	{
		*result = self->lnumber(expr, def);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_configset_lstring(::utils::ConfigSet const* self, char const* expr, char* buffer, int buflen, int* result) NOEXCEPT
{
	try
	{
		*result = self->lstring(expr, buffer, buflen);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_configset_boolean(::utils::ConfigSet const* self, char const* expr, bool def, bool* result) NOEXCEPT
{
	try
	{
		*result = self->lboolean(expr, def);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_databuffer_pcreate(int length, int capacity, void const* data, ::utils::DataBuffer** result) NOEXCEPT
{
	try
	{
		*result = ::utils::DataBuffer::pcreate(length, capacity, data);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_databuffer_getdata(::utils::DataBuffer* self, void** result) NOEXCEPT
{
	{
		*result = self->getdata();
		return true;
	}
}

static bool utils_databuffer_getlength(::utils::DataBuffer* self, int* result) NOEXCEPT
{
	{
		*result = self->getlength();
		return true;
	}
}

static bool utils_databuffer_setlength(::utils::DataBuffer* self, int length) NOEXCEPT
{
	{
		self->setlength(length);
		return true;
	}
}

static bool utils_databuffer_getcapacity(::utils::DataBuffer* self, int* result) NOEXCEPT
{
	{
		*result = self->getcapacity();
		return true;
	}
}

static bool utils_encoding_getencoding(int index, ::utils::encoding_t const** result) NOEXCEPT
{
	try
	{
		*result = ::utils::Encoding::getencoding(index);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_encoding_translatestr(::utils::translation_t* translation, int* result) NOEXCEPT
{
	try
	{
		*result = ::utils::Encoding::translatestr(translation);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool utils_refobject_addref(::utils::RefObject const* self) NOEXCEPT
{
	{
		self->vaddref();
		return true;
	}
}

static bool utils_refobject_release(::utils::RefObject const* self) NOEXCEPT
{
	{
		self->vrelease();
		return true;
	}
}

static bool rsbin_rsbin_isfileavailable(char const* path, bool* result) NOEXCEPT
{
	try
	{
		*result = ::rsbin::Common::isfileavailable(path);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_fstask_promote(::rsbin::FsTask* self) NOEXCEPT
{
	try
	{
		self->promote();
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_fstask_isfinished(::rsbin::FsTask* self, bool* result) NOEXCEPT
{
	{
		*result = self->isfinished();
		return true;
	}
}

static bool rsbin_fstask_getresult(::rsbin::FsTask* self, int* result) NOEXCEPT
{
	{
		*result = self->getresult();
		return true;
	}
}

static bool rsbin_fstask_geterror(::rsbin::FsTask* self, char const** result) NOEXCEPT
{
	{
		*result = self->geterror();
		return true;
	}
}

static bool rsbin_fileio_create(char const* path, int mode, ::rsbin::FileIo** result) NOEXCEPT
{
	try
	{
		*result = ::rsbin::FileIo::create(path, mode);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_fileio_startread(::rsbin::FileIo* self, uint64_t offset, int length, void* buffer, ::rsbin::FsTask** result) NOEXCEPT
{
	try
	{
		*result = self->startread(offset, length, buffer);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_fileio_startwrite(::rsbin::FileIo* self, uint64_t offset, int length, void const* buffer, ::rsbin::FsTask** result) NOEXCEPT
{
	try
	{
		*result = self->startwrite(offset, length, buffer);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_fileio_setsize(::rsbin::FileIo* self, uint64_t size) NOEXCEPT
{
	try
	{
		self->setsize(size);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngreader_create(int format, ::rsbin::PngReader** result) NOEXCEPT
{
	try
	{
		*result = ::rsbin::PngReader::create(format);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngreader_release(::rsbin::PngReader* self) NOEXCEPT
{
	try
	{
		self->release();
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngreader_feed(::rsbin::PngReader* self, int length, void const* buffer) NOEXCEPT
{
	try
	{
		self->feed(length, buffer);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngreader_isfinished(::rsbin::PngReader* self, bool* result) NOEXCEPT
{
	{
		*result = self->isfinished();
		return true;
	}
}

static bool rsbin_pngreader_getwidth(::rsbin::PngReader* self, int* result) NOEXCEPT
{
	{
		*result = self->getwidth();
		return true;
	}
}

static bool rsbin_pngreader_getheight(::rsbin::PngReader* self, int* result) NOEXCEPT
{
	{
		*result = self->getheight();
		return true;
	}
}

static bool rsbin_pngreader_getdata(::rsbin::PngReader* self, ::utils::DataBuffer** result) NOEXCEPT
{
	{
		*result = self->getdata();
		return true;
	}
}

static bool rsbin_pngwriter_create(int format, int width, int height, ::utils::DataBuffer* data, ::rsbin::PngWriter** result) NOEXCEPT
{
	try
	{
		*result = ::rsbin::PngWriter::create(format, width, height, data);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngwriter_release(::rsbin::PngWriter* self) NOEXCEPT
{
	try
	{
		self->release();
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngwriter_grab(::rsbin::PngWriter* self, int length, void* buffer, int* result) NOEXCEPT
{
	try
	{
		self->grab(length, buffer, result);
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

static bool rsbin_pngwriter_isfinished(::rsbin::PngWriter* self, bool* result) NOEXCEPT
{
	try
	{
		*result = self->isfinished();
		return true;
	}
	catch( ::std::exception const& e )
	{
		::utils::cbase::seterror(e.what());
		return false;
	}
	catch(...)
	{
		::utils::cbase::seterror("unknown exception");
		return false;
	}
}

methodlist_t const methodlist = {
	utils_cbase_geterror,
	utils_cbase_seterror,
	utils_cbase_yield,
	utils_cbase_write,
	utils_cbase_getchar,
	utils_configset_integer,
	utils_configset_number,
	utils_configset_lstring,
	utils_configset_boolean,
	utils_databuffer_pcreate,
	utils_databuffer_getdata,
	utils_databuffer_getlength,
	utils_databuffer_setlength,
	utils_databuffer_getcapacity,
	utils_encoding_getencoding,
	utils_encoding_translatestr,
	utils_refobject_addref,
	utils_refobject_release,
	rsbin_rsbin_isfileavailable,
	rsbin_fstask_promote,
	rsbin_fstask_isfinished,
	rsbin_fstask_getresult,
	rsbin_fstask_geterror,
	rsbin_fileio_create,
	rsbin_fileio_startread,
	rsbin_fileio_startwrite,
	rsbin_fileio_setsize,
	rsbin_pngreader_create,
	rsbin_pngreader_release,
	rsbin_pngreader_feed,
	rsbin_pngreader_isfinished,
	rsbin_pngreader_getwidth,
	rsbin_pngreader_getheight,
	rsbin_pngreader_getdata,
	rsbin_pngwriter_create,
	rsbin_pngwriter_release,
	rsbin_pngwriter_grab,
	rsbin_pngwriter_isfinished,
};
