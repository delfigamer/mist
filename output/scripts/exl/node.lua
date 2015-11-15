local modname = ...
local object = require('base.object')
local node = object:module(modname)

node.lpindent = '\t'

function node:init(pr)
	self.spos = pr.spos
	self.epos = pr.epos
end

function node:build(env, ...)
end

function node:defstring(lp)
	return '< ' .. self._NAME .. ' >'
end

function node.instmeta:__tostring()
	return self:defstring('')
end
