local modname = ...
local node = require('exl.node')
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

function farg:build(pc)
	local typeinfo
	if self.typev then
		self.typev:build(pc)
		typeinfo = self.typev:gettivalue()
		if not typeinfo then
			pc.env:log(
				'error',
				'this value does not define a type',
				self.typev.spos,
				self.typev.epos)
		end
	end
	local ft
	if typeinfo then
		ft = fulltype:create(typeinfo, true, true)
	end
	self.symbol = symlocal:create{
		context = pc,
		defpos = self.epos,
		fulltype = ft,
	}
	if self.target then
		pc:setsymbol(self.target, self.symbol)
	end
end

function farg:compilelocal(stream)
	if self.symbol and not self.brvalue and self.blvalue then
		stream:writetoken('a_createl', self.symbol.id, 0)
	end
end

function farg:defstring(lp)
	local am
	if self.blvalue then
		if self.brvalue then
			am = 'inout'
		else
			am = 'out'
		end
	else
		if self.brvalue then
			am = 'in'
		else
			am = '<error>'
		end
	end
	if self.target then
		return string.format('%s %s %s',
			am,
			common.defstring(self.typev, lp .. self.lpindent),
			common.defstring(self.target, lp .. self.lpindent))
	else
		return string.format('%s %s',
			am,
			common.defstring(self.typev, lp .. self.lpindent))
	end
end

common = require('exl.common')
context = require('exl.context')
fulltype = require('exl.fulltype')
symlocal = require('exl.symbol.local')
