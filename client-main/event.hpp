#pragma once

#include <common.hpp>

namespace window
{
	R_ENUM( name = eventname )
	enum class EventName
	{
		close = 0,
		pointdown = 1, // point code, window x, y
		pointup = 2, // point code
		pointmove = 3, // point code, window x, y
		keydown = 4, // key code
		keyup = 5, // key code
		character = 6, // unicode char index
		focus = 7, // 0 - lost, 1 - gained
		invalid = 8,
	};
	R_STRUCT( name = event )
	struct Event
	{
		EventName name;
		int a;
		int b;
		int c;
		int d;
	};
}

/*
R_EMIT( target = lua_beforemetatypes )
function event.instmeta:__tostring()
	if self.name == 0 then
		return 'event< close >'
	elseif self.name == 1 then
		return string.format('event< pointdown, %i, %i, %i >',
			self.a, self.b, self.c)
	elseif self.name == 2 then
		return string.format('event< pointup, %i >',
			self.a)
	elseif self.name == 3 then
		return string.format('event< pointmove, %i, %i, %i >',
			self.a, self.b, self.c)
	elseif self.name == 4 then
		return string.format('event< keydown, %i >',
			self.a)
	elseif self.name == 5 then
		return string.format('event< keyup, %i >',
			self.a)
	elseif self.name == 6 then
		return string.format('event< char, %i >',
			self.a)
	elseif self.name == 7 then
		return string.format('event< focus, %s >',
			self.a == 0 and 'false' or 'true')
	else
		return string.format('event< %i, %i, %i, %i, %i >',
			self.name, self.a, self.b, self.c, self.d)
	end
end
R_END()
*/
