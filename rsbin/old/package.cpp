#include "package.hpp"
#include <utils/functionaltask.hpp>
#include <utils/taskqueue.hpp>
#include <utils/console.hpp>
#include <utils/cbase.hpp>
#include <thread>
#include <stdexcept>
#include <exception>
#include <new>
#include <cstring>
#include <cstdio>
#include <ctime>

#undef LOG
#define LOG( ... )

namespace rsbin {
	beu64 const DefaultHeapOffset = 0x400;
	beu64 const DefaultItemListStride = 0x100;
	beu64 const FirstChunkSize = 0x1000;

	int const state_invalid = 0;
	int const state_loading = 1;
	int const state_ready = 2;

	struct PackageLoadTask: public utils::FunctionalTask {
		enum stage_t: int {
			stage_headerreading,
			stage_headerparsing,
			stage_heaploading,
			stage_iocreation,
			stage_itemloading
		};
		utils::Ref< Package > m_package;
		utils::TaskQueueBase* m_queue;
		PackageHeader m_header;
		stage_t m_stage;
		PackageHeap::chunk_t m_headerspace[ 0x36 ];
		int m_ioresult;

		PackageLoadTask( int priority, Package* package, utils::TaskQueueBase* queue ) :
			FunctionalTask( priority ) ,
			m_package( package ) ,
			m_queue( queue ) ,
			m_stage( stage_headerreading ) {
		}

		virtual ~PackageLoadTask() = default;
		virtual void doaction() override;
	};

	struct PackageHeaderParseTask: public utils::FunctionalTask {
		PackageLoadTask* m_pltask;

		PackageHeaderParseTask( int priority, PackageLoadTask* pltask ) :
			FunctionalTask( priority ) ,
			m_pltask( pltask ) {
		}

		virtual ~PackageHeaderParseTask() = default;
		virtual void doaction() override;
	};

	struct PackageIoCreateTask: public utils::FunctionalTask {
		PackageLoadTask* m_pltask;

		PackageIoCreateTask( int priority, PackageLoadTask* pltask ) :
			FunctionalTask( priority ) ,
			m_pltask( pltask ) {
		}

		virtual ~PackageIoCreateTask() = default;
		virtual void doaction() override;
	};

	struct PackageItemLoadTask: public utils::FunctionalTask {
		PackageLoadTask* m_pltask;
		int m_itemindex;
		ItemHeader m_itemheader;
		int m_ioresult;

		PackageItemLoadTask( int priority, PackageLoadTask* pltask ) :
			FunctionalTask( priority ) ,
			m_pltask( pltask ) ,
			m_itemindex( -1 ) {
		}

		virtual ~PackageItemLoadTask() = default;
		virtual void doaction() override;
	};

	PackageLoadTask::stage_t& operator++( PackageLoadTask::stage_t& stage ) {
		return stage = PackageLoadTask::stage_t( int( stage ) + 1 );
	}

	void PackageLoadTask::doaction() {
		if( m_subtask ) {
			++m_stage;
			m_subtask = nullptr;
		}
		switch( m_stage ) {
		case stage_headerreading:
			LOG( "stage_headerreading" );
			m_subtask = m_package->m_io->startread( 0, sizeof( PackageHeader ), &m_header, &m_ioresult, priority() + 1 );
		break;
		case stage_headerparsing:
			LOG( "stage_headerparsing" );
			m_subtask = ( utils::TaskBase* )utils::Ref< PackageHeaderParseTask >::create( priority() + 1, this );
			m_queue->insert( m_subtask );
		break;
		case stage_heaploading:
			LOG( "stage_heaploading" );
			m_subtask = m_package->m_heap.startload( m_package->m_headerio, &m_header, priority() + 1, m_queue );
		break;
		case stage_iocreation:
			LOG( "stage_iocreation" );
			m_subtask = ( utils::TaskBase* )utils::Ref< PackageIoCreateTask >::create( priority() + 1, this );
			m_queue->insert( m_subtask );
		break;
		case stage_itemloading:
			LOG( "stage_itemloading" );
			m_subtask = ( utils::TaskBase* )utils::Ref< PackageItemLoadTask >::create( priority() + 1, this );
			m_queue->insert( m_subtask );
		break;
		default:
			LOG( "finished" );
			setfinished( true );
		}
	}

