local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local initoi = baseoi:module(modname)
local fulltype

function initoi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.args = pr.args
end

function initoi:rcompile(stream)
	if self.retname == nil then
		self.retname = false
		self.args[1]:lcompile(stream, 0)
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
