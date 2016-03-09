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

/*
R_EMIT( target = lua_beforemetatypes )
function event.instmeta:__tostring()
	if self.name == 0 then
		return 'event< close >'
	elseif self.name == 1 then
		return string.format('event< pointdown, %i, %i, %i >',
			self.arg[0], self.arg[1], self.arg[2])
	elseif self.name == 2 then
		return string.format('event< pointup, %i >',
			self.arg[0])
	elseif self.name == 3 then
		return string.format('event< pointmove, %i, %i, %i >',
			self.arg[0], self.arg[1], self.arg[2])
	elseif self.name == 4 then
		return string.format('event< keydown, %i >',
			self.arg[0])
	elseif self.name == 5 then
		return string.format('event< keyup, %i >',
			self.arg[0])
	elseif self.name == 6 then
		return string.format('event< char, %i >',
			self.arg[0])
	elseif self.name == 7 then
		return string.format('event< focus, %s >',
			self.arg[0] == 0 and 'false' or 'true')
	else
		return string.format('event< %i, %i, %i, %i, %i >',
			self.name, self.arg[0], self.arg[1], self.arg[2], self.arg[3])
	end
end
R_END()
*/

#endif
