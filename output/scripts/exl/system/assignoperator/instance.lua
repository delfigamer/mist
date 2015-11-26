local modname = ...
local ebase = require('exl.node.expr.base')
local aoinstance = ebase:module(modname)

function aoinstance:init(pr)
	ebase.init(self, pr)
	self.aofunc = pr.aofunc
	self.args = pr.args
	self.context = pr.context
end

function aoinstance:rcompile(stream)
	if not self.retname then
		self.retname = self.args[2]:rcompile(stream)
		self.args[1]:lcompile(stream, self.retname)
	end
	return self.retname
end

function aoinstance:getfulltype()
	return self.aofunc.rettype
end
