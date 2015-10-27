#ifndef RSBIN_PACKAGE_HPP__
#define RSBIN_PACKAGE_HPP__ 1

#include "packageio.hpp"
#include "packageheap.hpp"
#include "packageitem.hpp"
#include "io.hpp"
#include "int.hpp"
#include <utils/taskqueuebase.hpp>
#include <utils/refobject.hpp>
#include <utils/ref.hpp>
#include <utils/string.hpp>
#include <unordered_map>

namespace rsbin {
	struct HeaderChunkPos {
		beu64 offset;	// 0x0
		beu64 length;	// 0x8
	};	// 0x10
	
	struct DateTime {
		beu16 year;	// 0x0
		beu8 month;	// 0x2
		beu8 day;	// 0x3
		beu8 hour;	// 0x4
		beu8 minute;	// 0x5
		beu16 millisecond;	// 0x6
	};	// 0x8
	
	struct PackageHeader {
		beu64 signature;	// 0x0
		beu64 heapoffset;	// 0x8
		beu32 heaplength;	// 0x10
		beu32 reserved_14;	// 0x14
		beu64 heapemptyspace;	// 0x18
		beu64 reserved_20;	// 0x20
		beu64 reserved_28;	// 0x28
		beu64 itemlistoffset;	// 0x30
		beu32 itemlistlength;	// 0x38
		beu32 itemliststride;	// 0x3c
		HeaderChunkPos headerchunks[ 0x36 ];	// 0x40
		beu8 reserved_last[ 0x60 ]; // 0x3a0
	};	// 0x400
	
	struct ItemHeader {
		beu64 uuid;	// 0x0
		beu64 offset;	// 0x8
		beu64 tag;	// 0x10
		beu64 metaoffset;	// 0x18
		beu32 metalength;	// 0x20
		beu32 format;	// 0x24
		DateTime savetime;	// 0x28
		beu8 reserved_30[ 0x20 ];	// 0x30
		beu8 type[ 0x30 ];	// 0x50
		beu8 name[ 0x80 ];	// 0x80
	};	// 0x100
	
	beu64 const PackageSignature = 0x356f84f7989bfdfeLL;	// crc32( "Mist" ) .. crc32( "Package" )
	
	typedef struct { char c[ 32 ]; } Package_iterator;
	
	class Package: public utils::RefObject {
	public:
		friend struct PackageLoadTask;
		friend struct PackageHeaderParseTask;
		friend struct PackageIoCreateTask;
		friend struct PackageItemLoadTask;
		
	private:
		typedef std::unordered_map< uint64_t, PackageItem* > uuidmap_t;
		typedef std::unordered_map< utils::String, PackageItem*, utils::StringHasher > namemap_t;
		
	private:
		int m_state;
		uint64_t m_heapoffset;
		uint64_t m_itemliststride;
		utils::Ref< Io > m_io;
		PackageHeap m_heap;
		utils::Ref< PackageIo > m_headerio;
		utils::Ref< PackageIo > m_itemlistio;
		uuidmap_t m_uuidmap;
		namemap_t m_namemap;
		
	public:
		class iterator: public uuidmap_t::iterator {};
		
	public:
		virtual bool query( int id, void** target ) override;
		Package() = delete;
		explicit Package( Io* io );
		Package( Io* io, utils::TaskBase** loadtask, int taskpriority, utils::TaskQueueBase* queue );
		virtual ~Package() override;
		Package( Package const& other ) = delete;
		Package( Package&& other ) = delete;
		Package& operator=( Package const& other ) = delete;
		Package& operator=( Package&& other ) = delete;
		
		void save();
		PackageItem* createitem( uint64_t uuid, uint32_t format, utils::String const& type, utils::String const& name );
		PackageItem* getitembyuuid( uint64_t uuid );
		PackageItem* getitembyname( utils::String const& name );
		bool isvalid();
		bool isready();
		void begin( iterator* it );
		void end( iterator* it );
		PackageHeap* getheap();
		utils::Ref< Io > const& getio();
		void registeritemuuid( uint64_t uuid, PackageItem* item );
		void registeritemname( utils::String const& name, PackageItem* item );
	};
}

DEFINE_REFID( rsbin::Package, utils::RefObject, 0x9f86e515 )

namespace rsbin {
	extern "C" {
		Package* rsbin_package_new( Io* io ) noexcept;
		Package* rsbin_package_load( Io* io, utils::TaskBase** loadtask, int taskpriority, utils::TaskQueueBase* queue ) noexcept;
		bool rsbin_package_save( Package* package ) noexcept;
		PackageItem* rsbin_package_createitem( Package* package, ItemUuid const* uuid, uint32_t format, char const* type, char const* name ) noexcept;
		PackageItem* rsbin_package_getitembyuuid( Package* package, ItemUuid const* uuid ) noexcept;
		PackageItem* rsbin_package_getitembyname( Package* package, char const* name ) noexcept;
		int rsbin_package_isvalid( Package* package ) noexcept;
		int rsbin_package_isready( Package* package ) noexcept;
		bool rsbin_package_begin( Package* package, Package_iterator* it ) noexcept;
		bool rsbin_package_end( Package* package, Package_iterator* it ) noexcept;
		bool rsbin_package_iterator_inc( Package_iterator* it ) noexcept;
		int rsbin_package_iterator_eq( Package_iterator* a, Package_iterator* b ) noexcept;
		bool rsbin_package_iterator_deref( Package_iterator* it, ItemUuid* uuid, PackageItem** item ) noexcept;
		bool rsbin_package_iterator_close( Package_iterator* it ) noexcept;
	}
}

#endif
