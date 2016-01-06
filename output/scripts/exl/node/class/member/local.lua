local modname = ...
local cmbase = package.relrequire(modname, 1, 'base')
local cmlocal = cmbase:module(modname)
local common
local fulltype
local csymlocal

function cmlocal:init(pr)
	cmbase.init(self, pr)
	self.typev = pr.typev
	self.targetname = pr.targetname
end

function cmlocal:build(pc)
	cmbase.build(self, pc)
	local typeinfo
	self.typev:build(pc)
	typeinfo = self.typev:gettivalue()
	if not typeinfo then
		pc.env:error(
			'this value does not define a type',
			self.typev.spos, self.typev.epos)
	end
	local ft = fulltype:create(typeinfo, true, true)
	self.symbol = csymlocal:create{
		context = pc,
		defpos = self.epos,
		bexternal = self.bexternal,
		bpublic = self.bpublic,
		fulltype = ft,
	}
	pc:setsymbol(self.targetname, self.symbol)
end

function cmlocal:compile(stream)
end

function cmlocal:defstring(lp)
	return string.format('local %s %s',
		self.typev:defstring(lp .. self.lpindent),
		common.identstring(self.targetname))
end

common = package.relrequire(modname, 4, 'common')
fulltype = package.relrequire(modname, 4, 'fulltype')
csymlocal = package.relrequire(modname, 2, 'symbol.local')
