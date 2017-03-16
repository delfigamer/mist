local modname = ...
local cmbase = require(modname, 1, 'base')
local cmconst = cmbase:module(modname)
local common
local fulltype
local csymconst

function cmconst:init(pr)
	cmbase.init(self, pr)
	self.targetname = pr.targetname
	self.value = pr.value
end

function cmconst:dobuild(pc)
	cmbase.dobuild(self, pc)
	self.value:dobuild(pc)
	local value = self.value:getconstvalue()
	if not value then
		common.nodeerror(
			'the value is not constant',
			self.value)
	end
	local ft = fulltype:create(value:getfulltype().ti, false, true)
	self.symbol = csymconst:create{
		context = pc,
		defpos = self.epos,
		bexternal = self.bexternal,
		bpublic = self.bpublic,
		fulltype = ft,
		constvalue = value,
	}
	pc:setsymbol(self.targetname, self.symbol, self)
	self.symtarget = pc:getsymtarget()
end

function cmconst:compile(stream)
	local symtarget = self.symtarget:rcompile(stream)
	local valname = self.value:rcompile(stream)
	stream:append{
		[0]='move',
		{[0]='ssa', valname}, -- value
		{[0]='member',
			{[0]='ssa', symtarget},
			self.symbol.id}, -- id
	}
end

function cmconst:defstring(lp)
	return string.format('const %s = %s',
		common.identstring(self.targetname),
		self.value:defstring(lp .. self.lpindent))
end

common = require(modname, 4, 'common')
fulltype = require(modname, 4, 'fulltype')
csymconst = require(modname, 2, 'symbol.const')
