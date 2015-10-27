#ifndef UTILS_LIST_HPP_
#define UTILS_LIST_HPP_

#include "smalloc.hpp"

namespace utils {
	class ListItem {
	public:
		class ConstIterator {
		private:
			const ListItem* m_root;
			const ListItem* m_next;
			
		public:
			ConstIterator() = delete;
			explicit ConstIterator( const ListItem* root );
			
			virtual bool iterate( const ListItem** next );
		};
		
		class Iterator {
		private:
			ListItem* m_root;
			ListItem* m_next;
			
		public:
			Iterator() = delete;
			explicit Iterator( ListItem* root );
			
			virtual bool iterate( ListItem** next );
		};
		
	private:
		static PoolAllocator* m_allocator;
		ListItem* m_next;
		ListItem* m_prev;
		void* m_data;
		
	public:
		ListItem();
		explicit ListItem( ListItem* next, void* data = 0 );
		ListItem( ListItem& ) = delete;
		ListItem( ListItem&& ) = delete;
		~ListItem();
		
		void remove();
		const ListItem* next() const;
		const ListItem* prev() const;
		ListItem* next();
		ListItem* prev();
		void* data() const;
		void* data( void* newdata );
		ConstIterator iterator() const;
		Iterator iterator();
		
		void* operator new( size_t size );
		void operator delete( void* ptr );
	};
}

#endif