	void PackageHeaderParseTask::doaction() {
		Package* package = m_pltask->m_package;
		PackageHeader* header = &m_pltask->m_header;
		if( m_pltask->m_ioresult != sizeof( PackageHeader ) ||
			header->signature != PackageSignature ||
			header->heapoffset < sizeof( PackageHeader ) ||
			header->itemliststride < sizeof( ItemHeader ) ||
			header->headerchunks[ 0 ].length == 0 ) {
			throw std::runtime_error( "invalid input file" );
		}
		package->m_heapoffset = header->heapoffset;
		package->m_itemliststride = header->itemliststride;
		PackageHeap::chunk_t* headerspace = m_pltask->m_headerspace;
		int i;
		for( i = 0; i < 0x36; ++i ) {
			if( header->headerchunks[ i ].offset == 0 && header->headerchunks[ i ].length == 0 ) {
				headerspace[ i - 1 ].next = 0;
				break;
			}
			headerspace[ i ].offset = header->headerchunks[ i ].offset;
			headerspace[ i ].length = header->headerchunks[ i ].length;
			headerspace[ i ].next = &headerspace[ i + 1 ];
		}
		if( i == 0x36 ) {
			headerspace[ 0x35 ].next = 0;
		}
		package->m_headerio = utils::Ref< PackageIo >::create( package->m_io, nullptr, headerspace );
		setfinished( true );
	}

	void PackageIoCreateTask::doaction() {
		Package* package = m_pltask->m_package;
		PackageHeader* header = &m_pltask->m_header;
		package->m_state = state_loading;
		package->m_headerio = utils::Ref< PackageIo >::create(
			package->m_io, &package->m_heap, package->m_heap.find( 0 ) );
		package->m_itemlistio = utils::Ref< PackageIo >::create(
			package->m_io, &package->m_heap, package->m_heap.find( header->itemlistoffset ) );
		setfinished( true );
	}

	void PackageItemLoadTask::doaction() {
		Package* package = m_pltask->m_package;
		PackageHeader* header = &m_pltask->m_header;
		if( m_itemindex >= 0 ) {
			if( m_ioresult != sizeof( ItemHeader ) ||
				m_itemheader.uuid == 0 ) {
				throw std::runtime_error( "invalid item header" );
			}
			PackageItem* item = new PackageItem( package, &m_itemheader );
// 			LOG( "loaded an item:" );
// 			LOG( "\tuuid = 0x%.16llx", item->getuuid() );
// 			LOG( "\tname = \"%s\"", item->getname().chars() );
// 			LOG( "\ttype = \"%s\"", item->gettype().chars() );
// 			LOG( "\tformat = 0x%.8x", ( unsigned )item->getformat() );
			( ( void )item );	// suppress "unused variable" warning with stripped logging
		}
		m_itemindex += 1;
		if( m_itemindex == ( int )header->itemlistlength ) {
			package->m_state = state_ready;
			setfinished( true );
		} else {
			LOG( "+" );
			m_subtask = package->m_itemlistio->startread(
				( uint64_t( header->itemliststride ) ) * m_itemindex,
				sizeof( ItemHeader ), &m_itemheader, &m_ioresult, priority() + 1 );
			LOG( "-" );
		}
	}

	bool Package::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	Package::Package( Io* io ) :
		m_state( state_ready ) ,
		m_heapoffset( DefaultHeapOffset ) ,
		m_itemliststride( DefaultItemListStride ) ,
		m_io( io ) ,
		m_heap( FirstChunkSize ) {
		m_headerio = utils::Ref< PackageIo >::create( m_io, &m_heap, m_heap.find( 0 ) );
		m_itemlistio = utils::Ref< PackageIo >::create( m_io, &m_heap, nullptr );
	}

