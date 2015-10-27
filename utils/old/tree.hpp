#ifndef UTILS_TREE_HPP_
#define UTILS_TREE_HPP_

#include "smalloc.hpp"
#include "list.hpp"

namespace utils {
	class TreeItem {
	public:
		class ConstIterator : public ::utils::Iterator< const TreeItem* > {
		private:
			ListItem::ConstIterator m_listi;
			
		public:
			ConstIterator() = delete;
			explicit ConstIterator( const TreeItem* parent );
			
			virtual bool iterate( const TreeItem** next );
		};
		
		class Iterator : public ::utils::Iterator< TreeItem* > {
		private:
			ListItem::Iterator m_listi;
			
		public:
			Iterator() = delete;
			explicit Iterator( TreeItem* parent );
			
			virtual bool iterate( TreeItem** next );
		};
		
	private:
		static PoolAllocator* m_allocator;
		TreeItem* m_parent;
		ListItem* m_item;
		ListItem m_children;
		void* m_data;
		
	public:
		TreeItem();
		explicit TreeItem( TreeItem* parent, void* data = 0 );
		TreeItem( TreeItem& ) = delete;
		TreeItem( TreeItem&& ) = delete;
		~TreeItem();
		
		const TreeItem* parent() const;
		TreeItem* parent();
		TreeItem* parent( TreeItem* item );
		void* data() const;
		void* data( void* newdata );
		ConstIterator iterator() const;
		Iterator iterator();
		
		void* operator new( size_t size );
		void operator delete( void* ptr );
	};
}

#endif
