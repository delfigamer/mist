local modname = ...
local node = package.relrequire(modname, 3, 'base')
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
	self.typev:build(pc)
	typeinfo = self.typev:gettivalue()
	if not typeinfo then
		pc.env:error(
			'this value does not define a type',
			self.typev.spos, self.typev.epos)
	end
	local ft
	ft = fulltype:create(typeinfo, true, true)
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
	if not self.brvalue and self.blvalue then
		stream:writetoken('a_createl', self.symbol.id, 0)
	end
end

function farg:defstring(lp)
	local am
	if self.blvalue then
		if self.brvalue then
			am = 'inout '
		else
			am = 'out '
		end
	else
		if self.brvalue then
			am = ''
		else
			am = '<error> '
		end
	end
	if self.target then
		return string.format('%s%s %s',
			am,
			self.typev:defstring(lp .. self.lpindent),
			common.identstring(self.target, lp .. self.lpindent))
	else
		return string.format('%s%s',
			am,
			self.typev:defstring(lp .. self.lpindent))
	end
end

common = package.relrequire(modname, 4, 'common')
context = package.relrequire(modname, 4, 'context')
fulltype = package.relrequire(modname, 4, 'fulltype')
symlocal = package.relrequire(modname, 4, 'symbol.local')
