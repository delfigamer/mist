local modname = ...
local etypedef = require(modname, 1, 'typedef')
local etypeof = etypedef:module(modname)
local common

function etypeof:init(pr)
	etypedef.init(self, pr)
	self.expression = pr.expression
end

function etypeof:dobuild(pc)
	self.expression:build(pc)
	self.typeinfo = self.expression:getfulltype().ti
end

function etypeof:defstring(lp)
	return string.format(
		'type %s',
		self.expression:defstring(lp .. self.lpindent))
end

common = require(modname, 3, 'common')
