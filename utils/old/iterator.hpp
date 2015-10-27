#ifndef UTILS_ITERATOR_HPP_
#define UTILS_ITERATOR_HPP_ 1

namespace utils {
	template< typename T >
	class Iterator {
	public:
		virtual bool iterate( T* next ) = 0;
		
		bool operator()( T* next ) {
			return iterate( next );
		}
	};
}

#endif
