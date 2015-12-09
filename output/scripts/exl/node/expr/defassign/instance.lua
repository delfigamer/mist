local modname = ...
local ebase = require('exl.node.expr.base')
local aoinstance = ebase:module(modname)
local fulltype

function aoinstance:init(pr)
	ebase.init(self, pr)
	self.context = pr.context
	self.args = pr.args
	self.fulltype = fulltype:create(pr.ti, false, true)
end

function aoinstance:rcompile(stream)
	if not self.retname then
		self.retname = self.args[2]:rcompile(stream)
		self.args[1]:lcompile(stream, self.retname)
	end
	return self.retname
end

fulltype = require('exl.fulltype')
