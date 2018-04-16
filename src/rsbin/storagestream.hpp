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
		Ref< MapTask > m_currentmaptask;
		bool m_flagread;
		bool m_flagwrite;
		uint64_t m_position;

		void releasecurrentmap();

	public:
		StorageStream(
			Storage* storage,
			bool flagread, bool flagwrite,
			uint64_t offset );
		~StorageStream();
		StorageStream( StorageStream const& other ) = delete;
		StorageStream& operator=( StorageStream const& other ) = delete;

		[[ r::method ]] static Ref< StorageStream > create(
			Storage* storage,
			bool flagread, bool flagwrite,
			uint64_t offset )
		{
			externalassert( storage );
			return Ref< StorageStream >::create(
				storage, flagread, flagwrite, offset );
		}

		virtual Ref< MapTask > startadvance( uint32_t length ) override;
		virtual void skip( uint32_t length ) override;
		virtual uint64_t getposition() override;
	};
}
