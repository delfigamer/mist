local modname = ...
local node = require('exl.node')
local sconst = node:module(modname)
local common
local context
local fulltype
local symconst

function sconst:init(pr)
	node.init(self, pr)
	self.targetname = pr.targetname
	self.value = pr.value
end

function sconst:build(pc)
	local value
	if self.value then
		self.value:build(pc)
		value = self.value:getconstvalue()
		if not value then
			pc.env:log(
				'error',
				'the value is not constant',
				self.value.spos,
				self.value.epos)
		end
	end
	local ft
	if value then
		ft = fulltype:create(value:getfulltype().ti, false, true)
	end
	self.symbol = symconst:create{
		context = pc,
		defpos = self.epos,
		fulltype = ft,
		constvalue = value,
	}
	if self.targetname then
		pc:setsymbol(self.targetname, self.symbol)
	end
end

function sconst:compile(stream)
	if not self.value then
		return
	end
	local valname = self.value:rcompile(stream)
	if not valname then
		return
	end
	stream:writetoken('a_createl', self.symbol.id, valname)
end

function sconst:defstring(lp)
	return string.format('const %s = %s',
		common.defstring(self.targetname, lp .. self.lpindent),
		common.defstring(self.value, lp .. self.lpindent))
end

common = require('exl.common')
context = require('exl.context')
fulltype = require('exl.fulltype')
symconst = require('exl.symbol.const')
