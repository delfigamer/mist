local modname = ...
local baseoi = package.relrequire(modname, 2, 'base.instance')
local classcalloi = baseoi:module(modname)
local fulltype

function classcalloi:init(pr)
	baseoi.init(self, pr)
	self.context = pr.context
	self.base = pr.base
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function classcalloi:rcompile(stream)
	if self.retname == nil then
		self.retname = stream:genname()
		stream:writetoken('v_table', self.retname)
		stream:writetoken('v_table_end')
	end
	return self.retname
end

fulltype = package.relrequire(modname, 4, 'fulltype')
