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
	local name = self.args[2]:rcompile(stream)
	self.args[1]:lcompile(stream, name)
	return name
end

function aoinstance:getfulltype()
	return self.fulltype
end

fulltype = require('exl.fulltype')
