local modname = ...
local node = require('exl.node')
local soperator = node:module(modname)
local common
local dcfunc
local efunctionbase
local fulltype
local prototype
local symconst

function soperator:init(pr)
	node.init(self, pr)
	self.operator = pr.operator
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

function soperator:build(pc)
	self.value:build(pc)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.spos,
		fulltype = fulltype:create(self.value:getfulltype().ti, false, true),
		constvalue = self.value,
	}
	if self.operator then
		local proto = {}
		for i, arg in ipairs(self.arglist.args) do
			if not arg.typev then
				goto opfail
			end
			local argti = arg.typev:gettivalue()
			if not argti then
				goto opfail
			end
			proto[i] = fulltype:create(argti, arg.blvalue, arg.brvalue)
		end
		pc:setop(self.operator, prototype:create(proto), self)
	end
::opfail::
	self.value.body:build(self.value.context)
end

function soperator:compile(stream)
	stream:writetoken('a_createl', self.symbol.id)
	local valname = self.value:rcompile(stream)
	if not valname then
		return
	end
	stream:writetoken('a_setl', self.symbol.id, valname)
end

function soperator:createinstance(it)
	local ref = common.createnode{
		name = 'expr.reference',
		context = it.context,
		spos = it.spos,
		epos = it.spos,
		target = self.symbol,
	}
	local dcargs = {ref}
	for i, arg in ipairs(it.args) do
		dcargs[i+1] = arg
	end
	return dcfunc:createinstance{
		context = it.context,
		spos = it.spos,
		epos = it.epos,
		args = dcargs,
	}
end

function soperator:defstring(lp)
	if self.rettype then
		return string.format('operator %s%s: %s%s\n%send',
			common.defstring(self.operator, lp .. self.lpindent),
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.rettype, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	else
		return string.format('operator %s%s%s\n%send',
			common.defstring(self.operator, lp .. self.lpindent),
			common.defstring(self.arglist, lp .. self.lpindent),
			common.defstring(self.body, lp .. self.lpindent),
			lp)
	end
end

common = require('exl.common')
dcfunc = require('exl.node.expr.defcall.func')
efunctionbase = require('exl.node.expr.function.base')
fulltype = require('exl.fulltype')
prototype = require('exl.prototype')
symconst = require('exl.symbol.const')
