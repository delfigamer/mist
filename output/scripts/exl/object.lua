local modname = ...
local object = require('base.object')
local exlobject = object:module(modname)

exlobject.lpindent = '\t'

function exlobject:defstring(lp)
	return '< ' .. self._NAME .. ' >'
end

function exlobject.instmeta:__tostring()
	return self:defstring('')
end
