local modname = ...
local node = require(modname, 3, 'base')
local farg = node:module(modname)
local common
local context
local fulltype
local symlocal

function farg:init(pr)
	node.init(self, pr)
	self.typev = pr.typev
	self.target = pr.target
	self.blvalue = pr.lvalue
	self.brvalue = pr.rvalue
end

function farg:dobuild(pc)
	self.typev:build(pc)
	self.localdef = common.createnode{
		name = 'stat.local',
		spos = self.spos,
		epos = self.epos,
		filename = self.filename,
		typev = self.typev,
		targetname = self.target,
	}
	self.localdef:build(pc)
	self.symbol = self.localdef.symbol
end

function farg:defstring(lp)
	local am
	if self.blvalue then
		if self.brvalue then
			am = 'inout '
		else
			am = 'out '
		end
	else
		if self.brvalue then
			am = ''
		else
			am = '<error> '
		end
	end
	if self.target then
		return string.format('%s%s %s',
			am,
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.target, lp .. self.lpindent))
	else
		return string.format('%s%s',
			am,
			self.typev:defstring(lp .. self.lpindent))
	end
end

common = require(modname, 4, 'common')
context = require(modname, 4, 'context')
fulltype = require(modname, 4, 'fulltype')
symlocal = require(modname, 4, 'symbol.local')
