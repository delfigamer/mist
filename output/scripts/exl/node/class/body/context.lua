local modname = ...
local context = package.relrequire(modname, 4, 'context')
local classbodycontext = context:module(modname)

function classbodycontext:init(parent)
	context.init(self, parent)
end

function classbodycontext:setsymbol(name, symbol)
	self.parent:setsymbol(name, symbol)
end

function classbodycontext:getsymbol(name)
	return
		context.getsymbol(self, name) or
		self.parent.outer:getsymbol(name)
end

function classbodycontext:defstring(lp)
	return 'class body context:\n' .. context.defstring(self, lp)
end