local modname = ...
local node = require('exl.node')
local slocal = node:module(modname)
local common
local context
local fulltype
local symlocal

function slocal:init(pr)
	node.init(self, pr)
	self.typev = pr.typev
	self.targetname = pr.targetname
	self.value = pr.value
end

function slocal:build(pc)
	local typeinfo
	if self.typev then
		self.typev:build(pc)
		local ctypev = self.typev:getconstvalue()
		if not ctypev then
			pc.env:log(
				'error',
				'type definition must be a constant value',
				self.typev.spos,
				self.typev.epos)
		else
			typeinfo = ctypev:gettivalue()
			if not typeinfo then
				pc.env:log(
					'error',
					'this value does not define a type',
					self.typev.spos,
					self.typev.epos)
			end
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
	if self.targetname then
		pc:setsymbol(self.targetname, self.symbol)
	end
	if self.value then
		self.targetdummy = common.createnode{
			name = 'expr.dummy',
			spos = self.spos,
			epos = self.spos,
			fulltype = fulltype:create(typeinfo, true, false),
		}
		self.assignment = common.createnode{
			name = 'expr.operator',
			operator = 'assign',
			spos = self.spos,
			epos = self.epos,
			args = {self.targetdummy, self.value},
		}
		self.assignment:build(pc)
	end
end

function slocal:compile(stream)
	if self.assignment then
		self.assignment:rcompile(stream)
		local name = self.targetdummy.retname
		if name then
			stream:writetoken('a_initl', self.symbol.id, name)
			return
		end
	end
	stream:writetoken('a_createl', self.symbol.id)
end

function slocal:defstring(lp)
	if self.value then
		return string.format('local %s %s = %s',
			common.defstring(self.typev, lp .. self.lpindent),
			common.defstring(self.targetname, lp .. self.lpindent),
			common.defstring(self.value, lp .. self.lpindent))
	else
		return string.format('local %s %s',
			common.defstring(self.typev, lp .. self.lpindent),
			common.defstring(self.targetname, lp .. self.lpindent))
	end
end

common = require('exl.common')
context = require('exl.context')
fulltype = require('exl.fulltype')
symlocal = require('exl.symbol.local')
