#ifndef WINDOW_EVENT_HPP__
#define WINDOW_EVENT_HPP__ 1

#include <common.hpp>

namespace window
{
	R_CLASS( name = event )
	struct Event
	{
		enum
		{
			Close = 0,
			PointDown = 1, // point code, window x, y
			PointUp = 2, // point code
			PointMove = 3, // point code, window x, y
			KeyDown = 4, // key code
			KeyUp = 5, // key code
			Char = 6, // unicode char index
			Focus = 7, // 0 - lost, 1 - gained
			Invalid = 8,
		};
	R_STRUCT()
		int name;
		int arg[ 4 ];
	R_END()
	};
}

#endif
