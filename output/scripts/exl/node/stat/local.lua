local modname = ...
local node = package.relrequire(modname, 2, 'base')
local slocal = node:module(modname)
local common
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
	self.typev:build(pc)
	typeinfo = self.typev:gettivalue()
	if not typeinfo then
		pc.env:error(
			'this value does not define a type',
			self.typev.spos, self.typev.epos)
	end
	local ft = fulltype:create(typeinfo, true, true)
	self.symbol = symlocal:create{
		context = pc,
		defpos = self.epos,
		fulltype = ft,
	}
	pc:setsymbol(self.targetname, self.symbol)
	if self.value then

	end
end

function slocal:compile(stream)
	stream:writetoken('a_createl', self.symbol.id, 0)
end

function slocal:defstring(lp)
	if self.value then
		return string.format('local %s %s = %s',
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.targetname, lp .. self.lpindent),
			self.value:defstring(lp .. self.lpindent))
	else
		return string.format('local %s %s',
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.targetname, lp .. self.lpindent))
	end
end

common = package.relrequire(modname, 3, 'common')
fulltype = package.relrequire(modname, 3, 'fulltype')
symlocal = package.relrequire(modname, 3, 'symbol.local')
