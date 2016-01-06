local modname = ...
local esymbolbase = package.relrequire(modname, 1, 'symbolbase')
local eidentifier = esymbolbase:module(modname)
local common

function eidentifier:init(pr)
	esymbolbase.init(self, pr)
	self.targetname = pr.targetname
end

function eidentifier:build(pc)
	self.target = pc:getsymbol(self.targetname)
	if not self.target then
		common.nodeerror(string.format(
			'unknown identifier: %s', self.targetname), self)
	end
	esymbolbase.build(self, pc)
end

function eidentifier:defstring(lp)
	return common.identstring(self.targetname)
end

common = package.relrequire(modname, 3, 'common')
