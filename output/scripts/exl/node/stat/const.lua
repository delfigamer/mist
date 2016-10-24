local modname = ...
local node = require(modname, 2, 'base')
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

function sconst:dobuild(pc)
	self.value:build(pc)
	local value = self.value:getconstvalue()
	if not value then
		common.nodeerror(
			'the value is not constant',
			self.value)
	end
	local ft = fulltype:create(value:getfulltype().ti, false, true)
	self.symbol = symconst:create{
		context = pc,
		defpos = self.epos,
		fulltype = ft,
		constvalue = value,
	}
	pc:setsymbol(self.targetname, self.symbol, self)
end

function sconst:compile(stream)
	local valname = self.value:rcompile(stream)
	stream:append{
		[0]='local_create',
		{[0]='ssa', valname}, -- value
		{[0]='local', self.symbol.id}, -- id
	}
end

function sconst:defstring(lp)
	return string.format('const %s = %s',
		common.identstring(self.targetname),
		self.value:defstring(lp .. self.lpindent))
end

common = require(modname, 3, 'common')
context = require(modname, 3, 'context')
fulltype = require(modname, 3, 'fulltype')
symconst = require(modname, 3, 'symbol.const')
