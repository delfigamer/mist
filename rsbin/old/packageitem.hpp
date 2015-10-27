#ifndef RSBIN_PACKAGEITEM_HPP__
#define RSBIN_PACKAGEITEM_HPP__ 1

#include "packageio.hpp"
#include "io.hpp"
#include "int.hpp"
#include <utils/ref.hpp>
#include <utils/string.hpp>

namespace rsbin {
	class Package;
	
	struct ItemHeader;
	
	struct doubleword_t {
		uint32_t hi;
		uint32_t lo;
		
		doubleword_t& operator=( uint64_t uuid );
		operator uint64_t() const;
	};
	
	typedef doubleword_t ItemUuid, ItemTag;
	
	class PackageItem {
	private:
		Package* m_package;
		uint64_t m_uuid;
		uint64_t m_tag;
		uint64_t m_offset;
		uint32_t m_format;
		uint64_t m_metaoffset;
		uint32_t m_metalength;
		int m_savetime[ 6 ];
		utils::String m_type;
		utils::String m_name;
		utils::Ref< PackageIo > m_io;
		utils::Ref< PackageIo > m_metaio;
		
	public:
		PackageItem() = delete;
		PackageItem( Package* package, ItemHeader const* header );
		PackageItem( Package* package, uint64_t uuid, uint32_t format, utils::String const& type, utils::String const& name );
		~PackageItem();
		PackageItem( PackageItem const& other ) = delete;
		PackageItem( PackageItem&& other ) = delete;
		PackageItem operator=( PackageItem const& other ) = delete;
		PackageItem operator=( PackageItem&& other ) = delete;
		
		uint64_t getuuid();
		utils::Ref< PackageIo > const& getitemio( bool create );
		uint64_t gettag();
		void settag( uint64_t tag );
		utils::Ref< PackageIo > const& getmetaio( bool create );
		uint32_t getmetalength();
		void setmetalength( uint32_t length );
		uint32_t getformat();
		void getsavetime( int* dt );
		void setsavetime( int const* dt );
		utils::String const& gettype();
		utils::String const& getname();
		void setname( utils::String const& name );
		void fillheader( ItemHeader* header );
	};
}

extern "C" {
	bool rsbin_packageitem_getuuid( rsbin::PackageItem* item, rsbin::ItemUuid* uuid ) noexcept;
	rsbin::PackageIo* rsbin_packageitem_getitemio( rsbin::PackageItem* item, bool create ) noexcept;
	bool rsbin_packageitem_gettag( rsbin::PackageItem* item, rsbin::ItemTag* tag ) noexcept;
	bool rsbin_packageitem_settag( rsbin::PackageItem* item, rsbin::ItemTag const* tag ) noexcept;
	rsbin::PackageIo* rsbin_packageitem_getmetaio( rsbin::PackageItem* item, bool create ) noexcept;
	uint32_t rsbin_packageitem_getmetalength( rsbin::PackageItem* item ) noexcept;
	bool rsbin_packageitem_setmetalength( rsbin::PackageItem* item, uint32_t length ) noexcept;
	bool rsbin_packageitem_getsavetime( rsbin::PackageItem* item, int* dt ) noexcept;
	bool rsbin_packageitem_setsavetime( rsbin::PackageItem* item, int const* dt ) noexcept;
	uint32_t rsbin_packageitem_getformat( rsbin::PackageItem* item ) noexcept;
	char const* rsbin_packageitem_gettype( rsbin::PackageItem* item ) noexcept;
	char const* rsbin_packageitem_getname( rsbin::PackageItem* item ) noexcept;
	bool rsbin_packageitem_setname( rsbin::PackageItem* item, char const* name ) noexcept;
}

#endif
