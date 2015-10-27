#include "packageitem.hpp"
#include "package.hpp"
#include <utils/cbase.hpp>

namespace rsbin {
	uint64_t generateuuid() {
		union {
			uint64_t i;
			time_t time;
			clock_t clock;
		} u64;
		static int counter = 0;
		uint64_t result;
		u64.clock = clock();
		result = u64.i << 32 | ( u64.i >> 32 );
		u64.time = time( 0 );
		result ^= u64.i;
		result += counter++;
		return result;
	}
	
	void timetodt( int* dt, time_t const& time ) {
		tm* t = gmtime( &time );
		dt[ 0 ] = t->tm_year + 1900;
		dt[ 1 ] = t->tm_mon + 1;
		dt[ 2 ] = t->tm_mday;
		dt[ 3 ] = t->tm_hour;
		dt[ 4 ] = t->tm_min;
		dt[ 5 ] = t->tm_sec * 1000;
	}
	
	void currenttime( int* dt ) {
		time_t t = time( 0 );
		timetodt( dt, t );
	}
	
	void dttorecord( DateTime* r, int const* dt ) {
		r->year = dt[ 0 ];
		r->month = dt[ 1 ];
		r->day = dt[ 2 ];
		r->hour = dt[ 3 ];
		r->minute = dt[ 4 ];
		r->millisecond = dt[ 5 ];
	}
	
	void recordtodt( int* dt, DateTime const* r ) {
		dt[ 0 ] = r->year;
		dt[ 1 ] = r->month;
		dt[ 2 ] = r->day;
		dt[ 3 ] = r->hour;
		dt[ 4 ] = r->minute;
		dt[ 5 ] = r->millisecond;
	}
	
	void fillstring( utils::String const& str, beu8* target, int maxlen ) {
		int len = str.length();
		if( len > maxlen ) {
			throw std::runtime_error( "the string is too large" );
		} else if( len == 0 ) {
			target[ 0 ] = 0;
			return;
		}
		char const* chars = str.chars();
		memcpy( target, chars, len );
		if( len < maxlen ) {
			target[ len ] = 0;
		}
	}
	
	void getstring( utils::String* str, beu8 const* source, int maxlen ) {
		int length = maxlen;
		for( int i = 0; i < maxlen; ++i ) {
			if( source[ i ] == 0 ) {
				length = i;
				break;
			}
		}
		str->chars( ( char const* )source, length );
	}
	
	ItemUuid& ItemUuid::operator=( uint64_t uuid ) {
		lo = ( uint32_t )uuid;
		hi = ( uint32_t )( uuid >> 32 );
		return *this;
	}
	
	ItemUuid::operator uint64_t() const {
		return ( ( uint64_t )hi ) << 32 | ( ( uint64_t )lo );
	}
	
	PackageItem::PackageItem( Package* package, ItemHeader const* header ) :
		m_package( package ) ,
		m_uuid( header->uuid ) ,
		m_tag( header->tag ) ,
		m_offset( header->offset ) ,
		m_format( header->format ) ,
		m_metaoffset( header->metaoffset ) ,
		m_metalength( header->metalength ) {
		recordtodt( m_savetime, &header->savetime );
		getstring( &m_type, header->type, sizeof( header->type ) );
		getstring( &m_name, header->name, sizeof( header->name ) );
		package->registeritemuuid( m_uuid, this );
		if( m_name ) {
			package->registeritemname( m_name, this );
		}
	}
	
	PackageItem::PackageItem( Package* package, uint64_t uuid, uint32_t format, utils::String const& type, utils::String const& name ) :
		m_package( package ) ,
		m_uuid( uuid == 0 ? generateuuid() : uuid ) ,
		m_tag( 0 ) ,
		m_offset( 0 ) ,
		m_format( format ) ,
		m_metaoffset( 0 ) ,
		m_metalength( 0 ) ,
		m_type( type ) ,
		m_name( name ) {
		currenttime( m_savetime );
		package->registeritemuuid( m_uuid, this );
		if( m_name ) {
			package->registeritemname( m_name, this );
		}
	}
	
	PackageItem::~PackageItem() {
// 		utils::MainLogger.writeln( "( ( rsbin::PackageItem* )0x%.8x )->~PackageItem()\t1", ( unsigned )this );
// 		m_io = 0;
// 		utils::MainLogger.writeln( "( ( rsbin::PackageItem* )0x%.8x )->~PackageItem()\t2", ( unsigned )this );
// 		m_metaio = 0;
// 		utils::MainLogger.writeln( "( ( rsbin::PackageItem* )0x%.8x )->~PackageItem()\t3", ( unsigned )this );
	}
	
	uint64_t PackageItem::getuuid() {
		return m_uuid;
	}
	
	uint32_t PackageItem::getformat() {
		return m_format;
	}
	
	utils::Ref< PackageIo > const& PackageItem::getitemio( bool create ) {
		if( !m_io ) {
			PackageHeap* heap = m_package->getheap();
			if( m_offset != 0 ) {
				m_io = utils::Ref< PackageIo >::create( m_package->getio(), heap, heap->find( m_offset ) );
			} else if( create ) {
				m_io = utils::Ref< PackageIo >::create( m_package->getio(), heap, nullptr );
			}
		}
		return m_io;
	}
	
