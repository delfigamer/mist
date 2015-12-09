local modname = ...
local node = require('exl.node')
local sfunction = node:module(modname)
local common
local efunctionbase
local fulltype
local functionti
local symconst

function sfunction:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.arglist = pr.arglist
	self.rettype = pr.rettype
	self.body = pr.body
	self.value = efunctionbase:create{
		spos = self.spos,
		epos = self.epos,
		arglist = self.arglist,
		rettype = self.rettype,
		body = self.body,
	}
end

function sfunction:build(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	if self.targetname then
		pc:setsymbol(self.targetname, self.symbol)
	end
	self.value.body:build(self.value.context)
end

function sfunction:compile(stream)
	stream:writetoken('a_createl', self.symbol.id)
	local valname = self.value:rcompile(stream)
	if not valname then
		return
	end
	stream:writetoken('a_setl', self.symbol.id, valname)
end

function sfunction:defstring(lp)
	if self.rettype then
		return string.format('function %s%s: %s%s\n%send',
			common.defstring(self.targetname, lp .. self.lpindent),
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.rettype, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	else
		return string.format('function %s%s%s\n%send',
			common.defstring(self.targetname, lp .. self.lpindent),
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	end
end

common = require('exl.common')
efunctionbase = require('exl.node.expr.function.base')
fulltype = require('exl.fulltype')
functionti = require('exl.node.expr.function.ti')
symconst = require('exl.symbol.const')
