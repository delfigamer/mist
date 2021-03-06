#include <src/rsbin/memorystorage.hpp>
#include <reflection.hpp>
#include <common.hpp>
namespace
{
/*
	src/rsbin/memorystorage.hpp:13:2:
	rsbin> MemoryStorage = class(Storage) [[r::class]] [[r::name("memorystorage")]]
*/
/*
	src/rsbin/memorystorage.hpp:44:26:
	rsbin::MemoryStorage> create = static object function []: Ref<MemoryStorage> [[r::method]]
*/
	extern "C" r::objectbox r_rsbin_memorystorage_create_wrapper(r::exceptionbox* exception)
	{
		try
		{
			return r::makerefbox(::rsbin::MemoryStorage::create());
		}
		catch(std::exception const& e)
		{
			r::exceptionbox::make(exception);
			return r::objectbox{nullptr};
		}
	}
	struct r_ptr_t
	{
/*
	src/rsbin/memorystorage.hpp:13:2:
	rsbin> MemoryStorage = class(Storage) [[r::class]] [[r::name("memorystorage")]]
*/
/*
	src/rsbin/memorystorage.hpp:44:26:
	rsbin::MemoryStorage> create = static object function []: Ref<MemoryStorage> [[r::method]]
*/
		decltype(&r_rsbin_memorystorage_create_wrapper) rsbin_memorystorage_create;
	};
	r_ptr_t r_ptr = {
/*
	src/rsbin/memorystorage.hpp:13:2:
	rsbin> MemoryStorage = class(Storage) [[r::class]] [[r::name("memorystorage")]]
*/
/*
	src/rsbin/memorystorage.hpp:44:26:
	rsbin::MemoryStorage> create = static object function []: Ref<MemoryStorage> [[r::method]]
*/
		&r_rsbin_memorystorage_create_wrapper,
	};
	uint8_t const r_chunk[] = {
		0x1b, 0x4c, 0x4a, 0x01, 0x00, 0x24, 0x72, 0x65,
		0x66, 0x6c, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e,
		0x2f, 0x72, 0x73, 0x62, 0x69, 0x6e, 0x2f, 0x6d,
		0x65, 0x6d, 0x6f, 0x72, 0x79, 0x73, 0x74, 0x6f,
		0x72, 0x61, 0x67, 0x65, 0x2e, 0x72, 0x2e, 0x6c,
		0x75, 0x61, 0xdc, 0x01, 0x00, 0x01, 0x06, 0x03,
		0x05, 0x00, 0x13, 0x4e, 0x2e, 0x07, 0x2b, 0x01,
		0x00, 0x00, 0x10, 0x02, 0x01, 0x00, 0x37, 0x01,
		0x00, 0x01, 0x3e, 0x01, 0x02, 0x02, 0x2b, 0x02,
		0x01, 0x00, 0x37, 0x02, 0x01, 0x02, 0x10, 0x03,
		0x01, 0x00, 0x3e, 0x02, 0x02, 0x02, 0x37, 0x03,
		0x02, 0x01, 0x0a, 0x03, 0x00, 0x00, 0x54, 0x03,
		0x05, 0x80, 0x34, 0x03, 0x03, 0x00, 0x10, 0x05,
		0x01, 0x00, 0x37, 0x04, 0x04, 0x01, 0x3e, 0x04,
		0x02, 0x00, 0x3d, 0x03, 0x00, 0x01, 0x2b, 0x03,
		0x02, 0x00, 0x10, 0x04, 0x02, 0x00, 0x40, 0x03,
		0x02, 0x00, 0x02, 0xc0, 0x06, 0xc0, 0x03, 0xc0,
		0x09, 0x77, 0x68, 0x61, 0x74, 0x0a, 0x65, 0x72,
		0x72, 0x6f, 0x72, 0x0c, 0x77, 0x68, 0x61, 0x74,
		0x70, 0x74, 0x72, 0x1f, 0x72, 0x73, 0x62, 0x69,
		0x6e, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79,
		0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x5f,
		0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x08, 0x6e,
		0x65, 0x77, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02,
		0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04,
		0x04, 0x04, 0x06, 0x06, 0x06, 0x72, 0x5f, 0x65,
		0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e,
		0x62, 0x6f, 0x78, 0x00, 0x72, 0x00, 0x72, 0x5f,
		0x77, 0x72, 0x61, 0x70, 0x6f, 0x62, 0x6a, 0x65,
		0x63, 0x74, 0x00, 0x73, 0x65, 0x6c, 0x66, 0x00,
		0x00, 0x14, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74,
		0x69, 0x6f, 0x6e, 0x00, 0x05, 0x0f, 0x72, 0x65,
		0x73, 0x75, 0x6c, 0x74, 0x00, 0x04, 0x0b, 0x00,
		0xda, 0x06, 0x03, 0x00, 0x0a, 0x00, 0x15, 0x00,
		0x2b, 0x8e, 0x01, 0x00, 0x3f, 0x43, 0x00, 0x00,
		0x02, 0x34, 0x01, 0x00, 0x00, 0x25, 0x02, 0x01,
		0x00, 0x3e, 0x01, 0x02, 0x02, 0x34, 0x02, 0x00,
		0x00, 0x25, 0x03, 0x02, 0x00, 0x3e, 0x02, 0x02,
		0x02, 0x34, 0x03, 0x00, 0x00, 0x25, 0x04, 0x03,
		0x00, 0x3e, 0x03, 0x02, 0x02, 0x34, 0x04, 0x00,
		0x00, 0x25, 0x05, 0x04, 0x00, 0x3e, 0x04, 0x02,
		0x02, 0x37, 0x05, 0x05, 0x01, 0x25, 0x06, 0x06,
		0x00, 0x25, 0x07, 0x07, 0x00, 0x25, 0x08, 0x06,
		0x00, 0x24, 0x06, 0x08, 0x06, 0x3e, 0x05, 0x02,
		0x01, 0x25, 0x05, 0x08, 0x00, 0x25, 0x06, 0x06,
		0x00, 0x25, 0x07, 0x09, 0x00, 0x24, 0x05, 0x07,
		0x05, 0x37, 0x06, 0x0a, 0x01, 0x10, 0x07, 0x05,
		0x00, 0x10, 0x08, 0x00, 0x00, 0x3e, 0x06, 0x03,
		0x02, 0x34, 0x07, 0x0b, 0x00, 0x37, 0x07, 0x0c,
		0x07, 0x37, 0x07, 0x0d, 0x07, 0x10, 0x08, 0x07,
		0x00, 0x37, 0x07, 0x0e, 0x07, 0x25, 0x09, 0x0f,
		0x00, 0x3e, 0x07, 0x03, 0x02, 0x25, 0x08, 0x11,
		0x00, 0x3a, 0x08, 0x10, 0x07, 0x31, 0x08, 0x13,
		0x00, 0x3a, 0x08, 0x12, 0x07, 0x10, 0x09, 0x07,
		0x00, 0x37, 0x08, 0x14, 0x07, 0x3e, 0x08, 0x02,
		0x01, 0x30, 0x00, 0x00, 0x80, 0x47, 0x00, 0x01,
		0x00, 0x12, 0x62, 0x75, 0x69, 0x6c, 0x64, 0x6d,
		0x65, 0x74, 0x61, 0x74, 0x79, 0x70, 0x65, 0x00,
		0x0b, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x1e,
		0x72, 0x5f, 0x72, 0x73, 0x62, 0x69, 0x6e, 0x5f,
		0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x73, 0x74,
		0x6f, 0x72, 0x61, 0x67, 0x65, 0x5f, 0x62, 0x6f,
		0x78, 0x0c, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,
		0x66, 0x18, 0x72, 0x73, 0x62, 0x69, 0x6e, 0x2e,
		0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x73, 0x74,
		0x6f, 0x72, 0x61, 0x67, 0x65, 0x0b, 0x6d, 0x6f,
		0x64, 0x75, 0x6c, 0x65, 0x12, 0x72, 0x73, 0x62,
		0x69, 0x6e, 0x2e, 0x73, 0x74, 0x6f, 0x72, 0x61,
		0x67, 0x65, 0x0b, 0x6c, 0x6f, 0x61, 0x64, 0x65,
		0x64, 0x0c, 0x70, 0x61, 0x63, 0x6b, 0x61, 0x67,
		0x65, 0x09, 0x63, 0x61, 0x73, 0x74, 0x68, 0x09,
		0x09, 0x72, 0x5f, 0x72, 0x73, 0x62, 0x69, 0x6e,
		0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x73,
		0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x5f, 0x62,
		0x6f, 0x78, 0x20, 0x28, 0x2a, 0x72, 0x73, 0x62,
		0x69, 0x6e, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72,
		0x79, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
		0x5f, 0x63, 0x72, 0x65, 0x61, 0x74, 0x65, 0x29,
		0x28, 0x72, 0x65, 0x66, 0x6c, 0x65, 0x63, 0x74,
		0x69, 0x6f, 0x6e, 0x5f, 0x65, 0x78, 0x63, 0x65,
		0x70, 0x74, 0x69, 0x6f, 0x6e, 0x62, 0x6f, 0x78,
		0x2a, 0x20, 0x65, 0x78, 0x63, 0x65, 0x70, 0x74,
		0x69, 0x6f, 0x6e, 0x29, 0x3b, 0x0a, 0x09, 0x7d,
		0x2a, 0x0a, 0x0e, 0x09, 0x73, 0x74, 0x72, 0x75,
		0x63, 0x74, 0x7b, 0x0a, 0xdd, 0x01, 0x09, 0x74,
		0x79, 0x70, 0x65, 0x64, 0x65, 0x66, 0x20, 0x76,
		0x6f, 0x69, 0x64, 0x20, 0x72, 0x73, 0x62, 0x69,
		0x6e, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79,
		0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x3b,
		0x0a, 0x09, 0x74, 0x79, 0x70, 0x65, 0x64, 0x65,
		0x66, 0x20, 0x73, 0x74, 0x72, 0x75, 0x63, 0x74,
		0x20, 0x72, 0x5f, 0x72, 0x73, 0x62, 0x69, 0x6e,
		0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72, 0x79, 0x73,
		0x74, 0x6f, 0x72, 0x61, 0x67, 0x65, 0x5f, 0x62,
		0x6f, 0x78, 0x20, 0x7b, 0x0a, 0x09, 0x09, 0x72,
		0x73, 0x62, 0x69, 0x6e, 0x5f, 0x6d, 0x65, 0x6d,
		0x6f, 0x72, 0x79, 0x73, 0x74, 0x6f, 0x72, 0x61,
		0x67, 0x65, 0x2a, 0x20, 0x6f, 0x62, 0x6a, 0x65,
		0x63, 0x74, 0x3b, 0x0a, 0x09, 0x09, 0x76, 0x6f,
		0x69, 0x64, 0x28, 0x2a, 0x63, 0x66, 0x72, 0x65,
		0x65, 0x70, 0x74, 0x72, 0x29, 0x28, 0x73, 0x74,
		0x72, 0x75, 0x63, 0x74, 0x20, 0x72, 0x5f, 0x72,
		0x73, 0x62, 0x69, 0x6e, 0x5f, 0x6d, 0x65, 0x6d,
		0x6f, 0x72, 0x79, 0x73, 0x74, 0x6f, 0x72, 0x61,
		0x67, 0x65, 0x5f, 0x62, 0x6f, 0x78, 0x2a, 0x20,
		0x73, 0x65, 0x6c, 0x66, 0x29, 0x3b, 0x0a, 0x09,
		0x09, 0x76, 0x6f, 0x69, 0x64, 0x2a, 0x20, 0x66,
		0x72, 0x65, 0x65, 0x70, 0x74, 0x72, 0x3b, 0x0a,
		0x09, 0x7d, 0x20, 0x72, 0x5f, 0x72, 0x73, 0x62,
		0x69, 0x6e, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72,
		0x79, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
		0x5f, 0x62, 0x6f, 0x78, 0x3b, 0x0a, 0x05, 0x09,
		0x63, 0x64, 0x65, 0x66, 0x1c, 0x72, 0x65, 0x66,
		0x6c, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x2e,
		0x75, 0x6e, 0x77, 0x72, 0x61, 0x70, 0x6f, 0x62,
		0x6a, 0x65, 0x63, 0x74, 0x1a, 0x72, 0x65, 0x66,
		0x6c, 0x65, 0x63, 0x74, 0x69, 0x6f, 0x6e, 0x2e,
		0x77, 0x72, 0x61, 0x70, 0x6f, 0x62, 0x6a, 0x65,
		0x63, 0x74, 0x1c, 0x72, 0x65, 0x66, 0x6c, 0x65,
		0x63, 0x74, 0x69, 0x6f, 0x6e, 0x2e, 0x65, 0x78,
		0x63, 0x65, 0x70, 0x74, 0x69, 0x6f, 0x6e, 0x62,
		0x6f, 0x78, 0x08, 0x66, 0x66, 0x69, 0x0c, 0x72,
		0x65, 0x71, 0x75, 0x69, 0x72, 0x65, 0x01, 0x02,
		0x02, 0x02, 0x03, 0x03, 0x03, 0x04, 0x04, 0x04,
		0x05, 0x05, 0x05, 0x06, 0x07, 0x11, 0x15, 0x15,
		0x06, 0x18, 0x1c, 0x22, 0x22, 0x23, 0x23, 0x23,
		0x23, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28, 0x28,
		0x29, 0x29, 0x35, 0x2e, 0x3a, 0x3a, 0x3a, 0x3a,
		0x3a, 0x72, 0x70, 0x74, 0x72, 0x00, 0x02, 0x2a,
		0x66, 0x66, 0x69, 0x00, 0x03, 0x27, 0x72, 0x5f,
		0x65, 0x78, 0x63, 0x65, 0x70, 0x74, 0x69, 0x6f,
		0x6e, 0x62, 0x6f, 0x78, 0x00, 0x03, 0x24, 0x72,
		0x5f, 0x77, 0x72, 0x61, 0x70, 0x6f, 0x62, 0x6a,
		0x65, 0x63, 0x74, 0x00, 0x03, 0x21, 0x72, 0x5f,
		0x75, 0x6e, 0x77, 0x72, 0x61, 0x70, 0x6f, 0x62,
		0x6a, 0x65, 0x63, 0x74, 0x00, 0x03, 0x1e, 0x70,
		0x74, 0x72, 0x74, 0x79, 0x70, 0x65, 0x00, 0x0a,
		0x14, 0x72, 0x00, 0x04, 0x10, 0x72, 0x73, 0x62,
		0x69, 0x6e, 0x5f, 0x6d, 0x65, 0x6d, 0x6f, 0x72,
		0x79, 0x73, 0x74, 0x6f, 0x72, 0x61, 0x67, 0x65,
		0x00, 0x07, 0x09, 0x00, 0x00,	};
}
extern r::module const r_rsbin_memorystorage_module;
r::module const r_rsbin_memorystorage_module = {
	&r_ptr,
	&r_chunk,
	sizeof( r_chunk ),
	"rsbin/memorystorage.r.lua" };