	Package::Package( Io* io, utils::TaskBase** loadtask, int taskpriority, utils::TaskQueueBase* queue ) :
		m_state( state_invalid ) ,
		m_heapoffset( 0 ) ,
		m_itemliststride( 0 ) ,
		m_io( io ) ,
		m_heap() {
		utils::Ref< PackageLoadTask > task = utils::Ref< PackageLoadTask >::create( taskpriority, this, queue );
		queue->insert( task );
		*loadtask = task;
	}

	Package::~Package() {
		uuidmap_t::iterator it = m_uuidmap.begin();
		uuidmap_t::iterator eit = m_uuidmap.end();
		for( ; it != eit; ++it ) {
			delete it->second;
		}
	}

	void writechunks( HeaderChunkPos* list, PackageHeap::chunk_t const* chunk ) {
		while( chunk ) {
			list->offset = chunk->offset;
			list->length = chunk->length;
			++list;
			chunk = chunk->next;
		}
		list->offset = 0;
		list->length = 0;
	}

	void Package::save() {
// 		LOG( "saving started" );
		if( m_state != state_ready ) {
			throw std::runtime_error( "cannot save a package which is not fully loaded" );
		}
		while( m_headerio->extend( m_heapoffset + 2 * sizeof( beu64 ) * m_heap.getlength() ) ) {
		}
// 		LOG( "header space extended" );
		uuidmap_t::iterator it = m_uuidmap.begin();
		uuidmap_t::iterator eit = m_uuidmap.end();
		ItemHeader iheader;
		uint64_t offset = 0;
// 		LOG( "item headers saving started" );
		for( ; it != eit; ++it ) {
// 			LOG( "\tnext item" );
			it->second->fillheader( &iheader );
// 			LOG( "\theader filled" );
			m_itemlistio->syncwrite( offset, sizeof( ItemHeader ), &iheader );
// 			LOG( "\theader saved" );
			offset += m_itemliststride;
// 			LOG( "\toffset updated" );
		}
// 		LOG( "item headers saved, saving package header..." );
		PackageHeader header;
		header.signature = PackageSignature;
		header.heapoffset = m_heapoffset;
		header.heaplength = m_heap.getlength();
		header.heapemptyspace = m_heap.getemptyspace();
		header.itemlistoffset = m_itemlistio->getspace()->offset;
		header.itemlistlength = m_uuidmap.size();
		header.itemliststride = m_itemliststride;
		writechunks( header.headerchunks, m_headerio->getspace() );
		m_headerio->syncwrite( 0, sizeof( PackageHeader ), &header );
// 		LOG( "package header saved, saving heap data..." );
		m_heap.write( m_headerio, m_heapoffset );
// 		LOG( "heap saved, saving finished" );
	}

	PackageItem* Package::createitem( uint64_t uuid, uint32_t format, utils::String const& type, utils::String const& name ) {
		if( m_state == state_invalid ) {
			throw std::runtime_error( "this package is not in a valid state" );
		}
		PackageItem* item = new PackageItem( this, uuid, format, type, name );
		return item;
	}

	PackageItem* Package::getitembyuuid( uint64_t uuid ) {
// 		LOG( ":%.16llx", uuid );
		uuidmap_t::iterator it = m_uuidmap.find( uuid );
		if( it != m_uuidmap.end() ) {
			return it->second;
		}
		return 0;
	}

	PackageItem* Package::getitembyname( utils::String const& name ) {
		namemap_t::iterator it = m_namemap.find( name );
		if( it != m_namemap.end() ) {
			return it->second;
		}
		return 0;
	}

	bool Package::isvalid() {
		return m_state != state_invalid;
	}

