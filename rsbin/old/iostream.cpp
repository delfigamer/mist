#include "iostream.hpp"
#include <utils/cbase.hpp>
#include <utils/console.hpp>

namespace rsbin {
	bool IoStream::query( int id, void** target ) {
		return utils::queryobject( this, id, target );
	}

	IoStream::IoStream( Io* io, uint64_t offset, bool direction ) :
		m_io( io ) ,
		m_offset( offset ) ,
		m_direction( direction ) {
	}

	IoStream::~IoStream() {
	}

	utils::TaskBase* IoStream::dostarttransfer( int length, void* buffer, int* result, int taskpriority ) {
		uint64_t offset = m_offset;
		m_offset += length;
		if( m_direction ) {
			return m_io->dostartread( offset, length, buffer, result, taskpriority );
		} else {
			return m_io->dostartwrite( offset, length, buffer, result, taskpriority );
		}
	}

	extern "C" {
		IoStream* rsbin_iostream_new( Io* io, uint64_t offset, bool readdir ) noexcept {
		CBASE_PROTECT(
			return new IoStream( io, offset, readdir );
		)
		}
	}
}
