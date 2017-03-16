local modname = ...
local node = require(modname, 3, 'base')
local farg = node:module(modname)
local common
local context
local fulltype
local symlocal

function farg:init(pr)
	node.init(self, pr)
	self.blvalue = pr.lvalue
	self.brvalue = pr.rvalue
	self.target = pr.target
	self.value = pr.value
	self.typev = pr.typev
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
	local targetstr
	if self.target then
		targetstr = common.identstring(self.target) .. ' '
	else
		targetstr = ''
	end
	local valuestr
	if self.value then
		valuestr = '= ' .. self.value:defstring(lp .. self.lpindent) .. ' '
	else
		valuestr = ''
	end
	return string.format('%s%s%s: %s',
		am, targetstr, valuestr,
		self.typev:defstring(lp .. self.lpindent))
end

common = require(modname, 4, 'common')
context = require(modname, 4, 'context')
fulltype = require(modname, 4, 'fulltype')
symlocal = require(modname, 4, 'symbol.local')