	bool Package::isready() {
		return m_state == state_ready;
	}

	void Package::begin( iterator* it ) {
		new( ( uuidmap_t::iterator* )it )uuidmap_t::iterator( m_uuidmap.begin() );
	}

	void Package::end( iterator* it ) {
		new( ( uuidmap_t::iterator* )it )uuidmap_t::iterator( m_uuidmap.end() );
	}

	PackageHeap* Package::getheap() {
		return &m_heap;
	}

	utils::Ref< Io > const& Package::getio() {
		return m_io;
	}

	void Package::registeritemuuid( uint64_t uuid, PackageItem* item ) {
		if( item ) {
			m_uuidmap[ uuid ] = item;
		} else {
			m_uuidmap.erase( uuid );
		}
	}

	void Package::registeritemname( utils::String const& name, PackageItem* item ) {
		if( item ) {
			m_namemap[ name ] = item;
		} else {
			m_namemap.erase( name );
		}
	}

	extern "C" {
		Package* rsbin_package_new( Io* io ) noexcept {
			CBASE_PROTECT(
				return new Package( io );
			)
		}

		Package* rsbin_package_load( Io* io, utils::TaskBase** loadtask, int taskpriority, utils::TaskQueueBase* queue ) noexcept {
			CBASE_PROTECT(
				return new Package( io, loadtask, taskpriority, queue );
			)
		}

		bool rsbin_package_save( Package* package ) noexcept {
			CBASE_PROTECT(
				package->save();
				return 1;
			)
		}

		PackageItem* rsbin_package_createitem( Package* package, ItemUuid const* uuid, uint32_t format, char const* type, char const* name ) noexcept {
			CBASE_PROTECT(
				return package->createitem( uuid ? *uuid : 0, format, type, name );
			)
		}

		PackageItem* rsbin_package_getitembyuuid( Package* package, ItemUuid const* uuid ) noexcept {
			CBASE_PROTECT(
// 				LOG( "0x%.8x -> :%.8x%.8x", uuid, uuid->hi, uuid->lo );
				return package->getitembyuuid( *uuid );
			)
		}

		PackageItem* rsbin_package_getitembyname( Package* package, char const* name ) noexcept {
			CBASE_PROTECT(
				return package->getitembyname( name );
			)
		}

		int rsbin_package_isvalid( Package* package ) noexcept {
			CBASE_PROTECT(
				return package->isvalid() ? 1 : 2;
			)
		}

		int rsbin_package_isready( Package* package ) noexcept {
			CBASE_PROTECT(
				return package->isready() ? 1 : 2;
			)
		}

		bool rsbin_package_begin( Package* package, Package_iterator* it ) noexcept {
			CBASE_PROTECT(
				package->begin( ( Package::iterator* )it );
				return 1;
			)
		}

		bool rsbin_package_end( Package* package, Package_iterator* it ) noexcept {
			CBASE_PROTECT(
				package->end( ( Package::iterator* )it );
				return 1;
			)
		}

		bool rsbin_package_iterator_inc( Package_iterator* it ) noexcept {
			CBASE_PROTECT(
				++ *( Package::iterator* )it;
				return 1;
			)
		}

		int rsbin_package_iterator_eq( Package_iterator* a, Package_iterator* b ) noexcept {
			CBASE_PROTECT(
				return
					*( Package::iterator* )a == *( Package::iterator* )b ?
					1 : 2;
			)
		}

		bool rsbin_package_iterator_deref( Package_iterator* it, ItemUuid* uuid, PackageItem** item ) noexcept {
			CBASE_PROTECT(
				*uuid = ( *( Package::iterator* )it )->first;
				*item = ( *( Package::iterator* )it )->second;
				return 1;
			)
		}

		bool rsbin_package_iterator_close( Package_iterator* it ) noexcept {
			CBASE_PROTECT(
				( ( Package::iterator* )it )->~iterator();
				return 1;
			)
		}
	}
}
