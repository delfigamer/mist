local modname = ...
local object = require('base.object')
local node = object:module(modname)

node.lpindent = '\t'

function node:createnode(pr, ...)
	return self:create(pr, ...)
end

function node:init(pr, context)
	self.context = context
end

function node:defstring(lp)
	return '< ' .. self._NAME .. ' >'
end

function node:build()
	return '< ' .. self._NAME .. ' >'
end

function node.instmeta:__tostring()
	return self:defstring('')
end
