#pragma once

#include <rsbin/stream.hpp>
#include <rsbin/storage.hpp>
#include <common/ref.hpp>
#include <common/refobject.hpp>
#include <common.hpp>

namespace rsbin
{
	class [[ r::class, r::name( "storagestream" ) ]] StorageStream: public Stream
	{
	private:
		Ref< Storage > m_storage;
		Ref< StorageMap > m_currentmap;
		bool m_flagread;
		bool m_flagwrite;
		uint64_t m_position;

		void releasecurrentmap();

	public:
		[[ r::method ]] StorageStream(
			Storage* storage [[ r::required ]],
			bool flagread, bool flagwrite,
			uint64_t offset );
		~StorageStream();
		StorageStream( StorageStream const& other ) = delete;
		StorageStream& operator=( StorageStream const& other ) = delete;

		virtual StorageMap* advance( unsigned length ) override;
		virtual void skip( unsigned length ) override;
		virtual uint64_t getposition() override;
	};
}
