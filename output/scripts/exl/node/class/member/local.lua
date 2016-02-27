local modname = ...
local cmbase = require(modname, 1, 'base')
local cmlocal = cmbase:module(modname)
local common
local fulltype
local csymlocal

function cmlocal:init(pr)
	cmbase.init(self, pr)
	self.typev = pr.typev
	self.targetname = pr.targetname
end

function cmlocal:dobuild(pc)
	cmbase.dobuild(self, pc)
	local typeinfo
	self.typev:build(pc)
	typeinfo = self.typev:gettivalue()
	if not typeinfo then
		common.nodeerror(
			'this value does not define a type',
			self.typev)
	end
	local ft = fulltype:create(typeinfo, true, true)
	self.symbol = csymlocal:create{
		context = pc,
		defpos = self.epos,
		bexternal = self.bexternal,
		bpublic = self.bpublic,
		fulltype = ft,
	}
	pc:setsymbol(self.targetname, self.symbol, self)
end

function cmlocal:compile(stream)
end

function cmlocal:defstring(lp)
	return string.format('local %s %s',
		self.typev:defstring(lp .. self.lpindent),
		common.identstring(self.targetname))
end

common = require(modname, 4, 'common')
fulltype = require(modname, 4, 'fulltype')
csymlocal = require(modname, 2, 'symbol.local')