	uint64_t PackageItem::gettag() {
		return m_tag;
	}
	
	void PackageItem::settag( uint64_t tag ) {
		m_tag = tag;
	}
	
	utils::Ref< PackageIo > const& PackageItem::getmetaio( bool create ) {
		if( !m_metaio ) {
			PackageHeap* heap = m_package->getheap();
			if( m_metaoffset != 0 ) {
				m_metaio = utils::Ref< PackageIo >::create( m_package->getio(), heap, heap->find( m_metaoffset ) );
			} else if( create ) {
				m_metaio = utils::Ref< PackageIo >::create( m_package->getio(), heap, nullptr );
			}
		}
		return m_metaio;
	}
	
	uint32_t PackageItem::getmetalength() {
		return m_metalength;
	}
	
	void PackageItem::setmetalength( uint32_t length ) {
		m_metalength = length;
	}
	
	void PackageItem::getsavetime( int* dt ) {
		for( int i = 0; i < 6; ++i ) {
			dt[ i ] = m_savetime[ i ];
		}
	}
	
	void PackageItem::setsavetime( int const* dt ) {
		for( int i = 0; i < 6; ++i ) {
			m_savetime[ i ] = dt[ i ];
		}
	}
	
	utils::String const& PackageItem::gettype() {
		return m_type;
	}
	
	utils::String const& PackageItem::getname() {
		return m_name;
	}
	
	void PackageItem::setname( utils::String const& name ) {
		if( m_name ) {
			m_package->registeritemname( m_name, 0 );
		}
		m_name = name;
		if( m_name ) {
			m_package->registeritemname( m_name, this );
		}
	}
	
	void PackageItem::fillheader( ItemHeader* header ) {
		header->uuid = m_uuid;
		header->offset = m_io ? m_io->getspace()->offset : m_offset;
		header->tag = m_tag;
		header->metaoffset = m_metaio ? m_metaio->getspace()->offset : m_metaoffset;
		header->metalength = m_metalength;
		header->format = m_format;
		dttorecord( &header->savetime, m_savetime );
		fillstring( m_type, header->type, sizeof( header->type ) );
		fillstring( m_name, header->name, sizeof( header->name ) );
	}
}

extern "C" {
	bool rsbin_packageitem_getuuid( rsbin::PackageItem* item, rsbin::ItemUuid* uuid ) noexcept {
		CBASE_PROTECT(
			*uuid = item->getuuid();
			return 1;
		)
	}
	
	rsbin::PackageIo* rsbin_packageitem_getitemio( rsbin::PackageItem* item, bool create ) noexcept {
		CBASE_PROTECT(
			return item->getitemio( create );
		)
	}
	
	bool rsbin_packageitem_gettag( rsbin::PackageItem* item, rsbin::ItemTag* tag ) noexcept {
		CBASE_PROTECT(
			*tag = item->gettag();
			return 1;
		)
	}
	
	bool rsbin_packageitem_settag( rsbin::PackageItem* item, rsbin::ItemTag const* tag ) noexcept {
		CBASE_PROTECT(
			item->settag( *tag );
			return 1;
		)
	}
	
	rsbin::PackageIo* rsbin_packageitem_getmetaio( rsbin::PackageItem* item, bool create ) noexcept {
		CBASE_PROTECT(
			return item->getmetaio( create );
		)
	}
	
	uint32_t rsbin_packageitem_getmetalength( rsbin::PackageItem* item ) noexcept {
		CBASE_PROTECT(
			return item->getmetalength();
		)
	}
	
	bool rsbin_packageitem_setmetalength( rsbin::PackageItem* item, uint32_t length ) noexcept {
		CBASE_PROTECT(
			item->setmetalength( length );
			return 1;
		)
	}
	
	bool rsbin_packageitem_getsavetime( rsbin::PackageItem* item, int* dt ) noexcept {
		CBASE_PROTECT(
			item->getsavetime( dt );
			return 1;
		)
	}
	
	bool rsbin_packageitem_setsavetime( rsbin::PackageItem* item, int const* dt ) noexcept {
		CBASE_PROTECT(
			item->setsavetime( dt );
			return 1;
		)
	}
	
	uint32_t rsbin_packageitem_getformat( rsbin::PackageItem* item ) noexcept {
		CBASE_PROTECT(
			return item->getformat();
		)
	}
	
	char const* rsbin_packageitem_gettype( rsbin::PackageItem* item ) noexcept {
		CBASE_PROTECT(
			return item->gettype();
		)
	}
	
	char const* rsbin_packageitem_getname( rsbin::PackageItem* item ) noexcept {
		CBASE_PROTECT(
			return item->getname();
		)
	}
	
	bool rsbin_packageitem_setname( rsbin::PackageItem* item, char const* name ) noexcept {
		CBASE_PROTECT(
			item->setname( name );
			return 1;
		)
	}
}
