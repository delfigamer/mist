#pragma once

#include <common.hpp>

namespace window
{
	enum class [[ r::enum, r::name( "eventname" ) ]] EventName
	{
		close,
		pointdown, // point code, window x, y
		pointup, // point code
		pointmove, // point code, window x, y
		keydown, // key code
		keyup, // key code
		character, // unicode char index
		focus, // 0 - lost, 1 - gained
		invalid,
	};

	struct [[ r::struct, r::name( "event" ) ]] Event
	{
		EventName name;
		int a;
		int b;
		int c;
		int d;
	};
}

r_emit(<<
	function window_event.instmeta:__tostring()
		if self.name == window_eventname.close then
			return 'event< close >'
		elseif self.name == window_eventname.pointdown then
			return string.format('event< pointdown, %i, %i, %i >',
				self.a, self.b, self.c)
		elseif self.name == window_eventname.pointup then
			return string.format('event< pointup, %i >',
				self.a)
		elseif self.name == window_eventname.pointmove then
			return string.format('event< pointmove, %i, %i, %i >',
				self.a, self.b, self.c)
		elseif self.name == window_eventname.keydown then
			return string.format('event< keydown, %i >',
				self.a)
		elseif self.name == window_eventname.keyup then
			return string.format('event< keyup, %i >',
				self.a)
		elseif self.name == window_eventname.char then
			return string.format('event< char, %i >',
				self.a)
		elseif self.name == window_eventname.focus then
			return string.format('event< focus, %s >',
				self.a == 0 and 'false' or 'true')
		else
			return string.format('event< %i, %i, %i, %i, %i >',
				self.name, self.a, self.b, self.c, self.d)
		end
	end
>>)
