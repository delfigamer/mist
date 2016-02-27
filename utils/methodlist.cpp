#include "methodlist.hpp"
#include "cbase.hpp"
#include "databuffer.hpp"
#include "encoding.hpp"
#include "refobject.hpp"
#include <utils/cbase.hpp>
#include <exception>

static bool utils_cbase_geterror(char const** result) noexcept
{
	{
		*result = utils::cbase_geterror();
		return true;
	}
}

static bool utils_cbase_seterror(char const* error) noexcept
{
	{
		utils::cbase_seterror(error);
		return true;
	}
}

static bool utils_cbase_yield() noexcept
{
	{
		utils::cbase_yield();
		return true;
	}
}

static bool utils_cbase_write(char const* str) noexcept
{
	try
	{
		utils::cbase_write(str);
		return true;
	}
	catch( std::exception const& e )
	{
		utils::cbase_seterror(e.what());
		return false;
	}
	catch(...)
	{
		utils::cbase_seterror("unknown exception");
		return false;
	}
}

static bool utils_cbase_getchar(char* str) noexcept
{
	try
	{
		utils::cbase_getchar(str);
		return true;
	}
	catch( std::exception const& e )
	{
		utils::cbase_seterror(e.what());
		return false;
	}
	catch(...)
	{
		utils::cbase_seterror("unknown exception");
		return false;
	}
}

static bool utils_databuffer_new(int length, int capacity, void const* data, utils::DataBuffer** result) noexcept
{
	try
	{
		*result = utils::DataBuffer::pcreate(length, capacity, data);
		return true;
	}
	catch( std::exception const& e )
	{
		utils::cbase_seterror(e.what());
		return false;
	}
	catch(...)
	{
		utils::cbase_seterror("unknown exception");
		return false;
	}
}

static bool utils_databuffer_getdata(utils::DataBuffer* self, void** result) noexcept
{
	{
		*result = self->getdata();
		return true;
	}
}

static bool utils_databuffer_getlength(utils::DataBuffer* self, int* result) noexcept
{
	{
		*result = self->getlength();
		return true;
	}
}

static bool utils_databuffer_setlength(utils::DataBuffer* self, int length) noexcept
{
	{
		self->setlength(length);
		return true;
	}
}

static bool utils_databuffer_getcapacity(utils::DataBuffer* self, int* result) noexcept
{
	{
		*result = self->getcapacity();
		return true;
	}
}

static bool utils_getencoding(int index, utils::encoding_t const** result) noexcept
{
	try
	{
		*result = utils::getencoding(index);
		return true;
	}
	catch( std::exception const& e )
	{
		utils::cbase_seterror(e.what());
		return false;
	}
	catch(...)
	{
		utils::cbase_seterror("unknown exception");
		return false;
	}
}

static bool utils_translatestr(utils::translation_t* translation, int* result) noexcept
{
	try
	{
		*result = utils::translatestr(translation);
		return true;
	}
	catch( std::exception const& e )
	{
		utils::cbase_seterror(e.what());
		return false;
	}
	catch(...)
	{
		utils::cbase_seterror("unknown exception");
		return false;
	}
}

static bool utils_refobject_addref(utils::RefObject const* self, int* result) noexcept
{
	{
		*result = self->addref();
		return true;
	}
}

static bool utils_refobject_release(utils::RefObject const* self, int* result) noexcept
{
	{
		*result = self->release();
		return true;
	}
}

utils_methodlist_t const utils_methodlist = {
	utils_cbase_geterror,
	utils_cbase_seterror,
	utils_cbase_yield,
	utils_cbase_write,
	utils_cbase_getchar,
	utils_databuffer_new,
	utils_databuffer_getdata,
	utils_databuffer_getlength,
	utils_databuffer_setlength,
	utils_databuffer_getcapacity,
	utils_getencoding,
	utils_translatestr,
	utils_refobject_addref,
	utils_refobject_release,
};
