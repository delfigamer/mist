local modname = ...
local object = require('base.object')
local point = object:module(modname)

point.lpindent = '\t'

function point:createpoint(pr, ...)
	return self:create(pr, ...)
end

function point:defstring(lp)
	return '< ' .. self._NAME .. ' >'
end

function point.instmeta:__tostring()
	return self:defstring('')
end
