local modname = ...
local baseti = package.relrequire(modname, 2, 'base.ti')
local literalti = baseti:module(modname)

function literalti:init(pr)
	baseti.init(self, pr)
	self.serial = pr.serial
	self.fullname = pr.fullname
end

function literalti:defstring(lp)
	return self.fullname
end
