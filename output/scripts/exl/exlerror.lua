local modname = ...
local exception = require('base.exception')
local exlerror = exception:module(modname)

function exlerror:init(message, spos, epos, filename)
	self.message = message
	self.spos = spos
	self.epos = epos
	self.filename = filename
end

function exlerror:tostring()
	local fn = filename and (filename .. ':') or ''
	if self.spos and self.epos then
		return string.format('%s%s-%s %s%s',
			fn, self.spos, self.epos, self.message, self.traceback)
	elseif self.spos then
		return string.format('%s%s %s%s',
			fn, self.spos, self.message, self.traceback)
	else
		return string.format('%s%s%s',
			fn, self.message, self.traceback)
	end
end
